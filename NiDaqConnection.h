#pragma once

#include <QString>
#include <NIDAQmx.h>

class NiDaqConnection
{
public:
    bool connect(const QString& deviceName);
    void disconnect();

    bool isConnected() const;
    QString getDeviceName() const;

private:
    QString deviceName;
    bool connected = false;
};