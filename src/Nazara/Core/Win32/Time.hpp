// Copyright (C) 2012 J�r�me Leclercq
// This file is part of the "Nazara Engine".
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_WINDOWS_TIME_HPP
#define NAZARA_WINDOWS_TIME_HPP

#include <ctime>
#include <windows.h>

time_t FileTimeToTime(FILETIME* time)
{
	SYSTEMTIME stUTC, stLocal;

	FileTimeToSystemTime(time, &stUTC);
	SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);

	std::tm timeinfo;
	timeinfo.tm_sec = stLocal.wSecond;
	timeinfo.tm_min = stLocal.wMinute;
	timeinfo.tm_hour = stLocal.wHour;
	timeinfo.tm_mday = stLocal.wDay;
	timeinfo.tm_mon = stLocal.wMonth-1;
	timeinfo.tm_year = stLocal.wYear-1900;
	timeinfo.tm_isdst = -1;

	return std::mktime(&timeinfo);
}

#endif // NAZARA_WINDOWS_TIME_HPP
