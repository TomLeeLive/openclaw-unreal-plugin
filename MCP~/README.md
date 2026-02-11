# OpenClaw Unreal MCP Server

MCP (Model Context Protocol) server for direct Claude Code integration with Unreal Engine.

## Architecture

```
┌─────────────────┐     stdio      ┌──────────────────┐     HTTP      ┌──────────────────┐
│   Claude Code   │ ←───────────→  │  MCP Server      │ ←──────────→  │  Unreal Editor   │
│   or Desktop    │    MCP         │  (this package)  │   :27184      │  (MCP Bridge)    │
└─────────────────┘                └──────────────────┘               └──────────────────┘
```

## Prerequisites

1. Unreal project with OpenClaw Unreal Plugin installed
2. MCP Bridge enabled in Unreal (Window > OpenClaw > Start MCP Bridge)
3. Node.js 18+

## Installation

```bash
cd MCP~
npm install
```

## Usage with Claude Code

```bash
claude mcp add unreal -- node /path/to/openclaw-unreal-plugin/MCP~/index.js
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `UNREAL_HOST` | `127.0.0.1` | Unreal MCP Bridge host |
| `UNREAL_PORT` | `27184` | Unreal MCP Bridge port |

## License

MIT License
