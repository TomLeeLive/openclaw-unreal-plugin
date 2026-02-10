# 🧪 OpenClaw Unreal Plugin - 테스트 가이드

이 문서는 OpenClaw Unreal Plugin의 테스트 가이드입니다. 40개 이상의 도구에 대한 사용법과 예제를 포함합니다.

## 목차

1. [테스트 환경 설정](#테스트-환경-설정)
2. [기본 연결 테스트](#기본-연결-테스트)
3. [Level 도구](#level-도구)
4. [Actor 도구](#actor-도구)
5. [Transform 도구](#transform-도구)
6. [Component 도구](#component-도구)
7. [Editor 도구](#editor-도구)
8. [Debug 도구](#debug-도구)
9. [Input 도구](#input-도구)
10. [Asset 도구](#asset-도구)
11. [Console 도구](#console-도구)
12. [Blueprint 도구](#blueprint-도구)
13. [자동화 테스트 시나리오](#자동화-테스트-시나리오)

---

## 테스트 환경 설정

### 1. OpenClaw Gateway 시작

```bash
# Gateway 상태 확인
openclaw gateway status

# Gateway 시작 (필요시)
openclaw gateway start
```

### 2. Unreal 프로젝트 준비

1. OpenClaw Unreal Plugin 설치 (README.md 참조)
2. Unreal 프로젝트 열기
3. Output Log에서 `[OpenClaw] 🦞 Plugin starting...` 확인
4. Window → OpenClaw Status로 연결 확인

### 3. 테스트 세션 확인

Output Log에서 확인:
```
LogOpenClaw: State changed to: Connected
LogOpenClaw: Session ID: unreal_1234567890_abc123
```

---

## 기본 연결 테스트

### 플러그인 상태 확인

**설명:** 플러그인이 실행 중이고 연결되어 있는지 확인합니다.

**방법:** Window → OpenClaw Status

**예상:** "Connected to OpenClaw Gateway" 대화상자 표시

---

## Level 도구

### level.getCurrent - 현재 레벨 가져오기

**설명:** 현재 열린 레벨의 정보를 반환합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "현재 열려있는 레벨이 뭐야?"
```

**응답 예시:**
```json
{
  "success": true,
  "name": "Main",
  "path": "/Game/Maps/Main.Main"
}
```

---

### level.list - 모든 레벨 목록

**설명:** 프로젝트의 모든 레벨 목록을 반환합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "프로젝트의 모든 레벨을 보여줘"
```

**응답 예시:**
```json
{
  "success": true,
  "levels": [
    { "name": "Main", "path": "/Game/Maps/Main" },
    { "name": "Menu", "path": "/Game/Maps/Menu" },
    { "name": "TestLevel", "path": "/Game/Maps/TestLevel" }
  ]
}
```

---

### level.open - 레벨 열기

**설명:** 경로로 레벨을 엽니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| path | string | ✓ | 레벨 에셋 경로 |

**예제:**
```
OpenClaw에게: "Main 레벨을 열어줘"
```

---

### level.save - 현재 레벨 저장

**설명:** 현재 레벨을 저장합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "현재 레벨을 저장해"
```

---

## Actor 도구

### actor.find - Actor 찾기

**설명:** 이름이나 라벨로 Actor를 찾습니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 또는 라벨 |

**예제:**
```
OpenClaw에게: "PlayerStart actor를 찾아줘"
```

**응답 예시:**
```json
{
  "success": true,
  "actor": {
    "name": "PlayerStart",
    "label": "PlayerStart",
    "class": "PlayerStart",
    "transform": {
      "position": { "x": 0, "y": 0, "z": 100 },
      "rotation": { "pitch": 0, "yaw": 0, "roll": 0 },
      "scale": { "x": 1, "y": 1, "z": 1 }
    },
    "components": [
      { "name": "CollisionComponent", "class": "CapsuleComponent" },
      { "name": "GoodSprite", "class": "BillboardComponent" }
    ]
  }
}
```

---

### actor.getAll - 모든 Actor 가져오기

**설명:** 현재 레벨의 모든 Actor를 반환합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "레벨의 모든 액터를 보여줘"
```

**응답 예시:**
```json
{
  "success": true,
  "count": 15,
  "actors": [
    { "name": "PlayerStart", "class": "PlayerStart", "transform": {...} },
    { "name": "DirectionalLight", "class": "DirectionalLight", "transform": {...} },
    { "name": "Floor", "class": "StaticMeshActor", "transform": {...} }
  ]
}
```

---

### actor.create - Actor 생성

**설명:** 레벨에 새 Actor를 스폰합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| type | string | | Actor 타입 (Cube, PointLight, Camera 등) |
| name | string | | Actor 라벨 |
| x | number | | X 위치 |
| y | number | | Y 위치 |
| z | number | | Z 위치 |

**예제:**
```
OpenClaw에게: "위치 100, 200, 50에 큐브를 만들어줘"
```

**응답 예시:**
```json
{
  "success": true,
  "message": "Created actor: Cube_1",
  "actor": {
    "name": "Cube_1",
    "class": "StaticMeshActor",
    "transform": {
      "position": { "x": 100, "y": 200, "z": 50 }
    }
  }
}
```

**지원되는 타입:**
- `Cube` / `StaticMeshActor` - 스태틱 메시 큐브
- `PointLight` - 포인트 라이트
- `Camera` - 카메라 액터
- (기본값) - 빈 액터

---

### actor.delete / actor.destroy - Actor 삭제

**설명:** 레벨에서 Actor를 제거합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |

**예제:**
```
OpenClaw에게: "Cube_1 액터를 삭제해"
```

---

### actor.getData - Actor 상세 정보

**설명:** 컴포넌트를 포함한 Actor의 상세 정보를 반환합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |

**예제:**
```
OpenClaw에게: "PlayerStart 액터의 상세 정보를 알려줘"
```

---

## Transform 도구

### transform.getPosition - 위치 가져오기

**설명:** Actor의 월드 위치를 반환합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |

**예제:**
```
OpenClaw에게: "PlayerStart가 어디에 있어?"
```

**응답 예시:**
```json
{
  "success": true,
  "x": 0,
  "y": 0,
  "z": 100
}
```

---

### transform.setPosition - 위치 설정

**설명:** Actor의 월드 위치를 설정합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |
| x | number | | X 위치 |
| y | number | | Y 위치 |
| z | number | | Z 위치 |

**예제:**
```
OpenClaw에게: "PlayerStart를 500, 0, 100으로 이동해"
```

---

### transform.getRotation / setRotation

**설명:** Actor 회전을 가져오거나 설정합니다 (pitch, yaw, roll, 도 단위).

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |
| pitch | number | | Pitch 각도 (도) |
| yaw | number | | Yaw 각도 (도) |
| roll | number | | Roll 각도 (도) |

---

### transform.getScale / setScale

**설명:** Actor 스케일을 가져오거나 설정합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| name | string | ✓ | Actor 이름 |
| x | number | | X 스케일 |
| y | number | | Y 스케일 |
| z | number | | Z 스케일 |

---

## Component 도구

### component.get - 컴포넌트 목록

**설명:** Actor의 컴포넌트를 나열합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| actor | string | ✓ | Actor 이름 |
| component | string | | 컴포넌트 이름 필터 |

**예제:**
```
OpenClaw에게: "PlayerStart에 어떤 컴포넌트가 있어?"
```

**응답 예시:**
```json
{
  "success": true,
  "components": [
    { "name": "CollisionComponent", "class": "CapsuleComponent" },
    { "name": "GoodSprite", "class": "BillboardComponent" },
    { "name": "ArrowComponent", "class": "ArrowComponent" }
  ]
}
```

---

## Editor 도구

### editor.play - PIE 시작

**설명:** Play in Editor를 시작합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "게임 시작해"
```

**응답 예시:**
```json
{
  "success": true,
  "message": "Started play mode"
}
```

---

### editor.stop - PIE 중지

**설명:** Play in Editor를 중지합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "게임 중지해"
```

---

### editor.pause / resume

**설명:** PIE 중 게임 실행을 일시정지하거나 재개합니다.

**예제:**
```
OpenClaw에게: "게임 일시정지해"
OpenClaw에게: "게임 재개해"
```

---

### editor.getState - 에디터 상태 가져오기

**설명:** 현재 에디터 상태를 반환합니다.

**파라미터:** 없음

**예제:**
```
OpenClaw에게: "게임이 실행 중이야?"
```

**응답 예시:**
```json
{
  "success": true,
  "isPlaying": true,
  "isPaused": false,
  "state": "playing"
}
```

**상태 값:**
- `editing` - 일반 에디터 모드
- `playing` - PIE 활성
- `paused` - PIE 일시정지

---

## Debug 도구

### debug.hierarchy - Actor 계층 구조

**설명:** 레벨의 모든 Actor 트리 뷰를 반환합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| depth | number | | 탐색할 최대 깊이 |

**예제:**
```
OpenClaw에게: "레벨 계층 구조를 보여줘"
```

**응답 예시:**
```json
{
  "success": true,
  "level": "Main",
  "actorCount": 15,
  "actors": [
    { "name": "PlayerStart", "class": "PlayerStart", ... },
    { "name": "Floor", "class": "StaticMeshActor", ... }
  ]
}
```

---

### debug.screenshot - 스크린샷 촬영

**설명:** 뷰포트 스크린샷을 캡처합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| filename | string | | 사용자 지정 파일명 |

**예제:**
```
OpenClaw에게: "스크린샷 찍어줘"
```

**응답 예시:**
```json
{
  "success": true,
  "path": "/Saved/Screenshots/screenshot_20260211_145000.png",
  "message": "Screenshot requested"
}
```

---

### debug.log - 메시지 로깅

**설명:** Unreal 로그에 메시지를 출력합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| message | string | ✓ | 로그할 메시지 |
| level | string | | 로그 레벨 (log/warning/error) |

**예제:**
```
OpenClaw에게: "'테스트 메시지'를 경고로 로그해"
```

---

## Input 도구

### input.simulateKey - 키보드 입력

**설명:** 키보드 입력을 시뮬레이션합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| key | string | ✓ | 키 이름 (W, A, S, D, Space 등) |
| pressed | boolean | | 누름은 true, 뗌은 false |

**예제:**
```
OpenClaw에게: "W 키를 눌러"
```

---

### input.simulateMouse - 마우스 입력

**설명:** 마우스 입력을 시뮬레이션합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| action | string | ✓ | 동작 (click/move/scroll) |
| x | number | | X 좌표 |
| y | number | | Y 좌표 |
| button | string | | 버튼 (left/right/middle) |

---

### input.simulateAxis - 축 입력

**설명:** 게임패드/축 입력을 시뮬레이션합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| axis | string | ✓ | 축 이름 |
| value | number | ✓ | 값 (-1 ~ 1) |

---

## Asset 도구

### asset.list - 에셋 목록

**설명:** 경로의 에셋을 나열합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| path | string | | 에셋 경로 (기본값: /Game) |
| type | string | | 에셋 타입 필터 |

**예제:**
```
OpenClaw에게: "/Game/Blueprints의 모든 에셋을 보여줘"
```

---

## Console 도구

### console.execute - 명령 실행

**설명:** Unreal 콘솔 명령을 실행합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| command | string | ✓ | 콘솔 명령 |

**예제:**
```
OpenClaw에게: "'stat fps' 명령을 실행해"
```

---

## Blueprint 도구

### blueprint.list - 블루프린트 목록

**설명:** 경로의 모든 블루프린트를 나열합니다.

**파라미터:**
| 이름 | 타입 | 필수 | 설명 |
|------|------|------|------|
| path | string | | 검색 경로 (기본값: /Game) |

**예제:**
```
OpenClaw에게: "모든 블루프린트를 보여줘"
```

---

## 자동화 테스트 시나리오

### 시나리오 1: 레벨 설정

```
1. "TestLevel을 열어"
2. "0, 0, 100에 큐브를 만들어"
3. "0, 0, 300에 포인트 라이트를 만들어"
4. "레벨을 저장해"
5. "스크린샷 찍어"
```

### 시나리오 2: Actor 조작

```
1. "레벨의 모든 액터를 보여줘"
2. "Cube_1을 찾아"
3. "Cube_1을 200, 0, 100으로 이동해"
4. "Cube_1을 yaw 45도로 회전해"
5. "Cube_1을 2, 2, 2로 스케일 조정해"
```

### 시나리오 3: PIE 테스트

```
1. "게임 시작해"
2. "2초 기다려"
3. "스크린샷 찍어"
4. "게임 일시정지해"
5. "에디터 상태를 알려줘"
6. "게임 중지해"
```

---

## 문제 해결

### "Actor not found" 오류

- World Outliner에서 정확한 Actor 이름/라벨 확인
- 이름은 대소문자 구분
- PIE 중에는 에디터 Actor에 접근 불가능할 수 있음

### 스크린샷이 저장되지 않음

- Saved/Screenshots 디렉토리 존재 확인
- 쓰기 권한 확인
- Output Log에서 오류 확인

### 입력 시뮬레이션이 작동하지 않음

- 입력 시뮬레이션은 PIE 활성화 필요할 수 있음
- 일부 입력은 게임 뷰포트 포커스 필요
- Enhanced Input이 시뮬레이션된 입력 차단하는지 확인
