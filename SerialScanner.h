#pragma once

#include <QList>
#include "DeviceInfo.h"

class SerialScanner
{

public:
	QList<DeviceInfo> scan();
};
