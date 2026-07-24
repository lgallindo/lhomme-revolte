# Enemy AI Behavior

The current enemy AI in the L'Homme Révolté (LHR) engine is highly deterministic and designed to run entirely on integer math without floating-point physics, perfectly suited for the raycasting backend.

## AI State Machine
Enemies exist in one of several states defined in `core/game.h` (`LHR_MonsterState`):
- **Idle:** Standing still, unaware of the player.
- **Moving:** 8-directional movement states (N, S, E, W, NE, NW, SE, SW).
- **Attacking:** Entering a firing/melee animation state.

## Line of Sight (LoS) & Aggro
The AI cycle evaluates on every logic frame in `LHR_monsterPerformAI`:
1. **Distance Check:** The engine calculates the Manhattan distance to the player using bitwise absolute values.
2. **Raycasting LoS:** If the player is within range, the monster casts a single logic ray (`LHR_raycast`) towards the player's coordinates.
3. **Aggro:** If the ray hits the player without colliding with a solid wall, the monster becomes aware.

## Movement Logic
- Monsters cannot pathfind around complex maze structures natively; they use direct line-of-sight vectoring.
- If the player is visible, the monster evaluates the player's relative position (Delta X, Delta Y) and snaps to the nearest of the 8 directional movement states.
- Collision detection checks the target tile map properties (`LHR_getMapTile`). If a wall blocks the path, the monster stays idle or slides along the wall if diagonal.

## Attack Logic
When within attack range and having clear LoS, the monster triggers its attack type based on its definition:
- **Melee:** Deals immediate hitscan damage if adjacent.
- **Projectile (Plasma/Bullet):** Spawns a `LHR_ProjectileRecord` entity traveling along the vector towards the player.
- **Explode:** Self-detonates (kamikaze style) dealing AoE damage.

This minimalist AI ensures that 100+ enemies can be processed in a fraction of a millisecond on extreme low-end hardware.
