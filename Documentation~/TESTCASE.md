# OpenClaw Unreal Plugin — Test Cases (v1.2.1)

36 tools + Editor Panel UI tests.

Legend: ✅ = verified working, 🔧 = newly implemented/fixed, 🚧 = not yet implemented, ⚠️ = partial/caveats

---

## Level (4 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 1 | `level.getCurrent` | ✅ | Returns current level name |
| 2 | `level.list` | ✅ | Lists all .umap files |
| 3 | `level.open` | ⚠️ | Opens level by name |
| 4 | `level.save` | ⚠️ | Saves current level |

## Actor (6 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 5 | `actor.find` | ✅ | Find by name or class |
| 6 | `actor.getAll` | ✅ | Returns all actors in level |
| 7 | `actor.create` | ✅ | Types: StaticMeshActor/Cube, Sphere, Cylinder, Cone, PointLight, Camera |
| 8 | `actor.delete` | ✅ | Delete by actor name |
| 9 | `actor.getData` | ✅ | Returns detailed actor properties |
| 10 | `actor.setProperty` | 🔧 | Newly implemented via UE reflection system |

## Transform (6 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 11 | `transform.getPosition` | ⚠️ | Requires RootComponent — works on StaticMeshActor, not bare Actor |
| 12 | `transform.setPosition` | ⚠️ | Same RootComponent requirement |
| 13 | `transform.getRotation` | ⚠️ | Same RootComponent requirement |
| 14 | `transform.setRotation` | ⚠️ | Same RootComponent requirement |
| 15 | `transform.getScale` | ⚠️ | Same RootComponent requirement |
| 16 | `transform.setScale` | ⚠️ | Same RootComponent requirement |

> **Note:** All transform tools work on actors with a RootComponent (StaticMeshActor, PointLight, Camera, etc.) but fail on generic Actor which has no RootComponent.

## Component (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 17 | `component.get` | ⚠️ | Fixed parameter handling |
| 18 | `component.add` | 🚧 | Not yet implemented |
| 19 | `component.remove` | 🚧 | Not yet implemented |

## Editor (5 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 20 | `editor.play` | 🔧 | Fixed — now uses `RequestPlaySession` instead of direct play |
| 21 | `editor.stop` | ✅ | Stops PIE session |
| 22 | `editor.pause` | ⚠️ | Pauses PIE |
| 23 | `editor.resume` | ⚠️ | Resumes PIE |
| 24 | `editor.getState` | ✅ | Returns editor/PIE state |

## Debug (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 25 | `debug.hierarchy` | ✅ | Actor hierarchy tree |
| 26 | `debug.screenshot` | ✅ | Captures editor viewport |
| 27 | `debug.log` | ⚠️ | Writes to UE output log |

## Input (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 28 | `input.simulateKey` | 🚧 | TODO stub |
| 29 | `input.simulateMouse` | 🚧 | TODO stub |
| 30 | `input.simulateAxis` | 🚧 | TODO stub |

## Asset (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 31 | `asset.list` | ✅ | Lists assets at content path |
| 32 | `asset.import` | 🚧 | Not yet implemented |

## Console (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 33 | `console.execute` | ✅ | Executes UE console commands |
| 34 | `console.getLogs` | 🔧 | Newly implemented — reads project log file, supports `count` and `filter` params |

## Blueprint (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 35 | `blueprint.list` | ✅ | Lists blueprint assets |
| 36 | `blueprint.open` | 🚧 | Not yet implemented |

---

## Editor Panel UI Tests

| Test | Expected |
|------|----------|
| Window → OpenClaw Unreal Plugin | Panel opens as dockable tab |
| Status display | Shows connection state (connected/disconnected) |
| Connect button | Initiates MCP connection |
| Disconnect button | Closes MCP connection |
| MCP section | Displays server address and protocol info |
| Log entries | Shows scrollable log of tool calls and messages |

---

## Summary

| Category | Total | ✅ Working | 🔧 New/Fixed | ⚠️ Caveats | 🚧 Not Implemented |
|----------|-------|-----------|--------------|------------|-------------------|
| Level | 4 | 2 | 0 | 2 | 0 |
| Actor | 6 | 5 | 1 | 0 | 0 |
| Transform | 6 | 0 | 0 | 6 | 0 |
| Component | 3 | 0 | 0 | 1 | 2 |
| Editor | 5 | 2 | 1 | 2 | 0 |
| Debug | 3 | 2 | 0 | 1 | 0 |
| Input | 3 | 0 | 0 | 0 | 3 |
| Asset | 2 | 1 | 0 | 0 | 1 |
| Console | 2 | 1 | 1 | 0 | 0 |
| Blueprint | 2 | 1 | 0 | 0 | 1 |
| **Total** | **36** | **14** | **3** | **12** | **7** |
