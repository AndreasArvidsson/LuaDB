#pragma once
#include "String.h"

class Date {
public:
	static const time_t getCurrentTimeMillis();
	static const String toIsoString(time_t timestamp);
	static const bool fromIsoString(const char *isoString, time_t *timestampOut);

};