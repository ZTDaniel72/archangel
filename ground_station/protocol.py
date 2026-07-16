from dataclasses import dataclass

MARKER = "AGL"
MODES = {0: "MANUAL", 1: "STABILIZED", 2: "FAILSAFE"}


@dataclass
class Telemetry:
    seq: int
    t_ms: int
    roll: float
    pitch: float
    yaw: float
    alt_m: float
    vbat: float
    mode: int

    @property
    def mode_name(self) -> str:
        return MODES.get(self.mode, f"?{self.mode}")


def parse(line: str):
    line = line.strip()
    if not line.startswith(MARKER + ","):
        return None
    parts = line.split(",")
    if len(parts) != 9:
        return None
    try:
        return Telemetry(
            seq=int(parts[1]),
            t_ms=int(parts[2]),
            roll=float(parts[3]),
            pitch=float(parts[4]),
            yaw=float(parts[5]),
            alt_m=float(parts[6]),
            vbat=float(parts[7]),
            mode=int(parts[8]),
        )
    except ValueError:
        return None


def encode(t: Telemetry) -> str:
    return (f"{MARKER},{t.seq},{t.t_ms},{t.roll:.1f},{t.pitch:.1f},"
            f"{t.yaw:.1f},{t.alt_m:.1f},{t.vbat:.2f},{t.mode}")
