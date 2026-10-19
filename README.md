# DeviceTester

### C++/Qt-based Hardware Interface Manager

DeviceTester는 다양한 하드웨어 장치를 검색하고 연결 상태를 관리하기 위해 개발한 **C++/Qt 기반 데스크톱 애플리케이션**입니다.

Serial, VISA, NI-DAQmx 인터페이스를 하나의 GUI에서 관리할 수 있도록 구성했으며, 장치 검색과 연결 관리 기능을 분리하여 유지보수와 기능 확장을 고려했습니다.

## 1. 프로젝트 개요

| 항목 | 내용 |
|---|---|
| 프로젝트명 | DeviceTester |
| 개발 형태 | 개인 프로젝트 |
| 개발 언어 | C++ |
| GUI 프레임워크 | Qt Widgets |
| 개발 환경 | Visual Studio / Windows |
| 주요 기술 | Qt SerialPort, VISA, NI-DAQmx |
| 프로젝트 유형 | 하드웨어 인터페이스 관리 프로그램 |

### 개발 목적

하드웨어 장치마다 서로 다른 통신 인터페이스와 연결 방식이 사용됩니다.

DeviceTester는 이러한 장치들을 하나의 프로그램에서 검색하고 연결할 수 있도록 구현한 프로젝트입니다.

특히 장치별 검색 로직과 연결 관리 로직을 분리하고, 인터페이스 유형에 따라 통신 설정 화면이 변경되도록 구성했습니다.

## 2. 주요 기능

### 2.1 하드웨어 장치 검색

- Serial(COM Port) 장치 검색
- VISA 리소스 검색
- NI-DAQmx 장치 검색
- 검색된 장치 목록 및 기본 정보 표시

### 2.2 장치 연결 관리

- 선택한 장치의 인터페이스 유형 확인
- 인터페이스별 연결 처리
- 장치 연결 및 연결 해제
- 연결 상태에 따른 GUI 표시 변경

### 2.3 인터페이스별 통신 설정

**Serial**

- Baud Rate
- Data Bits
- Parity
- Stop Bits

**VISA**

- VISA Resource
- Timeout 설정

**NI-DAQmx**

- NI-DAQ 장치 식별 및 연결 관리

### 2.4 GUI

Qt Widgets 기반으로 장치 검색 영역과 통신 설정 영역을 구성했습니다.

선택한 장치의 인터페이스 유형에 따라 적절한 설정 화면을 표시하도록 설계했습니다.

## 3. 프로젝트 구조

```text
DeviceTester/
├── DeviceTester.cpp
├── DeviceTester.h
├── DeviceTester.ui
├── DeviceInfo.cpp
├── DeviceInfo.h
│
├── SerialScanner.cpp
├── SerialScanner.h
├── VisaScanner.cpp
├── VisaScanner.h
├── NiDaqScanner.cpp
├── NiDaqScanner.h
│
├── VisaConnection.cpp
├── VisaConnection.h
├── NiDaqConnection.cpp
├── NiDaqConnection.h
│
├── main.cpp
├── DeviceTester.qrc
├── DeviceTester.slnx
└── DeviceTester.vcxproj
```

### 주요 모듈 역할

| 모듈 | 역할 |
|---|---|
| DeviceTester | 메인 GUI 및 사용자 이벤트 처리 |
| DeviceInfo | 장치 정보 데이터 관리 |
| SerialScanner | Serial 장치 검색 |
| VisaScanner | VISA 리소스 검색 |
| NiDaqScanner | NI-DAQmx 장치 검색 |
| VisaConnection | VISA 세션 및 연결 관리 |
| NiDaqConnection | NI-DAQmx 연결 관리 |

## 4. 프로그램 동작 흐름

```text
프로그램 실행
      ↓
장치 검색 (Scan)
      ↓
검색된 장치 목록 표시
      ↓
장치 선택
      ↓
인터페이스 유형 확인
      ↓
통신 설정 화면 표시
      ↓
장치 연결 (Connect)
      ↓
연결 상태 표시
      ↓
연결 해제 (Disconnect)
```

## 5. 주요 구현 내용

### 5.1 인터페이스별 기능 분리

Serial, VISA, NI-DAQmx 장치 검색 기능을 각각 별도의 클래스로 구성했습니다.

이를 통해 장치 검색 기능과 GUI 처리 로직을 구분하고, 인터페이스별 기능을 독립적으로 관리할 수 있도록 했습니다.

### 5.2 장치 정보 구조화

장치명, 인터페이스 유형, 제조사, 모델명, 리소스 정보 등을 `DeviceInfo`를 통해 관리하도록 구성했습니다.

검색된 장치 정보를 공통 형식으로 전달하여 GUI에서 일관된 방식으로 표시할 수 있도록 했습니다.

### 5.3 연결 상태 관리

장치 연결 및 연결 해제 과정에서 연결 상태를 확인하고, 해당 상태를 GUI에 반영하도록 구현했습니다.

### 5.4 인터페이스별 GUI 전환

Qt의 `QStackedWidget`을 활용하여 선택한 인터페이스에 따라 Serial, VISA, NI-DAQmx 설정 화면을 전환하도록 구성했습니다.

## 6. 개발 환경 및 의존성

- Windows
- Visual Studio
- C++
- Qt Widgets
- Qt SerialPort
- VISA 라이브러리
- NI-DAQmx 드라이버 및 SDK

VISA 및 NI-DAQmx 기능을 사용하려면 관련 라이브러리와 드라이버가 설치되어 있어야 합니다.

실제 장치 검색 및 연결 기능은 연결된 하드웨어와 개발 환경에 따라 동작이 달라질 수 있습니다.

## 7. 실행 화면

프로그램 실행 화면은 추후 추가할 예정입니다.

## 8. 향후 개선 사항

- 장치별 통신 테스트 기능 확장
- 송수신 데이터 로그 기능
- 장치 연결 오류 처리 개선
- 인터페이스별 기능 확장
- 코드 구조 및 예외 처리 개선

## 9. 프로젝트를 통해 학습한 내용

본 프로젝트를 개발하면서 다음 내용을 학습하고 적용했습니다.

- C++ 클래스 및 객체를 활용한 기능 분리
- Qt Widgets 기반 GUI 구성
- Signal/Slot을 활용한 이벤트 처리
- Serial, VISA, NI-DAQmx 인터페이스의 기본 구조
- 장치 검색 및 연결 상태 관리
- 하드웨어 제어 프로그램의 모듈 구성

---

**DeviceTester — C++/Qt-based Hardware Interface Manager**
