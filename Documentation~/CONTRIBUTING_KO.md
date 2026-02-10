# 🤝 기여 가이드

OpenClaw Unreal Plugin에 기여해 주셔서 감사합니다!

## 목차

1. [기여 방법](#기여-방법)
2. [개발 환경 설정](#개발-환경-설정)
3. [코드 기여](#코드-기여)
4. [Pull Request 가이드라인](#pull-request-가이드라인)
5. [이슈 리포팅](#이슈-리포팅)
6. [코드 리뷰 프로세스](#코드-리뷰-프로세스)
7. [커뮤니티 가이드라인](#커뮤니티-가이드라인)

---

## 기여 방법

### 기여 분야

| 분야 | 설명 | 난이도 |
|------|------|--------|
| 🐛 버그 수정 | 문제 해결, 안정성 개선 | ⭐ |
| 📝 문서화 | README, 예제, 번역 | ⭐ |
| 🔧 새 도구 | OpenClawTools.cpp에 기능 추가 | ⭐⭐ |
| ⚡ 성능 | 최적화, 메모리 관리 | ⭐⭐⭐ |
| 🏗️ 아키텍처 | 핵심 구조 변경 | ⭐⭐⭐ |

### 첫 기여자를 위한 추천 이슈

GitHub에서 `good first issue` 라벨 확인:

```
https://github.com/openclaw/openclaw-unreal-plugin/labels/good%20first%20issue
```

---

## 개발 환경 설정

### 1. Fork & Clone

```bash
# 1. GitHub에서 Fork

# 2. Clone
git clone https://github.com/YOUR_USERNAME/openclaw-unreal-plugin.git
cd openclaw-unreal-plugin

# 3. upstream 리모트 추가
git remote add upstream https://github.com/openclaw/openclaw-unreal-plugin.git
```

### 2. 테스트 환경 설정

```bash
# 테스트용 Unreal 프로젝트 생성
# Plugins 폴더에 플러그인 복사
cp -r openclaw-unreal-plugin YourTestProject/Plugins/OpenClaw

# OpenClaw Gateway 시작
openclaw gateway start
```

### 3. 브랜치 전략

```bash
# 기능 개발
git checkout -b feature/audio-tools

# 버그 수정
git checkout -b fix/connection-timeout

# 문서화
git checkout -b docs/korean-translation
```

---

## 코드 기여

### 예제 1: 새 도구 추가 (`material.set`)

#### 1단계: 이슈 확인/생성

```markdown
## 기능 요청: material.set 도구

### 설명
런타임에 Actor 머티리얼을 변경하는 도구

### 사용 사례
- AI가 시각적 외형 테스트
- 빠른 머티리얼 반복 작업

### 제안 API
material.set {actor: "Cube_1", slot: 0, material: "/Game/Materials/M_Red"}
material.get {actor: "Cube_1"}
```

#### 2단계: 도구 구현

```cpp
// OpenClawTools.h에 추가
static TSharedPtr<FJsonObject> Material_Set(const TSharedPtr<FJsonObject>& Params);
static TSharedPtr<FJsonObject> Material_Get(const TSharedPtr<FJsonObject>& Params);

// OpenClawTools.cpp에 추가
TSharedPtr<FJsonObject> FOpenClawTools::Material_Set(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName = Params->GetStringField(TEXT("actor"));
    int32 SlotIndex = Params->HasField(TEXT("slot")) ? Params->GetIntegerField(TEXT("slot")) : 0;
    FString MaterialPath = Params->GetStringField(TEXT("material"));
    
    AActor* Actor = FindActorByName(ActorName);
    if (!Actor)
    {
        return MakeErrorResult(FString::Printf(TEXT("Actor를 찾을 수 없음: %s"), *ActorName));
    }
    
    UStaticMeshComponent* MeshComp = Actor->FindComponentByClass<UStaticMeshComponent>();
    if (!MeshComp)
    {
        return MakeErrorResult(TEXT("Actor에 StaticMeshComponent가 없음"));
    }
    
    UMaterial* Material = LoadObject<UMaterial>(nullptr, *MaterialPath);
    if (!Material)
    {
        return MakeErrorResult(FString::Printf(TEXT("머티리얼을 찾을 수 없음: %s"), *MaterialPath));
    }
    
    MeshComp->SetMaterial(SlotIndex, Material);
    
    return MakeSuccessResult(FString::Printf(TEXT("%s의 슬롯 %d에 머티리얼 설정"), *ActorName, SlotIndex));
}

// ExecuteTool에 등록
if (ToolName == TEXT("material.set")) return Material_Set(Params);
if (ToolName == TEXT("material.get")) return Material_Get(Params);
```

#### 3단계: Extension 업데이트

```typescript
// extension/index.ts에 추가
const TOOLS = [
    // ... 기존 도구들
    "material.set",
    "material.get",
] as const;

// 설명과 파라미터 추가
```

#### 4단계: 테스트 작성

PR에 테스트 케이스 문서화:
```markdown
## 테스트 케이스

1. 기존 Actor에 머티리얼 설정 ✅
2. Actor가 없을 때 오류 ✅
3. 머티리얼 경로가 잘못되었을 때 오류 ✅
4. PIE 중 작동 ✅
```

---

## Pull Request 가이드라인

### PR 제목 형식

```
feat: material.set 및 material.get 도구 추가
fix: transform 도구에서 null 컴포넌트 처리
docs: TESTING.md 한국어 번역 추가
refactor: JSON 응답 헬퍼 간소화
```

### PR 템플릿

```markdown
## 설명
변경 사항 간단 설명

## 변경 유형
- [ ] 버그 수정
- [ ] 새 기능
- [ ] 문서화
- [ ] 리팩토링

## 테스트
- [ ] 에디터 모드에서 테스트
- [ ] PIE 중 테스트
- [ ] 문서 추가/업데이트

## 관련 이슈
Closes #123

## 스크린샷 (해당되는 경우)
```

### 제출 전 체크리스트

- [ ] 경고 없이 컴파일됨
- [ ] Unreal Engine 코딩 표준 준수
- [ ] 문서 추가/업데이트
- [ ] CHANGELOG.md 업데이트
- [ ] 모든 테스트 통과

---

## 이슈 리포팅

### 버그 리포트 템플릿

```markdown
## 버그 설명
명확하고 간결한 설명

## 재현 단계
1. Unreal Editor 열기
2. OpenClaw 플러그인 활성화
3. 'actor.create' 명령 실행
4. 오류 발생

## 예상 동작
일어나야 할 것

## 실제 동작
실제로 일어난 것

## 환경
- Unreal Engine 버전: 5.3
- OpenClaw Plugin 버전: 0.9.0
- OS: Windows 11 / macOS 14.x
- OpenClaw Gateway 버전: x.x.x

## 로그
```
관련 Output Log 항목
```

## 스크린샷
해당되는 경우
```

### 기능 요청 템플릿

```markdown
## 기능 설명
기능에 대한 명확한 설명

## 사용 사례
- 사용 사례 1
- 사용 사례 2

## 제안 API
```
tool.name {param1: value, param2: value}
```

## 추가 정보
기타 정보
```

---

## 코드 리뷰 프로세스

### 리뷰 기준

1. **기능성**: 의도대로 작동하는가?
2. **코드 품질**: 깔끔하고, 가독성 있고, 표준을 따르는가?
3. **성능**: 불필요한 오버헤드 없는가?
4. **안전성**: 적절한 null 체크, 오류 처리?
5. **문서화**: 문서와 주석 업데이트?

### 리뷰 일정

- 초기 응답: 48시간 이내
- 리뷰 완료: 1주일 이내
- 승인 후 머지: 24시간 이내

### 피드백 대응

1. 모든 코멘트에 응답
2. 수정 사항은 새 커밋으로 푸시 (리뷰하기 쉬움)
3. 준비되면 재리뷰 요청
4. 요청 시 머지 전 커밋 스쿼시

---

## 커뮤니티 가이드라인

### 행동 강령

- 존중하고 포용적으로
- 건설적인 피드백 제공
- 새로운 분들이 배우도록 도움
- 사람이 아닌 코드에 집중

### 소통 채널

- **GitHub Issues**: 버그 리포트, 기능 요청
- **GitHub Discussions**: 일반 질문, 아이디어
- **Discord**: 실시간 채팅, 커뮤니티 지원

### 인정

기여자는 다음에서 인정됩니다:
- 중요한 기여에 대해 CHANGELOG.md
- README.md 기여자 섹션
- 릴리스 노트

---

## 도움 받기

### 리소스

- [개발 가이드](DEVELOPMENT_KO.md)
- [테스트 가이드](TESTING_KO.md)
- [OpenClaw 문서](https://docs.openclaw.ai)

### 연락처

- GitHub: https://github.com/openclaw/openclaw-unreal-plugin
- Discord: https://discord.com/invite/clawd

---

OpenClaw를 더 좋게 만드는 데 도움 주셔서 감사합니다! 🦞
