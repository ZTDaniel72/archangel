

import math
import sys
import threading
import time
import tkinter as tk

PULSE_MIN, PULSE_MID, PULSE_MAX = 1000, 1500, 2000

state = {
    "roll": 1500, "pitch": 1500, "thr": 1000, "rudd": 1500, "aux": 1000,
    "ailL": 1500, "ailR": 1500, "ruddL": 1500, "ruddR": 1500,
    "healthy": 0, "imu_roll": 0.0, "imu_pitch": 0.0, "imu_alt": 0.0, "last_rx": 0.0,
}

INT_KEYS = ["roll", "pitch", "thr", "rudd", "aux",
            "ailL", "ailR", "ruddL", "ruddR", "healthy"]


def parse_line(line):
    line = line.strip()
    if not line.startswith("ABN,"):
        return
    p = line.split(",")
    if len(p) != 14:
        return
    try:
        for k, v in zip(INT_KEYS, p[1:11]):
            state[k] = int(v)
        state["imu_roll"] = float(p[11])
        state["imu_pitch"] = float(p[12])
        state["imu_alt"] = float(p[13])
        state["last_rx"] = time.time()
    except ValueError:
        pass


def sim_thread():
    t0 = time.time()
    while True:
        time.sleep(0.05)
        t = time.time() - t0
        roll = int(1500 + 400 * math.sin(t * 0.9))
        pitch = int(1500 + 300 * math.sin(t * 0.6))
        thr = int(1500 + 490 * math.sin(t * 0.25))
        rudd = int(1500 + 350 * math.sin(t * 1.3))
        aux = 1900 if int(t) % 8 < 4 else 1100
        d = rudd - PULSE_MID
        ruddL = max(PULSE_MIN, min(PULSE_MAX, pitch + d))
        ruddR = max(PULSE_MIN, min(PULSE_MAX, pitch - d))
        ailL = roll
        ailR = 2 * PULSE_MID - roll
        healthy = 0 if int(t) % 12 >= 10 else 1
        imu_roll = 35 * math.sin(t * 0.7)
        imu_pitch = 20 * math.sin(t * 0.5)
        alt = 20 + 15 * math.sin(t * 0.35)
        parse_line(f"ABN,{roll},{pitch},{thr},{rudd},{aux},"
                   f"{ailL},{ailR},{ruddL},{ruddR},{healthy},"
                   f"{imu_roll:.1f},{imu_pitch:.1f}, {alt:.1f}")


def serial_thread(port):
    try:
        import serial
    except ImportError:
        sys.exit("pyserial missing. Run: pip install pyserial")
    ser = serial.Serial(port, 115200, timeout=1)
    while True:
        raw = ser.readline().decode("utf-8", "replace")
        if raw:
            parse_line(raw)



BG = "#000000"
PANEL = "#141a24"
EDGE = "#2a3546"
FG = "#d7e0ee"
MUTE = "#7f92b0"
ACCENT = "#4da3ff"
WARN = "#ff4d5e"
OK = "#39d98a"
SKY = "#2f6fb0"
GROUND = "#6b4a2a"


class App:
    def __init__(self, root, mode):
        self.root = root
        root.title("ARCHANGEL bench station")
        root.configure(bg=BG)
        self.c = tk.Canvas(root, width=900, height=600, bg=BG,
                           highlightthickness=0)
        self.c.pack(padx=10, pady=10)
        self.mode = mode
        self.tick()

    def bar(self, x, y, w, h, frac, label, value, color):
        c = self.c
        c.create_rectangle(x, y, x + w, y + h, outline=EDGE, width=1,
                           fill=PANEL)
        frac = max(0.0, min(1.0, frac))
        c.create_rectangle(x + 2, y + 2, x + 2 + (w - 4) * frac, y + h - 2,
                           fill=color, width=0)
        c.create_text(x, y - 9, text=label, fill=FG, anchor="w",
                      font=("Consolas", 10, "bold"))
        c.create_text(x + w, y - 9, text=str(value), fill=color, anchor="e",
                      font=("Consolas", 10))

    def surface(self, cx, cy, angle_deg, length, label):
        a = math.radians(angle_deg)
        x2 = cx + length * math.cos(a)
        y2 = cy - length * math.sin(a)
        self.c.create_line(cx, cy, x2, y2, fill=ACCENT, width=6,
                           capstyle="round")
        self.c.create_text(cx, cy + 16, text=label, fill=MUTE,
                           font=("Consolas", 9))
    
    def alt_tape(self, x, y, h, alt):
        c = self.c
        w = 24
        c.create_rectangle(x, y, x+w, y+h, outline=EDGE, width=1, fill=PANEL)
        lo, hi = -10.0, 60.0
        def yfor(a):
            a = max(lo, min(hi,a))
            return y + h - (a - lo) / (hi-lo) * h
        for a in range(-10, 61, 10):
            ty = yfor(a)
            c.create_line(x, ty, x + 5, ty, fill=MUTE)
            c.create_text(x+w+3, ty, text=str(a), fill=MUTE, anchor="w", font=("Consolas", 7))
        ind = yfor(alt)
        c.create_line(x-3, ind, x+w+3, ind, fill=ACCENT, width=2)
        c.create_text(x+w / 2, y-10, text="ALT m", fill=MUTE, font=("Consolas", 9, "bold"))
        c.create_text(x+w / 2, y+h+12, text=f"{alt:+.1f}", fill=FG, font=("Consolas", 10, "bold"))

    def horizon(self, cx, cy, r, roll_deg, pitch_deg):
        c = self.c
        rr = math.radians(-roll_deg)
        cos, sin = math.cos(rr), math.sin(rr)
        off = max(-r, min(r, -pitch_deg * 3.0))   

        N = 96
        circle = [(r * math.cos(2 * math.pi * i / N),
                   r * math.sin(2 * math.pi * i / N)) for i in range(N)]

        def dist(pt):
            return (-pt[0] * sin + pt[1] * cos) - off

        def clip(keep_below):
            out = []
            n = len(circle)
            for i in range(n):
                a = circle[i]
                b = circle[(i + 1) % n]
                fa, fb = dist(a), dist(b)
                ina = (fa >= 0) if keep_below else (fa <= 0)
                inb = (fb >= 0) if keep_below else (fb <= 0)
                if ina:
                    out.append(a)
                if ina != inb and fa != fb:
                    t = fa / (fa - fb)
                    out.append((a[0] + t * (b[0] - a[0]),
                                a[1] + t * (b[1] - a[1])))
            return out

        def to_screen(poly):
            flat = []
            for (x, y) in poly:
                flat += [cx + x, cy + y]
            return flat

        sky = clip(keep_below=False)   
        gnd = clip(keep_below=True)      
        if len(sky) >= 3:
            c.create_polygon(to_screen(sky), fill=SKY, outline="")
        if len(gnd) >= 3:
            c.create_polygon(to_screen(gnd), fill=GROUND, outline="")

        t = math.sqrt(max(0.0, r * r - off * off))
        p0 = (-off * sin, off * cos)
        d = (cos, sin)
        i1 = (cx + p0[0] + t * d[0], cy + p0[1] + t * d[1])
        i2 = (cx + p0[0] - t * d[0], cy + p0[1] - t * d[1])
        c.create_line(i1[0], i1[1], i2[0], i2[1], fill="white", width=2)

        c.create_oval(cx - r, cy - r, cx + r, cy + r, outline=EDGE, width=4)
    
        c.create_line(cx - 28, cy, cx - 9, cy, fill="#ffd633", width=3)
        c.create_line(cx + 9, cy, cx + 28, cy, fill="#ffd633", width=3)
        c.create_oval(cx - 2, cy - 2, cx + 2, cy + 2, fill="#ffd633",
                      outline="")

        c.create_text(cx, cy - r - 14, text="ATTITUDE (IMU)", fill=MUTE,
                      font=("Consolas", 11, "bold"))
        c.create_text(cx, cy + r + 16,
                      text=f"R {roll_deg:+6.1f}   P {pitch_deg:+6.1f}",
                      fill=FG, font=("Consolas", 11))

    def tick(self):
        s = dict(state)
        c = self.c
        c.delete("all")

        stale = time.time() - s["last_rx"] > 1.0
        healthy = s["healthy"] and not stale

        c.create_text(20, 24, text="ARCHANGEL BENCH STATION", fill=FG,
                      anchor="w", font=("Consolas", 16, "bold"))
        status = "LINK OK" if healthy else "NO LINK / FAILSAFE"
        c.create_rectangle(700, 12, 884, 38, outline=(OK if healthy else WARN),
                           width=2, fill=PANEL)
        c.create_text(792, 25, text=status, fill=(OK if healthy else WARN),
                      font=("Consolas", 11, "bold"))
        mode = "SPORT" if s["aux"] > PULSE_MID else "GENTLE"
        c.create_text(20, 48, text=f"rates {mode}    source {self.mode}",
                      fill=MUTE, anchor="w", font=("Consolas", 10))

        c.create_text(20, 84, text="RC INPUT", fill=MUTE, anchor="w",
                      font=("Consolas", 11, "bold"))
        chans = [("ROLL", s["roll"]), ("PITCH", s["pitch"]),
                 ("THROTTLE", s["thr"]), ("RUDDER", s["rudd"]),
                 ("AUX", s["aux"])]
        y = 108
        for name, v in chans:
            frac = (v - PULSE_MIN) / (PULSE_MAX - PULSE_MIN)
            self.bar(20, y, 360, 18, frac, name, v,
                     ACCENT if healthy else "#41506a")
            y += 46

        c.create_text(430, 84, text="MIXER OUTPUT", fill=MUTE, anchor="w",
                      font=("Consolas", 11, "bold"))
        outs = [("AIL L", s["ailL"]), ("AIL R", s["ailR"]),
                ("RUDDV L", s["ruddL"]), ("RUDDV R", s["ruddR"])]
        y = 108
        for name, v in outs:
            frac = (v - PULSE_MIN) / (PULSE_MAX - PULSE_MIN)
            self.bar(430, y, 360, 18, frac, name, v,
                     OK if healthy else "#41506a")
            y += 46

        self.horizon(170, 470, 92, s["imu_roll"], s["imu_pitch"])

        self.alt_tape(300, 392, 150, s["imu_alt"])

        c.create_text(560, 372, text="V-TAIL", fill=MUTE, anchor="w",
                      font=("Consolas", 11, "bold"))
        bx, by = 660, 480
        c.create_oval(bx - 16, by - 16, bx + 16, by + 16, outline=EDGE,
                      width=2, fill=PANEL)
        self.surface(bx, by, 145 - (s["ruddL"] - PULSE_MID) / 500.0 * 25,
                     104, "L")
        self.surface(bx, by, 35 + (s["ruddR"] - PULSE_MID) / 500.0 * 25,
                     104, "R")

        if s["thr"] > 1150:
            c.create_text(450, 576, anchor="center", fill=WARN,
                          font=("Consolas", 11, "bold"),
                          text="THROTTLE LIVE  -  PROPS OFF")

        self.root.after(50, self.tick)


def main():
    if len(sys.argv) > 1:
        port = sys.argv[1]
        threading.Thread(target=serial_thread, args=(port,),
                         daemon=True).start()
        mode = port
    else:
        threading.Thread(target=sim_thread, daemon=True).start()
        mode = "SIMULATION"
    root = tk.Tk()
    App(root, mode)
    root.mainloop()


if __name__ == "__main__":
    main()
