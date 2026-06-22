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
- [ ] Actor with explicit `VisualRoot` sorts correctly.
- [ ] Actor with explicit `Sprite Sort Origin` overrides automatic origin.
- [ ] Actor without `VisualRoot` logs a clear warning when auto-find fails.
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
- [ ] `bInvertSort` reverses order.
- [ ] Lowering `DepthScale` reduces visual movement.

## Update Modes

- [ ] `OnBeginPlayOnly` updates once and does not keep ticking.
- [ ] `WhenMoved` updates only after movement exceeds `MovementThreshold`.
- [ ] `WhenMovedTickInterval` throttles movement checks when set above zero.
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
