
import math
import random
import sys
import threading
import time
import tkinter as tk

PULSE_MIN, PULSE_MID, PULSE_MAX = 1000, 1500, 2000

state = {
    "roll": 1500, "pitch": 1500, "thr": 1000, "rudd": 1500, "aux": 1000,
    "ailL": 1500, "ailR": 1500, "ruddL": 1500, "ruddR": 1500,
    "healthy": 0, "last_rx": 0.0,
}


def parse_line(line):
    line = line.strip()
    if not line.startswith("ABN,"):
        return
    p = line.split(",")
    if len(p) != 11:
        return
    try:
        keys = ["roll", "pitch", "thr", "rudd", "aux",
                "ailL", "ailR", "ruddL", "ruddR", "healthy"]
        for k, v in zip(keys, p[1:]):
            state[k] = int(v)
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
        parse_line(f"ABN,{roll},{pitch},{thr},{rudd},{aux},"
                   f"{ailL},{ailR},{ruddL},{ruddR},{healthy}")


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


BG, PANEL, FG, ACCENT, WARN, OK = ("#10141c", "#1a2230", "#d7e0ee",
                                   "#4da3ff", "#ff4d5e", "#39d98a")


class App:
    def __init__(self, root, mode):
        self.root = root
        root.title("ARCHANGEL bench station")
        root.configure(bg=BG)
        self.c = tk.Canvas(root, width=900, height=560, bg=BG,
                           highlightthickness=0)
        self.c.pack(padx=10, pady=10)
        self.mode = mode
        self.tick()

    def bar(self, x, y, w, h, frac, label, value, color=ACCENT):
        c = self.c
        c.create_rectangle(x, y, x + w, y + h, outline="#2c3a52", width=1,
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
        self.c.create_text(cx, cy + 16, text=label, fill=FG,
                           font=("Consolas", 9))

    def tick(self):
        s = dict(state)
        c = self.c
        c.delete("all")

        stale = time.time() - s["last_rx"] > 1.0
        healthy = s["healthy"] and not stale

        # header
        c.create_text(20, 24, text="ARCHANGEL BENCH STATION", fill=FG,
                      anchor="w", font=("Consolas", 16, "bold"))
        status = "LINK OK" if healthy else "FAILSAFE / NO LINK"
        c.create_rectangle(690, 10, 890, 40, fill=(OK if healthy else WARN),
                           width=0)
        c.create_text(790, 25, text=status, fill="#08101c",
                      font=("Consolas", 11, "bold"))
        mode = "SPORT" if s["aux"] > PULSE_MID else "GENTLE"
        c.create_text(20, 50, text=f"rates: {mode}   source: {self.mode}",
                      fill="#7f92b0", anchor="w", font=("Consolas", 10))

        c.create_text(20, 86, text="RC INPUT", fill="#7f92b0", anchor="w",
                      font=("Consolas", 11, "bold"))
        chans = [("ROLL", s["roll"]), ("PITCH", s["pitch"]),
                 ("THROTTLE", s["thr"]), ("RUDDER", s["rudd"]),
                 ("AUX", s["aux"])]
        y = 110
        for name, v in chans:
            frac = (v - PULSE_MIN) / (PULSE_MAX - PULSE_MIN)
            color = ACCENT if healthy else "#5a6a85"
            self.bar(20, y, 380, 20, frac, name, v, color)
            y += 52

        c.create_text(480, 86, text="MIXER OUTPUT", fill="#7f92b0",
                      anchor="w", font=("Consolas", 11, "bold"))
        outs = [("AIL L", s["ailL"]), ("AIL R", s["ailR"]),
                ("RUDDV L", s["ruddL"]), ("RUDDV R", s["ruddR"])]
        y = 110
        for name, v in outs:
            frac = (v - PULSE_MIN) / (PULSE_MAX - PULSE_MIN)
            self.bar(480, y, 380, 20, frac, name, v,
                     OK if healthy else "#5a6a85")
            y += 52

        c.create_text(480, 340, text="V-TAIL (rear view)", fill="#7f92b0",
                      anchor="w", font=("Consolas", 11, "bold"))
        base_x, base_y = 670, 470
        defl_l = (s["ruddL"] - PULSE_MID) / 500.0 * 25
        defl_r = (s["ruddR"] - PULSE_MID) / 500.0 * 25
        c.create_oval(base_x - 18, base_y - 18, base_x + 18, base_y + 18,
                      outline="#2c3a52", width=2, fill=PANEL)
        self.surface(base_x, base_y, 145 - defl_l, 110, "L")
        self.surface(base_x, base_y, 35 + defl_r, 110, "R")

        if s["thr"] > 1150:
            c.create_text(20, 380, anchor="w", fill=WARN,
                          font=("Consolas", 11, "bold"),
                          text="THROTTLE LIVE - PROPS OFF!")

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
