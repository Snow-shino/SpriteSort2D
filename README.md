# Sprite Sort 2D

Sprite Sort 2D is a small Unreal Engine 5.5+ plugin for top-down 2D games that need reliable sprite sorting without making translucent materials the default solution.

It sorts by a character or prop's feet/base position, then moves only the assigned visual component along a render-depth axis. The actor root, collision, gameplay location, AI location, saved transform, traces, and navigation stay untouched.

## What It Is

Sprite Sort 2D is a masked-material-compatible render-depth sorting plugin for Paper2D sprites, Paper2D flipbooks, and generic primitive visual components.

It works with:

- Paper2D sprites
- Paper2D flipbooks
- Generic scene or primitive visual components
- Masked unlit materials
- Masked lit materials
- Opaque materials where the render-depth offset makes sense

## What It Is Not

Sprite Sort 2D is not just a translucent priority wrapper.

The default mode does not require translucent materials and does not rely on `Translucent Sort Priority` as the main sorting strategy. A translucent fallback exists only for special cases.

## Basic Setup

Fast path:

1. Add `Sprite Sort Component` to the actor.
2. Press Play.

By default the component auto-finds a visual component, infers a base point from the visual bounds, and updates when the actor moves.

Production setup:

1. Add `Sprite Sort Component`.
2. Create or assign a child scene component named `VisualRoot`.
3. Put only visual components under `VisualRoot`.
4. Leave `OriginMode` on `Auto`, or add `Sprite Sort Origin` at the feet/base for exact art-directed sorting.
5. Choose a shared `SortAxis`, usually world Y for top-down Paper2D.
6. Choose `CameraDepthAxis`, usually the axis your camera/depth buffer should sort on.
7. Press Play.

The core formula is:

```cpp
SortOrigin world position
-> projected onto SortAxis
-> multiplied by DepthScale
-> applied to VisualRoot along CameraDepthAxis
```

Only `VisualRoot` moves visually. Collision and gameplay transforms stay where they are.

## Recommended Actor Hierarchies

Player:

```text
BP_Player
|-- CapsuleComponent
|-- VisualRoot
|   `-- PaperFlipbookComponent
|-- SortOriginFeet
`-- SpriteSortComponent
```

Tree:

```text
BP_Tree
|-- SceneRoot
|-- VisualRoot
|   `-- PaperSpriteComponent
|-- SortOriginTrunkBase
`-- SpriteSortComponent
```

Tall grass:

```text
BP_TallGrass
|-- SceneRoot
|-- GroundPatchSprite
|-- VisualRoot
|   `-- GrassBladeSprite
|-- SortOriginGrassBase
`-- SpriteSortComponent
```

Fence or wall:

```text
BP_Fence
|-- SceneRoot
|-- Collision
|-- VisualRoot
|   `-- FenceSprite
|-- SortOriginFenceBase
`-- SpriteSortComponent
```

Collision should not live inside `VisualRoot`, because `VisualRoot` is the part that receives the render-depth offset.

`Sprite Sort Origin` should also live outside `VisualRoot`. If the origin is attached under `VisualRoot`, the visual offset can move the origin too, which makes sorting harder to reason about.

## Components

### Sprite Sort Component

Main actor component.

Important settings:

- `VisualRoot`: the visual scene component to move.
- `TargetPrimitive`: primitive used by fallback modes.
- `SortOrigin`: optional scene component at the feet/base.
- `SortOffset`: fallback offset from actor location when no origin is assigned.
- `OriginMode`: where the sort point comes from.
- `BoundsBaseAxis`: axis used to infer the base of visual bounds, usually world Z.
- `SortAxis`: world axis used to calculate sort order.
- `CameraDepthAxis`: world axis used to move the visual.
- `DepthScale`: converts sort value into visual depth offset.
- `MovementThreshold`: minimum actor/origin movement before `WhenMoved` updates.
- `WhenMovedTickInterval`: optional throttle interval for large maps.
- `bInvertSort`: flips the sort result.
- `UpdateMode`: `OnBeginPlayOnly`, `WhenMoved`, `EveryTick`, or `Manual`.
- `SortingMode`: `VisualDepthOffset`, `Manual`, or `TranslucentPriorityFallback`.

Default update mode is `WhenMoved`.

Default sorting mode is `VisualDepthOffset`.

Default origin mode is `Auto`.

Bounds-based origin modes are calculated from the original visual transform before applying the new visual depth offset. This prevents repeated updates from drifting the inferred base point.

### Sprite Sort Origin

A simple scene component artists can place at the sprite's feet/base.

You do not have to create one for every actor. In `Auto` mode the sorter uses this order:

1. Assigned `SortOrigin`.
2. Inferred base of `VisualRoot` bounds.
3. Inferred base of `TargetPrimitive` bounds.
4. Actor location plus `SortOffset`.

If `OriginMode` is `ActorLocationPlusOffset`, the sorter uses:

```cpp
OwnerActorLocation + SortOffset
```

### Project Settings

Project-wide defaults are available under:

```text
Project Settings -> Plugins -> Sprite Sort 2D
```

The settings include default axes, depth scale, update mode, debug drawing, and auto-find behavior.

## Auto-Find Behavior

If `VisualRoot` is not assigned, Sprite Sort 2D tries:

1. A child scene component named `VisualRoot`.
2. A `PaperFlipbookComponent`.
3. A `PaperSpriteComponent`.
4. Any non-root primitive that does not look like collision.

If `TargetPrimitive` is not assigned, it tries:

1. First `PaperFlipbookComponent`.
2. First `PaperSpriteComponent`.
3. First primitive under `VisualRoot`.
4. Any primitive that does not look like collision.

Warnings are logged when no useful component can be found.

## Update Modes

`OnBeginPlayOnly` caches the original transform and updates once. Use it for static props.

`WhenMoved` updates only when the actor or sort origin moves beyond a tiny threshold. This is the default.

For large games, increase `MovementThreshold` or set `WhenMovedTickInterval` above zero to reduce how often movement checks run.

`EveryTick` updates every frame. Use it for prototypes or unusual camera/depth setups.

`Manual` never updates automatically. Call `UpdateSortNow()` yourself.

## Material Notes

Sprite Sort 2D works best with masked materials. It does not require translucent materials and does not force sprites to be unlit.

Masked lit materials are compatible where your Paper2D/material setup supports them. Opaque materials can also benefit when the visual-depth offset is appropriate for the camera setup.

`TranslucentPriorityFallback` exists only for special cases. It is not the main system.

## Debugging

Enable `bDebugDraw` on the component to draw:

- Sort origin sphere/cross
- Line from original visual position to adjusted visual position
- Label with sort value, depth offset, update mode, and visual root name

## Troubleshooting

Sprite does not sort:

- Check `VisualRoot`.
- Check `SortOrigin` or `SortOffset`.
- Check `DepthScale`.
- Check `CameraDepthAxis`.
- Check update mode.

Sprite moves visually too far:

- Lower `DepthScale`.

Sorting is reversed:

- Enable `bInvertSort`, or flip `SortAxis`.

Collision feels wrong:

- Make sure collision is not inside `VisualRoot`.
- Only visuals should be moved.

Player always appears in front:

- Check sort origin positions.
- Check that all sortable actors use the same sort axis and depth axis.

## TileForge2D Notes

Sprite Sort 2D is intentionally small so it can later be integrated with TileForge2D.

Future TileForge2D usage can generate sortable actors for trees, grass, props, walls, doors, cliffs, and signs with:

- `VisualRoot`
- `SpriteSortOrigin`
- `SpriteSortComponent`
- sort metadata such as offset, sort mode, depth layer, collision behavior, and foliage overlay behavior

Tilemap and tileset integration is intentionally not implemented in this MVP.

## Release Files

- [CHANGELOG.md](./CHANGELOG.md)
- [QA.md](./QA.md)
- [TODO.md](./TODO.md)
- [LICENSE](./LICENSE)
