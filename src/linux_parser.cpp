#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

// Helper Functions
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

// Parse OS
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Parse kernel info
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return memory utilization in percent format
float LinuxParser::MemoryUtilization() { 
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

// Function to parse the system uptime
long LinuxParser::UpTime() { 
  string line;
  long upTime;

  string time = getValueOfFile<string>(kUptimeFilename);
  upTime = std::stol(time);
  return upTime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies(); }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long timeTotal;
  string line, time;
  vector<string> timeArr;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while(linestream >> time) {
      timeArr.emplace_back(time);
    }
  }

  timeTotal = std::stol(timeArr[13])
     + std::stol(timeArr[14])
     + std::stol(timeArr[15])
     + std::stol(timeArr[16]);

  return timeTotal / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  auto jiffies = CpuUtilization();

  auto active = stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
                stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
                stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);

  return active;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  auto jiffies = CpuUtilization();

  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> jiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;

    jiffies = {user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice};
  }
  return jiffies;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string Command;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  if (filestream.is_open()) {
    std::getline(filestream, Command);
  }

  return Command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line, key, ramUsed;
  long memoryUsage;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmRSS:") { // using VmRSS instead of VmSize to get exact physical memory being used rather than virtual memory
        linestream >> memoryUsage;
        ramUsed = std::to_string(memoryUsage / 1000);
      } 
    }
  }

  return ramUsed;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, uid;
  
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);

  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string line, x, y, user;
  string name;
  std::ifstream stream(kPasswordPath);

  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);

      linestream >> x >> y >> user;
      if (user == uid) {
        name = x;
      }
    }
  }

  return name;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line, time;
  vector<string> timeValues;
  long timeInit = 0;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);

  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    while (linestream >> time) {
      timeValues.push_back(time);
    }
  }

  try {
    timeInit = stol(timeValues[21]) / sysconf(_SC_CLK_TCK);
  } catch (...) {
    timeInit = 0;
  }

  return timeInit;
}
