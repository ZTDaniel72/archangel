# Archangel Development Journal

-Started the project and ruminated on the exact idea of the UAV and all characteristics


## 2026-07-14: Aileron CAD

Recording: https://lapse.hackclub.com/timelapse/XpcbddsRDkn5
The aileron includes a very unique design that minimalizes drag, thanks to hidden servos inside the wing structure.
The servos capitalize on the flexibility of LW-PLA, therefore instead of needing aileron flaps, the whole wing flexes.

## 2026-07-14: Firmware skeleton

The firmware coding also started today, and I made some major improvements.
I havent coded in any type of C-based language in a long while so it took a bit to figure everything out.
In the end, I think that it went really well, and I am excited to test all of my code out for-real when my arduino and million other electrical components arrive.
I implemented a failsafe, dual rates and also coded the receiver pass-through

## 2026-07-15: Fuselage, Tail mount and more wing improvements + wingtip

Recording: https://lapse.hackclub.com/timelapse/FDi42ch49GMw
Another recording: https://lapse.hackclub.com/timelapse/CiaGbsH7okC-

This day was one of my largest so far, I sat down for a good while, just contemplating the best design choices.
I started off with some minor improvements to yesterday's wing shape and design, which also led me down a rabbit hole of efficient airfoils.
The wingtip was also designed this day, with a lot of hard streches, but I managed to figure out the design.
Some of the challenges included: the size and weight design of the tip, whilst also minimizing drag, but also making it look cool.
Then came the "final boss" of this CAD session: the fuselage.
I used a clever, tube-like design for the fuselage, so that it looks realistic.
I also made a rough design of the V-tail mount for the combined elevator and rudder design, which is also common in UAV-s today.

## 2026-07-16: Telemetry, ground station and battery

Today I quickly put together a simulated version of a ground station and what that would look.
I also decided on the LoRa, for long rage telemetry, which helps extend my range up kilometers even.
I thought through battery options and ended up on a Li-ion battery. The exact model is yet to be chosen.
Got LW-ASA instead of LW-PLA (they delivered the wrong one) but eventually I figured that this is in my favor, since ASA is more resistant to the sun and UV compared to PLA.
Today, i will start on the tail mixer, which is a complex code, especially because i would have to balance the operation on two wings instead of three.