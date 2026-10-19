#pragma once// 헤더파일을 한번만 포함시키라는 의미


#include <QString> // qt가 제공하는 문자열 클래스

struct DeviceInfo
{
    QString name;
    QString description;
    QString interfaceType;
    QString manufacturer;
    QString model;
    QString serialNumber;
    QString resource;
    QString vendorId;
    QString productId;

};