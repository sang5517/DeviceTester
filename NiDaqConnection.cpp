#include "NiDaqConnection.h"

#include <QDebug>

bool NiDaqConnection::connect(const QString& deviceName)
{
    char productType[256] = {};

    // 새로운 장비가 실제로 존재하는지 먼저 확인
    int32 result = DAQmxGetDevProductType(
        deviceName.toLocal8Bit().constData(),
        productType,
        sizeof(productType)
    );

    if (result != 0)
    {
        qDebug() << "NI-DAQmx Connection Failed:"
            << deviceName
            << "Result:"
            << result;

        // 기존 연결 상태는 변경하지 않음
        return false;
    }

    // 새로운 장비 확인에 성공한 경우에만 상태 변경
    this->deviceName = deviceName;
    connected = true;

    qDebug() << "NI-DAQmx Connected:"
        << deviceName;

    qDebug() << "Product Type:"
        << productType;

    return true;
}

void NiDaqConnection::disconnect()
{
    if (connected)
    {
        qDebug() << "NI-DAQmx Disconnected:"
            << deviceName;
    }

    connected = false;
    deviceName.clear();
}

bool NiDaqConnection::isConnected() const
{
    return connected;
}

QString NiDaqConnection::getDeviceName() const
{
    return deviceName;
}