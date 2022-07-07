# TML

TML is a Time Machine Library for SDL.

The programmer can specify points in time to travel back and forth and resume
the simulation from there.

<img src="jump.gif" width="250">
<img src="cards.gif" width="250">

Simulations have two modes, which the programmer controls through the API:

1. Record mode:
    The simulation time moves forward, and TML internally records all events
    and periodic memory snapshots.
    This is the initial mode.
2. Time travel mode:
    The simulation time freezes, and the programmer can specify a point in time
    to travel, which TML restores automatically.

When time travelling, TML restores the closest past snapshot and then simulates
the remaining events to reach the exact requested point in time.
Because TML relies on memory snapshots and deterministic simulation, there are
severe limitations in the programming API:

- No stateful calls are allowed (e.g., `rand`, `fopen`, etc).
- All memory state must be static and/or preallocated.

# API
