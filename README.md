# TML

TML is a Time Machine Library for SDL.

The programmer can specify points in time to travel back and forth and resume
the simulation from there.

Simulations have two modes, which the programmer controls through the API:

1. Recording mode:
    In this default mode, the simulation time moves forward, and events are
    recorded.
2. Time travel mode:
    In this mode, the simulation time freezes, events are not recorded, and the
    programmer can specify a point in time to travel.

TML how it works...

For these reasons, ...
There are a number of limitations:

- No stateful calls are allowed (e.g., `rand`, `fopen`, etc).
- All simulation memory state must be static.
