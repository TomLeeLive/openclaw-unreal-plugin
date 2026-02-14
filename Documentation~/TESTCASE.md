# OpenClaw Unreal Plugin — Test Cases (v1.2.1)

36 tools + Editor Panel UI + MCP Connection tests.

Legend: ✅ = passed, 🔧 = newly implemented/fixed, 🚧 = not yet implemented

**Test Date:** 2026-02-14  
**UE Version:** 5.7  
**Platform:** macOS arm64

---

## Level (4 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 1 | `level.getCurrent` | ✅ | Returns `Lvl_FirstPerson` |
| 2 | `level.list` | ✅ | 14 levels found |
| 3 | `level.open` | ✅ | Opens by path `/Game/FirstPerson/Lvl_FirstPerson` |
| 4 | `level.save` | ✅ | Saves current level |

## Actor (6 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 5 | `actor.find` | ✅ | Find by label (TestCube1) |
| 6 | `actor.getAll` | ✅ | 71 actors returned |
| 7 | `actor.create` | ✅ | Tested: Cube, Sphere, PointLight — all with position |
| 8 | `actor.delete` | ✅ | Deleted TestLight1 |
| 9 | `actor.getData` | ✅ | Returns transform + components |
| 10 | `actor.setProperty` | ✅🔧 | `bHidden=true` via UE reflection (FindPropertyByName + ImportText) |

## Transform (6 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 11 | `transform.getPosition` | ✅ | Verified (100, 200, 600) after set |
| 12 | `transform.setPosition` | ✅ | Set to (100, 200, 600) |
| 13 | `transform.getRotation` | ✅ | Verified yaw≈45 after set |
| 14 | `transform.setRotation` | ✅ | Set yaw=45 |
| 15 | `transform.getScale` | ✅ | Verified (2, 2, 2) after set |
| 16 | `transform.setScale` | ✅ | Set to (2, 2, 2) |

> **Note:** All transform tools require RootComponent. Works on StaticMeshActor (default for actor.create), PointLight, Camera, etc. Fails silently on bare AActor.

## Component (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 17 | `component.get` | ✅ | Returns StaticMeshComponent0 — accepts both `name` and `actor` params |
| 18 | `component.add` | 🚧 | Not yet implemented |
| 19 | `component.remove` | 🚧 | Not yet implemented |

## Editor (5 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 20 | `editor.play` | ✅🔧 | Uses `RequestPlaySession` (UE 5.7 compat) |
| 21 | `editor.stop` | ✅🔧 | Uses `RequestEndPlayMap` — deferred teardown to avoid TaskGraph crash |
| 22 | `editor.pause` | ✅ | State correctly reports `paused` |
| 23 | `editor.resume` | ✅ | Resumes from paused state |
| 24 | `editor.getState` | ✅ | Returns `editing` / `playing` / `paused` |

## Debug (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 25 | `debug.hierarchy` | ✅ | 71 actors with full transform data |
| 26 | `debug.screenshot` | ✅ | Saves to `Saved/Screenshots/` |
| 27 | `debug.log` | ✅ | Writes to UE output log |

## Input (3 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 28 | `input.simulateKey` | ✅ | W key press confirmed during PIE |
| 29 | `input.simulateMouse` | 🚧 | Not yet implemented |
| 30 | `input.simulateAxis` | 🚧 | Not yet implemented |

## Asset (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 31 | `asset.list` | ✅ | 241 assets at `/Game` |
| 32 | `asset.import` | 🚧 | Not yet implemented |

## Console (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 33 | `console.execute` | ✅ | `stat fps` executed |
| 34 | `console.getLogs` | ✅🔧 | filter=OpenClaw returned 4 entries, count param works |

## Blueprint (2 tools)

| # | Tool | Status | Notes |
|---|------|--------|-------|
| 35 | `blueprint.list` | ✅ | 16 blueprints found |
| 36 | `blueprint.open` | 🚧 | Not yet implemented |

---

## Editor Panel UI Tests

| # | Test | Status | Notes |
|---|------|--------|-------|
| P-01 | Window → OpenClaw Unreal Plugin | ✅ | Panel opens as dockable NomadTab |
| P-02 | Status display | ✅ | Shows Connected/Disconnected |
| P-03 | Connect/Disconnect buttons | ✅ | Functional |
| P-04 | MCP section visible | ✅ | Shows Host, Port, Protocol |
| P-05 | Log entries | ✅ | Scrollable tool call log with timestamps |
| P-06 | Plugin name in menu | ✅ | "OpenClaw Unreal Plugin" in Window menu |

---

## MCP Connection Tests

### Mode A: Gateway (Remote Access)

```
Telegram/Discord/Web → OpenClaw Gateway → HTTP Polling → Unreal Plugin
```

| # | Test | Status | Notes |
|---|------|--------|-------|
| M-01 | Plugin connects to Gateway on startup | ✅ | Auto-connect to localhost:18789 |
| M-02 | Session registration | ✅ | POST /unreal/register successful |
| M-03 | Heartbeat keepalive | ✅ | POST /unreal/heartbeat periodic |
| M-04 | Tool command polling | ✅ | GET /unreal/poll receives commands |
| M-05 | Tool result submission | ✅ | POST /unreal/result sends back results |
| M-06 | All 36 tools via Gateway | ✅ | 29 implemented tools all pass through Gateway |
| M-07 | Reconnect after PIE stop | ✅ | Plugin auto-reconnects after editor.stop |
| M-08 | Panel shows MCP info | ✅ | Host: localhost, Port: 42424, Protocol: HTTP Polling (SSE) |
| M-09 | Session status via /unreal/status | ✅ | Returns connected session info |

### Mode B: MCP Direct (Local Development)

```
Claude Code / Cursor → MCP Server → Unreal Plugin
```

| # | Test | Status | Notes |
|---|------|--------|-------|
| M-10 | MCP server starts with plugin | ⏳ | Requires MCP server binary/script |
| M-11 | Claude Code connects via stdio | ⏳ | Requires Claude Code MCP config |
| M-12 | Tool execution via MCP direct | ⏳ | Same tools, direct path |
| M-13 | Cursor IDE integration | ⏳ | Cursor MCP settings |
| M-14 | Concurrent Gateway + MCP Direct | ⏳ | Both modes active simultaneously |

> **Mode B tests pending:** MCP Direct mode requires separate MCP server setup. Currently all tool testing uses Mode A (Gateway).

---

## Bug Fixes Verified

| Issue | Fix | Verified |
|-------|-----|----------|
| editor.play broken (UE 5.7) | `RequestPlaySession()` | ✅ |
| editor.stop crash (TaskGraph assertion) | `RequestEndPlayMap()` deferred teardown | ✅ |
| Transform fails on dynamic actors | Default to `StaticMeshActor` in actor.create | ✅ |
| component.get param handling | Accept both `name` and `actor` params | ✅ |
| PIE viewport param (UE 5.7) | Removed deprecated parameter | ✅ |

---

## Summary

| Category | Total | ✅ Passed | 🚧 Not Impl |
|----------|-------|----------|-------------|
| Level | 4 | 4 | 0 |
| Actor | 6 | 6 | 0 |
| Transform | 6 | 6 | 0 |
| Component | 3 | 1 | 2 |
| Editor | 5 | 5 | 0 |
| Debug | 3 | 3 | 0 |
| Input | 3 | 1 | 2 |
| Asset | 2 | 1 | 1 |
| Console | 2 | 2 | 0 |
| Blueprint | 2 | 1 | 1 |
| Panel UI | 6 | 6 | 0 |
| MCP Mode A | 9 | 9 | 0 |
| MCP Mode B | 5 | 0 | 5 |
| **Total** | **56** | **45** | **11** |

**Pass Rate: 45/45 (implemented) = 100%**
