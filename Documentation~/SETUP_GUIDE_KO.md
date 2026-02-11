# OpenClaw Unreal Plugin - 셋업 가이드

다양한 사용 사례에 맞게 OpenClaw Unreal Plugin을 설정하는 방법을 설명합니다.

## 🤔 어떤 모드가 필요한가요?

| AI 사용 방식 | 필요한 모드 | 이유 |
|-------------|------------|------|
| **채팅 앱** (Telegram, Discord) | 모드 A: Gateway | OpenClaw가 Unreal로 명령 전달 |
| **Claude Code** 터미널에서 | 모드 B: MCP | 직접 연결 필요 |
| **둘 다** | 하이브리드 | 모든 상황 대응 |

### 이미 OpenClaw를 사용 중이라면...

OpenClaw를 통해 AI 어시스턴트와 채팅 중이라면, **MCP 설정이 필요 없습니다** - 어시스턴트가 이미 Gateway를 통해 Unreal 도구에 접근 가능!

```
나 (Telegram) → OpenClaw Gateway → AI 어시스턴트 → unreal_execute 도구 → Unreal
                                   ↑
                            이미 접근 가능!
```

### MCP가 유용한 경우

MCP가 필요한 경우:
1. **Claude Code를 터미널에서 직접 사용**
2. **Claude Desktop** 앱 사용
3. **Cursor** 또는 기타 MCP 호환 에디터 사용

## 🅰️ 모드 A: OpenClaw Gateway (원격 접속)

**언제 사용?** Telegram, Discord, 웹에서 원격으로 게임 개발하고 싶을 때

### 설정 단계

```bash
# 1. OpenClaw 설치
npm install -g openclaw

# 2. Gateway 시작
openclaw gateway start

# 3. Unreal 플러그인 설치
cp -r openclaw-unreal-plugin YourProject/Plugins/OpenClaw

# 4. 플러그인 활성화
#    Edit > Plugins > "OpenClaw" 검색 > Enable > 에디터 재시작

# 5. 채팅 연동 설정 (선택)
openclaw config
```

### 작동 방식

```
┌──────────────┐      ┌─────────────────┐      ┌──────────────┐
│  Telegram/   │ ───→ │    OpenClaw     │ ───→ │   Unreal     │
│  Discord/Web │ ←─── │    Gateway      │ ←─── │   Editor     │
└──────────────┘      └─────────────────┘      └──────────────┘
      폰                  내 컴퓨터               내 컴퓨터
```

### 사용 예시

폰에서:
```
나: "레벨에 있는 Actor 목록 알려줘"
AI: 12개 Actor 발견: PlayerStart, DirectionalLight, Floor, ...

나: "(0, 0, 300)에 PointLight 생성해"
AI: (0, 0, 300)에 PointLight_1 생성 완료

나: "PIE 시작해"
AI: Play In Editor 시작됨
```

---

## 🅱️ 모드 B: MCP 직접 연결 (로컬 개발)

**언제 사용?** Claude Code나 Cursor에서 Unreal을 직접 제어하고 싶을 때

### 설정 단계

```bash
# 1. Unreal 플러그인 설치 (위와 동일)

# 2. MCP 서버 의존성 설치
cd /path/to/openclaw-unreal-plugin/MCP~
npm install

# 3. Claude Code에 MCP 서버 등록
claude mcp add unreal -- node /full/path/to/openclaw-unreal-plugin/MCP~/index.js

# 4. Unreal에서 MCP Bridge 시작
#    Window > OpenClaw > Start MCP Bridge

# 5. Claude Code 사용
claude
```

### 작동 방식

```
┌──────────────┐      ┌─────────────────┐      ┌──────────────┐
│ Claude Code  │ ───→ │   MCP Server    │ ───→ │   Unreal     │
│ 또는 Desktop │ ←─── │   (Node.js)     │ ←─── │   Editor     │
└──────────────┘      └─────────────────┘      └──────────────┘
     터미널            localhost:27184         localhost:27184
```

### 사용 예시

```
$ claude
> Actor 계층 구조 보여줘
AI: 레벨에 15개 Actor 발견...

> (100, 0, 50) 위치에 Cube 스폰해
AI: (100, 0, 50)에 Cube 생성 완료

> PIE 시작하고 W키 눌러
AI: PIE 시작, W키 시뮬레이션 중
```

---

## 🔀 하이브리드 모드 (둘 다 사용)

두 모드를 동시에 사용할 수 있습니다.

| 서비스 | 기본 포트 |
|--------|----------|
| MCP Bridge | 27184 |
| OpenClaw Gateway | 18789 |

---

## 빠른 참조

| 작업 | 명령어 |
|------|--------|
| Gateway 시작 | `openclaw gateway start` |
| MCP 추가 | `claude mcp add unreal -- node /path/to/MCP~/index.js` |
| MCP Bridge 시작 | Window > OpenClaw > Start MCP Bridge |
