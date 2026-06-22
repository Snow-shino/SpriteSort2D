# Sprite Sort 2D

Sprite Sort 2D is a small Unreal Engine 5.5+ plugin for top-down 2D games that need reliable sprite sorting without making translucent materials the default solution.

It sorts by a character or prop's feet/base position, then moves only the actor's visual components along a render-depth axis. The actor root, collision, gameplay location, AI location, saved transform, traces, and navigation stay untouched.

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
- Sorts sibling visuals together, including equipment/clothing flipbooks.
- Ignores likely collision components.
- Infers a feet/base point from visual bounds.
- Uses `Smart Auto` updates.
- Preserves each visual component's original local offset.

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
Feet/base world position
-> projected onto SortAxis
-> multiplied by DepthScale
-> applied to visuals along CameraDepthAxis
```

Only visuals move. Collision and gameplay transforms stay where they are.

## Important Settings

- `bSortAllVisualComponents`: default on. Sorts the actor's safe visual primitives together.
- `DepthPadding`: default `0.2`. Tiny smooth extra spacing on the render-depth axis.
- `OriginMode`: where the sort point comes from. Leave on `Auto` for most actors.
- `SortOffset`: optional feet/base nudge when automatic bounds are not quite right.
- `SortAxis`: world axis used to calculate sort order. Usually world Y for top-down Paper2D.
- `CameraDepthAxis`: world axis used to move visuals for depth-buffer sorting. Usually world Z in this setup.
- `DepthScale`: converts sort value into visual depth offset.
- `MovementThreshold`: minimum actor/origin movement before movement updates re-sort.
- `WhenMovedTickInterval`: timed movement check interval. Default is `0.05` seconds.
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

`WhenMoved` updates only when the actor or sort origin moves beyond `MovementThreshold`.

`EveryTick` updates every frame. Use it for prototypes or unusual camera/depth setups.

`Manual` never updates automatically. Call `UpdateSortNow()` yourself.

## Sort Origins

You do not need to create a sort origin for every actor.

In `Auto` mode, the sorter uses this order:

1. Assigned `SortOrigin`, if you made one.
2. Inferred base of visual bounds.
3. Actor location plus `SortOffset`.

If a sprite has unusual art, add a `Sprite Sort Origin` at the feet/base and assign it. Keep it outside any moving visual subtree.

## Auto-Find Behavior

With `bSortAllVisualComponents` enabled, Sprite Sort 2D collects all safe non-collision visual primitives on the actor.

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

- The map/ground sprite is still writing to the depth buffer even without `Sprite Sort Component`.
- Put the map/ground sprite on a background render/depth layer, or use a ground material that does not compete with character depth.
- SpriteSort2D intentionally does not force characters to a fake Z height, because that causes floating and bad offsets.
- Existing actors made during the ground-bias test should reset removed values by re-adding the component or recompiling the Blueprint.

Player flickers/clips against a pillar:

- Make sure the pillar also has `Sprite Sort Component`.
- Set both player and pillar to the same `SortAxis`, `CameraDepthAxis`, and `DepthScale`.
- Keep `DepthScale` large enough to create real depth separation, but not so large that visuals visibly drift. Default is `0.01`.
- If the pillar art has a weird base, add a `Sprite Sort Origin` only for that pillar.

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

- Keep gameplay actors near a consistent 2D plane when using depth-buffer sorting.
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

The settings include default axes, depth scale, update mode, debug drawing, depth padding, actor-depth flattening, and auto-find behavior.

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
