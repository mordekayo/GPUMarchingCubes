#pragma once

#include "pch.h"
#include <iostream>


class GAMEFRAMEWORK_API ZCycleProfiler
{
	std::string profName;
	std::chrono::steady_clock::time_point start;

public:
	ZCycleProfiler(std::string_view measureName);
	~ZCycleProfiler();
};


#define CYCLES_PROFILER(name, measureName) ZCycleProfiler _prof##name(measureName)


