Space Runner 3D

In Space Runner 3D, the player pilots a spaceship flying endlessly through a canyon of rocky hills. The spaceship must avoid crashing into the terrain while the score increases with every passing second. Difficulty ramps up as the speed increases every 5 points, testing the player’s reflexes. The spaceship can move left and right using the arrow keys. If the ship collides with the terrain, the game is over. The player can press R to restart and try again.

---
Work Division (3 Team Members)

Team Member 1: Environment & Terrain Generation

Implement 3D rendering of hills/canyon walls using OpenGL primitives.

Make the terrain scroll forward to simulate spaceship movement.

Add variety: different hill heights or patterns generated randomly for more challenge.

Add simple shapes or colors to hills to make the environment more immersive. ( Not Actual OpenGl textures)

Implement booster activation: pressing Space makes the spaceship move faster for 3 seconds.


Team Member 2: Spaceship & Player Controls

Model/render the spaceship (simple 3D shape like a cone/cuboid).

Implement left/right movement with arrow keys.

Up and down keys

Ensure movement constraints so the spaceship stays within the screen/canyon.

Handle collision detection with terrain edges (in sync with Member 1).



Team Member 3: Game Logic & UI

Develop the scoring system: +1 point for every second survived.

Create the Game Over state.

Display a message when the spaceship crashes.

Stop all movement and input until restart.

Show motivational or inspiring messages after every 5 seconds to encourage replayability.

Speed increase after 5 points

Implement Restart Logic: pressing R resets the spaceship position, terrain generation, and score counter.

Add a Countdown Start Screen (3…2…1…Go!) before each run for anticipation.

Track and display the Final Score when the game ends.

Track and display the Highest Score so far across runs for replay motivation.



