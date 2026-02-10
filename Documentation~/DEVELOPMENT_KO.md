# 🛠️ OpenClaw Unreal Plugin - 개발 가이드

이 문서는 OpenClaw Unreal Plugin의 개발 가이드입니다. 아키텍처, 새 도구 추가 방법, 디버깅 팁을 다룹니다.

## 목차

1. [아키텍처 개요](#아키텍처-개요)
2. [프로젝트 구조](#프로젝트-구조)
3. [핵심 컴포넌트](#핵심-컴포넌트)
4. [새 도구 추가하기](#새-도구-추가하기)
5. [JSON 처리](#json-처리)
6. [플레이 모드 처리](#플레이-모드-처리)
7. [디버깅](#디버깅)
8. [기여 가이드라인](#기여-가이드라인)

---

## 아키텍처 개요

### 통신 흐름

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
                                      │ 다음 poll에 결과 포함              │
                                      │◄──────────────────────────────────│
```

### 핵심 설계 원칙

1. **에디터 모드 우선**: 에디터 자동화에 주력
2. **자동 재연결**: 연결 끊김 시 자동 복구
3. **PIE 지원**: Play in Editor 세션 중에도 동작
4. **게임 스레드 실행**: 모든 UE API 호출은 게임 스레드에서

---

## 프로젝트 구조

```
openclaw-unreal-plugin/
├── OpenClaw.uplugin           # 플러그인 정의
├── README.md                  # 사용자 문서
├── CHANGELOG.md               # 버전 이력
│
├── Source/OpenClaw/
│   ├── OpenClaw.Build.cs                    # 빌드 설정
│   │
│   ├── Public/
│   │   ├── OpenClawModule.h                 # 모듈 인터페이스
│   │   ├── OpenClawConnectionManager.h      # HTTP 연결
│   │   └── OpenClawTools.h                  # 도구 선언
│   │
│   └── Private/
│       ├── OpenClawModule.cpp               # 모듈 구현
│       ├── OpenClawConnectionManager.cpp    # HTTP 폴링 로직
│       └── OpenClawTools.cpp                # 40+ 도구 구현
│
└── Documentation~/
    ├── DEVELOPMENT_KO.md      # 이 파일
    ├── TESTING_KO.md          # 테스트 가이드
    └── CONTRIBUTING_KO.md     # 기여 가이드
```

---

## 핵심 컴포넌트

### OpenClawModule.cpp

에디터 시작 시 초기화되는 진입점입니다.

```cpp
void FOpenClawModule::StartupModule()
{
    UE_LOG(LogTemp, Log, TEXT("[OpenClaw] 🦞 Plugin starting..."));
    
    // 연결 관리자 초기화
    FOpenClawConnectionManager::Get().Initialize();
    
    // 메뉴 확장 등록
    RegisterMenuExtension();
}
```

**주요 기능:**
- `PostEngineInit` 로딩 단계에서 자동 실행
- Window 메뉴에 상태 표시 항목 등록
- 연결 관리자 싱글톤 초기화

### OpenClawConnectionManager.cpp

HTTP 통신과 명령 실행을 담당하는 싱글톤입니다.

```cpp
class FOpenClawConnectionManager : public FTickableGameObject
{
public:
    static FOpenClawConnectionManager& Get();
    
    void Initialize();
    void Shutdown();
    void Connect();
    void Disconnect();
    
    // FTickableGameObject 인터페이스
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickableInEditor() const override { return true; }
};
```

**주요 기능:**
- 에디터 틱을 위한 `FTickableGameObject`
- `FHttpModule`을 통한 HTTP 폴링
- 스레드 안전한 결과 큐
- 연결 끊김 시 자동 재연결

### OpenClawTools.cpp

40개 이상의 AI 도구 구현입니다.

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::ExecuteTool(
    const FString& ToolName, 
    const TSharedPtr<FJsonObject>& Params)
{
    if (ToolName == TEXT("actor.find")) return Actor_Find(Params);
    if (ToolName == TEXT("actor.create")) return Actor_Create(Params);
    // ... 40+ 도구
    
    return MakeErrorResult(FString::Printf(TEXT("Unknown tool: %s"), *ToolName));
}
```

---

## 새 도구 추가하기

### 1단계: 도구 메서드 선언

`OpenClawTools.h`에 선언 추가:

```cpp
class OPENCLAW_API FOpenClawTools
{
private:
    // 새 도구 선언 추가
    static TSharedPtr<FJsonObject> MyCategory_MyNewTool(const TSharedPtr<FJsonObject>& Params);
};
```

### 2단계: 도구 메서드 구현

`OpenClawTools.cpp`에 구현 추가:

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::MyCategory_MyNewTool(const TSharedPtr<FJsonObject>& Params)
{
    // 파라미터 추출
    FString Name = Params->GetStringField(TEXT("name"));
    int32 Count = Params->HasField(TEXT("count")) ? Params->GetIntegerField(TEXT("count")) : 1;
    bool bEnabled = !Params->HasField(TEXT("enabled")) || Params->GetBoolField(TEXT("enabled"));
    
    // 필수 파라미터 검증
    if (Name.IsEmpty())
    {
        return MakeErrorResult(TEXT("name 파라미터가 필요합니다"));
    }
    
    // 도구 로직 구현
    // ...
    
    // 성공 결과 반환
    TSharedPtr<FJsonObject> Result = MakeShareable(new FJsonObject());
    Result->SetBoolField(TEXT("success"), true);
    Result->SetStringField(TEXT("message"), TEXT("작업 완료"));
    Result->SetStringField(TEXT("name"), Name);
    
    return Result;
}
```

### 3단계: 도구 등록

`ExecuteTool` 메서드에 추가:

```cpp
TSharedPtr<FJsonObject> FOpenClawTools::ExecuteTool(
    const FString& ToolName, 
    const TSharedPtr<FJsonObject>& Params)
{
    // ... 기존 도구들 ...
    
    // 새 도구 추가
    if (ToolName == TEXT("myCategory.myNewTool")) return MyCategory_MyNewTool(Params);
    
    return MakeErrorResult(FString::Printf(TEXT("Unknown tool: %s"), *ToolName));
}
```

### 4단계: Extension 업데이트 (선택사항)

OpenClaw extension에 추가하려면 `extension/index.ts` 업데이트:

```typescript
const TOOLS = [
    // ... 기존 도구들 ...
    "myCategory.myNewTool",
] as const;

function getToolDescription(tool: ToolName): string {
    const descriptions: Record<ToolName, string> = {
        // ... 기존 ...
        "myCategory.myNewTool": "새 도구 설명",
    };
    return descriptions[tool] || tool;
}

function getToolParameters(tool: ToolName): Record<string, any> {
    const params: Record<string, Record<string, any>> = {
        // ... 기존 ...
        "myCategory.myNewTool": {
            name: { type: "string", description: "객체 이름", required: true },
            count: { type: "number", description: "작업 횟수" },
            enabled: { type: "boolean", description: "기능 활성화" },
        },
    };
    return params[tool] || {};
}
```

### 예제: Actor 이름 변경 도구 추가

```cpp
// OpenClawTools.h에 추가
static TSharedPtr<FJsonObject> Actor_Rename(const TSharedPtr<FJsonObject>& Params);

// OpenClawTools.cpp에 추가
TSharedPtr<FJsonObject> FOpenClawTools::Actor_Rename(const TSharedPtr<FJsonObject>& Params)
{
    FString OldName = Params->GetStringField(TEXT("name"));
    FString NewName = Params->GetStringField(TEXT("newName"));
    
    if (OldName.IsEmpty() || NewName.IsEmpty())
    {
        return MakeErrorResult(TEXT("name과 newName이 필요합니다"));
    }
    
    AActor* Actor = FindActorByName(OldName);
    if (!Actor)
    {
        return MakeErrorResult(FString::Printf(TEXT("Actor를 찾을 수 없음: %s"), *OldName));
    }
    
    Actor->SetActorLabel(*NewName);
    
    return MakeSuccessResult(FString::Printf(TEXT("'%s'를 '%s'로 이름 변경"), *OldName, *NewName));
}

// ExecuteTool에 추가
if (ToolName == TEXT("actor.rename")) return Actor_Rename(Params);
```

---

## JSON 처리

### FJsonObject 사용

```cpp
// 값 읽기
FString Str = Params->GetStringField(TEXT("key"));
int32 Num = Params->GetIntegerField(TEXT("key"));
double Dbl = Params->GetNumberField(TEXT("key"));
bool Flag = Params->GetBoolField(TEXT("key"));

// 필드 존재 여부 확인
if (Params->HasField(TEXT("optionalKey")))
{
    // 값 사용
}

// 중첩 객체 읽기
TSharedPtr<FJsonObject> NestedObj = Params->GetObjectField(TEXT("nested"));

// 배열 읽기
const TArray<TSharedPtr<FJsonValue>>& Arr = Params->GetArrayField(TEXT("items"));
for (const auto& Item : Arr)
{
    FString Value = Item->AsString();
}
```

### 결과 객체 생성

```cpp
// 성공 결과
TSharedPtr<FJsonObject> Result = MakeShareable(new FJsonObject());
Result->SetBoolField(TEXT("success"), true);
Result->SetStringField(TEXT("message"), TEXT("완료"));

// 중첩 데이터 포함
TSharedPtr<FJsonObject> DataObj = MakeShareable(new FJsonObject());
DataObj->SetStringField(TEXT("name"), TEXT("MyActor"));
DataObj->SetNumberField(TEXT("x"), 100.0);
Result->SetObjectField(TEXT("data"), DataObj);

// 배열 포함
TArray<TSharedPtr<FJsonValue>> Items;
Items.Add(MakeShareable(new FJsonValueString(TEXT("item1"))));
Items.Add(MakeShareable(new FJsonValueString(TEXT("item2"))));
Result->SetArrayField(TEXT("items"), Items);
```

---

## 플레이 모드 처리

### PIE 감지

```cpp
bool bIsPlaying = GEditor && GEditor->PlayWorld != nullptr;
bool bIsPaused = bIsPlaying && GEditor->PlayWorld->bDebugPauseExecution;
```

### 올바른 World 가져오기

```cpp
UWorld* FOpenClawTools::GetEditorWorld()
{
    if (GEditor)
    {
        // PIE 중에는 플레이 월드가 아닌 에디터 월드 반환
        return GEditor->GetEditorWorldContext().World();
    }
    return nullptr;
}

// PIE 전용 작업을 위해
UWorld* GetPlayWorld()
{
    if (GEditor && GEditor->PlayWorld)
    {
        return GEditor->PlayWorld;
    }
    return nullptr;
}
```

### PIE 중 Actor 작업

```cpp
// 주의: 에디터 월드와 플레이 월드의 액터는 다름
if (GEditor->PlayWorld)
{
    // PIE 중 에디터 월드 액터 수정은 보이지 않을 수 있음
    // 경고 표시 고려
    UE_LOG(LogOpenClaw, Warning, TEXT("PIE 중 액터 수정"));
}
```

---

## 디버깅

### Unreal Output Log

플러그인은 `[OpenClaw]` 접두사로 로그를 출력합니다:

```
LogOpenClaw: [OpenClaw] 🦞 Plugin starting...
LogOpenClaw: Connecting with session ID: unreal_1234567890_abc123
LogOpenClaw: State changed to: Connected
LogOpenClaw: Received command: debug.hierarchy (id: call_123)
```

### 상세 로깅 활성화

프로젝트의 `DefaultEngine.ini`에서:

```ini
[Core.Log]
LogOpenClaw=Verbose
```

### Gateway 로그

```bash
openclaw gateway status
# Unreal 세션 등록 확인
```

### 연결 문제 해결

1. **Gateway 상태 확인**
   ```bash
   openclaw gateway status
   ```

2. **에디터에서 확인**
   - Window → OpenClaw Status
   - Output Log에서 연결 오류 확인

3. **직접 HTTP 테스트**
   ```bash
   curl http://localhost:27742/api/plugin/poll -X POST \
     -H "Content-Type: application/json" \
     -d '{"sessionId":"test","engine":"unreal"}'
   ```

### 일반적인 문제

#### 모듈이 로드되지 않음

- Output Log에서 플러그인 오류 확인
- `.uplugin` 파일이 유효한 JSON인지 확인
- `.Build.cs`의 모든 종속성이 있는지 확인

#### 도구가 실행되지 않음

- 연결이 수립되었는지 확인
- Output Log에서 예외 확인
- 파라미터 타입이 맞는지 확인

---

## 기여 가이드라인

### 코드 스타일

- Unreal Engine 코딩 표준
- 해당되는 경우 `UPROPERTY`, `UFUNCTION` 사용
- 접두사가 있는 명확한 변수 이름 (bool은 b 등)

### 커밋 메시지

```
feat: actor.rename 도구 추가
fix: transform 도구에서 null actor 처리
docs: DEVELOPMENT_KO.md 새 예제로 업데이트
refactor: JSON 응답 생성 간소화
```

### 테스트

제출 전:
1. 에디터 모드에서 테스트
2. PIE 중 테스트
3. 다양한 액터 타입으로 테스트
4. 재연결 동작 확인

### Pull Request

1. feature 브랜치 생성: `feature/your-feature`
2. 변경 사항 작성
3. CHANGELOG.md 업데이트
4. 명확한 설명과 함께 PR 제출

---

## 연락처

- GitHub: https://github.com/openclaw/openclaw-unreal-plugin
- OpenClaw Discord: https://discord.com/invite/clawd
