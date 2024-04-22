#include "TimeMeasure.h"
#include <ostream>
#include <iostream>
#include <fstream>
void TimeMeasure::CaptureStart()
{
    vramStart = CaptureVRam();
    ramStart = CaptureRam();
    startPoint = std::chrono::steady_clock::now();
}

void TimeMeasure::CaptureEnd()
{
    endPoint = std::chrono::steady_clock::now();
    vramEnd = CaptureVRam();
    ramEnd = CaptureRam();
}

double TimeMeasure::GetTimeInSeconds()
{
    auto s = std::chrono::duration_cast<std::chrono::seconds>(endPoint - startPoint).count();
    return s;
}

double TimeMeasure::GetTimeInMilliseconds()
{
    auto s = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count();
    return s;
}

double TimeMeasure::GetTimeInNanoseconds()
{
    auto s = std::chrono::duration_cast<std::chrono::nanoseconds>(endPoint - startPoint).count();
    return s;
}
void TimeMeasure::SingleOutput(std::string name)
{
	//File output
	std::string filename = name + ".csv";
	std::ofstream file;
	file.open(filename.c_str());

    file << "Time in seconds: " << GetTimeInSeconds() << "\n";
    file << "Time in milliseconds: " << GetTimeInMilliseconds() << "\n";
    file << "Time in nanoseconds: " << GetTimeInNanoseconds() << "\n";

	file.close();
}

void TimeMeasure::SingleAverageOutput(std::string name)
{
    //File output
    std::string filename = name + ".csv";
    std::ofstream file;
    file.open(filename.c_str());

    file << "Time in milliseconds: " << (averageDiff/amount) << "\n";

    file.close();
}

void TimeMeasure::SingleOutputWithRam(std::string name)
{
    std::string filename = name + ".csv";
    std::ofstream file;
    file.open(filename.c_str());

    file << "Time in seconds: " << GetTimeInSeconds() << "\n";
    file << "Time in milliseconds: " << GetTimeInMilliseconds() << "\n";
    file << "Time in nanoseconds: " << GetTimeInNanoseconds() << "\n";

    file << "VRAM used at start: " << vramStart << "\n";
    file << "VRAM used at end: " << vramEnd << "\n";
    file << "VRAM change: " << vramEnd - vramStart << "\n";
    file << "RAM used at start:: " << ramStart << "\n";
    file << "RAM used at end: " << ramEnd << "\n";
    file << "RAM change: " << ramEnd - ramStart << "\n";

    file.close();
}

void TimeMeasure::ContinuousCaptureStart()
{
    CaptureStart();
}

void TimeMeasure::ContinuousCaptureEnd()
{
    CaptureEnd();
    amount++;
    auto s = std::chrono::duration_cast<std::chrono::milliseconds>(endPoint - startPoint).count();
    averageDiff += s;
    
}

SIZE_T TimeMeasure::CaptureVRam()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
    return virtualMemUsedByMe;
}

SIZE_T TimeMeasure::CaptureRam()
{
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
    return physMemUsedByMe;
}


