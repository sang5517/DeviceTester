#pragma once

#include <QtWidgets/QMainWindow> 
#include "ui_DeviceTester.h"
#include "VisaConnection.h"
#include "NiDaqConnection.h"
#include <QSerialPort>
#include <QList>
#include "DeviceInfo.h"
#include <QCloseEvent>
class DeviceTester : public QMainWindow
{
    Q_OBJECT

public:
    DeviceTester(QWidget* parent = nullptr);
    ~DeviceTester();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void disconnectCurrentDevice();
    void updateConnectionStatus();
    Ui::DeviceTesterClass ui;

    // Serial 포트 2개: 현재 연결 유지 및 새 연결 시도에 사용
    QSerialPort serialPort;
    QSerialPort pendingSerialPort;

    // 현재 실제 연결에 사용 중인 포트
    QSerialPort* activeSerialPort = &serialPort;

    VisaConnection visaConnection;
    NiDaqConnection niDaqConnection;

    QList<DeviceInfo> devices;
};