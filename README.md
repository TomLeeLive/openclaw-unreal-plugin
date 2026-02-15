# OpenClaw Unreal Plugin

> MCP (Model Context Protocol) bridge for Unreal Engine — lets AI agents control the Unreal Editor in real time.

**Version 1.3.0**

## Features

- **36 MCP tools** for level, actor, transform, component, editor, debug, input, asset, console, and blueprint control
- **Editor Panel** — Window → OpenClaw Unreal Plugin: a dockable tab showing connection status, MCP info, connect/disconnect buttons, and a live log
- **Real-time control** of the Unreal Editor from any MCP-compatible AI agent
- **Zero config** — install the plugin, connect OpenClaw, and go

## Installation

1. Copy the `OpenClaw` folder into your project's `Plugins/` directory
2. Restart the Unreal Editor
3. Enable the plugin in Edit → Plugins if not already enabled
4. Open the panel: **Window → OpenClaw Unreal Plugin**

## Tools (36)

### Level
| Tool | Description |
|------|-------------|
| `level.getCurrent` | Get the current level name |
| `level.list` | List all levels in the project |
| `level.open` | Open a level by name |
| `level.save` | Save the current level |

### Actor
| Tool | Description |
|------|-------------|
| `actor.find` | Find actors by name or class |
| `actor.getAll` | Get all actors in the level |
| `actor.create` | Create an actor (StaticMeshActor/Cube, Sphere, Cylinder, Cone, PointLight, Camera) |
| `actor.delete` | Delete an actor by name |
| `actor.getData` | Get detailed actor data |
| `actor.setProperty` | Set actor properties via UE reflection system |

### Transform
| Tool | Description |
|------|-------------|
| `transform.getPosition` | Get actor position |
| `transform.setPosition` | Set actor position |
| `transform.getRotation` | Get actor rotation |
| `transform.setRotation` | Set actor rotation |
| `transform.getScale` | Get actor scale |
| `transform.setScale` | Set actor scale |

### Component
| Tool | Description |
|------|-------------|
| `component.get` | Get component data from an actor |
| `component.add` | Add a component to an actor |
| `component.remove` | Remove a component from an actor |

### Editor
| Tool | Description |
|------|-------------|
| `editor.play` | Start Play-In-Editor (uses RequestPlaySession) |
| `editor.stop` | Stop Play-In-Editor |
| `editor.pause` | Pause Play-In-Editor |
| `editor.resume` | Resume Play-In-Editor |
| `editor.getState` | Get current editor state |

### Debug
| Tool | Description |
|------|-------------|
| `debug.hierarchy` | Get the actor hierarchy tree |
| `debug.screenshot` | Take an editor screenshot |
| `debug.log` | Write to the output log |

### Input
| Tool | Description |
|------|-------------|
| `input.simulateKey` | Simulate a key press |
| `input.simulateMouse` | Simulate mouse input |
| `input.simulateAxis` | Simulate axis input |

### Asset
| Tool | Description |
|------|-------------|
| `asset.list` | List assets in a path |
| `asset.import` | Import an external asset |

### Console
| Tool | Description |
|------|-------------|
| `console.execute` | Execute a console command |
| `console.getLogs` | Read project log file (supports `count` and `filter` params) |

### Blueprint
| Tool | Description |
|------|-------------|
| `blueprint.list` | List blueprints in the project |
| `blueprint.open` | Open a blueprint in the editor |

## Connection Modes

### Mode A: OpenClaw Gateway (Remote Access)

```
Telegram/Discord/Web → OpenClaw Gateway → HTTP Polling → Unreal Plugin
```

Works automatically when the Gateway is running. No extra setup needed.

### Mode B: MCP Direct (Claude Code / Cursor)

```
Claude Code / Cursor → MCP Server (stdio) → HTTP → Unreal Plugin (port 27184)
```

The plugin runs an embedded HTTP server on port **27184**. Use the included MCP bridge:

**Claude Code:**
```bash
claude mcp add unreal -- node /path/to/Plugins/OpenClaw/MCP~/index.js
```

**Cursor:** Add to `.cursor/mcp.json`:
```json
{
  "mcpServers": {
    "unreal": {
      "command": "node",
      "args": ["/path/to/Plugins/OpenClaw/MCP~/index.js"]
    }
  }
}
```

Both modes run simultaneously — you can use Gateway and MCP Direct at the same time.

## Editor Panel

Access via **Window → OpenClaw Unreal Plugin**. The dockable panel provides:

- **Status indicator** — connection state at a glance
- **MCP info** — server address and protocol details
- **Connect / Disconnect** buttons
- **Live log** — scrollable log of MCP messages and tool calls

## Troubleshooting

### Plugin not loading or stale binaries

If the plugin fails to load, shows outdated behavior, or you get linker errors after updating, clear the build cache:

```bash
rm -rf YourProject/Plugins/OpenClaw/Binaries YourProject/Plugins/OpenClaw/Intermediate
```

Then restart the Unreal Editor — it will recompile the plugin from source.

### Connection issues

1. Make sure OpenClaw Gateway is running (`openclaw gateway status`)
2. Check the Editor Panel log for error messages
3. Verify firewall isn't blocking the MCP port

## License

MIT
