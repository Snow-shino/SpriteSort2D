# SpriteSort2D TODO

This list is intentionally scoped. SpriteSort2D should stay a focused sorting plugin, not become a full 2D framework.

## Before Public Release

- [ ] Create a small demo project or demo map with player, tree, tall grass, fence, static prop, and moving NPC.
- [ ] Capture before/after screenshots or a short GIF.
- [ ] Verify UE 5.6 builds.
- [ ] Test in a clean Blueprint-only UE project.
- [ ] QA the one-component workflow on player, NPC, tree, grass, fence, and equipment-heavy character Blueprints.
- [ ] Confirm default axes for the target camera style used in the demo.
- [ ] Decide whether public repo name should be `SpriteSort2D` or `UE-SpriteSort2D`.

## Short-Term Improvements

- [ ] Add automated runtime sanity tests for root/collision transform preservation.
- [ ] Add a sample Blueprint actor hierarchy screenshot to docs.
- [ ] Add optional console variables for global debug drawing.
- [ ] Add example masked unlit and masked lit materials.

## Later, Only If Needed

- [ ] Add batch manager for projects with thousands of moving sortable actors.
- [ ] Add optional TileForge2D metadata adapter after TileForge2D integration is ready.
- [ ] Add editor visualization icons for sort origins.
- [ ] Add per-camera presets for common Paper2D top-down/isometric setups.

## Not Planned For MVP

- Tilemap editing.
- Aseprite import.
- Replacing Paper2D.
- Requiring translucent materials.
- Moving gameplay collision to fake visual order.
