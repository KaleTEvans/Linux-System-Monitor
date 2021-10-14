#include <string>

#include "format.h"

using std::string;

// Time formatting helper function
string Format::ElapsedTime(long seconds) { 
    long time = seconds;

    int timeHours = time / 3600;
    string hours = std::to_string(timeHours);

    time = time % 3600;

    int timeMins = time / 60;
    string mins = std::to_string(timeMins);

    time = time % 60;

    int timeSecs = time;
    string secs = std::to_string(timeSecs);

    string formattedTime = hours + ":" + mins + ":" + secs;
    return formattedTime;
}