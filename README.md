# TML

TML is a Time Machine Library for SDL.

The programmer can specify points in time to travel back and forth and resume
the simulation from there.

Simulations have two modes, which the programmer controls through the API:

1. Record mode:
    The simulation time moves forward and TML internally records events and
    periodic memory state snapshots. This is the initial mode.
2. Time travel mode:
    The simulation time freezes, and the programmer can specify a point in time
    to travel, which TML restores automatically.

When time travelling, TML restores the closest past snapshot simulates the
remaining events to reach the exact requested point in time.
Because TML relies on state snapshots and deterministic simulation, there are a
number of limitations in the programming API:

- No stateful calls are allowed (e.g., `rand`, `fopen`, etc).
- All memory state must be static and/or preallocated.
