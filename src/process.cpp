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
    _command = LinuxParser::Command(_pid);
    _ram = std::stol(LinuxParser::Ram(_pid));
    _user = LinuxParser::User(_pid);
    _uptime = LinuxParser::UpTime(_pid);

    long uptime = LinuxParser::UpTime() - _uptime;
    long totalTime = LinuxParser::ActiveJiffies(_pid);

    _CpuUtilization = float(uptime) / float(totalTime);
}

// Return this process's ID
int Process::Pid() { return _pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() { return _CpuUtilization; }

// Return the command that generated this process
string Process::Command() { return _command; }

// Return this process's memory utilization
string Process::Ram() { return _ram; }

// Return the user (name) that generated this process
string Process::User() { return _user; }

// Return the age of this process (in seconds)
long int Process::UpTime() { return _uptime; }

//  Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return CpuUtilization() < a.CpuUtilization(); }