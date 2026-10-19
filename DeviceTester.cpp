#include "DeviceTester.h"
#include "SerialScanner.h"
#include "VisaScanner.h"
#include "NiDaqScanner.h"
#include <NIDAQmx.h>
#include <QPushButton>
#include <QSerialPort>
#include <QListWidget>
#include <QStackedWidget>
#include <QListWidgetItem>
#include <QCloseEvent>
// =====================================================
// DeviceTester 생성자
// - Qt Designer UI 초기화
// - 프로그램 시작 시 기본 화면 및 연결 상태 설정
// - 장치 검색, 선택, 연결 버튼의 이벤트 처리 등록
DeviceTester::DeviceTester(QWidget* parent)
    : QMainWindow(parent) // 부모 클래스인 QMainWindow 초기화
{   
    // Qt Designer에서 작성한 UI를 현재 DeviceTester 객체에 적용
    ui.setupUi(this);

    // 프로그램 시작 시 장치가 선택되지 않은 화면 표시 
    ui.connectionStack->setCurrentWidget(ui.pageNoDevice);
    
    // 연결 상태를 기본값인 연결 해제로 설정
    ui.lblConnectionStatus->setText("Status: Disconnected");

    // VISA 및 NI-DAQmx 장치가 선택되지 않았음을 안내
    ui.lblVisaResourceValue->setText("Select a VISA device.");
    ui.lblNiDaqDeviceValue->setText("Select an NI-DAQmx device.");
    // --------------------------------------------------
    // Applied Settings 초기 화면 설정
    // 프로그램 시작 시에는 Serial 설정 항목만 표시
    // --------------------------------------------------
    ui.lblAppliedBaudRate->setVisible(true);
    ui.lblAppliedBaudRateValue->setVisible(true);

    ui.lblAppliedDataBits->setVisible(true);
    ui.lblAppliedDataBitsValue->setVisible(true);

    ui.lblAppliedParity->setVisible(true);
    ui.lblAppliedParityValue->setVisible(true);

    ui.lblAppliedStopBits->setVisible(true);
    ui.lblAppliedStopBitsValue->setVisible(true);

    // VISA 설정 항목은 프로그램 시작 시 숨김
    // 이후 선택한 장치의 인터페이스에 따라 표시 여부를 변경
    ui.lblAppliedPhysicalInterface->setVisible(false);
    ui.lblAppliedPhysicalInterfaceValue->setVisible(false);

    ui.lblAppliedVisaSession->setVisible(false);
    ui.lblAppliedVisaSessionValue->setVisible(false);

    ui.lblAppliedTimeout->setVisible(false);
    ui.lblAppliedTimeoutValue->setVisible(false);

    // =====================================================
    // [SCAN] 장치 검색 버튼 이벤트
    // - Scan 버튼 클릭 시 실행되는 람다 함수
    // - 기존 장치 목록 및 화면 정보 초기화
    // - Serial/ VISA / NI-DAQmx 장치검색
    // - 검색 결과를 하나의 devices 목록으로 통합
    // =====================================================

    // Qt의 connect()로 btnScan 버튼의 clicked 신호와 람다 함수 연결
    // [this] : 람다 함수 내부에서 현재 DeviceTester 객체의 멤버에 접근
    connect(ui.btnScan, &QPushButton::clicked, this, [this]()
        {
            // 기존 장치 목록 화면 초기화
            ui.deviceList->clear();

            // 장치 선택 화면을 기본 화면으로 변경
            ui.connectionStack->setCurrentWidget(
                ui.pageNoDevice
            );

            // 기존에 표시된 장치 상세 정보 초기화
            ui.lblDeviceNameValue->setText("-");
            ui.lblDescriptionValue->setText("-");
            ui.lblModelValue->setText("-");
            ui.lblManufacturerValue->setText("-");
            ui.lblSerialNumberValue->setText("-");
            ui.lblResourceValue->setText("-");
            ui.lblVendorIdValue->setText("-");
            ui.lblProductIdValue->setText("-");

            // 인터페이스 및 프로토콜 정보 초기화 
            ui.lblInterfaceValue->setText("-");
            ui.lblProtocolValue->setText("Unknown");

            // 장치별 설정 화면의 안내 문구 초기화
            ui.lblVisaResourceValue->setText(
                "Select a VISA device."
            );

            ui.lblNiDaqDeviceValue->setText(
                "Select an NI-DAQmx device."
            );

            // 1. Serial 장치 검색
            // scan()이 반환한 QList<DeviceInfo>를 devices에 저장
            SerialScanner scanner;

            devices = scanner.scan();

            // 2. VISA 장치 검색
            // 검색 결과를 별도 리스트에 저장한 후 devices에 추가
            VisaScanner visaScanner;

            QList<DeviceInfo> visaDevices = visaScanner.scan();

            devices.append(visaDevices);

            // 3. NI-DAQmx 장치 검색
            // VISA와 동일하게 검색 결과를 devices에 추가
            NiDaqScanner niDaqScanner;

            QList<DeviceInfo> niDaqDevices = niDaqScanner.scan();

            devices.append(niDaqDevices);

            // 4. 통합된 장치 목록을 QListWidget에 표시
            // const DeviceInfo& : 객체 복사 없이 읽기 전용 참조로 순회
            for (const DeviceInfo& device : devices)
            {
                ui.deviceList->addItem(device.name);
            }

            // 5. 실제 연결 상태 갱신
            updateConnectionStatus();
        });

    // =====================================================
    // [DEVICE SELECTION] 장치 목록 선택 이벤트
    // - 장치 목록에서 항목을 클릭하면 실행
    // - 선택한 장치의 연결 상태 확인
    // - 인터페이스 종류에 맞는 설정 화면 표시
    // - 장치 상세 정보 및 적용된 설정값 갱신

    // QListWidget의 itemClicked 신호 발생 시 람다 함수 실행
    connect(ui.deviceList, &QListWidget::itemClicked, this, [this]()
        {
            if (ui.deviceList->currentItem() == nullptr)
            {
                return;
            }

            QString deviceName = ui.deviceList->currentItem()->text();


            // =====================================================
            // [추가 시작] 선택한 장비의 실제 연결 상태 확인
            // =====================================================

            bool selectedDeviceConnected = false;

            for (const DeviceInfo& device : devices)
            {
                if (device.name != deviceName)
                {
                    continue;
                }

                if (device.interfaceType == "Serial")
                {
                    selectedDeviceConnected =
                        activeSerialPort->isOpen() &&
                        activeSerialPort->portName() == device.name;
                }
                else if (device.interfaceType == "VISA")
                {
                    selectedDeviceConnected =
                        visaConnection.isConnected() &&
                        visaConnection.getResource() == device.resource;
                    // 주의: VISA는 현재 연결된 리소스와 선택한
                    // 리소스가 같은지까지 확인해야 완전히 정확함
                }
                else if (device.interfaceType == "NI-DAQmx")
                {
                    selectedDeviceConnected =
                        niDaqConnection.isConnected() &&
                        niDaqConnection.getDeviceName() == device.resource;
                }

                if (selectedDeviceConnected)
                {
                    ui.lblConnectionStatus->setText(
                        QString("Status: Connected - %1 (%2)")
                        .arg(device.name)
                        .arg(device.interfaceType)
                    );
                }
                else
                {
                    ui.lblConnectionStatus->setText(
                        "Status: Disconnected"
                    );
                }

                break;
            }

            // =====================================================
            // [추가 끝]
            // =====================================================
            for (const DeviceInfo& device : devices)
            {
                if (device.name == deviceName)
                {
                    qDebug() << "Selected device:" << device.name;

                    // 장치 종류에 따라 설정 화면 전환
                    if (device.interfaceType == "Serial")
                    {
                        ui.connectionStack->setCurrentWidget(
                            ui.pageSerialSettings
                        );
                    }
                    else if (device.interfaceType == "VISA")
                    {
                        ui.connectionStack->setCurrentWidget(
                            ui.pageVisaSettings
                        );

                        ui.lblVisaResourceValue->setText(
                            device.resource
                        );
                    }
                    else if (device.interfaceType == "NI-DAQmx")
                    {
                        ui.connectionStack->setCurrentWidget(
                            ui.pageNiDaqSettings
                        );

                        ui.lblNiDaqDeviceValue->setText(
                            device.resource
                        );
                    }
                    else
                    {
                        ui.connectionStack->setCurrentWidget(
                            ui.pageNoDevice
                        );
                    }

                    qDebug() << "Description:" << device.description;

                    ui.lblDeviceNameValue->setText(device.name);
                    ui.lblDescriptionValue->setText(device.description);
                    ui.lblModelValue->setText(device.model);
                    ui.lblManufacturerValue->setText(device.manufacturer);
                    ui.lblSerialNumberValue->setText(device.serialNumber);
                    ui.lblResourceValue->setText(device.resource);
                    ui.lblVendorIdValue->setText(device.vendorId);
                    ui.lblProductIdValue->setText(device.productId);

                    ui.lblInterfaceValue->setText(device.interfaceType);
                    ui.lblProtocolValue->setText("Unknown");

                    if (device.interfaceType == "Serial")
                    {
                        ui.lblCppQtValue->setText("QSerialPort");
                        ui.lblPythonValue->setText("pySerial");
                        ui.lblCsharpValue->setText("System.IO.Ports");

                        // Applied Settings - Serial 표시
                        ui.lblAppliedBaudRate->setVisible(true);
                        ui.lblAppliedBaudRateValue->setVisible(true);

                        ui.lblAppliedDataBits->setVisible(true);
                        ui.lblAppliedDataBitsValue->setVisible(true);

                        ui.lblAppliedParity->setVisible(true);
                        ui.lblAppliedParityValue->setVisible(true);

                        ui.lblAppliedStopBits->setVisible(true);
                        ui.lblAppliedStopBitsValue->setVisible(true);

                        // VISA 설정 숨김
                        ui.lblAppliedPhysicalInterface->setVisible(false);
                        ui.lblAppliedPhysicalInterfaceValue->setVisible(false);

                        ui.lblAppliedVisaSession->setVisible(false);
                        ui.lblAppliedVisaSessionValue->setVisible(false);

                        ui.lblAppliedTimeout->setVisible(false);
                        ui.lblAppliedTimeoutValue->setVisible(false);
                    }
                    else if (device.interfaceType == "VISA")
                    {
                        ui.lblCppQtValue->setText("NI-VISA");
                        ui.lblPythonValue->setText("PyVISA");
                        ui.lblCsharpValue->setText("NI-VISA");

                        // Serial 설정 숨김
                        ui.lblAppliedBaudRate->setVisible(false);
                        ui.lblAppliedBaudRateValue->setVisible(false);

                        ui.lblAppliedDataBits->setVisible(false);
                        ui.lblAppliedDataBitsValue->setVisible(false);

                        ui.lblAppliedParity->setVisible(false);
                        ui.lblAppliedParityValue->setVisible(false);

                        ui.lblAppliedStopBits->setVisible(false);
                        ui.lblAppliedStopBitsValue->setVisible(false);

                        // VISA 설정 표시
                        ui.lblAppliedPhysicalInterface->setVisible(true);
                        ui.lblAppliedPhysicalInterfaceValue->setVisible(true);

                        ui.lblAppliedVisaSession->setVisible(true);
                        ui.lblAppliedVisaSessionValue->setVisible(true);

                        ui.lblAppliedTimeout->setVisible(true);
                        ui.lblAppliedTimeoutValue->setVisible(true);

                        // 현재 선택한 VISA 장치가 연결되어 있으면 정보 표시
                        if (visaConnection.isConnected() &&
                            visaConnection.getResource() == device.resource)
                        {
                            ui.lblAppliedPhysicalInterfaceValue->setText(
                                visaConnection.getInterfaceType()
                            );

                            ui.lblAppliedVisaSessionValue->setText("Open");

                            ui.lblAppliedTimeoutValue->setText(
                                visaConnection.getTimeout()
                            );
                        }
                        else
                        {
                            ui.lblAppliedPhysicalInterfaceValue->setText("-");
                            ui.lblAppliedVisaSessionValue->setText("-");
                            ui.lblAppliedTimeoutValue->setText("-");
                        }
                    }
                    else if (device.interfaceType == "NI-DAQmx")
                    {
                        ui.lblCppQtValue->setText("NI-DAQmx");
                        ui.lblPythonValue->setText("nidaqmx");
                        ui.lblCsharpValue->setText("NI-DAQmx");

                        // Serial 설정 숨김
                        ui.lblAppliedBaudRate->setVisible(false);
                        ui.lblAppliedBaudRateValue->setVisible(false);

                        ui.lblAppliedDataBits->setVisible(false);
                        ui.lblAppliedDataBitsValue->setVisible(false);

                        ui.lblAppliedParity->setVisible(false);
                        ui.lblAppliedParityValue->setVisible(false);

                        ui.lblAppliedStopBits->setVisible(false);
                        ui.lblAppliedStopBitsValue->setVisible(false);

                        // Physical Interface 표시
                        ui.lblAppliedPhysicalInterface->setVisible(true);
                        ui.lblAppliedPhysicalInterfaceValue->setVisible(true);

                        // VISA 설정 숨김
                        ui.lblAppliedVisaSession->setVisible(false);
                        ui.lblAppliedVisaSessionValue->setVisible(false);

                        ui.lblAppliedTimeout->setVisible(false);
                        ui.lblAppliedTimeoutValue->setVisible(false);

                        // 현재 NI-DAQmx 연결 상태 확인
                        if (niDaqConnection.isConnected() &&
                            niDaqConnection.getDeviceName() == device.resource)
                        {
                            ui.lblAppliedPhysicalInterfaceValue->setText(
                                niDaqConnection.getDeviceName()
                            );
                        }
                        else
                        {
                            ui.lblAppliedPhysicalInterfaceValue->setText("-");
                        }
                    }
                    break;
                }
            }
        });
   

        connect(ui.btnConnect, &QPushButton::clicked, this, [this]()
            {
                QListWidgetItem* selectedItem = ui.deviceList->currentItem();

                if (selectedItem == nullptr)
                {
                    qDebug() << "No device selected.";
                    return;
                }

                QString deviceName = selectedItem->text();

                for (const DeviceInfo& device : devices)
                {
                    if (device.name != deviceName)
                    {
                        continue;
                    }

                    // =========================================
                    // 1. Serial 연결
                    // =========================================
                    if (device.interfaceType == "Serial")
                    {
                        QString portName = device.name;

                        // 현재 활성 Serial 포트
                        QSerialPort* currentPort = activeSerialPort;

                        // 이미 같은 포트에 연결되어 있다면 재연결하지 않음
                        if (currentPort->isOpen()
                            && currentPort->portName() == portName)
                        {
                            qDebug() << "Serial already connected:" << portName;

                            ui.lblConnectionStatus->setText(
                                QString("Status: Connected - %1 (Serial)")
                                .arg(portName)
                            );

                            break;
                        }

                        // 현재 활성 포트가 아닌 쪽을 새 연결 시도에 사용
                        QSerialPort* newPort =
                            (activeSerialPort == &serialPort)
                            ? &pendingSerialPort
                            : &serialPort;

                        // 혹시 이전 연결 시도에서 열린 상태라면 정리
                        if (newPort->isOpen())
                        {
                            newPort->close();
                        }

                        // Serial 설정 읽기
                        int baudRate = ui.comboBaudRate->currentText().toInt();
                        int dataBits = ui.comboDataBits->currentText().toInt();

                        QString parity = ui.comboParity->currentText();
                        QString stopBits = ui.comboStopBits->currentText();
                        qDebug() << "[Serial] Connection Attempt";
                        qDebug() << "  Port      :" << portName;
                        qDebug() << "  Baud Rate :" << baudRate;
                        qDebug() << "  Data Bits :" << dataBits;
                        qDebug() << "  Parity    :" << parity;
                        qDebug() << "  Stop Bits :" << stopBits;
                        // 1. 포트 이름 설정
                        newPort->setPortName(portName);
                        // 2. Baud Rate 설정 및 확인
                        if (!newPort->setBaudRate(baudRate))
                        {
                            qDebug() << "[Serial] Failed to Set Baud Rate";
                            qDebug() << "  Port  :" << portName;
                            qDebug() << "  Value :" << baudRate;
                            qDebug() << "  Error :" << newPort->errorString();

                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        // 3. Data Bits 설정 및 확인
                        if (!newPort->setDataBits(
                            static_cast<QSerialPort::DataBits>(dataBits)))
                        {
                            qDebug() << "[Serial] Failed to Set Data Bits";
                            qDebug() << "  Port  :" << portName;
                            qDebug() << "  Value :" << dataBits;
                            qDebug() << "  Error :" << newPort->errorString();


                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        // 4. Parity 설정 및 확인
                        bool paritySet = false;

                        if (parity == "None")
                        {
                            paritySet = newPort->setParity(QSerialPort::NoParity);
                        }
                        else if (parity == "Even")
                        {
                            paritySet = newPort->setParity(QSerialPort::EvenParity);
                        }
                        else if (parity == "Odd")
                        {
                            paritySet = newPort->setParity(QSerialPort::OddParity);
                        }
                        else if (parity == "Mark")
                        {
                            paritySet = newPort->setParity(QSerialPort::MarkParity);
                        }
                        else if (parity == "Space")
                        {
                            paritySet = newPort->setParity(QSerialPort::SpaceParity);
                        }
                        else
                        {
                            qDebug() << "Invalid Parity setting:" << parity;
                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        if (!paritySet)
                        {
                            qDebug() << "[Serial] Failed to Set Parity";
                            qDebug() << "  Port  :" << portName;
                            qDebug() << "  Value :" << parity;
                            qDebug() << "  Error :" << newPort->errorString();

                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        // 5. Stop Bits 설정 및 확인
                        bool stopBitsSet = false;

                        if (stopBits == "1")
                        {
                            stopBitsSet = newPort->setStopBits(QSerialPort::OneStop);
                        }
                        else if (stopBits == "1.5")
                        {
                            stopBitsSet = newPort->setStopBits(QSerialPort::OneAndHalfStop);
                        }
                        else if (stopBits == "2")
                        {
                            stopBitsSet = newPort->setStopBits(QSerialPort::TwoStop);
                        }
                        else
                        {
                            qDebug() << "Invalid Stop Bits setting:" << stopBits;
                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        if (!stopBitsSet)
                        {
                            qDebug() << "[Serial] Failed to Set Stop Bits";
                            qDebug() << "  Port  :" << portName;
                            qDebug() << "  Value :" << stopBits;
                            qDebug() << "  Error :" << newPort->errorString();

                            newPort->close();
                            updateConnectionStatus();
                            break;
                        }

                        // =========================================
                        // 새 Serial 포트 연결 시도
                        // 기존 연결은 아직 유지
                        // - 기존연결은 유지한 상태에서 새 포트 여길
                        // - QIODevice::ReadWrite : 읽기/ 쓰기 모두 허용
                        // - open() 성공 시 true, 실패 시 false 반환
                        // - ! 연산자로 연결 실패 여부 확인
                        // =========================================
                        if (!newPort->open(QIODevice::ReadWrite))
                        {
                            // 새 Serial 포트를 열지 못한 경우 오류 정보 출력
                            // errorString(): 오류 설명 문자열
                            // error() : QSerailPort 오류 종류
                            qDebug() << "[Serial] Failed to Open Port";
                            qDebug() << "  Port      :" << portName;
                            qDebug() << "  Baud Rate :" << baudRate;
                            qDebug() << "  Data Bits :" << dataBits;
                            qDebug() << "  Parity    :" << parity;
                            qDebug() << "  Stop Bits :" << stopBits;
                            qDebug() << "  Error     :" << newPort->errorString();
                            qDebug() << "  Error Code:" << newPort->error();

                            // 연결에 실패한 새 포트 객체만 정리
                            // 실패한 새 연결만 정리
                            newPort->close();

                            // =========================================
                            // 새 연결 실패 후 기존 연결 상태 확인
                            // =========================================

                            // 1. 기존 Serial 포트가 열려 있다면 연결 상태 유지 표시
                            if (activeSerialPort->isOpen())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (Serial)")
                                    .arg(activeSerialPort->portName())
                                );
                            }

                            // 2. 기존 VISA 장치가 연결되어 있다면 연결 상태 유지 표시
                            else if (visaConnection.isConnected())
                            {
                                ui.lblConnectionStatus->setText(
                                    "Status: Connected - Previous VISA Device"
                                );
                            }

                            // 3. 기존 NI-DAQmx 장치가 연결되어 있다면 연결 상태 유지 표시
                            else if (niDaqConnection.isConnected())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (NI-DAQmx)")
                                    .arg(niDaqConnection.getDeviceName())
                                );
                            }

                            // 4. 기존 연결도 없다면 연결 실패 상태 표시
                            else
                            {
                                ui.lblConnectionStatus->setText(
                                    "Status: Connection Failed"
                                );
                            }
                            // 새 연결 실패 처리가 완료되었으므로 장치 검색 반복문 종료
                            break;
                        }

                        // =========================================
                        // 새 Serial 연결 성공
                        // 이제 기존 연결을 정리하고 활성 포트 전환
                        // =========================================

                        // 기존 Serial 포트 해제
                        if (currentPort->isOpen())
                        {
                            currentPort->close();

                            qDebug() << "Previous Serial Disconnected:"
                                << currentPort->portName();
                        }

                        // 기존 VISA / NI 연결 해제
                        if (visaConnection.isConnected())
                        {
                            visaConnection.disconnect();
                            qDebug() << "Previous VISA Disconnected";
                        }

                        if (niDaqConnection.isConnected())
                        {
                            niDaqConnection.disconnect();
                            qDebug() << "Previous NI-DAQmx Disconnected";
                        }

                        // 새 포트를 활성 포트로 지정
                        activeSerialPort = newPort;

                        qDebug() << "Serial Connected:" << portName;

                        // 연결 상태 표시
                        ui.lblConnectionStatus->setText(
                            QString("Status: Connected - %1 (Serial)")
                            .arg(portName)
                        );

                        // 적용된 Serial 설정 표시
                        ui.lblAppliedBaudRateValue->setText(
                            QString::number(baudRate)
                        );

                        ui.lblAppliedDataBitsValue->setText(
                            QString::number(dataBits)
                        );

                        ui.lblAppliedParityValue->setText(parity);
                        ui.lblAppliedStopBitsValue->setText(stopBits);
                    }

                    // =========================================
                    // 2. VISA 연결
                    // =========================================
                    else if (device.interfaceType == "VISA")
                    {
                        const ViUInt32 timeoutMs =
                            static_cast<ViUInt32>(ui.spinVisaTimeout->value());

                        bool connected = visaConnection.connect(
                            device.resource,
                            timeoutMs
                        );

                        if (!connected)
                        {
                            qDebug() << "VISA Connection Failed:"
                                << device.resource;

                            // 기존 연결 유지
                            if (serialPort.isOpen())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (Serial)")
                                    .arg(serialPort.portName())
                                );
                            }
                            else if (niDaqConnection.isConnected())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (NI-DAQmx)")
                                    .arg(niDaqConnection.getDeviceName())
                                );
                            }
                            else
                            {
                                ui.lblConnectionStatus->setText(
                                    "Status: VISA Connection Failed"
                                );
                            }

                            break;
                        }

                        // 새 VISA 연결 성공 → 기존 연결 해제
                       
                        if (activeSerialPort->isOpen())
                        {
                            activeSerialPort->close();

                            qDebug() << "Previous Serial Disconnected";
                        }

                        if (niDaqConnection.isConnected())
                        {
                            niDaqConnection.disconnect();
                        }

                        qDebug() << "VISA Connected:" << device.resource;

                        ui.lblConnectionStatus->setText(
                            QString("Status: Connected - %1 (VISA)")
                            .arg(device.resource)
                        );

                        ui.lblAppliedPhysicalInterfaceValue->setText(
                            visaConnection.getInterfaceType()
                        );
                        ui.lblAppliedVisaSessionValue->setText("Open");
                        ui.lblAppliedTimeoutValue->setText(
                            visaConnection.getTimeout()
                        );

                        ui.lblAppliedBaudRateValue->setText("-");
                        ui.lblAppliedDataBitsValue->setText("-");
                        ui.lblAppliedParityValue->setText("-");
                        ui.lblAppliedStopBitsValue->setText("-");
                    }

                    // =========================================
                    // 3. NI-DAQmx 연결
                    // =========================================
                    else if (device.interfaceType == "NI-DAQmx")
                    {
                        bool connected = niDaqConnection.connect(device.resource);

                        if (!connected)
                        {
                            qDebug() << "NI-DAQmx Connection Failed:"
                                << device.resource;

                            // 기존 연결 유지
                            if (serialPort.isOpen())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (Serial)")
                                    .arg(serialPort.portName())
                                );
                            }
                            else if (visaConnection.isConnected())
                            {
                                ui.lblConnectionStatus->setText(
                                    QString("Status: Connected - %1 (VISA)")
                                    .arg(device.resource)
                                );
                            }
                            else
                            {
                                ui.lblConnectionStatus->setText(
                                    "Status: NI-DAQmx Connection Failed"
                                );
                            }

                            break;
                        }

                        
                        // 새 NI-DAQmx 연결 성공 → 기존 연결 해제
                        if (activeSerialPort->isOpen())
                        {
                            activeSerialPort->close();

                            qDebug() << "Previous Serial Disconnected";
                        }

                        if (visaConnection.isConnected())
                        {
                            visaConnection.disconnect();
                        }

                        qDebug() << "NI-DAQmx Connected:" << device.resource;

                        ui.lblConnectionStatus->setText(
                            QString("Status: Connected - %1 (NI-DAQmx)")
                            .arg(device.resource)
                        );

                        ui.lblAppliedPhysicalInterfaceValue->setText("NI-DAQmx");
                        ui.lblAppliedVisaSessionValue->setText("-");
                        ui.lblAppliedTimeoutValue->setText("-");

                        ui.lblAppliedBaudRateValue->setText("-");
                        ui.lblAppliedDataBitsValue->setText("-");
                        ui.lblAppliedParityValue->setText("-");
                        ui.lblAppliedStopBitsValue->setText("-");
                    }

                    break;
                }
            });

    connect(ui.btnDisconnect, &QPushButton::clicked, this, [this]()
        {
            disconnectCurrentDevice();
        });
        
}

void DeviceTester::disconnectCurrentDevice()
{
    // 1. 선택된 장비 확인
    QListWidgetItem* selectedItem = ui.deviceList->currentItem();

    if (selectedItem == nullptr)
    {
        qDebug() << "Disconnect Failed: No device selected";
        return;
    }

    QString deviceName = selectedItem->text();

    // 2. 선택된 장비의 DeviceInfo 찾기
    DeviceInfo selectedDevice;
    bool found = false;

    for (const DeviceInfo& device : devices)
    {
        if (device.name == deviceName)
        {
            selectedDevice = device;
            found = true;
            break;
        }
    }

    if (!found)
    {
        qDebug() << "Disconnect Failed: Device information not found:"
            << deviceName;
        return;
    }

    bool disconnected = false;

    // 3. Serial: 선택한 COM 포트가 실제 연결된 포트인지 확인
    if (selectedDevice.interfaceType == "Serial")
    {
        if (activeSerialPort->isOpen()
            && activeSerialPort->portName() == selectedDevice.name)
        {
            qDebug() << "Serial Disconnected:"
                << activeSerialPort->portName();

            activeSerialPort->close();

            ui.lblAppliedBaudRateValue->setText("-");
            ui.lblAppliedDataBitsValue->setText("-");
            ui.lblAppliedParityValue->setText("-");
            ui.lblAppliedStopBitsValue->setText("-");

            disconnected = true;
        }
    }

    // 4. VISA: 선택한 리소스가 실제 연결된 리소스인지 확인
    else if (selectedDevice.interfaceType == "VISA")
    {
        if (visaConnection.isConnected()
            && visaConnection.getResource() == selectedDevice.resource)
        {
            qDebug() << "VISA Disconnected:"
                << visaConnection.getResource();

            visaConnection.disconnect();

            ui.lblAppliedPhysicalInterfaceValue->setText("-");
            ui.lblAppliedVisaSessionValue->setText("-");
            ui.lblAppliedTimeoutValue->setText("-");

            disconnected = true;
        }
    }

    // 5. NI-DAQmx: 선택한 장비가 실제 연결된 장비인지 확인
    else if (selectedDevice.interfaceType == "NI-DAQmx")
    {
        if (niDaqConnection.isConnected()
            && niDaqConnection.getDeviceName() == selectedDevice.resource)
        {
            qDebug() << "NI-DAQmx Disconnected:"
                << niDaqConnection.getDeviceName();

            niDaqConnection.disconnect();

            disconnected = true;
        }
    }

    // 6. 결과 확인
    if (disconnected)
    {
        qDebug() << "Selected Device Disconnected:"
            << deviceName;
    }
    else
    {
        qDebug() << "Selected Device Is Not Connected:"
            << deviceName;
    }

    // 7. 실제 연결 상태를 화면에 반영
    updateConnectionStatus();
}

void DeviceTester::updateConnectionStatus()
{
    // Serial 연결 상태 확인
    if (activeSerialPort->isOpen())
    {
        ui.lblConnectionStatus->setText(
            QString("Status: Connected - %1 (Serial)")
            .arg(activeSerialPort->portName())
        );

        return;
    }

    // VISA 연결 상태 확인
    if (visaConnection.isConnected())
    {
        ui.lblConnectionStatus->setText(
            QString("Status: Connected - %1 (VISA)")
            .arg(visaConnection.getResource())
        );

        return;
    }

    // NI-DAQmx 연결 상태 확인
    if (niDaqConnection.isConnected())
    {
        ui.lblConnectionStatus->setText(
            QString("Status: Connected - %1 (NI-DAQmx)")
            .arg(niDaqConnection.getDeviceName())
        );

        return;
    }

    // 연결된 장비가 없는 경우
    ui.lblConnectionStatus->setText("Status: Disconnected");
}

DeviceTester::~DeviceTester()
{
}

void DeviceTester::closeEvent(QCloseEvent* event)
{
    qDebug() << "Application closing. Disconnecting all devices.";

    // 1. Serial 연결 해제
    if (activeSerialPort->isOpen())
    {
        qDebug() << "Closing Serial:"
            << activeSerialPort->portName();

        activeSerialPort->close();
    }

    // 2. VISA 연결 해제
    if (visaConnection.isConnected())
    {
        qDebug() << "Closing VISA:"
            << visaConnection.getResource();

        visaConnection.disconnect();
    }

    // 3. NI-DAQmx 연결 해제
    if (niDaqConnection.isConnected())
    {
        qDebug() << "Closing NI-DAQmx:"
            << niDaqConnection.getDeviceName();

        niDaqConnection.disconnect();
    }

    qDebug() << "All device connections closed.";

    // 모든 정리가 끝나면 창 닫기 허용
    event->accept();
}