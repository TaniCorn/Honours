#include "TimeMeasure.h"
#include <ostream>
#include <iostream>
#include <fstream>
void TimeMeasure::CaptureStart()
{
    startPoint = std::chrono::steady_clock::now();
}

void TimeMeasure::CaptureEnd()
{
    endPoint = std::chrono::steady_clock::now();
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


