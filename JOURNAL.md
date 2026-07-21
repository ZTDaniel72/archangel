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

## 2026-07-17 -> 2026-07-20: CAD design finishes

During this timeline, I improved and finished the whole entire CAD design of the UAV.
It "should" now work and fly. I gave it a sleek, modern look, with plenty space in the nose cone for future upgrades.
I was also in doubt in this period, since a couple of my account info got tangled up, but luckily everything was fixed and now we're back better than ever.

## 2026-07-21: Coding, soldering, and more coding

Today I set up the ESP32. I wired the receiver to the ESP32 and got it reading all of the RC channels, after a long hassle. It took ages, since there was an apparent driver problem.
I rewrote the receiver code to use pulseIn.
I also wrote the V-tail mixer, which will be used on the aircraft in the near future. It consists of two ruddervators sharing pitch and rudder. I tested it on real servos.
I built a python bench dashboard that shows the live channels, mixer outputs, and a V-tail animation, plus a pitch/roll screen, just like on real aircraft!
This was my first time ever soldering as well. I put together the MPU/GY-91 headers in a great battle.
I got the sensor talking over 12C, I read raw accelerometer data, and converted it into roll/pitch angles via mathematical formulas.
I wrote a complementary filter as well, so the gyro and accelerometer blend nicely. Then, I added gyro-bias calibration, so there is zero drifting while standing still.
I somehow managed to package it all as an IMU module in the main firmware.
Lastly, I got the BMP280 barometer talking finally. It reads altitude, and then I smoothed it out with a low-pass filter.
There were a lot of struggles with this session, but I am glad that I managed to hold on and keep calm. It lead me down a path that I will remember for a while.
In the next following days, I will be printing the airframe, and ordering the electronical parts necessary for the rest of this build.

All of the videos recorded during this session will be as an unlisted youtube link
You can find it here: https://youtube.com/shorts/0uXKaofNkJc?feature=share