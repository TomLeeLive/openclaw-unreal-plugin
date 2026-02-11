# OpenClaw Unreal Plugin - Setup Guide

This guide explains how to set up the OpenClaw Unreal Plugin for different use cases.

## 🅰️ Mode A: OpenClaw Gateway (Remote Access)

**When to use:** When you want to develop games remotely via Telegram, Discord, or web.

### Setup Steps

```bash
# 1. Install OpenClaw
npm install -g openclaw

# 2. Start Gateway
openclaw gateway start

# 3. Install Unreal Plugin
cp -r openclaw-unreal-plugin YourProject/Plugins/OpenClaw

# 4. Enable Plugin
#    Edit > Plugins > Search "OpenClaw" > Enable > Restart Editor

# 5. Configure chat integration (optional)
openclaw config
```

### How it works

```
┌──────────────┐      ┌─────────────────┐      ┌──────────────┐
│  Telegram/   │ ───→ │    OpenClaw     │ ───→ │   Unreal     │
│  Discord/Web │ ←─── │    Gateway      │ ←─── │   Editor     │
└──────────────┘      └─────────────────┘      └──────────────┘
     Phone              Your Computer           Your Computer
```

### Example Usage

From your phone:
```
You: "What actors are in the level?"
AI: Found 12 actors: PlayerStart, DirectionalLight, Floor, ...

You: "Create a PointLight at (0, 0, 300)"
AI: Created PointLight_1 at (0, 0, 300)

You: "Start PIE"
AI: Play In Editor started
```

---

## 🅱️ Mode B: MCP Direct (Local Development)

**When to use:** When you want to use Claude Code or Cursor to directly control Unreal.

### Setup Steps

```bash
# 1. Install Unreal Plugin (same as above)

# 2. Install MCP server dependencies
cd /path/to/openclaw-unreal-plugin/MCP~
npm install

# 3. Register MCP server with Claude Code
claude mcp add unreal -- node /full/path/to/openclaw-unreal-plugin/MCP~/index.js

# 4. Start MCP Bridge in Unreal
#    Window > OpenClaw > Start MCP Bridge

# 5. Use Claude Code
claude
```

### How it works

```
┌──────────────┐      ┌─────────────────┐      ┌──────────────┐
│ Claude Code  │ ───→ │   MCP Server    │ ───→ │   Unreal     │
│ or Desktop   │ ←─── │   (Node.js)     │ ←─── │   Editor     │
└──────────────┘      └─────────────────┘      └──────────────┘
    Terminal             localhost:27184        localhost:27184
```

### Example Usage

```
$ claude
> Show me the actor hierarchy
AI: Found 15 actors in level...

> Spawn a Cube at position (100, 0, 50)
AI: Created Cube at (100, 0, 50)

> Start PIE and press W key
AI: PIE started, simulating W key press
```

---

## 🔀 Hybrid Mode (Both)

Both modes can run simultaneously.

| Service | Default Port |
|---------|--------------|
| MCP Bridge | 27184 |
| OpenClaw Gateway | 18789 |

---

## Quick Reference

| Task | Command |
|------|---------|
| Start Gateway | `openclaw gateway start` |
| Add MCP to Claude | `claude mcp add unreal -- node /path/to/MCP~/index.js` |
| Start MCP Bridge | Window > OpenClaw > Start MCP Bridge |
