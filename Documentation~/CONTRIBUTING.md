# 🤝 Contributing Guide

Thank you for contributing to the OpenClaw Unreal Plugin!

## Table of Contents

1. [Ways to Contribute](#ways-to-contribute)
2. [Development Environment Setup](#development-environment-setup)
3. [Code Contributions](#code-contributions)
4. [Pull Request Guidelines](#pull-request-guidelines)
5. [Issue Reporting](#issue-reporting)
6. [Code Review Process](#code-review-process)
7. [Community Guidelines](#community-guidelines)

---

## Ways to Contribute

### Contribution Areas

| Area | Description | Difficulty |
|------|-------------|------------|
| 🐛 Bug Fixes | Resolve issues, improve stability | ⭐ |
| 📝 Documentation | README, examples, translations | ⭐ |
| 🔧 New Tools | Add features to OpenClawTools.cpp | ⭐⭐ |
| ⚡ Performance | Optimization, memory management | ⭐⭐⭐ |
| 🏗️ Architecture | Core structural changes | ⭐⭐⭐ |

### Recommended Issues for First-Time Contributors

Check GitHub for `good first issue` label:

```
https://github.com/openclaw/openclaw-unreal-plugin/labels/good%20first%20issue
```

---

## Development Environment Setup

### 1. Fork & Clone

```bash
# 1. Fork on GitHub

# 2. Clone
git clone https://github.com/YOUR_USERNAME/openclaw-unreal-plugin.git
cd openclaw-unreal-plugin

# 3. Add upstream remote
git remote add upstream https://github.com/openclaw/openclaw-unreal-plugin.git
```

### 2. Set Up Test Environment

```bash
# Create test Unreal project
# Copy plugin to Plugins folder
cp -r openclaw-unreal-plugin YourTestProject/Plugins/OpenClaw

# Start OpenClaw Gateway
openclaw gateway start
```

### 3. Branch Strategy

```bash
# Feature development
git checkout -b feature/audio-tools

# Bug fix
git checkout -b fix/connection-timeout

# Documentation
git checkout -b docs/korean-translation
```

---

## Code Contributions

### Example 1: Adding a New Tool (`material.set`)

#### Step 1: Check/Create Issue

```markdown
## Feature Request: material.set tool

### Description
Tool to change actor material at runtime

### Use Cases
- AI tests visual appearance
- Rapid material iteration

### Proposed API
material.set {actor: "Cube_1", slot: 0, material: "/Game/Materials/M_Red"}
material.get {actor: "Cube_1"}
```

#### Step 2: Implement Tool

```cpp
// In OpenClawTools.h
static TSharedPtr<FJsonObject> Material_Set(const TSharedPtr<FJsonObject>& Params);
static TSharedPtr<FJsonObject> Material_Get(const TSharedPtr<FJsonObject>& Params);

// In OpenClawTools.cpp
TSharedPtr<FJsonObject> FOpenClawTools::Material_Set(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName = Params->GetStringField(TEXT("actor"));
    int32 SlotIndex = Params->HasField(TEXT("slot")) ? Params->GetIntegerField(TEXT("slot")) : 0;
    FString MaterialPath = Params->GetStringField(TEXT("material"));
    
    AActor* Actor = FindActorByName(ActorName);
    if (!Actor)
    {
        return MakeErrorResult(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }
    
    UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
    if (!MeshComp)
    {
        return MakeErrorResult(TEXT("Actor has no StaticMeshComponent"));
    }
    
    UMaterial* Material = LoadObject<UMaterial>(nullptr, *MaterialPath);
    if (!Material)
    {
        return MakeErrorResult(FString::Printf(TEXT("Material not found: %s"), *MaterialPath));
    }
    
    MeshComp->SetMaterial(SlotIndex, Material);
    
    return MakeSuccessResult(FString::Printf(TEXT("Set material on %s slot %d"), *ActorName, SlotIndex));
}

// Register in ExecuteTool
if (ToolName == TEXT("material.set")) return Material_Set(Params);
if (ToolName == TEXT("material.get")) return Material_Get(Params);
```

#### Step 3: Update Extension

```typescript
// In extension/index.ts
const TOOLS = [
    // ... existing tools
    "material.set",
    "material.get",
] as const;

// Add descriptions and parameters
```

#### Step 4: Write Tests

Document test cases in the PR:
```markdown
## Test Cases

1. Set material on existing actor ✅
2. Error when actor doesn't exist ✅
3. Error when material path invalid ✅
4. Works during PIE ✅
```

---

## Pull Request Guidelines

### PR Title Format

```
feat: Add material.set and material.get tools
fix: Handle null component in transform tools
docs: Add Korean translation for TESTING.md
refactor: Simplify JSON response helpers
```

### PR Template

```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Documentation
- [ ] Refactoring

## Testing
- [ ] Tested in Editor mode
- [ ] Tested during PIE
- [ ] Added/updated documentation

## Related Issues
Closes #123

## Screenshots (if applicable)
```

### Checklist Before Submitting

- [ ] Code compiles without warnings
- [ ] Follows Unreal Engine coding standards
- [ ] Added/updated documentation
- [ ] Updated CHANGELOG.md
- [ ] All tests pass

---

## Issue Reporting

### Bug Report Template

```markdown
## Bug Description
Clear and concise description

## Steps to Reproduce
1. Open Unreal Editor
2. Enable OpenClaw plugin
3. Execute 'actor.create' command
4. See error

## Expected Behavior
What should happen

## Actual Behavior
What actually happens

## Environment
- Unreal Engine Version: 5.3
- OpenClaw Plugin Version: 0.9.0
- OS: Windows 11 / macOS 14.x
- OpenClaw Gateway Version: x.x.x

## Logs
```
Relevant Output Log entries
```

## Screenshots
If applicable
```

### Feature Request Template

```markdown
## Feature Description
Clear description of the feature

## Use Cases
- Use case 1
- Use case 2

## Proposed API
```
tool.name {param1: value, param2: value}
```

## Additional Context
Any other information
```

---

## Code Review Process

### Review Criteria

1. **Functionality**: Does it work as intended?
2. **Code Quality**: Clean, readable, follows standards?
3. **Performance**: No unnecessary overhead?
4. **Safety**: Proper null checks, error handling?
5. **Documentation**: Updated docs and comments?

### Review Timeline

- Initial response: Within 48 hours
- Review completion: Within 1 week
- Merge after approval: Within 24 hours

### Addressing Feedback

1. Respond to all comments
2. Push fixes as new commits (easier to review)
3. Request re-review when ready
4. Squash commits before merge if requested

---

## Community Guidelines

### Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Help newcomers learn
- Focus on the code, not the person

### Communication Channels

- **GitHub Issues**: Bug reports, feature requests
- **GitHub Discussions**: General questions, ideas
- **Discord**: Real-time chat, community support

### Recognition

Contributors are recognized in:
- CHANGELOG.md for significant contributions
- README.md contributors section
- Release notes

---

## Getting Help

### Resources

- [Development Guide](DEVELOPMENT.md)
- [Testing Guide](TESTING.md)
- [OpenClaw Documentation](https://docs.openclaw.ai)

### Contact

- GitHub: https://github.com/openclaw/openclaw-unreal-plugin
- Discord: https://discord.com/invite/clawd

---

Thank you for helping make OpenClaw better! 🦞
