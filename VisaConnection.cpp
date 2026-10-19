#include "VisaConnection.h"
#include <QDebug>

bool VisaConnection::connect(
    const QString& resource,
    ViUInt32 timeoutMs
)
{
    // 새로운 VISA 연결을 임시 핸들로 관리
    ViSession newResourceManager = VI_NULL;
    ViSession newInstrument = VI_NULL;

    ViStatus status;

    // 1. 새로운 VISA Resource Manager 열기
    status = viOpenDefaultRM(&newResourceManager);

    if (status < VI_SUCCESS)
    {
        qDebug() << "VISA Resource Manager Open Failed:"
            << status;

        return false;
    }

    // 2. 새로운 VISA 장비 세션 열기
    QByteArray resourceName = resource.toLatin1();

    status = viOpen(
        newResourceManager,
        resourceName.constData(),
        VI_NULL,
        VI_TMO_IMMEDIATE,
        &newInstrument
    );

    if (status < VI_SUCCESS)
    {
        qDebug() << "VISA Instrument Open Failed:"
            << resource
            << "Result:"
            << status;

        viClose(newResourceManager);

        return false;
    }

    // 3. 새로운 장비의 타임아웃 설정
    status = viSetAttribute(
        newInstrument,
        VI_ATTR_TMO_VALUE,
        static_cast<ViAttrState>(timeoutMs)
    );

    if (status < VI_SUCCESS)
    {
        qDebug() << "VISA Timeout Setting Failed:"
            << timeoutMs
            << "Result:"
            << status;

        // 새로운 연결만 정리
        viClose(newInstrument);
        viClose(newResourceManager);

        return false;
    }

    // 4. 새로운 세션의 인터페이스 확인
    ViUInt16 interfaceType = 0;

    status = viGetAttribute(
        newInstrument,
        VI_ATTR_INTF_TYPE,
        &interfaceType
    );

    if (status < VI_SUCCESS)
    {
        qDebug() << "Failed to get VISA Interface Type:"
            << status;
    }
    else
    {
        qDebug() << "VISA Interface Type:"
            << interfaceType;
    }

    qDebug() << "New VISA Connection Ready:"
        << resource;

    qDebug() << "VISA Timeout Set:"
        << timeoutMs
        << "ms";

    // 5. 새 연결 준비가 완료된 후에만 기존 연결 해제
    if (instrument != VI_NULL)
    {
        viClose(instrument);
        instrument = VI_NULL;
    }

    if (resourceManager != VI_NULL)
    {
        viClose(resourceManager);
        resourceManager = VI_NULL;
    }

    // 6. 새로운 세션을 실제 연결 객체에 등록
    resourceManager = newResourceManager;
    instrument = newInstrument;
    connectedResource = resource;

    qDebug() << "VISA Connected:"
        << connectedResource;

    return true;
}


QString VisaConnection::getInterfaceType()
{
    ViUInt16 interfaceType;

    ViStatus status = viGetAttribute(
        instrument,
        VI_ATTR_INTF_TYPE,
        &interfaceType
    );

    if (status != VI_SUCCESS)
    {
        return "-";
    }

    if (interfaceType == VI_INTF_USB)
    {
        return "USB";
    }

    return QString::number(interfaceType);
}


QString VisaConnection::getTimeout()
{
    ViUInt32 timeout;

    ViStatus status = viGetAttribute(
        instrument,
        VI_ATTR_TMO_VALUE,
        &timeout
    );

    if (status != VI_SUCCESS)
    {
        return "-";
    }

    return QString::number(timeout) + " ms";
}

QString VisaConnection::getResource() const
{
    return connectedResource;
}

bool VisaConnection::isConnected() const
{
    return instrument != VI_NULL;
}


void VisaConnection::disconnect()
{
    if (instrument != VI_NULL)
    {
        viClose(instrument);
        instrument = VI_NULL;
    }

    if (resourceManager != VI_NULL)
    {
        viClose(resourceManager);
        resourceManager = VI_NULL;
    }
    // 연결된 VISA 리소스 이름 초기화
    connectedResource.clear();
}