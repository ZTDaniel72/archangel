# Archangel bench wiring guide (Milestone 1)

Total beginner friendly. Follow top to bottom. Do NOT connect the flight
battery until the step that says so. NO PROPELLER ON THE MOTOR. EVER, on
the bench.

## The parts in front of you

- ESP32 board (NodeMCU-ESP-32S). Pins are labeled on the board itself in
  tiny white text. The labels we use: `P34 P35 P32 P33 P18 P19 P21 P23`,
  `VIN` (this is 5V from USB), and `GND` (there are several GND pins,
  they are all the same).
- Receiver (the small box that came with your transmitter). It has rows
  of 3-pin channels labeled CH1..CH6. On each row:
  the pin CLOSEST to the channel label / inner side = SIGNAL,
  middle = + (power), outer = - (ground). If your receiver is different,
  look up its pinout, but FlySky iA6B style is: signal on top row,
  + in middle row, - on bottom row.
- Servo plug (3 wires): ORANGE (or yellow/white) = signal,
  RED = + power, BROWN (or black) = ground.
- Breadboard: the two long rows along each edge are "rails". All holes
  in one rail row are connected together. We use one rail row as the
  GROUND BUS (everything's ground meets here) and another as the 5V BUS.
- Jumper wires: M-M = pin on both ends, M-F = pin on one end, socket on
  the other. Receiver and servo plugs need the PIN end pushed into them,
  so receiver->breadboard connections use M-M or M-F depending on side.

## Phase 1: receiver only (power comes from USB)

Goal: the ESP32 reads your transmitter sticks. Nothing moves yet.

1. Unplug everything. ESP32 NOT connected to USB yet.
2. Put the ESP32 across the middle groove of the breadboard if it fits,
   or leave it beside the breadboard and use M-F jumpers to its pins.
3. Build the ground bus: jumper from any ESP32 `GND` pin to the blue (-)
   rail of the breadboard.
4. Build the 5V bus: jumper from ESP32 `VIN` to the red (+) rail.
   (With USB plugged in, VIN carries about 5V. That is enough to power
   the receiver.)
5. Receiver power: M-F jumper from the red (+) rail to the MIDDLE pin of
   receiver CH1 row. Another from the blue (-) rail to the OUTER pin of
   the CH1 row. (Power on any one channel row powers the whole receiver.)
6. Receiver signals to ESP32 (SIGNAL pin of each channel row):
   - CH1 signal -> ESP32 `P34`   (aileron / right stick left-right)
   - CH2 signal -> ESP32 `P35`   (elevator / right stick up-down)
   - CH3 signal -> ESP32 `P32`   (throttle / left stick up-down)
   - CH5 signal -> ESP32 `P33`   (the switch you assigned / AUX)
   If CH5 does nothing later, your transmitter may need the switch
   assigned to channel 5 in its own menu (AUX setup).
7. Now plug the ESP32 into the laptop by USB. Receiver LED should light
   (solid if bound to the transmitter).
8. Transmitter ON. Open Serial Monitor (115200). With the debug print in
   loop(), you should see four numbers around 1000-2000 that follow the
   sticks. Write down which stick moves which number.

Phase 1 is DONE when all four values track their sticks.

## Phase 2: one servo (still USB power)

Goal: stick moves servo, through your firmware.

9. Take ONE servo. Push three M-M jumpers into its plug:
   - BROWN  -> blue (-) rail
   - RED    -> red (+) rail
   - ORANGE -> ESP32 `P18`
10. Move the right stick left-right. The servo should follow.
11. Flip the AUX switch: the throw should get bigger/smaller
    (dual rates). One servo on USB power is fine, do not add more yet.

Phase 2 is DONE when the servo mirrors the stick.

## Phase 3: full rig with ESC power (NO PROP!)

Goal: everything powered properly, all 4 servos + ESC.

12. UNPLUG USB and the battery (nothing powered while wiring).
13. Solder an XT60 onto the ESC battery wires if not done
    (red = +, black = -, heat-shrink both).
14. The ESC's thin 3-wire lead (its BEC) plugs like a servo:
    - its BROWN/BLACK -> blue (-) rail
    - its RED         -> red (+) rail   <- the BEC now FEEDS the rail
    - its ORANGE/WHITE (throttle signal) -> ESP32 `P23`
    IMPORTANT: from now on the red rail is powered by the BEC, NOT by
    VIN. REMOVE the jumper between VIN and the red rail from step 4.
    Ground stays shared: ESP32 GND remains connected to the blue rail.
    (Common ground = mandatory. Never remove that one.)
15. All four servos now connect like step 9:
    brown -> blue rail, red -> red rail, signals:
    - Left aileron  -> `P18`
    - Right aileron -> `P19`
    - Ruddervator L -> `P21`  (elevator output in current firmware)
    - Ruddervator R -> (added when the V-tail mixer is written)
16. Receiver stays as wired (its power now also comes from the BEC rail).
17. Motor: secured/clamped down, NO propeller, wires clear.
18. Power-up ORDER, every single time:
    a) Transmitter ON, throttle stick fully DOWN.
    b) ESP32 USB in (for serial monitor + its own power).
    c) Flight battery -> ESC XT60. ESC beeps its startup melody.
19. All four servos should follow sticks. Wiggle everything. If the
    ESP32 randomly reboots while servos move: the BEC is too weak,
    note it, order a separate 5V UBEC.

## Phase 4: throttle + failsafe test

20. Fingers, hair, wires AWAY from the motor. Gentle throttle: motor
    spins. Note direction (counterclockwise seen from the front is
    correct for a standard prop; to reverse, swap any two of the three
    thick motor wires).
21. FAILSAFE TEST: gentle throttle, then switch the TRANSMITTER OFF.
    Motor must stop and servos must center within ~half a second.
    TX back on: control returns. Repeat 3x.
22. Disconnect battery FIRST when done, then USB.

Film step 21. That video is the Milestone 1 demo.

## If something doesn't work

- Channel reads 0 in serial: wrong pin, or signal/ground swapped, or
  receiver not bound (LED blinking).
- Servo doesn't move but values track: servo power (red/brown) not on
  the rails, or signal on wrong pin.
- Everything dead in Phase 3: check the BEC red is on the red rail and
  the VIN jumper was removed; check common ground.
- ESC beeps endlessly, motor won't arm: throttle not at minimum, or
  throttle channel not reaching the ESC pin (`P23` in config.h).
