<h1 align="center">🧊 KCCI-Fridger</h1>
<p align="center"><b>이기종 보드 연동 스마트 냉장고 IoT 모니터링 시스템</b></p>

<p align="center">
  <img src="https://img.shields.io/badge/STM32-03234B?style=flat-square&logo=stmicroelectronics&logoColor=white" />
  <img src="https://img.shields.io/badge/Arduino-00979D?style=flat-square&logo=arduino&logoColor=white" />
  <img src="https://img.shields.io/badge/Raspberry_Pi-A22846?style=flat-square&logo=raspberrypi&logoColor=white" />
  <img src="https://img.shields.io/badge/FreeRTOS-449920?style=flat-square&logo=freertos&logoColor=white" />
  <img src="https://img.shields.io/badge/MariaDB-003545?style=flat-square&logo=mariadb&logoColor=white" />
  <img src="https://img.shields.io/badge/C-99.4%25-blue?style=flat-square" />
</p>

---

## 📖 개요

STM32 · Arduino · Raspberry Pi 이기종 보드를 블루투스/Wi-Fi로 연동하여 냉장고 사용 패턴을 감지, **독거노인 안전 모니터링**을 지원하는 IoT 시스템입니다.
대한상공회의소 AI 시스템반도체 SW개발자 과정에서 **2인 팀 프로젝트**로 진행했습니다. (수행기간: 2026.05 ~ 2026.06)

## 🎯 수행 목표

- STM32·Arduino·Raspberry Pi 이기종 보드를 블루투스·Wi-Fi로 연동한 IoT 모니터링 시스템 구현
- 문 개폐 패턴으로 고독사 위험을 조기 감지하고, 가스 농도로 식자재 부패를 모니터링
- 통신 장애 상황에서도 데이터 전송이 끊기지 않는 안정적 시스템 구축

## 🏗️ 시스템 구성

```
[STM32]  ──Bluetooth──▶  [Arduino]  ──Wi-Fi──▶  [Raspberry Pi]  ──▶  [MariaDB]
 홀센서·가스·온습도 센서      통신 중계 · 재연결          중앙 관리 · LCD 출력      데이터 적재
```

- **STM32**: 홀센서(문 개폐)·가스·온습도 센서 데이터 수집 및 블루투스 송신
- **Arduino**: STM32 ↔ Raspberry Pi 사이 통신 중계, Wi-Fi 클라이언트로 재연결 로직 포함
- **Raspberry Pi**: 수신 데이터를 자체 프로토콜로 파싱, DB 적재, 우선순위 기반 LCD 출력

## ✅ 담당 역할

| 영역 | 내용 |
|---|---|
| 통신 | Arduino ↔ Raspberry Pi 간 Wi-Fi 클라이언트 통신 코드 및 재연결 로직 구현 |
| 통신 | STM32 ↔ Arduino 간 블루투스 통신 코드 수정 및 연동 |
| 프로토콜 | 자체 프로토콜 기반 명령어 파싱 적용 |
| DB | MariaDB 연동 및 DB 초기화 절차 수립 |
| UI | 우선순위 기반 LCD 출력 로직 설계 |

## 🔍 트러블슈팅 — 통신 구간 신뢰성 확보

한 구간이라도 데이터가 끊기거나 밀리면 독거노인 안전 모니터링이라는 시스템 목적 자체가 무너질 수 있어, 각 통신 구간마다 데이터 유실 없는 안정적 전달을 최우선 목표로 개발했습니다.

- **블루투스 패킷 유실**: STM32에서 패킷을 지연 없이 연속 전송하면서 Raspberry Pi 수신 측 UART 버퍼가 이전 패킷을 비우기 전에 다음 패킷이 덮어쓰는 문제 발생 → 패킷 전송 사이에 `osDelay(100)`을 삽입해 수신 버퍼 처리 시간을 확보, 이후 패킷 유실 없이 안정적으로 수신되는 것을 확인
- **Wi-Fi 연결 끊김**: 약 30분 연속 구동 테스트에서 연결이 예상보다 자주 끊겨, 재연결 로직만으로는 그 사이 이벤트가 유실될 수 있는 한계를 확인 → STM32에 RTC 모듈을 추가하고, RTC 기반 이벤트 큐 로직을 통신·DB 파이프라인에 통합해 끊긴 동안의 이벤트가 시각 정보와 함께 저장됐다가 재연결 시 순차 복원되는 구조를 완성

## 🔧 개발 환경

- MCU: STM32F411XE
- IDE / Build: STM32CubeIDE, CMake
- RTOS: FreeRTOS
- DB: MariaDB

- ## 🖥️ Raspberry Pi 소켓 통신 코드

> `iot_server.c` / `iot_client.c`는 교육과정에서 제공된 베이스 코드(중계 서버/클라이언트 템플릿)입니다.
> `iot_client_bluetooth.c`가 제가 직접 작성한 부분으로, STM32/Arduino에서 블루투스로 들어오는 센서 데이터를
> 자체 프로토콜로 파싱해 `[KJM_SQL]` 태그를 붙여 TCP로 서버에 재전송하는 로직입니다.

## 📂 프로젝트 구조

```
KCCI-Fridger/
├── Core/                                   # STM32 메인 애플리케이션 코드
├── Drivers/                                # HAL / 보드 드라이버
├── Middlewares/Third_Party/FreeRTOS/       # FreeRTOS 소스
├── MyApp/                                  # 사용자 정의 애플리케이션 로직
├── cmake/                                  # CMake 빌드 설정
├── Fridger.ioc                             # STM32CubeMX 설정 파일
└── CMakeLists.txt
```

## 🚀 빌드 방법

```bash
git clone https://github.com/deonaeun01/KCCI-Fridger.git
cd KCCI-Fridger
# STM32CubeIDE로 프로젝트 열기 또는
cmake --preset <preset-name>
cmake --build build
```

## 🙌 팀 구성

- 2인 팀 프로젝트 (대한상공회의소 AI 시스템반도체 SW개발자 과정)
