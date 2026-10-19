#pragma once

#include <QList>
#include "DeviceInfo.h"

class VisaScanner
{
public:
	QList<DeviceInfo> scan();
};