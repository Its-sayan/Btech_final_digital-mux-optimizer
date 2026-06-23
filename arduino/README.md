# Arduino Timing Verification

This directory contains the Arduino sketch used to physically verify
the timing characteristics of different multiplexer implementations.

## Hardware Requirements

- Arduino Uno (ATmega328P, 16MHz)
- No external components needed (uses internal pull-ups)

## Setup

1. Open `mux_timing_verify.ino` in Arduino IDE
2. Select Board: "Arduino Uno"
3. Upload and open Serial Monitor (115200 baud)

## What It Measures

Three multiplexer implementations are emulated in software:
- **Sum-of-Products**: Textbook AND-OR structure
- **Tree of 2:1 MUXes**: Hierarchical selection
- **Direct Bit-Manipulation**: Optimized control case

Execution time is measured in microseconds using `micros()` over 1000 trials.

## Expected Results

The Direct Bit-Manipulation approach is fastest, confirming that
structural optimization (reducing gate depth) directly translates
to improved execution time on a sequential processor.

These results validate the C++ simulator's delay predictions.