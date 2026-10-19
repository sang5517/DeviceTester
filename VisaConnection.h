#pragma once

#include <QString>
#include <visa.h>

class VisaConnection
{
public:
    bool connect(const QString& resource, ViUInt32 timeoutMs);
    void disconnect();

    QString getInterfaceType();
    QString getTimeout();
    QString getResource() const;
    bool isConnected() const;

private:
    ViSession resourceManager = VI_NULL;
    ViSession instrument = VI_NULL;

    // 현재 연결된 VISA 장비의 리소스 이름
    QString connectedResource;
};