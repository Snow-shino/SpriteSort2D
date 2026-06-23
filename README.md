# Sprite Sort 2D

Sprite Sort 2D is a small Unreal Engine 5.7+ plugin for top-down 2D games that need reliable sprite sorting without making translucent materials the default solution.

It sorts by a character or prop's feet/base position, puts sortable actors into a shared invisible 2D render-depth band, then moves only visual components inside that band. The actor root, collision, gameplay location, AI location, saved transform, traces, and navigation stay untouched for normal character setups.

## What It Is

Sprite Sort 2D is a masked-material-compatible render-depth sorting plugin for Paper2D sprites, Paper2D flipbooks, and generic primitive visual components.

It works with:

- Paper2D sprites
- Paper2D flipbooks
- Actors with multiple visual flipbooks, such as clothes, hair, weapons, and accessories
- Generic scene or primitive visual components
- Masked unlit materials
- Masked lit materials
- Opaque materials where the render-depth offset makes sense

## What It Is Not

Sprite Sort 2D is not just a translucent priority wrapper.

The default mode does not require translucent materials and does not rely on `Translucent Sort Priority` as the main sorting strategy. A translucent fallback exists only for special cases.

## Basic Setup

For most actors:

1. Add `Sprite Sort Component` to the actor.
2. Press Play.

That is the intended default workflow.

Leave these fields empty unless you have a special case:

- `VisualRoot`
- `TargetPrimitive`
- `SortOrigin`

By default, the component:

- Finds safe visual primitives automatically.
- Supports root-only sprite prop actors, such as a pole/pillar actor whose `RenderComponent` is the root.
- Sorts sibling visuals together, including equipment/clothing flipbooks.
- Ignores likely collision components.
- Uses a `Sprite Sort Origin` automatically if the actor has one.
- Uses the visual component pivot/origin as the sort point, then falls back to visual bounds.
- Places sortable actors in a shared 2D sort band so floors/maps do not interfere unless they also have `Sprite Sort Component`.
- Uses `Smart Auto` updates.
- Preserves each visual component's original local offset.

## Simple Prop Sort Line

For a pillar, tree, wall, counter, or other prop where you want direct control:

1. Add `Sprite Sort Component` to the prop.
2. Add `Sprite Sort Origin` to the same prop.
3. Move `Sprite Sort Origin` to the Y line where actors should switch behind/in front of the prop.
4. Leave `OriginMode` on `Auto`.

The sorter automatically uses that `Sprite Sort Origin` as the prop's explicit sort line. If the player's feet move above that line on the sort axis, the player sorts behind the prop. If the player's feet move below it, the player sorts in front.

You do not need `VisualRoot` or `TargetPrimitive` for this setup.

## Recommended Actor Hierarchies

Simple player:

```text
BP_Player
|-- CapsuleComponent
|-- BodyFlipbook
|-- HatFlipbook
|-- SwordFlipbook
`-- SpriteSortComponent
```

Tree:

```text
BP_Tree
|-- SceneRoot
|-- TreeSprite
|-- Collision
`-- SpriteSortComponent
```

Tall grass:

```text
BP_TallGrass
|-- SceneRoot
|-- GroundPatchSprite
|-- GrassBladeSprite
`-- SpriteSortComponent
```

Fence or wall:

```text
BP_Fence
|-- SceneRoot
|-- FenceSprite
|-- Collision
`-- SpriteSortComponent
```

Optional advanced hierarchy:

```text
BP_Tree_Advanced
|-- SceneRoot
|-- Collision
|-- VisualRoot
|   `-- TreeSprite
|-- SortOriginTrunkBase
`-- SpriteSortComponent
```

Use `VisualRoot` or `Sprite Sort Origin` only when you want explicit art-directed control. They are not required for normal actors.

## How Sorting Works

The core formula is:

```cpp
SortKey = dot(feet/base world position, SortAxis)
SortKey += SortingLayer and OrderInLayer bias
SortKey -> shared camera-depth band
```

Only visuals move. Collision and gameplay transforms stay where they are for normal actors where visuals are children of the gameplay root.

This follows the same idea used by 2D engines:

- Unity: sorting layer/order first, then distance/custom-axis sorting.
- Godot: Y-sort compares the node's sort position, usually aligned to the base/feet.
- SpriteSort2D: sorting layer/order first, then `SortAxis`, using a shared masked-material-friendly depth band in Unreal.

## Important Settings

- `bSortAllVisualComponents`: default on. Sorts the actor's safe visual primitives together.
- `DepthPadding`: default `0.2`. Tiny smooth extra spacing on the render-depth axis.
- `OriginMode`: where the sort point comes from. Leave on `Auto` for most actors.
- `VisualComponentPivot`: uses the sprite/component pivot. This is useful when your PaperSprite origin is placed at the base of a pole, tree, prop, or character.
- `SortOffset`: optional feet/base nudge when automatic bounds are not quite right.
- `SortAxis`: world axis used to calculate sort order. Usually world Y for top-down Paper2D.
- `bUseCameraForwardDepthAxis`: default on. Uses the active player camera's forward vector for the render-depth push so sprite height/Z placement does not decide sorting.
- `CameraDepthAxis`: fallback world axis used when camera-forward depth is disabled or no player camera is available.
- `SortingLayer`: coarse layer. Higher values render in front.
- `OrderInLayer`: manual order inside the same layer. Higher values render in front.
- `SortBandDistance`: default `0`, meaning auto. Positive values force the distance from the active camera to the shared 2D sort band.
- `SortLayerDepthStep`: depth spacing between `SortingLayer` values.
- `OrderDepthStep`: depth spacing between `OrderInLayer` values.
- `DepthScale`: converts sort value into visual depth offset.
- `ForegroundDepthBias`: default `500`. Used by auto band distance to place sortable actors in front of their original art plane.
- `MovementThreshold`: minimum actor/origin movement before movement updates re-sort.
- `WhenMovedTickInterval`: timed movement check interval. Default is `0.1` seconds.
- `bInvertSort`: flips the sort result.
- `UpdateMode`: `SmartAuto`, `OnBeginPlayOnly`, `WhenMoved`, `EveryTick`, or `Manual`.
- `SortingMode`: `VisualDepthOffset`, `Manual`, or `TranslucentPriorityFallback`.
- `VisualRoot`: optional advanced override for the visual subtree to move.
- `TargetPrimitive`: optional primitive used by translucent fallback modes.
- `SortOrigin`: optional scene component at the feet/base for exact art direction.

Default update mode is `SmartAuto`.

Default sorting mode is `VisualDepthOffset`.

Default origin mode is `Auto`.

## Update Modes

`SmartAuto` is the default. It updates static props once, and uses movement-based timed updates for pawns or actors with movement components.

`OnBeginPlayOnly` caches the original transform and updates once. Use it for known static props.

`WhenMoved` checks movement on a timer, not every frame by default. It only updates when the actor or sort origin moves beyond `MovementThreshold`.

`EveryTick` updates every frame. Use it for prototypes or unusual camera/depth setups.

`Manual` never updates automatically. Call `UpdateSortNow()` yourself.

## Sort Origins

You do not need to create a sort origin for every actor.

In `Auto` mode, the sorter uses this order:

1. Assigned `SortOrigin`, if you made one.
2. First `Sprite Sort Origin` component on the actor with `bAutoUseAsSortOrigin` enabled.
3. Visual component pivot/origin.
4. Inferred base of visual bounds.
5. Actor location plus `SortOffset`.

If a sprite has unusual art, add a `Sprite Sort Origin` at the feet/base and assign it. Keep it outside any moving visual subtree.

## Auto-Find Behavior

With `bSortAllVisualComponents` enabled, Sprite Sort 2D collects all safe non-collision visual primitives on the actor.

If the actor has a `Sprite Sort Origin`, Auto mode uses it as the explicit sort line. This is the recommended setup for pillars, trees, fences, walls, and props that need hand tuning.

If `bSortAllVisualComponents` is disabled and `VisualRoot` is not assigned, Sprite Sort 2D tries:

1. A child scene component named `VisualRoot`.
2. A `PaperFlipbookComponent`.
3. A `PaperSpriteComponent`.
4. Any non-root primitive that does not look like collision.

Warnings are logged only when no useful visual component can be found.

## Material Notes

Sprite Sort 2D works best with masked materials. It does not require translucent materials and does not force sprites to be unlit.

Masked lit materials are compatible where your Paper2D/material setup supports them. Opaque materials can also benefit when the visual-depth offset is appropriate for the camera setup.

`TranslucentPriorityFallback` exists only for special cases. It is not the main system.

## Debugging

Enable `bDebugDraw` on the component to draw:

- Sort origin sphere/cross
- Line from original visual position to adjusted visual position
- Label with sort value, depth offset, update mode, and visual component count

## Troubleshooting

Sprite does not sort:

- Make sure the actor has at least one PaperSprite, PaperFlipbook, mesh, or primitive visual component.
- Leave `bSortAllVisualComponents` enabled unless you have a specific advanced setup.
- Check `DepthScale`.
- Check `CameraDepthAxis`.
- Check update mode.
- Turn on `bDebugDraw` to see the base point.

Player disappears behind the map:

- Do not add `Sprite Sort Component` to the map/ground unless it should sort with actors.
- Leave `bUseCameraForwardDepthAxis` enabled.
- Leave `SortBandDistance` at `0` for auto. If the map still competes, set a positive `SortBandDistance` that places sortable actors closer to the camera.
- For best results, keep floor/map sprites as background art and use SpriteSort2D only on actors that should sort against props, foliage, walls, NPCs, and the player.
- Existing actors made during the ground-bias test should reset removed values by re-adding the component or recompiling the Blueprint.

Player flickers/clips against a pillar:

- Make sure the pillar also has `Sprite Sort Component`.
- Set both player and pillar to the same `SortAxis`, camera-depth behavior, and `DepthScale`.
- Leave `bUseCameraForwardDepthAxis` enabled for most games. This prevents visible Z/height placement from controlling depth order.
- Keep `DepthScale` large enough to create real depth separation, but not so large that visuals visibly drift. Default is `0.01`.
- Add `Sprite Sort Origin` to the pillar and move it to the line where the player should switch behind/in front.
- Leave `OriginMode` on `Auto`; it will use the origin component first.

Sprite moves visually too far:

- Lower `DepthScale`.

Sorting is reversed:

- Enable `bInvertSort`, or flip `SortAxis`.

Collision feels wrong:

- Sprite Sort 2D should ignore normal collision components automatically.
- If using an advanced `VisualRoot`, make sure collision is not inside that visual subtree.

Player always appears in front:

- Check automatic base positions with `bDebugDraw`.
- Check that all sortable actors use the same sort axis and depth axis.

Sorting breaks when actors have different Z heights:

- Leave `bUseCameraForwardDepthAxis` enabled so the renderer is pushed along camera depth instead of visible height.
- Use small local visual offsets or `DepthPadding` for tiny layer nudges like props sitting above tiles.
- SpriteSort2D preserves local visual offsets and does not force actors to a fixed Z height.

Weapons, clothes, or hair do not follow the character sort:

- Keep `bSortAllVisualComponents` enabled.
- Put those visual pieces on the same actor as sibling or child visual components.
- Make sure they are not named like collision/hitbox components.

## Project Settings

Project-wide defaults are available under:

```text
Project Settings -> Plugins -> Sprite Sort 2D
```

The settings include default axes, camera-forward depth, depth scale, update mode, debug drawing, depth padding, and auto-find behavior.

## TileForge2D Notes

Sprite Sort 2D is intentionally small so it can later be integrated with TileForge2D.

Future TileForge2D usage can generate sortable actors for trees, grass, props, walls, doors, cliffs, and signs with:

- `SpriteSortComponent`
- optional `VisualRoot`
- optional `SpriteSortOrigin`
- sort metadata such as offset, sort mode, depth layer, collision behavior, and foliage overlay behavior

Tilemap and tileset integration is intentionally not implemented in this MVP.

## Release Files

- [CHANGELOG.md](./CHANGELOG.md)
- [QA.md](./QA.md)
- [TODO.md](./TODO.md)
- [LICENSE](./LICENSE)
