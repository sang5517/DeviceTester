#pragma once // 헤더파일이 중복해서 포함되는 것을 방지한다.

#include <QList> // 여러 개의 데이터를 목록으로 관리하는 Qt 자료형
#include "DeviceInfo.h" // 장치 정보를 저장하기 위해 직접 정의한 자료형

// NI-DAQmx를 이용해 ni 장치를 검색하는 클래스
class NiDaqScanner
{
public:
	//NI 장치를 검색하고, 검색 결과를 DeviceInfo 목록으로 반환한다.
	// 실제 검색 로직은 NIDaqScanner.cpp에서 구현한다.
	QList<DeviceInfo> scan();

};