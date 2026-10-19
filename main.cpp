#include "DeviceTester.h" // 다른코드의 선언을 가져오는것
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{   
    //Qt 애플리케이션 객체 생성
    //argc: 명령행 인자의 개수
    //argv: 명령행 인자에 접근하기 위한 배열
    //QApplication은 GUI애플리케이션의 초기화와 이벤트 처리를 담당한다.
    QApplication app(argc, argv);

    // 메인 윈도우 객체 생성
    // DeviceTester 클래스의 정의된 GUI와 동작을 사용할 수 있다.
    DeviceTester window;

    // 메인 윈도우를 화면에 표시한다.
    window.show();

    //Qt 이벤트 루프를 실행한다.
    // 사용자 입력과 버튼 클릭 등의 gui이벤트를 처리한다.
    // 애플리케이션 종료 시  이벤트루프의 반환값을 main()에서 반환한다.

    return app.exec(); 
}
