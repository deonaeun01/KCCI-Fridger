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
대한상공회의소 AI 시스템반도체 SW개발자 과정에서 **2인 팀 프로젝트**로 진행했습니다.

## 🏗️ 시스템 구성

```
[STM32]  ──Bluetooth──▶  [Arduino]  ──Wi-Fi──▶  [Raspberry Pi]  ──▶  [MariaDB]
 센서 감지                통신 중계 · 재연결          중앙 관리 · LCD 출력      데이터 적재
```

- **STM32**: 냉장고 문 개폐 등 센서 이벤트 감지 및 블루투스 송신
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

## 🔧 개발 환경

- MCU: STM32F411XE
- IDE / Build: STM32CubeIDE, CMake
- RTOS: FreeRTOS
- DB: MariaDB

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
