# 🧪 OpenClaw Unreal Plugin - Testing Guide

This document is the testing guide for OpenClaw Unreal Plugin. It includes usage and examples for all 40+ tools.

## Table of Contents

1. [Test Environment Setup](#test-environment-setup)
2. [Basic Connection Test](#basic-connection-test)
3. [Level Tools](#level-tools)
4. [Actor Tools](#actor-tools)
5. [Transform Tools](#transform-tools)
6. [Component Tools](#component-tools)
7. [Editor Tools](#editor-tools)
8. [Debug Tools](#debug-tools)
9. [Input Tools](#input-tools)
10. [Asset Tools](#asset-tools)
11. [Console Tools](#console-tools)
12. [Blueprint Tools](#blueprint-tools)
13. [Automation Test Scenarios](#automation-test-scenarios)

---

## Test Environment Setup

### 1. Start OpenClaw Gateway

```bash
# Check Gateway status
openclaw gateway status

# Start Gateway (if needed)
openclaw gateway start
```

### 2. Prepare Unreal Project

1. Install OpenClaw Unreal Plugin (see README.md)
2. Open your Unreal project
3. Check Output Log for `[OpenClaw] 🦞 Plugin starting...`
4. Verify connection via Window → OpenClaw Status

### 3. Verify Test Session

Check Output Log for:
```
LogOpenClaw: State changed to: Connected
LogOpenClaw: Session ID: unreal_1234567890_abc123
```

---

## Basic Connection Test

### Check Plugin Status

**Description:** Verify the plugin is running and connected.

**Method:** Window → OpenClaw Status

**Expected:** Dialog showing "Connected to OpenClaw Gateway"

---

## Level Tools

### level.getCurrent - Get Current Level

**Description:** Returns information about the current level.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "What level is currently open?"
```

**Response Example:**
```json
{
  "success": true,
  "name": "Main",
  "path": "/Game/Maps/Main.Main"
}
```

---

### level.list - List All Levels

**Description:** Returns a list of all levels in the project.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "Show me all levels in the project"
```

**Response Example:**
```json
{
  "success": true,
  "levels": [
    { "name": "Main", "path": "/Game/Maps/Main" },
    { "name": "Menu", "path": "/Game/Maps/Menu" },
    { "name": "TestLevel", "path": "/Game/Maps/TestLevel" }
  ]
}
```

---

### level.open - Open Level

**Description:** Opens a level by path.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| path | string | ✓ | Level asset path |

**Example:**
```
Ask OpenClaw: "Open the Main level"
```

**Response Example:**
```json
{
  "success": true,
  "message": "Opened level: /Game/Maps/Main"
}
```

---

### level.save - Save Current Level

**Description:** Saves the current level.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "Save the current level"
```

---

## Actor Tools

### actor.find - Find Actor

**Description:** Finds an actor by name or label.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name or label |

**Example:**
```
Ask OpenClaw: "Find the PlayerStart actor"
```

**Response Example:**
```json
{
  "success": true,
  "actor": {
    "name": "PlayerStart",
    "label": "PlayerStart",
    "class": "PlayerStart",
    "transform": {
      "position": { "x": 0, "y": 0, "z": 100 },
      "rotation": { "pitch": 0, "yaw": 0, "roll": 0 },
      "scale": { "x": 1, "y": 1, "z": 1 }
    },
    "components": [
      { "name": "CollisionComponent", "class": "CapsuleComponent" },
      { "name": "GoodSprite", "class": "BillboardComponent" }
    ]
  }
}
```

---

### actor.getAll - Get All Actors

**Description:** Returns all actors in the current level.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "List all actors in the level"
```

**Response Example:**
```json
{
  "success": true,
  "count": 15,
  "actors": [
    { "name": "PlayerStart", "class": "PlayerStart", "transform": {...} },
    { "name": "DirectionalLight", "class": "DirectionalLight", "transform": {...} },
    { "name": "Floor", "class": "StaticMeshActor", "transform": {...} }
  ]
}
```

---

### actor.create - Create Actor

**Description:** Spawns a new actor in the level.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| type | string | | Actor type (Cube, PointLight, Camera, etc.) |
| name | string | | Actor label |
| x | number | | X position |
| y | number | | Y position |
| z | number | | Z position |

**Example:**
```
Ask OpenClaw: "Create a cube at position 100, 200, 50"
```

**Response Example:**
```json
{
  "success": true,
  "message": "Created actor: Cube_1",
  "actor": {
    "name": "Cube_1",
    "class": "StaticMeshActor",
    "transform": {
      "position": { "x": 100, "y": 200, "z": 50 }
    }
  }
}
```

**Supported Types:**
- `Cube` / `StaticMeshActor` - Static mesh cube
- `PointLight` - Point light source
- `Camera` - Camera actor
- (default) - Empty actor

---

### actor.delete / actor.destroy - Delete Actor

**Description:** Removes an actor from the level.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |

**Example:**
```
Ask OpenClaw: "Delete the Cube_1 actor"
```

---

### actor.getData - Get Actor Details

**Description:** Returns detailed information about an actor including components.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |

**Example:**
```
Ask OpenClaw: "Get details about the PlayerStart actor"
```

---

## Transform Tools

### transform.getPosition - Get Position

**Description:** Returns the world position of an actor.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |

**Example:**
```
Ask OpenClaw: "Where is the PlayerStart located?"
```

**Response Example:**
```json
{
  "success": true,
  "x": 0,
  "y": 0,
  "z": 100
}
```

---

### transform.setPosition - Set Position

**Description:** Sets the world position of an actor.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |
| x | number | | X position |
| y | number | | Y position |
| z | number | | Z position |

**Example:**
```
Ask OpenClaw: "Move the PlayerStart to position 500, 0, 100"
```

---

### transform.getRotation / setRotation

**Description:** Get or set actor rotation (pitch, yaw, roll in degrees).

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |
| pitch | number | | Pitch angle (degrees) |
| yaw | number | | Yaw angle (degrees) |
| roll | number | | Roll angle (degrees) |

---

### transform.getScale / setScale

**Description:** Get or set actor scale.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| name | string | ✓ | Actor name |
| x | number | | X scale |
| y | number | | Y scale |
| z | number | | Z scale |

---

## Component Tools

### component.get - Get Components

**Description:** Lists components on an actor.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| actor | string | ✓ | Actor name |
| component | string | | Component name filter |

**Example:**
```
Ask OpenClaw: "What components does the PlayerStart have?"
```

**Response Example:**
```json
{
  "success": true,
  "components": [
    { "name": "CollisionComponent", "class": "CapsuleComponent" },
    { "name": "GoodSprite", "class": "BillboardComponent" },
    { "name": "ArrowComponent", "class": "ArrowComponent" }
  ]
}
```

---

## Editor Tools

### editor.play - Start PIE

**Description:** Starts Play in Editor.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "Start the game"
```

**Response Example:**
```json
{
  "success": true,
  "message": "Started play mode"
}
```

---

### editor.stop - Stop PIE

**Description:** Stops Play in Editor.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "Stop the game"
```

---

### editor.pause / resume

**Description:** Pause or resume game execution during PIE.

**Example:**
```
Ask OpenClaw: "Pause the game"
Ask OpenClaw: "Resume the game"
```

---

### editor.getState - Get Editor State

**Description:** Returns the current editor state.

**Parameters:** None

**Example:**
```
Ask OpenClaw: "Is the game running?"
```

**Response Example:**
```json
{
  "success": true,
  "isPlaying": true,
  "isPaused": false,
  "state": "playing"
}
```

**State Values:**
- `editing` - Normal editor mode
- `playing` - PIE active
- `paused` - PIE paused

---

## Debug Tools

### debug.hierarchy - Get Actor Hierarchy

**Description:** Returns a tree view of all actors in the level.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| depth | number | | Maximum depth to traverse |

**Example:**
```
Ask OpenClaw: "Show me the level hierarchy"
```

**Response Example:**
```json
{
  "success": true,
  "level": "Main",
  "actorCount": 15,
  "actors": [
    { "name": "PlayerStart", "class": "PlayerStart", ... },
    { "name": "Floor", "class": "StaticMeshActor", ... }
  ]
}
```

---

### debug.screenshot - Take Screenshot

**Description:** Captures a screenshot of the viewport.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| filename | string | | Custom filename |

**Example:**
```
Ask OpenClaw: "Take a screenshot"
```

**Response Example:**
```json
{
  "success": true,
  "path": "/Saved/Screenshots/screenshot_20260211_145000.png",
  "message": "Screenshot requested"
}
```

---

### debug.log - Log Message

**Description:** Outputs a message to the Unreal log.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| message | string | ✓ | Message to log |
| level | string | | Log level (log/warning/error) |

**Example:**
```
Ask OpenClaw: "Log 'Test message' as a warning"
```

---

## Input Tools

### input.simulateKey - Keyboard Input

**Description:** Simulates keyboard input.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| key | string | ✓ | Key name (W, A, S, D, Space, etc.) |
| pressed | boolean | | True for press, false for release |

**Example:**
```
Ask OpenClaw: "Press the W key"
```

---

### input.simulateMouse - Mouse Input

**Description:** Simulates mouse input.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| action | string | ✓ | Action (click/move/scroll) |
| x | number | | X coordinate |
| y | number | | Y coordinate |
| button | string | | Button (left/right/middle) |

---

### input.simulateAxis - Axis Input

**Description:** Simulates gamepad/axis input.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| axis | string | ✓ | Axis name |
| value | number | ✓ | Value (-1 to 1) |

---

## Asset Tools

### asset.list - List Assets

**Description:** Lists assets in a path.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| path | string | | Asset path (default: /Game) |
| type | string | | Asset type filter |

**Example:**
```
Ask OpenClaw: "List all assets in /Game/Blueprints"
```

---

## Console Tools

### console.execute - Execute Command

**Description:** Executes an Unreal console command.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| command | string | ✓ | Console command |

**Example:**
```
Ask OpenClaw: "Run the 'stat fps' command"
```

---

## Blueprint Tools

### blueprint.list - List Blueprints

**Description:** Lists all blueprints in a path.

**Parameters:**
| Name | Type | Required | Description |
|------|------|----------|-------------|
| path | string | | Path to search (default: /Game) |

**Example:**
```
Ask OpenClaw: "Show me all blueprints"
```

---

## Automation Test Scenarios

### Scenario 1: Level Setup

```
1. "Open the TestLevel"
2. "Create a cube at 0, 0, 100"
3. "Create a point light at 0, 0, 300"
4. "Save the level"
5. "Take a screenshot"
```

### Scenario 2: Actor Manipulation

```
1. "List all actors in the level"
2. "Find the Cube_1"
3. "Move Cube_1 to position 200, 0, 100"
4. "Rotate Cube_1 by 45 degrees on yaw"
5. "Scale Cube_1 to 2, 2, 2"
```

### Scenario 3: PIE Testing

```
1. "Start the game"
2. "Wait 2 seconds"
3. "Take a screenshot"
4. "Pause the game"
5. "Get the editor state"
6. "Stop the game"
```

---

## Troubleshooting

### Tool returns "Actor not found"

- Check exact actor name/label in World Outliner
- Names are case-sensitive
- During PIE, editor actors may not be accessible

### Screenshot not saving

- Check Saved/Screenshots directory exists
- Verify write permissions
- Check Output Log for errors

### Input simulation not working

- Input simulation may require PIE to be active
- Some input may need focus on game viewport
- Check if Enhanced Input is blocking simulated input
