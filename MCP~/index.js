#!/usr/bin/env node
/**
 * OpenClaw Unreal MCP Server
 * Bridges MCP protocol to Unreal Plugin via HTTP
 * 
 * Usage with Claude Code:
 *   claude mcp add unreal -- node /path/to/MCP~/index.js
 */

import { Server } from '@modelcontextprotocol/sdk/server/index.js';
import { StdioServerTransport } from '@modelcontextprotocol/sdk/server/stdio.js';
import {
  CallToolRequestSchema,
  ListToolsRequestSchema,
} from '@modelcontextprotocol/sdk/types.js';

const UNREAL_HOST = process.env.UNREAL_HOST || '127.0.0.1';
const UNREAL_PORT = process.env.UNREAL_PORT || 27184;
const UNREAL_URL = `http://${UNREAL_HOST}:${UNREAL_PORT}`;

// Tool definitions - mirrors Unreal plugin tools
const TOOLS = [
  // Level tools
  { name: 'level.getCurrent', description: 'Get current level info', inputSchema: { type: 'object', properties: {} } },
  { name: 'level.list', description: 'List all levels in project', inputSchema: { type: 'object', properties: {} } },
  { name: 'level.open', description: 'Open a level by path', inputSchema: { type: 'object', properties: { path: { type: 'string' } }, required: ['path'] } },
  { name: 'level.save', description: 'Save current level', inputSchema: { type: 'object', properties: {} } },
  
  // Actor tools
  { name: 'actor.find', description: 'Find actor by name', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'actor.getAll', description: 'Get all actors in level', inputSchema: { type: 'object', properties: {} } },
  { name: 'actor.create', description: 'Spawn new actor', inputSchema: { type: 'object', properties: { type: { type: 'string' }, name: { type: 'string' }, x: { type: 'number' }, y: { type: 'number' }, z: { type: 'number' } }, required: ['type'] } },
  { name: 'actor.delete', description: 'Remove actor', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'actor.getData', description: 'Get actor details', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'actor.setProperty', description: 'Modify actor property', inputSchema: { type: 'object', properties: { name: { type: 'string' }, property: { type: 'string' }, value: {} }, required: ['name', 'property', 'value'] } },
  
  // Transform tools
  { name: 'transform.getPosition', description: 'Get actor position', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'transform.setPosition', description: 'Set actor position', inputSchema: { type: 'object', properties: { name: { type: 'string' }, x: { type: 'number' }, y: { type: 'number' }, z: { type: 'number' } }, required: ['name'] } },
  { name: 'transform.getRotation', description: 'Get actor rotation', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'transform.setRotation', description: 'Set actor rotation', inputSchema: { type: 'object', properties: { name: { type: 'string' }, pitch: { type: 'number' }, yaw: { type: 'number' }, roll: { type: 'number' } }, required: ['name'] } },
  { name: 'transform.getScale', description: 'Get actor scale', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'transform.setScale', description: 'Set actor scale', inputSchema: { type: 'object', properties: { name: { type: 'string' }, x: { type: 'number' }, y: { type: 'number' }, z: { type: 'number' } }, required: ['name'] } },
  
  // Component tools
  { name: 'component.get', description: 'Get actor components', inputSchema: { type: 'object', properties: { name: { type: 'string' } }, required: ['name'] } },
  { name: 'component.add', description: 'Add component to actor', inputSchema: { type: 'object', properties: { name: { type: 'string' }, component: { type: 'string' } }, required: ['name', 'component'] } },
  { name: 'component.remove', description: 'Remove component', inputSchema: { type: 'object', properties: { name: { type: 'string' }, component: { type: 'string' } }, required: ['name', 'component'] } },
  
  // Editor tools
  { name: 'editor.play', description: 'Start PIE', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor.stop', description: 'Stop PIE', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor.pause', description: 'Pause PIE', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor.resume', description: 'Resume PIE', inputSchema: { type: 'object', properties: {} } },
  { name: 'editor.getState', description: 'Get editor state', inputSchema: { type: 'object', properties: {} } },
  
  // Debug tools
  { name: 'debug.hierarchy', description: 'Get actor hierarchy', inputSchema: { type: 'object', properties: {} } },
  { name: 'debug.screenshot', description: 'Capture screenshot', inputSchema: { type: 'object', properties: { filename: { type: 'string' } } } },
  { name: 'debug.log', description: 'Log message', inputSchema: { type: 'object', properties: { message: { type: 'string' } }, required: ['message'] } },
  
  // Input simulation
  { name: 'input.simulateKey', description: 'Simulate keyboard', inputSchema: { type: 'object', properties: { key: { type: 'string' }, pressed: { type: 'boolean' } }, required: ['key'] } },
  { name: 'input.simulateMouse', description: 'Simulate mouse', inputSchema: { type: 'object', properties: { button: { type: 'string' }, x: { type: 'number' }, y: { type: 'number' } } } },
  { name: 'input.simulateAxis', description: 'Simulate gamepad axis', inputSchema: { type: 'object', properties: { axis: { type: 'string' }, value: { type: 'number' } }, required: ['axis', 'value'] } },
  
  // Asset tools
  { name: 'asset.list', description: 'List assets', inputSchema: { type: 'object', properties: { path: { type: 'string' } } } },
  { name: 'asset.import', description: 'Import asset', inputSchema: { type: 'object', properties: { path: { type: 'string' } }, required: ['path'] } },
  
  // Console
  { name: 'console.execute', description: 'Run console command', inputSchema: { type: 'object', properties: { command: { type: 'string' } }, required: ['command'] } },
  { name: 'console.getLogs', description: 'Get output log', inputSchema: { type: 'object', properties: { count: { type: 'number' } } } },
  
  // Blueprint
  { name: 'blueprint.list', description: 'List blueprints', inputSchema: { type: 'object', properties: {} } },
  { name: 'blueprint.open', description: 'Open blueprint', inputSchema: { type: 'object', properties: { path: { type: 'string' } }, required: ['path'] } },
];

class UnrealMCPServer {
  constructor() {
    this.server = new Server(
      { name: 'openclaw-unreal', version: '1.0.0' },
      { capabilities: { tools: {} } }
    );
    
    this.setupHandlers();
  }
  
  setupHandlers() {
    this.server.setRequestHandler(ListToolsRequestSchema, async () => ({
      tools: TOOLS
    }));
    
    this.server.setRequestHandler(CallToolRequestSchema, async (request) => {
      const { name, arguments: args } = request.params;
      
      try {
        const result = await this.executeUnrealTool(name, args || {});
        return {
          content: [{ type: 'text', text: JSON.stringify(result, null, 2) }]
        };
      } catch (error) {
        return {
          content: [{ type: 'text', text: `Error: ${error.message}` }],
          isError: true
        };
      }
    });
  }
  
  async executeUnrealTool(tool, args) {
    const url = `${UNREAL_URL}/tool`;
    
    const response = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ tool, arguments: args })
    });
    
    if (!response.ok) {
      throw new Error(`Unreal returned ${response.status}: ${await response.text()}`);
    }
    
    return await response.json();
  }
  
  async run() {
    const transport = new StdioServerTransport();
    await this.server.connect(transport);
    console.error('[OpenClaw Unreal MCP] Server started');
  }
}

const server = new UnrealMCPServer();
server.run().catch(console.error);
