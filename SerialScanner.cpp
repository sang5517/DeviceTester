#include "SerialScanner.h"
//우리가 만든 SerialScanner 클래스의 헤더파일을 가져온다.
//여기서 SerialScanner 클래스와 scan() 함수의 선언을 알 수있다.
#include <QSerialPortInfo>
// Qt가 제공하는 QSerialPortInfo 클래스를 사용하기 위해 포함한다.
// 현재 PC에 연결되어 있는 Serial Port(COM 포트) 정보를 조회할 수 있다.
QList<DeviceInfo> SerialScanner::scan()
// SerialScanner.h에서 선언한 scan() 함수를 실제로 구현한다.
// QList<DeviceInfo>는 scan() 함수의 반환 자료형이다.
{
    QList<DeviceInfo> devices;
    // DeviceInfo 객체 여러 개를 저장할 수 있는 빈 리스트 생성
    // 검색한 Serial 장치 정보를 이 리스트에 하나씩 추가한다.
    const auto ports = QSerialPortInfo::availablePorts();
    // 현재 PC에서 사용 가능한 Serial Port 목록을 가져온다.
    // auto : 컴파일러가 반환값을 보고 자료형을 결정한다.
    // const : ports의 내용을 변경하지 못하게 한다.
    for (const QSerialPortInfo& port : ports)
    {   
        // ports에 들어있는 포트 정보를 하나씩 순회한다.
        // QSerialPortInfo : 포트 정보를 나타내는 자료형
        // & : 복사하지 않고 원본을 참조한다.
        // const : 참조한 포트 정보를 수정하지 않는다.
        // port : 현재 반복에서 사용하는 변수 이름
        DeviceInfo device;
        // 우리가 만든 DeviceInfo 자료형으로 장치 하나의 정보를 저장할 객체 생성
        device.name = port.portName();
        // 현재 포트의 이름을 가져와 device.name에 저장한다.
        // 예: COM5
        device.description = port.description();
        // 현재 포트의 설명을 가져와 저장한다.
        // 예: USB Serial Port
        device.interfaceType = "Serial";
        // 해당 장치의 인터페이스 종류를 Serial로 지정한다.
        // Qt에서 가져온 값이 아니라 개발자가 직접 지정한 문자열이다.
        device.manufacturer = port.manufacturer();
        //현재 포트의 제조사 정볼르 가져와 저장한다.
        device.model = "-";
        // 모델명을 별도로 조회하지 않으므로 "-"로 저장한다.
        device.serialNumber = port.serialNumber();
        // 현재 포트의 시리얼 번호를 가져와 저장한다.
        device.resource = port.systemLocation();
        // 운영체제가 사용하는 포트위치 정보를 가져와 저장한다.
        // windows에서는 일반적으로 com5와 같은 값이다.
		if (port.hasVendorIdentifier()) 
        {   
            // 제조사 식별자 정보 존재여부확인함수
            // hasVendorIdentifier()는 bool 자로형으로 true 또는 false를 반환한다.
            device.vendorId =
                QString::number(port.vendorIdentifier(), 16).toUpper();
            // port.vendorIdentifier() : 현재 포트의 Vendor ID 숫자 값을 가져온다.
            // QString::number(값, 16) : 숫자를16진수 형태의 QString 문자열로 변환한다.
            //.toUpper() : 변환된 문자열의 영문자를 대문자로 변경한다.
            // 최종결과를 DeviceInfo 객체의 vendorId에 저장한다.
        }
        else
        {
            device.vendorId = "-";

            // Vendore ID 정보가 존재하지 않으면 "-"를 저장한다.
        }
        // 현재 포트에 vendor id정보가 있으면 그값을 가져와 저장하고,없으면 "-"로 저장한다.

        if (port.hasProductIdentifier())
        {   
            // 현재 포트에 ProductID 정복 존재하는지 확인한다.
            // hasProductIdentifier()는 bool 자료형으로 true 또는 false를 반환한다.
            device.productId =
                QString::number(port.productIdentifier(), 16).toUpper();
            // port.productIdentifier() : 현재 포트의 Product ID 숫자 값을 가져온다.
            // QString::number(값, 16) : 숫자를 16진수 형태의 QString 문자열로 변환한다.
            // .toUpper() : 변환된 문자열의 영문자를 대문자로 변경한다.
            // 최종결과를 DeviceInfo 객체의 productId에 저장한다.
        }
        else
        {
            device.productId = "-";
            //Product ID 정보가 존재하지 않으면 "-"를 저장한다.
        }

        devices.append(device);
        // 현재 포트에서 수집한 정보를 DeviceInfo 객체에 저장한 후
        // QList<DeviceInfo> 리스트인 devices에 추가한다.
        // 반복문이 실행될 때마다 장치 정보가 하나씩 추가한다.
    }

    return devices;
    // 모든 시리얼 포트에 대한 검색과 정보 저장이 완료되면
    // DeviceInfo 객체들이 담긴 devices 리스트를 호출한 곳으로 반환한다.
}