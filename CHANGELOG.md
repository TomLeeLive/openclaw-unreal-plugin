# Changelog

All notable changes to OpenClaw Unreal Plugin will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.9.3] - 2026-02-10

### Fixed
- UE 5.7 compatibility: Fixed `PlayInEditor` API change
- Added missing `SendRegister` and `HandleRegisterResponse` declarations to header
- Fixed include paths for `UnrealEdEngine.h` and `UnrealEdGlobals.h`

### Documentation
- Added troubleshooting guide for compilation errors
- Added UE 5.7+ compatibility notes

## [0.9.2] - 2026-02-10

### Changed
- Rewrote HTTP communication to use OpenClaw Gateway endpoints (/unreal/*)
- Changed default port from 27742 to 18789 (OpenClaw Gateway default)
- Added proper session registration flow (register → poll → result)
- Improved reconnection handling with automatic retry
- Added GetToolCount() for tool registration reporting

### Fixed
- Removed GEditor dependency in connection manager
- Fixed heartbeat/poll endpoint patterns to match Unity/Godot plugins

## [0.9.1] - 2026-02-10

### Added
- Initial release
- Level tools: getCurrent, list, open, save
- Actor tools: find, getAll, create, delete, getData, setProperty
- Transform tools: get/set position, rotation, scale
- Component tools: get, add, remove
- Editor tools: play, stop, pause, resume, getState
- Debug tools: hierarchy, screenshot, log
- Input simulation: key, mouse, axis
- Asset tools: list, import
- Console tools: execute, getLogs
- Blueprint tools: list, open
- HTTP polling connection to OpenClaw Gateway
- Auto-reconnect on connection loss
- Window menu status indicator
