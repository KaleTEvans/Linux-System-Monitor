#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : _pid(pid) {

    long uptime = LinuxParser::UpTime() - LinuxParser::UpTime(_pid);
    long totalTime = LinuxParser::ActiveJiffies(_pid);

    // using a try catch block here to get rid of 0s and infinite values
    try {
        _CpuUtilization = float(totalTime) / float(uptime);

    } catch (...) {
        _CpuUtilization = 0;
    }
}

// Return this process's ID
int Process::Pid() { return _pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return _CpuUtilization; }

// Return the command that generated this process
string Process::Command() {
    string command = LinuxParser::Command(_pid);

    if (command.size() > 50) {
        return command.substr(0, 50) + "...";
    } 

    return command;
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(_pid); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(_pid); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime() - LinuxParser::UpTime(_pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }