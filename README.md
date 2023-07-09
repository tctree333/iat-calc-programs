# IAT Calculator Programs

This is a collection of C/C++ programs compiled for the TI-84 Plus CE calculator, using the [CE Toolchain](https://ce-programming.github.io/toolchain/index.html), created for the It's About Time event from the 2023 Science Olympiad season.

Note: This repository contains an outdated version of the CE Toolchain. I'm probably unlikely to come back to this project so hopefully it works.

## Building

Projects can be built with the `make` command in the project directory. A `.8xp` file will be built in the `bin` directory, which can be copied to your calculator. Emulation is also possible. Note that after calculator OS versions 5.4.0, assembly programs are no longer allowed. You can get around this by using [arTIfiCE](https://yvantt.github.io/arTIfiCE/) to "jailbreak" your calculator, then use [ASMHOOK](https://github.com/jacobly0/asmhook) to run programs.

For more information, including options for compiling on Windows, check out the [toolchain's documentation](https://ce-programming.github.io/toolchain/index.html).

## Contents

- **calconv** is a program that converts between the Gregorian, Julian, Julian Day, Hebrew, Islamic, Persian, Mayan, and Indian Civil calendars. It is based on JS code from the [Fourmilab calendar converter](https://www.fourmilab.ch/documents/calendar/), with the algorithm for Hebrew calendar conversion from [this website](https://www.aa.quae.nl/en/reken/juliaansedag.html).
- **daywk** is a program that calculates the day of the week for a given Gregorian calendar date. It uses the built in `dayOfWk` function from the TI calculator, but uses `calconv` to handle pre-Gregorian dates too. To use this program, you must alo have `zcalconv` on your calculator.
- **stopwatch** is a simple stopwatch program that times in seconds, with lap times displayed as cumulative times from start. You can press any key to lap, pressing clear to stop (then reset) the time. The 2023 IAT build required 3 equally spaced signals, with scoring relative to the start. This stopwatch program is modified from the example stopwatch program from the [CE Toolchain](https://ce-programming.github.io/toolchain/index.html).

## CALCONV

This program converts between different calendars, which occasionally appears on tests. Instead of working it out by hand, I decided to create a program that would do this for me, since we are allowed programmable, graphing calculators for this event.

I wanted to add a converter for the Chinese calendar too, since it was on the rules this year, but could not find an easy algorithm to implement. The book [Calendrical Calculations: The Ultimate Edition](https://www.cs.tau.ac.il/~nachum/calendar-book/fourth-edition/) has information on the Chinese Calendar, including sample Lisp code, but I did not have time to convert it to C++.

### Usage

The C++ program handles the calendar conversion algorithms, while the user interface was created in TI-BASIC. After building the program with `make`, copy both the built `bin/ZCALCONV.8xp` and `src/CALCONV.8xp` programs to your calculator. Run the `CALCONV` program to use the calendar converter.

If you are on calculator OS 5.5.0 or later, you will need to use [arTIfiCE](https://yvantt.github.io/arTIfiCE/) to "jailbreak" your calculator, then install [ASMHOOK](https://github.com/jacobly0/asmhook) to allow the UI to call the backend converter.

When converting TO the Mayan calendar, the program outputs the Long Count date, then the Haab and Tzolkin dates. When converting FROM the Mayan calendar, the program expects the Long Count date only.

### Design

The program is split into two parts: the C++ backend and the TI-BASIC frontend. The backend handles the actual calendar conversion, while the frontend handles the user interface. These programs communicate through matrix J, with the frontend putting inputs in the first row, then calling ZCALCONV, which puts the outputs in the second row. The frontend then displays the outputs.

To perform the calendar conversions, each calendar provides methods to convert between that calendar and the Julian Day Number. After you input your date to convert, the program turns it into the JDN. After selecting the target calendar, the JDN is then converted to the target calendar.
