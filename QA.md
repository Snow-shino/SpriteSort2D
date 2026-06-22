# SpriteSort2D QA Checklist

Use this checklist before public release and after meaningful runtime changes.

## Engine Matrix

- [ ] UE 5.5 Editor Win64 builds.
- [ ] UE 5.5 Game Development Win64 builds.
- [ ] UE 5.5 Game Shipping Win64 builds.
- [ ] UE 5.6 Editor Win64 builds.
- [ ] UE 5.6 Game Development Win64 builds.
- [ ] UE 5.6 Game Shipping Win64 builds.

## Actor Setup

- [ ] PaperSprite actor sorts with only `Sprite Sort Component` added.
- [ ] PaperFlipbook actor sorts with only `Sprite Sort Component` added.
- [ ] Actor sorts with `VisualRoot`, `TargetPrimitive`, and `SortOrigin` left empty.
- [ ] Actor with sibling clothing/weapon flipbooks sorts all visuals together.
- [ ] Actor with explicit `VisualRoot` sorts correctly.
- [ ] Actor with explicit `Sprite Sort Origin` overrides automatic origin.
- [ ] Actor without `VisualRoot` does not warn when `bSortAllVisualComponents` is enabled.
- [ ] Actor with no safe visual primitives logs a clear warning.
- [ ] Actor with zero `SortAxis` logs a clear warning.
- [ ] Actor with zero `CameraDepthAxis` logs a clear warning.
- [ ] Actor with collision under `VisualRoot` logs a clear warning.
- [ ] Actor with `SortOrigin` under `VisualRoot` logs a clear warning.

## Sorting Behavior

- [ ] Player below a tree renders in front.
- [ ] Player behind a tree renders behind.
- [ ] Player crosses a fence or wall edge correctly.
- [ ] Tall grass overlay can render over the player when its sort origin/base requires it.
- [ ] Moving NPC sorts against static props.
- [ ] Two moving actors crossing each other remain visually stable.
- [ ] Actor placed higher on world Z still sorts by top-down base position when `bIgnoreActorDepth` is enabled.
- [ ] Small local visual offsets, such as `0.2` above a tile, are preserved.
- [ ] Moving player does not disappear behind an unsorted ground/map sprite.
- [ ] Pillar/player overlap does not flicker when both actors use matching Sprite Sort settings.
- [ ] `DepthSnapInterval` reduces flicker when sort depths are very close.
- [ ] `GroundDepthBias` can be raised for maps placed above the default ground plane.
- [ ] `bInvertSort` reverses order.
- [ ] Lowering `DepthScale` reduces visual movement.

## Update Modes

- [ ] `OnBeginPlayOnly` updates once and does not keep ticking.
- [ ] `SmartAuto` updates static props once.
- [ ] `SmartAuto` uses movement checks for pawns or actors with movement components.
- [ ] `WhenMoved` updates only after movement exceeds `MovementThreshold`.
- [ ] `WhenMovedTickInterval` throttles movement checks.
- [ ] `EveryTick` updates each frame.
- [ ] `Manual` does not update until `UpdateSortNow()` is called.

## Gameplay Safety

- [ ] Actor root world transform does not change after sorting.
- [ ] Collision component world transform does not change after sorting.
- [ ] Gameplay traces still hit collision, not the visual offset.
- [ ] Disabling sorting restores the original visual transform.
- [ ] EndPlay restores the original visual transform.
- [ ] `OriginMode: Auto` does not drift after repeated movement updates.

## Materials

- [ ] Masked unlit sprite material works.
- [ ] Masked lit sprite material works where the project setup supports it.
- [ ] Opaque material works where the camera/depth setup supports it.
- [ ] Translucent fallback changes only `TargetPrimitive` translucent sort priority.

## Scale Smoke Tests

- [ ] 100 static props using `OnBeginPlayOnly`.
- [ ] 100 moving actors using `WhenMoved`.
- [ ] 500 mixed simple sortable props with debug drawing disabled.
- [ ] 50 actors with debug drawing enabled for inspection only.

## Release Checks

- [ ] README setup matches current component names.
- [ ] Changelog updated.
- [ ] Version in `.uplugin` updated when releasing.
- [ ] Demo screenshots or short GIF recorded.
- [ ] Packaged plugin zip installs into a clean UE project.
