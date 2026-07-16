#!/usr/bin/env python3
# Archangel ground station.
# Run with no arguments for SIMULATION MODE: it generates fake telemetry so
# the whole app is testable with no hardware. Use --serial PORT for the real
# LoRa bridge once it exists.

import argparse
import math
import random
import sys
import time

import protocol


class SimTransport:
    def __init__(self, rate_hz=4.0, loss=0.05):
        self.dt = 1.0 / rate_hz
        self.loss = loss
        self.seq = 0
        self.t0 = time.time()

    def lines(self):
        while True:
            time.sleep(self.dt)
            self.seq = (self.seq + 1) & 0xFFFF
            t = time.time() - self.t0
            tele = protocol.Telemetry(
                seq=self.seq,
                t_ms=int(t * 1000),
                roll=25 * math.sin(t * 0.5),
                pitch=8 * math.sin(t * 0.3),
                yaw=(t * 20) % 360,
                alt_m=30 + 15 * math.sin(t * 0.1),
                vbat=max(9.6, 12.6 - t * 0.01),
                mode=1 if (int(t) // 5) % 2 else 0,
            )
            if random.random() < self.loss:
                continue
            yield protocol.encode(tele)


class SerialTransport:
    def __init__(self, port, baud=115200):
        try:
            import serial
        except ImportError:
            sys.exit("pyserial not installed. Run: pip install pyserial")
        self.ser = serial.Serial(port, baud, timeout=1)

    def lines(self):
        while True:
            raw = self.ser.readline().decode("utf-8", "replace")
            if raw:
                yield raw


class LinkStats:
    def __init__(self):
        self.received = 0
        self.lost = 0
        self.last_seq = None

    def update(self, seq):
        self.received += 1
        if self.last_seq is not None:
            gap = (seq - self.last_seq - 1) & 0xFFFF
            if gap < 1000:
                self.lost += gap
        self.last_seq = seq

    @property
    def loss_pct(self):
        total = self.received + self.lost
        return 100.0 * self.lost / total if total else 0.0


def render(t: protocol.Telemetry, stats: LinkStats):
    print("\033[2J\033[H", end="")
    print("=== ARCHANGEL GROUND STATION ===\n")
    print(f"  mode      {t.mode_name}")
    print(f"  roll      {t.roll:+6.1f} deg")
    print(f"  pitch     {t.pitch:+6.1f} deg")
    print(f"  yaw       {t.yaw:6.1f} deg")
    print(f"  altitude  {t.alt_m:6.1f} m")
    print(f"  battery   {t.vbat:6.2f} V  {_batt_bar(t.vbat)}")
    print(f"  uptime    {t.t_ms/1000:6.1f} s")
    print()
    print(f"  packets   {stats.received}   loss {stats.loss_pct:4.1f}%")
    if t.vbat < 10.5:
        print("\n  ** LOW BATTERY, LAND NOW **")


def _batt_bar(v, vmin=9.6, vmax=12.6):
    frac = max(0.0, min(1.0, (v - vmin) / (vmax - vmin)))
    n = int(frac * 20)
    return "[" + "#" * n + "-" * (20 - n) + "]"


def main():
    ap = argparse.ArgumentParser(description="Archangel ground station")
    ap.add_argument("--serial", metavar="PORT")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--sim", action="store_true")
    args = ap.parse_args()

    if args.serial:
        transport = SerialTransport(args.serial, args.baud)
        print(f"Listening on {args.serial} @ {args.baud} ...")
    else:
        transport = SimTransport()
        print("SIMULATION MODE: generating fake telemetry. Ctrl+C to quit.")
        time.sleep(1.0)

    stats = LinkStats()
    try:
        for line in transport.lines():
            tele = protocol.parse(line)
            if tele is None:
                continue
            stats.update(tele.seq)
            render(tele, stats)
    except KeyboardInterrupt:
        print("\nGround station stopped.")


if __name__ == "__main__":
    main()
