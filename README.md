James Phillips 8/29/25

This is my cpp physics engine.

So far, I have a single body physics simulation with a bouncing ball.
You can hold left mouse button down to move it and release to throw it.

ToDo:
- Continue optimizing
- Introduce a second ball (dun dun dunnnn)
- Add cool debugging features with debug mode:
    - Live fps counter
    - Replay with slider by tracking balls previous position
    - Determinsitc reruns to see effects of chaning things (like physics tick rate)

Issues:
- Annoying: Moving your cursor floods the event poll, reducing the fps by
a lot. Filtering out the event in multiple ways hasn't helped, even when it's
obvious that the program isn't responding to the event.
- Need to look in to position interpolating to make it less choppy (idrk what this means)