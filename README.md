# Archangel

Archangel is an RC aircraft, that is designed to be efficient, and innovative.
It includes a 3D-printed airframe, a CUSTOM MADE ESP32 flight controller, and many more electric parts, discussed below.
The roadmap includes an FPV camera and more sensors with larger range, so that one day, this could stay in the air for 24 hours. But we are quite far from that right now lol.
I am building this because I love aviation and aeronautical engineering and I aspire to be one someday.
My goal is to get into TU Delft, in the Netherlands.

## Features

My current plan is:
Milestone 1: manual RC flight, custom firmware pass-through, failsafe, dual rates
Milestone 2: auto-level stabilization (MPU6500 + PID), SD flight logging
Milestone 3: altitude hold (BMP280), FPV camera
Wel shall see how far in we get, I think we can achieve all, but if not, milestone 2 will for sure be completed.

## Design

- Wingspan: ~110 cm, chord 17 cm (airfoil is a variation of this: http://airfoiltools.com/airfoil/details?airfoil=naca2412-il)
- Wing printed in 5 modules on a Bambu Lab P1S (25.6 cm bed), joined on a 10 mm carbon spar
- Ailerons: trailing 25% of chord on outer segments, 9g servo + pushrod per side
- Motor: Surpass Hobby C2836 1120KV, 3S, 9x6 prop
- CAD source: `cad/` (committed work-in-progress each session)

## Firmware

Custom flight controller on ESP32 (Arduino framework). Source in repo root.
Requires the ESP32Servo library.

## Steps to Reproduce

Start by printing the airframe. The wing comes off the plate in five modules, each printed standing on its root rib in single-wall
LW-PLA, with density added only around the spar bores and servo pockets through a Bambu modifier volume — everywhere else stays a
foamed single wall to keep the tips light.
The fuselage prints as circular segments split to fit the bed, each ending in a sleeve that slides into the next; walls run 1–1.2mm through the skin and thicken to 3mm at the joints,
the nose ring, and the wing saddle, which print in regular PLA for stiffness.
The two V-tail panels are NACA 0009, single-wall LW-PLA, solid only at the root tongue, and the motor firewall prints in ASA or PA6-CF at high density with the C2836 bolt circle and a built-in 2° down / 2° right thrust offset.
Print the plain rounded wingtip caps for early flights and treat the winglets as optional swap-ons.


Assemble from the spar outward. Thread the carbon spar through all five wing modules, epoxy them to each other and to the spar, and add the smaller anti-rotation rod near the trailing edge so the segments can't twist.
Glue the fuselage segments at their sleeves and run the two internal carbon longerons across the joints to carry bending loads past the seams.
Bond the firewall into the nose and bolt the motor down over washers.
Plug each V-tail panel into its angled root slot at a 105–110° included angle, epoxy them, and check by eye that both sit at identical incidence and dead parallel to the fuselage centreline.
Bolt the wing to its saddle at +1.5° with two nylon screws.


Build the electronics as one stack. On perfboard, solder the ESP32 together with the MPU6500+BMP280 on I2C, the microSD module, a 10k/2.2k divider off the battery for voltage sensing, and the RGB status LED behind 220Ω resistors.
Fit four servos — two ailerons in the outer wing segments, two ruddervators at the tail — each driving its surface through an arm, short pushrod and control horn, hinged with a little hole in the part that the horn pushes and then put a little rod of metal in the hole and tie it to the servo horn; no printed gears anywhere.
Run the receiver into the ESP32's input pins, the output pins out to the four servos and the ESC, and the ESC to the motor and battery through an XT60. Mount the battery on a tray that slides fore and aft, since that slide is how you'll trim the balance.


Flash the firmware last. Open the sketch in the Arduino IDE, install ESP32Servo, pick your board, and set every pin in config.h to match how you actually wired it.
Fill in the V-tail mixer where the loop leaves a TODO: each ruddervator is elevator plus-or-minus rudder, and you verify the signs on the bench — both surfaces up should mean nose-up, a rudder input should send them opposite ways.
Then bench-test with the propeller removed, always: confirm the sticks move the right surfaces the right way, kill the transmitter to prove the failsafe cuts throttle and centres everything, check dual rates on the AUX switch, and confirm the motor spins the correct direction, swapping any two motor wires if it doesn't.
Finish with balance and the maiden flight. Slide the battery until the CG sits about 30% of the chord back from the leading edge, erring nose-heavy because tail-heavy is the dangerous kind of wrong;
set aileron and ruddervator throws to ±15–20°, weigh the aircraft aiming under about 900g, then hand-launch it level and firm at roughly two-thirds throttle in calm air over an open field, well 
clear of people and within local rules. Trim for hands-off level flight on the gentle rate before you go exploring the sport rate.

## Safety notes

<!-- Required by shipping guide. Keep and expand these: -->
- LiPo batteries: charge only on a balance charger, never unattended, store at storage voltage
- Printing: LW-PLA and PLA soften around 55–60°C, so never leave the airframe in a hot car; structural parts near the motor are printed in ASA or PA6-CF for exactly this reason.
- Arming: always connect the flight battery last, with the throttle already at zero and the transmitter on first; disconnect the battery before doing anything near the prop.
- ALL bench tests are done with the propeller removed
- Electrical: the ESC's BEC powers the electronics, so don't back-feed a second power source; insulate solder joints with heat-shrink, and check for shorts before the first power-up.
- Failsafe check: Do a test to see if the failsafe actually triggers when going out of range
- Failsafe: on signal loss the firmware cuts throttle and centers surfaces
- Fly only in open areas away from people, per local (Hungarian/EU) drone regulations

## Credits

- Airfoil profile imported from airfoiltools.com (http://airfoiltools.com/airfoil/details?airfoil=naca2412-il)
- Everything else (airframe CAD, firmware, electronics integration): me
