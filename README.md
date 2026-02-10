# 🦞 OpenClaw Unreal Plugin

AI-assisted Unreal Engine development. Control the editor, manipulate actors, simulate input, and automate workflows through natural language.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-blue)](https://www.unrealengine.com/)
[![Discord](https://img.shields.io/discord/1234567890?color=7289da&label=Discord&logo=discord&logoColor=white)](https://discord.com/invite/clawd)

## Features

- 🎮 **Editor Control** - Start/stop PIE, pause/resume
- 🎭 **Actor Management** - Create, find, delete, modify actors
- 📐 **Transform Tools** - Position, rotation, scale manipulation
- 🖼️ **Screenshots** - Capture viewport images
- ⌨️ **Input Simulation** - Keyboard, mouse, gamepad simulation
- 📦 **Asset Browsing** - List and explore project content
- 🔧 **Console Commands** - Execute Unreal console commands

## Quick Start

### 1. Install Plugin

Copy to your project's Plugins folder:

```bash
cp -r openclaw-unreal-plugin YourProject/Plugins/OpenClaw
```

### 2. Enable Plugin

1. Open your Unreal project
2. Go to Edit → Plugins
3. Search for "OpenClaw"
4. Enable the plugin
5. Restart the editor

### 3. Connect to OpenClaw

The plugin automatically connects to the OpenClaw gateway on startup.

Check connection: Window → OpenClaw Status

## Configuration

Create `openclaw.json` in your project root:

```json
{
  "host": "127.0.0.1",
  "port": 18789,
  "autoConnect": true,
  "secret": "optional-secret-key"
}
```

Or in `~/.openclaw/unreal-plugin.json` for global config.

## Available Tools (40+)

### Level Management
| Tool | Description |
|------|-------------|
| `level.getCurrent` | Get current level info |
| `level.list` | List all levels in project |
| `level.open` | Open level by path |
| `level.save` | Save current level |

### Actor Tools
| Tool | Description |
|------|-------------|
| `actor.find` | Find actor by name |
| `actor.getAll` | Get all actors in level |
| `actor.create` | Spawn new actor |
| `actor.delete` | Remove actor |
| `actor.getData` | Get detailed actor info |
| `actor.setProperty` | Modify actor property |

### Transform Tools
| Tool | Description |
|------|-------------|
| `transform.getPosition` | Get actor world position |
| `transform.setPosition` | Set actor position |
| `transform.getRotation` | Get actor rotation |
| `transform.setRotation` | Set actor rotation |
| `transform.getScale` | Get actor scale |
| `transform.setScale` | Set actor scale |

### Component Tools
| Tool | Description |
|------|-------------|
| `component.get` | List actor components |
| `component.add` | Add component to actor |
| `component.remove` | Remove component |

### Editor Control
| Tool | Description |
|------|-------------|
| `editor.play` | Start Play in Editor |
| `editor.stop` | Stop PIE session |
| `editor.pause` | Pause execution |
| `editor.resume` | Resume execution |
| `editor.getState` | Get current state |

### Debug Tools
| Tool | Description |
|------|-------------|
| `debug.hierarchy` | Get world actor hierarchy |
| `debug.screenshot` | Capture screenshot |
| `debug.log` | Log message to output |

### Input Simulation
| Tool | Description |
|------|-------------|
| `input.simulateKey` | Simulate keyboard input |
| `input.simulateMouse` | Simulate mouse input |
| `input.simulateAxis` | Simulate axis/gamepad |

### Asset Tools
| Tool | Description |
|------|-------------|
| `asset.list` | List assets in path |
| `asset.import` | Import external asset |

### Console
| Tool | Description |
|------|-------------|
| `console.execute` | Run console command |
| `console.getLogs` | Get output log |

### Blueprint
| Tool | Description |
|------|-------------|
| `blueprint.list` | List blueprints |
| `blueprint.open` | Open in editor |

## Example Interactions

```
User: Show me all actors in the level
AI: [Executes debug.hierarchy]
    Found 15 actors: PlayerStart, DirectionalLight, ...

User: Create a point light at (0, 0, 300)
AI: [Executes actor.create type="PointLight" x=0 y=0 z=300]
    Created PointLight_1 at position (0, 0, 300)

User: Start the game and move forward
AI: [Executes editor.play, then input.simulateKey key="W"]
    Started PIE, simulating W key press

User: Take a screenshot
AI: [Executes debug.screenshot]
    Screenshot saved to Saved/Screenshots/screenshot_20260211_141500.png
```

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Unreal Editor                         │
│  ┌─────────────────────────────────────────────────┐    │
│  │              OpenClaw Plugin                     │    │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────┐  │    │
│  │  │ Connection  │  │   Tools     │  │  Input  │  │    │
│  │  │  Manager    │  │  Handler    │  │ Simulate│  │    │
│  │  └──────┬──────┘  └──────┬──────┘  └────┬────┘  │    │
│  └─────────┼────────────────┼──────────────┼───────┘    │
└────────────┼────────────────┼──────────────┼────────────┘
             │                │              │
             ▼ HTTP Poll      │              │
┌────────────────────────────────────────────────────────┐
│                  OpenClaw Gateway                       │
└────────────────────────────────────────────────────────┘
             ▲
             │ AI Requests
┌────────────────────────────────────────────────────────┐
│                    AI Assistant                         │
└────────────────────────────────────────────────────────┘
```

## Requirements

- Unreal Engine 5.0 or later
- Windows, macOS, or Linux
- OpenClaw Gateway running

## Building from Source

```bash
# Clone repository
git clone https://github.com/openclaw/openclaw-unreal-plugin

# Copy to project
cp -r openclaw-unreal-plugin YourProject/Plugins/OpenClaw

# Regenerate project files
# Windows: Right-click .uproject → Generate Visual Studio files
# Mac: Open with Xcode, build
```

## 🔐 Security: Model Invocation Setting

When publishing to ClawHub or installing as a skill, you can configure `disableModelInvocation` in the skill metadata:

| Setting | AI Auto-Invoke | User Explicit Request |
|---------|---------------|----------------------|
| `false` (default) | ✅ Allowed | ✅ Allowed |
| `true` | ❌ Blocked | ✅ Allowed |

### Recommendation for Unreal Plugin: **`true`**

**Reason:** During Unreal development, it's useful for AI to autonomously perform supporting tasks like checking actor hierarchy, taking screenshots, and inspecting components.

**When to use `true`:** For sensitive tools (payments, deletions, message sending, etc.)

```yaml
# Example skill metadata
metadata:
  openclaw:
    disableModelInvocation: true  # Recommended for Unreal plugin
```

## Troubleshooting

### Compilation Error: "could not be compiled"

프로젝트 열 때 컴파일 오류가 발생하면:

1. **Binaries/Intermediate 삭제**
   ```bash
   rm -rf YourProject/Plugins/OpenClaw/Binaries
   rm -rf YourProject/Plugins/OpenClaw/Intermediate
   ```

2. **프로젝트 다시 열기** - Unreal이 플러그인을 새로 빌드합니다.

### UE 5.7+ 호환성

이 플러그인은 UE 5.7에서 테스트되었습니다. 다른 버전에서 컴파일 오류 발생 시:

| 오류 | 해결 방법 |
|------|----------|
| `PlayInEditor` not found | `GEditor->Exec(World, TEXT("PIE"))` 사용 |
| `SendRegister` undeclared | 헤더에 선언 추가 필요 |
| Missing headers | `UnrealEdEngine.h`, `UnrealEdGlobals.h` include |

### Plugin not loading
- Check Output Log for errors
- Verify plugin is in `Plugins/OpenClaw/` folder
- Ensure `OpenClaw.uplugin` exists

### Not connecting to gateway
- Verify gateway is running: `openclaw gateway status`
- Check firewall allows port 18789 (default)
- Look for `[OpenClaw]` messages in Output Log

### Tools not executing
- Confirm connected (Window → OpenClaw Status)
- Check Output Log for tool execution errors
- Ensure editor is not busy

## Contributing

1. Fork the repository
2. Create feature branch
3. Make changes
4. Submit pull request

## Support

- 📖 [Documentation](https://docs.openclaw.ai)
- 💬 [Discord](https://discord.com/invite/clawd)
- 🐛 [Issues](https://github.com/openclaw/openclaw-unreal-plugin/issues)

## License

MIT License - see [LICENSE](LICENSE) file.

---

Made with 🦞 by the OpenClaw community
