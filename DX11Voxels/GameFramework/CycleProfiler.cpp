#include "CycleProfiler.h"

ZCycleProfiler::ZCycleProfiler(std::string_view measureName) : profName(measureName)
{
	start = std::chrono::steady_clock::now();
}

ZCycleProfiler::~ZCycleProfiler()
{
	auto end = std::chrono::steady_clock::now();
	auto diff = end - start;
	auto elapsedMilli = std::chrono::duration_cast<std::chrono::milliseconds>(diff);
	auto ticksCount = diff.count();

	std::chrono::duration<double, std::milli> elapsedMilliDouble = diff;

	std::cout << profName << " milli: " << elapsedMilliDouble.count() << "\n";
}
