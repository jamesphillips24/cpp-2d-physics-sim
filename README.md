James Phillips 8/25/25

This is my cpp physics engine.

So far, I intend to make a rather simple 2D physics simulation.

ToDo:
- Working on moving the rect with the mouse.
- Mouse tracking works, but I realized in order to update the rect's position, I'd ned to call render_ball() which also deals with the ball
movement.
- Probably best to make 2 functions: move_ball() and render_ball() or
something similar.
- Afterwards, track the cursor's previous locations in order to average
out velocity when mouse lets go.