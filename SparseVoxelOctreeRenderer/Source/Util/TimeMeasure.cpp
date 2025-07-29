#include "TimeMeasure.h"
#include <ostream>
#include <iostream>
#include <fstream>

void TimeMeasure::Reset()
{
    AverageDiff = 0;
    Amount = 0;
    VramEnd = 0;
    VramStart = 0;
    RamStart = 0;
    RamEnd = 0;
    StartPoint = std::chrono::steady_clock::now();
    EndPoint = std::chrono::steady_clock::now();
    IsUniqueContinuousCapture = true;
}

void TimeMeasure::CaptureStart()
{
    VramStart = CaptureVRam();
    RamStart = CaptureRam();
    StartPoint = std::chrono::steady_clock::now();
}

void TimeMeasure::CaptureEnd()
{
    VramEnd = CaptureVRam();
    RamEnd = CaptureRam();
    EndPoint = std::chrono::steady_clock::now();
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


double TimeMeasure::GetTimeInSeconds()
{
    auto s = std::chrono::duration_cast<std::chrono::seconds>(EndPoint - StartPoint).count();
    return s;
}

double TimeMeasure::GetTimeInMilliseconds()
{
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(EndPoint - StartPoint).count();
    return ms;
}

double TimeMeasure::GetTimeInNanoseconds()
{
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(EndPoint - StartPoint).count();
    return ns;
}
double TimeMeasure::GetAverageTimeInMilliseconds()
{
    auto ms = AverageDiff / Amount;
    return ms;
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

    file << "Time in milliseconds: " << (AverageDiff/Amount) << "\n";

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

    file << "VRAM used at start: " << VramStart << "\n";
    file << "VRAM used at end: " << VramEnd << "\n";
    file << "VRAM change: " << VramEnd - VramStart << "\n";
    file << "RAM used at start:: " << RamStart << "\n";
    file << "RAM used at end: " << RamEnd << "\n";
    file << "RAM change: " << RamEnd - RamStart << "\n";

    file.close();
}

void TimeMeasure::ContinuousCaptureStart()
{
    if(IsUniqueContinuousCapture)
    {
        VramStart = CaptureVRam();
        RamStart = CaptureRam();
        IsUniqueContinuousCapture = false;
	}

    StartPoint = std::chrono::steady_clock::now();
}

void TimeMeasure::ContinuousCaptureEnd()
{
    CaptureEnd();
    Amount++;
    auto s = std::chrono::duration_cast<std::chrono::milliseconds>(EndPoint - StartPoint).count();
    AverageDiff += s;
    
}



