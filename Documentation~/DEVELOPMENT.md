# 🛠️ OpenClaw Unreal Plugin - Development Guide

This document is the development guide for OpenClaw Unreal Plugin. It covers architecture, how to add new tools, and debugging tips.

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Project Structure](#project-structure)
3. [Core Components](#core-components)
4. [Adding New Tools](#adding-new-tools)
5. [JSON Handling](#json-handling)
6. [Play Mode Handling](#play-mode-handling)
7. [Debugging](#debugging)
8. [Contribution Guidelines](#contribution-guidelines)

---

## Architecture Overview

### Communication Flow

```
┌─────────────┐     HTTP      ┌─────────────────┐     Tool Call    ┌─────────────┐
│   OpenClaw  │ ────────────► │ Gateway Plugin  │ ───────────────► │   Unreal    │
│   Agent     │               │ (index.ts)      │                  │   Plugin    │
└─────────────┘               └─────────────────┘                  └─────────────┘
                                      │                                   │
                                      │ POST /api/plugin/poll             │
                                      │◄──────────────────────────────────│
                                      │                                   │
                                      │ Command JSON                      │
                                      │──────────────────────────────────►│
                                      │                                   │
                                      │ Result in next poll               │
                                      │◄──────────────────────────────────│
```

### Core Design Principles

1. **Editor Mode First**: Primary focus on Editor automation
2. **Auto Reconnection**: Automatic recovery on connection loss
3. **PIE Support**: Works during Play in Editor sessions
4. **Game Thread Execution**: All UE API calls on Game Thread

---

## Project Structure

```
openclaw-unreal-plugin/
├── OpenClaw.uplugin           # Plugin definition
├── README.md                  # User documentation
├── CHANGELOG.md               # Version history
│
├── Source/OpenClaw/
│   ├── OpenClaw.Build.cs                    # Build configuration
│   │
│   ├── Public/
│   │   ├── OpenClawModule.h                 # Module interface
│   │   ├── OpenClawConnectionManager.h      # HTTP connection
│   │   └── OpenClawTools.h                  # Tool declarations
│   │
│   └── Private/
│       ├── OpenClawModule.cpp               # Module implementation
│       ├── OpenClawConnectionManager.cpp    # HTTP polling logic
│       └── OpenClawTools.cpp                # 40+ tool implementations
│
└── Documentation~/
    ├── DEVELOPMENT.md         # This file
    ├── TESTING.md             # Testing guide
    └── CONTRIBUTING.md        # Contribution guide
```

---

## Core Components

### OpenClawModule.cpp

Entry point that initializes when the Editor starts.

```cpp
void FOpenClawModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin starting..."));
    
    // Initialize connection manager
    FOpenClawConnectionManager::Get().Initialize();
    
    // Register menu extension
    RegisterMenuExtension();
}
```

**Key Features:**
- Auto-runs on Editor start via `PostEngineInit` loading phase
- Registers Window menu item for status display
- Initializes connection manager singleton

### OpenClawConnectionManager.cpp

Singleton handling HTTP communication and command execution.

```cpp
class FOpenClawConnectionManager : public FTickableGameObject
{
public:
    static FOpenClawConnectionManager& Get();
    
    void Initialize();
    void Shutdown();
    void Connect();
    void Disconnect();
    
    // FTickableGameObject interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickableInEditor() const override { return true; }
};
```

**Key Features:**
- `FTickableGameObject` for Editor ticking
- HTTP polling via `FHttpModule`
- Pending results queue with thread-safe access
- Auto-reconnection on connection loss

### OpenClawTools.cpp

Implementation of 40+ AI tools.

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::ExecuteTool(
    const FString& ToolName, 
    const TSharedPtr<FJsonObject>& Params)
{
    if (ToolName == TEXT("actor.find")) return Actor_Find(Params);
    if (ToolName == TEXT("actor.create")) return Actor_Create(Params);
    // ... 40+ tools
    
    return MakeErrorResult(FString::Printf(TEXT("Unknown tool: %s"), *ToolName));
}
```

---

## Adding New Tools

### Step 1: Declare Tool Method

Add declaration to `OpenClawTools.h`:

```cpp
class OPENCLAW_API FOpenClawTools
{
private:
    // Add your tool declaration
    static TSharedPtr<FJsonObject> MyCategory_MyNewTool(const TSharedPtr<FJsonObject>& Params);
};
```

### Step 2: Implement Tool Method

Add implementation to `OpenClawTools.cpp`:

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::MyCategory_MyNewTool(const TSharedPtr<FJsonObject>& Params)
{
    // Extract parameters
    FString Name = Params->GetStringField(TEXT("name"));
    int32 Count = Params->HasField(TEXT("count")) ? Params->GetIntegerField(TEXT("count")) : 1;
    bool bEnabled = !Params->HasField(TEXT("enabled")) || Params->GetBoolField(TEXT("enabled"));
    
    // Validate required parameters
    if (Name.IsEmpty())
    {
        return MakeErrorResult(TEXT("name parameter is required"));
    }
    
    // Implement tool logic
    // ...
    
    // Return success result
    TSharedPtr<FJsonObject> Result = MakeShareable(new FJsonObject());
    Result->SetBoolField(TEXT("success"), true);
    Result->SetStringField(TEXT("message"), TEXT("Operation completed"));
    Result->SetStringField(TEXT("name"), Name);
    
    return Result;
}
```

### Step 3: Register Tool

Add to `ExecuteTool` method:

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::ExecuteTool(
    const FString& ToolName, 
    const TSharedPtr<FJsonObject>& Params)
{
    // ... existing tools ...
    
    // Add your tool
    if (ToolName == TEXT("myCategory.myNewTool")) return MyCategory_MyNewTool(Params);
    
    return MakeErrorResult(FString::Printf(TEXT("Unknown tool: %s"), *ToolName));
}
```

### Step 4: Update Extension (Optional)

If adding to the OpenClaw extension, update `extension/index.ts`:

```typescript
const TOOLS = [
    // ... existing tools ...
    "myCategory.myNewTool",
] as const;

function getToolDescription(tool: ToolName): string {
    const descriptions: Record<ToolName, string> = {
        // ... existing ...
        "myCategory.myNewTool": "Description of my new tool",
    };
    return descriptions[tool] || tool;
}

function getToolParameters(tool: ToolName): Record<string, any> {
    const params: Record<string, Record<string, any>> = {
        // ... existing ...
        "myCategory.myNewTool": {
            name: { type: "string", description: "Object name", required: true },
            count: { type: "number", description: "Operation count" },
            enabled: { type: "boolean", description: "Enable feature" },
        },
    };
    return params[tool] || {};
}
```

### Example: Adding an Actor Rename Tool

```cpp
// In OpenClawTools.h
static TSharedPtr<FJsonObject> Actor_Rename(const TSharedPtr<FJsonObject>& Params);

// In OpenClawTools.cpp
TSharedPtr<FJsonObject> FOpenClawTools::Actor_Rename(const TSharedPtr<FJsonObject>& Params)
{
    FString OldName = Params->GetStringField(TEXT("name"));
    FString NewName = Params->GetStringField(TEXT("newName"));
    
    if (OldName.IsEmpty() || NewName.IsEmpty())
    {
        return MakeErrorResult(TEXT("name and newName are required"));
    }
    
    AActor* Actor = FindActorByName(OldName);
    if (!Actor)
    {
        return MakeErrorResult(FString::Printf(TEXT("Actor not found: %s"), *OldName));
    }
    
    Actor->SetActorLabel(*NewName);
    
    return MakeSuccessResult(FString::Printf(TEXT("Renamed '%s' to '%s'"), *OldName, *NewName));
}

// In ExecuteTool
if (ToolName == TEXT("actor.rename")) return Actor_Rename(Params);
```

---

## JSON Handling

### Using FJsonObject

```cpp
// Reading values
FString Str = Params->GetStringField(TEXT("key"));
int32 Num = Params->GetIntegerField(TEXT("key"));
double Dbl = Params->GetNumberField(TEXT("key"));
bool Flag = Params->GetBoolField(TEXT("key"));

// Check if field exists
if (Params->HasField(TEXT("optionalKey")))
{
    // Use the value
}

// Reading nested objects
TSharedPtr<FJsonObject> NestedObj = Params->GetObjectField(TEXT("nested"));

// Reading arrays
const TArray<TSharedPtr<FJsonValue>>& Arr = Params->GetArrayField(TEXT("items"));
for (const auto& Item : Arr)
{
    FString Value = Item->AsString();
}
```

### Creating Result Objects

```cpp
// Success result
TSharedPtr<FJsonObject> Result = MakeShareable(new FJsonObject());
Result->SetBoolField(TEXT("success"), true);
Result->SetStringField(TEXT("message"), TEXT("Done"));

// With nested data
TSharedPtr<FJsonObject> DataObj = MakeShareable(new FJsonObject());
DataObj->SetStringField(TEXT("name"), TEXT("MyActor"));
DataObj->SetNumberField(TEXT("x"), 100.0);
Result->SetObjectField(TEXT("data"), DataObj);

// With array
TArray<TSharedPtr<FJsonValue>> Items;
Items.Add(MakeShareable(new FJsonValueString(TEXT("item1"))));
Items.Add(MakeShareable(new FJsonValueString(TEXT("item2"))));
Result->SetArrayField(TEXT("items"), Items);
```

---

## Play Mode Handling

### PIE Detection

```cpp
bool bIsPlaying = GEditor && GEditor->PlayWorld != nullptr;
bool bIsPaused = bIsPlaying && GEditor->PlayWorld->bDebugPauseExecution;
```

### Getting the Right World

```cpp
UWorld* FOpenClawTools::GetEditorWorld()
{
    if (GEditor)
    {
        // In PIE, this returns the editor world, not the play world
        return GEditor->GetEditorWorldContext().World();
    }
    return nullptr;
}

// For PIE-specific operations
UWorld* GetPlayWorld()
{
    if (GEditor && GEditor->PlayWorld)
    {
        return GEditor->PlayWorld;
    }
    return nullptr;
}
```

### Actor Operations During PIE

```cpp
// Be careful: actors in editor world vs play world are different
if (GEditor->PlayWorld)
{
    // Modifying editor world actors during PIE may not be visible
    // Consider showing a warning
    UE_LOG(LogOpenClaw, Warning, TEXT("Modifying actors during PIE"));
}
```

---

## Debugging

### Unreal Output Log

The plugin outputs logs with `[OpenClaw]` prefix:

```
LogOpenClaw: [OpenClaw] 🦞 Plugin starting...
LogOpenClaw: Connecting with session ID: unreal_1234567890_abc123
LogOpenClaw: State changed to: Connected
LogOpenClaw: Received command: debug.hierarchy (id: call_123)
```

### Enable Verbose Logging

In your project's `DefaultEngine.ini`:

```ini
[Core.Log]
LogOpenClaw=Verbose
```

### Gateway Logs

```bash
openclaw gateway status
# Check for Unreal session registration
```

### Connection Troubleshooting

1. **Check Gateway Status**
   ```bash
   openclaw gateway status
   ```

2. **Verify in Editor**
   - Window → OpenClaw Status
   - Check for connection errors in Output Log

3. **Direct HTTP Test**
   ```bash
   curl http://localhost:27742/api/plugin/poll -X POST \
     -H "Content-Type: application/json" \
     -d '{"sessionId":"test","engine":"unreal"}'
   ```

### Common Issues

#### Module Not Loading

- Check Output Log for plugin errors
- Verify `.uplugin` file is valid JSON
- Ensure all dependencies in `.Build.cs` are available

#### Tools Not Executing

- Confirm connection is established
- Check for exceptions in Output Log
- Verify parameter types match expectations

---

## Contribution Guidelines

### Code Style

- Unreal Engine coding standard
- Use `UPROPERTY`, `UFUNCTION` where applicable
- Clear variable naming with prefixes (b for bool, etc.)

### Commit Messages

```
feat: Add actor.rename tool
fix: Handle null actor in transform tools
docs: Update DEVELOPMENT.md with new examples
refactor: Simplify JSON response creation
```

### Testing

Before submitting:
1. Test in Editor mode
2. Test during PIE
3. Test with various actor types
4. Verify reconnection works

### Pull Request

1. Create feature branch: `feature/your-feature`
2. Make changes
3. Update CHANGELOG.md
4. Submit PR with clear description

---

## Contact

- GitHub: https://github.com/openclaw/openclaw-unreal-plugin
- OpenClaw Discord: https://discord.com/invite/clawd
