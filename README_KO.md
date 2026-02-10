# 🦞 OpenClaw Unreal Plugin

AI 지원 Unreal Engine 개발. 에디터 제어, Actor 조작, 입력 시뮬레이션, 자연어를 통한 워크플로우 자동화.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.x-blue)](https://www.unrealengine.com/)
[![Discord](https://img.shields.io/discord/1234567890?color=7289da&label=Discord&logo=discord&logoColor=white)](https://discord.com/invite/clawd)

## 기능

- 🎮 **에디터 제어** - PIE 시작/중지, 일시정지/재개
- 🎭 **Actor 관리** - Actor 생성, 찾기, 삭제, 수정
- 📐 **Transform 도구** - 위치, 회전, 스케일 조작
- 🖼️ **스크린샷** - 뷰포트 이미지 캡처
- ⌨️ **입력 시뮬레이션** - 키보드, 마우스, 게임패드 시뮬레이션
- 📦 **에셋 탐색** - 프로젝트 콘텐츠 나열 및 탐색
- 🔧 **콘솔 명령** - Unreal 콘솔 명령 실행

## 빠른 시작

### 1. 플러그인 설치

프로젝트의 Plugins 폴더에 복사:

```bash
cp -r openclaw-unreal-plugin YourProject/Plugins/OpenClaw
```

### 2. 플러그인 활성화

1. Unreal 프로젝트 열기
2. Edit → Plugins로 이동
3. "OpenClaw" 검색
4. 플러그인 활성화
5. 에디터 재시작

### 3. OpenClaw 연결

플러그인은 시작 시 자동으로 OpenClaw Gateway에 연결됩니다.

연결 확인: Window → OpenClaw Status

## 설정

프로젝트 루트에 `openclaw.json` 생성:

```json
{
  "host": "127.0.0.1",
  "port": 27742,
  "autoConnect": true,
  "secret": "optional-secret-key"
}
```

또는 글로벌 설정을 위해 `~/.openclaw/unreal-plugin.json`에 저장.

## 사용 가능한 도구 (40+)

### Level 관리
| 도구 | 설명 |
|------|------|
| `level.getCurrent` | 현재 레벨 정보 가져오기 |
| `level.list` | 프로젝트의 모든 레벨 나열 |
| `level.open` | 경로로 레벨 열기 |
| `level.save` | 현재 레벨 저장 |

### Actor 도구
| 도구 | 설명 |
|------|------|
| `actor.find` | 이름으로 Actor 찾기 |
| `actor.getAll` | 레벨의 모든 Actor 가져오기 |
| `actor.create` | 새 Actor 스폰 |
| `actor.delete` | Actor 제거 |
| `actor.getData` | 상세 Actor 정보 가져오기 |
| `actor.setProperty` | Actor 속성 수정 |

### Transform 도구
| 도구 | 설명 |
|------|------|
| `transform.getPosition` | Actor 월드 위치 가져오기 |
| `transform.setPosition` | Actor 위치 설정 |
| `transform.getRotation` | Actor 회전 가져오기 |
| `transform.setRotation` | Actor 회전 설정 |
| `transform.getScale` | Actor 스케일 가져오기 |
| `transform.setScale` | Actor 스케일 설정 |

### Component 도구
| 도구 | 설명 |
|------|------|
| `component.get` | Actor 컴포넌트 나열 |
| `component.add` | Actor에 컴포넌트 추가 |
| `component.remove` | 컴포넌트 제거 |

### Editor 제어
| 도구 | 설명 |
|------|------|
| `editor.play` | Play in Editor 시작 |
| `editor.stop` | PIE 세션 중지 |
| `editor.pause` | 실행 일시정지 |
| `editor.resume` | 실행 재개 |
| `editor.getState` | 현재 상태 가져오기 |

### Debug 도구
| 도구 | 설명 |
|------|------|
| `debug.hierarchy` | World Actor 계층 구조 가져오기 |
| `debug.screenshot` | 스크린샷 캡처 |
| `debug.log` | Output에 메시지 로그 |

### Input 시뮬레이션
| 도구 | 설명 |
|------|------|
| `input.simulateKey` | 키보드 입력 시뮬레이션 |
| `input.simulateMouse` | 마우스 입력 시뮬레이션 |
| `input.simulateAxis` | 축/게임패드 시뮬레이션 |

### Asset 도구
| 도구 | 설명 |
|------|------|
| `asset.list` | 경로의 에셋 나열 |
| `asset.import` | 외부 에셋 가져오기 |

### Console
| 도구 | 설명 |
|------|------|
| `console.execute` | 콘솔 명령 실행 |
| `console.getLogs` | Output 로그 가져오기 |

### Blueprint
| 도구 | 설명 |
|------|------|
| `blueprint.list` | 블루프린트 나열 |
| `blueprint.open` | 에디터에서 열기 |

## 예제 상호작용

```
사용자: 레벨의 모든 Actor를 보여줘
AI: [debug.hierarchy 실행]
    15개 Actor 발견: PlayerStart, DirectionalLight, ...

사용자: (0, 0, 300)에 포인트 라이트 만들어
AI: [actor.create type="PointLight" x=0 y=0 z=300 실행]
    위치 (0, 0, 300)에 PointLight_1 생성됨

사용자: 게임 시작하고 앞으로 이동해
AI: [editor.play 실행 후 input.simulateKey key="W"]
    PIE 시작, W 키 입력 시뮬레이션

사용자: 스크린샷 찍어
AI: [debug.screenshot 실행]
    스크린샷 저장: Saved/Screenshots/screenshot_20260211_141500.png
```

## 요구 사항

- Unreal Engine 5.0 이상
- Windows, macOS, 또는 Linux
- OpenClaw Gateway 실행 중

## 문서

- [개발 가이드](Documentation~/DEVELOPMENT_KO.md)
- [테스트 가이드](Documentation~/TESTING_KO.md)
- [기여 가이드](Documentation~/CONTRIBUTING_KO.md)

## 문제 해결

### 플러그인이 로드되지 않음
- Output Log에서 오류 확인
- 플러그인이 `Plugins/OpenClaw/` 폴더에 있는지 확인
- `OpenClaw.uplugin`이 존재하는지 확인

### Gateway에 연결되지 않음
- Gateway 실행 확인: `openclaw gateway status`
- 방화벽이 포트 27742를 허용하는지 확인
- Output Log에서 `[OpenClaw]` 메시지 확인

### 도구가 실행되지 않음
- 연결 확인 (Window → OpenClaw Status)
- Output Log에서 도구 실행 오류 확인
- 에디터가 바쁜 상태가 아닌지 확인

## 지원

- 📖 [문서](https://docs.openclaw.ai)
- 💬 [Discord](https://discord.com/invite/clawd)
- 🐛 [이슈](https://github.com/openclaw/openclaw-unreal-plugin/issues)

## 라이선스

MIT 라이선스 - [LICENSE](LICENSE) 파일 참조.

---

🦞 OpenClaw 커뮤니티 제작
