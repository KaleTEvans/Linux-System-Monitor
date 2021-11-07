#include <string>

#include "format.h"

using std::string;

string LeadingZero(int value) {
    if (value < 10) {
        return "0";
    } else {
        return "";
    }
}

// Time formatting helper function
string Format::ElapsedTime(long seconds) { 
    long time = seconds;

    int timeHours = time / 3600;
    string hours = LeadingZero(timeHours) + std::to_string(timeHours);

    time = time % 3600;

    int timeMins = time / 60;
    string mins = LeadingZero(timeMins) + std::to_string(timeMins);

    time = time % 60;

    int timeSecs = time;
    string secs = LeadingZero(timeSecs) + std::to_string(timeSecs);

    string formattedTime = hours + ":" + mins + ":" + secs;
    return formattedTime;
}