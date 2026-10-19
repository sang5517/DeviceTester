#include "NiDaqScanner.h" //NIDaqScanner 클래스의 선언을 가져온다.

#include <NIDAQmx.h> //NI-DAQmx 장치 검색 API와 관련 자료형을 사용한다.
#include <QDebug> // 디버깅 메시지를 출력하기 위해 사용한다.
#include <QStringList> // 여러개의 QString 문자열을 목록으로 관리하기 위해 사용한다.

// NIDaqScanner 클래스에 선언된 scan() 함수를 구현한다.
// 검색한 NI장치 정보를 QList<DeviceInfo> 형태로 반환한다.
QList<DeviceInfo> NiDaqScanner::scan()
{
    // 검색한 장치 정볼르 저장할 빈 목록을 생성한다.
    QList<DeviceInfo> devices;

    // NI-Daqmx에서 반환하는 장치 이름을 저장할 문자배열을 생성한다.
    // 배열의 모든 요소를 0으로 초기화한다.
    char deviceNames[4096] = {};

    // 시스템에서 NI-DAQmx 장치 이름 목록을 가져온다.
    // deviceNames에 장치 이름을 저장하고, 배열의 크기르 전달한다.
    int32 result = DAQmxGetSysDevNames(
        deviceNames,
        sizeof(deviceNames)
    );

    //함수 실행 결과가 0이 아니면 오류로 판단한다.
    if (result != 0)
    {
        //오류 메시지와 오류 코드를 출력한다.
        qDebug() << "NI-DAQmx Device Scan Failed:"
            << result;
        //검색 결과 목록을 반환하고 함수를 종료한다.

        return devices;
    }

    // 문자 배열에 저장된 장치 이름을 QString으로 변환한다,
    QString names = QString::fromLocal8Bit(deviceNames);

    //가져온 장치 이름 목록을 디버깅 창에 출력한다.
    qDebug() << "NI-DAQmx Devices:"
        << names;

    //쉼표를 기준으로 장치 이름을 분리해 QStringList에 저장한다.
    // 빈 문자열은 목록에 포함하지 않는다.
    QStringList nameList = names.split(
        ",",
        Qt::SkipEmptyParts
    );

    // 분리한 장치 이름을 하나씩 순회한다.
    for (const QString& name : nameList)
    {
        // 장치 이름 앞뒤에 있는 공백을 제거한다.
        QString deviceName = name.trimmed();

        // 장치의 제품 유형을 저장할 문자 배열을 생성하고 0으로 초기화한다.
        char productType[256] = {};

        // 해당 장치의 제품 유형을 가져온다.
        result = DAQmxGetDevProductType(
            deviceName.toLocal8Bit().constData(),
            productType,
            sizeof(productType)
        );


        // 제품 유형 조회에 실패하면 해당 장치의 처리를 건너뛴다.
        if (result != 0)
        {   
            // 실패한 장치 이름과 오류 코드를 출력한다.
            qDebug() << "Failed to get Product Type:"
                << deviceName
                << "Result:"
                << result;

            //현재 반복을 종료하고 다음 장치를 처리한다.
            continue;
        }

        //제품 유형을 QString으로 변환한다.
        QString model =
            QString::fromLocal8Bit(productType);


        // 제품 유형이 "cDAQ-"로 시작하면 섀시로 판단한다.
        // 섀시는 Device List에 표시하지 않는다.
        if (model.startsWith("cDAQ-"))
        {
            qDebug() << "NI-DAQmx Chassis:"
                << deviceName
                << model;

            //현재 섀시는 목록에 추가하지 않고 다음 장치로 넘어간다.
            continue;
        }

        // 섀시가 아닌 장치의 정보를 저장할 객체를 생성한다.
        DeviceInfo device;

        // 공통 DeviceInfo 자료형에 장치 정보를 저장한다.
        device.name = deviceName;
        device.description = "NI-DAQmx Device";
        device.interfaceType = "NI-DAQmx";
        device.manufacturer = "National Instruments";
        device.model = model;
        device.serialNumber = "-";
        device.resource = deviceName;
        device.vendorId = "-";
        device.productId = "-";

        // 완성한 장치 정보를 검색 결과 목록에 추가한다.
        devices.append(device);

        // 검색한 모듈의 이름과 제품 유형을 출력한다.
        qDebug() << "NI-DAQmx Module:"
            << deviceName
            << model;
    }

    // 검색 결과 목록을 호출한 쪽으로 반환한다.
    return devices;
}