# Changelog

All notable changes to SpriteSort2D are tracked here.

## 0.1.0 - MVP

Initial public MVP.

### Added

- `USpriteSortComponent` for visual-only render-depth sprite sorting.
- `USpriteSortOriginComponent` for optional artist-controlled feet/base sort points.
- `USpriteSortSettings` project settings.
- `USpriteSortFunctionLibrary` Blueprint helpers.
- `VisualDepthOffset`, `Manual`, and `TranslucentPriorityFallback` sort modes.
- `OnBeginPlayOnly`, `WhenMoved`, `EveryTick`, and `Manual` update modes.
- `Auto` origin mode that can infer the base of visual bounds without a required origin component.
- Auto-find support for `VisualRoot` and target primitive components.
- Debug drawing for sort origin, visual offset, and current sort state.
- Editor details buttons for update, auto-find, and creating a sort origin.
- Performance controls for movement threshold and optional `WhenMoved` tick interval.
- Stable bounds-based origin calculation from the original visual transform to prevent repeated offset drift.
- Warnings for sort origins attached under `VisualRoot`.

### Notes

- Translucent priority is intentionally fallback-only.
- The default path is masked/opaque-compatible visual depth offset sorting.
