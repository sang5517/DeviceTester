#include "VisaScanner.h"
// VisaScanner 클래스의 헤더 파일을 포함한다.
// VisaScanner 클래스와 scan() 함수의 선언을 가져온다.
#include <visa.h>
// NI-VISA에서 제공하는 헤더 파일이다.
// VISA 장비 검색에 필요한 자료형(ViSession, ViStatus 등)과
// 함수(viOpenDefaultRM, viFindRsrc, viFindNext, viClose 등)를 사용하기 위해 포함한다.
#include <QDebug>
// Qt에서 디버그 메시지를 출력할 때 사용하는 헤더 파일이다.
// 현재 코드에서는 QDebug 기능을 직접 사용하지 않는다.
QList<DeviceInfo> VisaScanner::scan()
{
    // VisaScanner 클래스의 scan() 함수를 정의한다.
    // VISA 방식으로 검색한 장비 정보를 QList<DeviceInfo> 형태로 반환한다.
    QList<DeviceInfo> devices;
    // 검색된 장비 정보를 저장할 빈 리스트를 생성한다.
    // 검색 조건에 해당하는 장비를 발견하면 DeviceInfo 객체를 추가한다.
    ViSession resourceManager;
    // VISA 리소스 매니저의 세션 핸들을 저장할 변수를 선언한다.
    // 리소스 매니저는 VISA 리소스를 검색하고 접근하기 위한 관리 객체다.
    // 실제 세션 핸들은 viOpenDefaultRM() 함수가 기록한다.

    ViStatus status = viOpenDefaultRM(&resourceManager);
    // VISA 기본 리소스 매니저를 연다.
    // ViStatus는 VISA 함수의 실행 결과를 나타내는 자료형이다.
    // 함수의 반환값은 status에 저장한다.
    // &resourceManager는 resourceManager 변수의 주소를 전달한다.
    // 함수가 생성한 세션 핸들을 해당 변수에 기록할 수 있도록 하기 위함이다.
    if (status != VI_SUCCESS)
    {   
        //리소스 매니저를 여는데 실패한 경우 실행한다.
        // VI_SUCCESS는 VISA함수의 성공 상태를 나타낸다.
        // 검색 결과가 없으므로 현재 비어 있는 devices 리스트를 반환하고 함수를 종료한다.
        return devices;
    }

    ViFindList findList;
    // VISA 검색 목록의 핸들을 저장할 변수를 선언한다.
    // viFindRsrc() 함수가 검색 목록을 생성하면 해당 핸들을 기록한다.
    ViUInt32 count;
    // 검색된 VISA 리소스의 개수를 저장할 변수다.
    // ViUInt32는 VISA에서 제공하는 32비트 부호 없는 정수 자료형이다.
    ViChar resourceName[VI_FIND_BUFLEN];
    // 검색된 VISA 리소스 이름을 저장할 문자 배열이다.
    // ViChar는 VISA에서 사용하는 문자 자료형이다.
    // VI_FIND_BUFLEN은 리소스 이름을 저장할 버퍼의 크기를 나타낸다.

    status = viFindRsrc(
        resourceManager,
        "?*INSTR",
        &findList,
        &count,
        resourceName
    );
    // VISA 리소스 검색을 시작한다.
    // resourceManager : 앞에서 연 VISA 리소스 매니저의 세션 핸들
    // "?*INSTR"       : 검색할 VISA 리소스의 조건
    // &findList       : 검색 목록 핸들을 기록할 변수의 주소
    // &count          : 검색된 리소스 개수를 기록할 변수의 주소
    // resourceName    : 첫 번째 검색 결과의 이름을 기록할 문자 배열
    // 함수의 반환값은 status에 저장한다.
    if (status == VI_SUCCESS)
    {
        // 검색 성공 시 실행 
        for (ViUInt32 i = 0; i < count; ++i)
        {
            // 검색된 리소스 개수(count)만큼 반복한다.
            // i는 0부터 시작하며, 반복할 때마다 1씩 증가한다.
            QString resource = QString::fromLatin1(resourceName);
            // 문자 배열에 저장된 VISA 리소스 이름을 QString으로 변환한다.
            // 이후 Qt의 문자열 기능을 사용해 리소스 이름을 처리한다.
            if (!resource.startsWith("ASRL"))
            {   
                // 리소스 이름이 "ASRL"로 시작하지 않는 경우에만 실행한다.
                // !는 조건의 결과를 반대로 바꾸는 논리 부정 연산자다.
                // 따라서 "ASRL"로 시작하는 리소스는 이 조건에서 제외한다.
                
                DeviceInfo device;
                // 검색된 리소스의 정보를 저장한 DeviceInfo 객체를 생성한다.

                device.name = resource;
                // 리소스 이름을 장비 이름으로 저장한다.

                device.description = "VISA Resource";
                // 장비 설명을 "VISA Resource"로 지정한다.

                device.interfaceType = "VISA";
                // 인터페이스 유형을 VISA로 지정한다.

                device.manufacturer = "-";
                // 제조사 정보는 현재 조회하지 않으므로 "-"로 저장한다.

                device.model = "-";
                // 모델 정보는 현재 조회하지 않으므로 "-"로 지정한다. 

                device.serialNumber = "-";
                // 시리얼 번호는 현재 조회하지 않으므로 "-"로 지정한다.

                device.resource = resource;
                // VISA 리소스 이름을 resource 항목에 저장한다.

                device.vendorId = "-";
                // Vendor ID는 현재 조회하지 않으므로 "-"로 지정한다.

                device.productId = "-";
                // Product ID는 현재 조회하지 않으므로 "-"로 지정한다.

                devices.append(device);
                // 정보를 채운 DeviceInfo 객체를 devices 리스트에 추가.

            }

            if (i + 1 < count)
            {   
                // 아직 처리할 다음 리소스가 남아 있는 경우 실행한다.
                // i는 0부터 시작하므로 i+1과 count를 비교해 다음 결과의 존재 여부를 확인한다.
                status = viFindNext(findList, resourceName);
                // VISA 검색 목록에서 다음 리소스 이름을 가져온다.
                // findList는 검색 목록의 핸들이며,
                // resourceName 배열에 다음 검색 결과의 이름이 기록된다.
                // 함수의 반환값은 status에 저장한다.
            }
        }

        viClose(findList);
        // 리소스 검색이 끝났으므로 VISA 검색 목록을 닫는다.
    }

    viClose(resourceManager);
    // 리소스 매니저 세션을 닫는다.
    // viOpenDefaultRm()으로 열었던 세션을 정리한다.



    return devices;
    // 검색과정에서 devices 리스트에 추가한 장비정보를 반환한다.

}