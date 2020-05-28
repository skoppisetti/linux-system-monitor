#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/**
 * Function to extract operating system name from the /etc/os-release file
 * 
 * @return string, the name of the os
 */
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

/**
 * Function to extract the kernel information from /proc/version file
 * 
 * @return string, the name of the kernel
 */
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/**
 * Function to extract the process ids of all running processes
 * 
 * @return vector of all process ids
 */
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
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

/**
 * Function to memmory utilization using info in the /proc/meminfo file
 * 
 * @return float, the memory utilization
 */
float LinuxParser::MemoryUtilization() { 
  string line;
  float totalMem, availableMem;
  bool readTotalMem, readAvailableMem = false;
  string key, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      if (key == "MemTotal:") {
        readTotalMem = true;
        totalMem = std::stof(value);
      }

      if (key == "MemAvailable:") {
        readAvailableMem = true;
        availableMem = std::stof(value);
      }

      // Calculate utilization only if MemTotal and MemAvailable are both read from the file
      if(readTotalMem && readAvailableMem) return (totalMem - availableMem) / totalMem;
    }
  }
  return 0.0; 
}

/**
 * Function to extract the uptime information from /proc/uptime file
 * 
 * @return long, the system uptime
 */
long LinuxParser::UpTime() {
  string line;
  string uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  std::string::size_type sz;
  return std::stol(uptime, &sz); 
}

/**
 * Function to calculate the total number of ticks which is:
 * user + niced + system + idle + iowait + irq + softirq times
 * 
 * @return long, the total number of ticks
 */
long LinuxParser::Jiffies() { 
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[0]) + std::stol(cpureadings[1]) 
    + std::stol(cpureadings[2]) + std::stol(cpureadings[3]) 
    + std::stol(cpureadings[4]) + std::stol(cpureadings[5])
    + std::stol(cpureadings[6]) + std::stol(cpureadings[7]); 
}

/**
 * Function to extract the process tick information from /proc/[pid]/stat file as:
 * utime (time spent on user code) + 
 * stime (time spent on kernel code) + 
 * cutime (time spent on the child's user code) +
 * cstime (time spent on the kernel by the child's code)
 * 
 * @param pid, process id
 * @return long, the active ticks used by the process
 */
long LinuxParser::ActiveJiffies(int pid) { 
  vector<string> pstat{22};
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pstat[0] >> pstat[1] >> pstat[2] >> pstat[3] >> pstat[4] >> pstat[5] >> pstat[6] >> pstat[7]
      >> pstat[8] >> pstat[9] >> pstat[10] >> pstat[11] >> pstat[12] >> pstat[13] >> pstat[14] >> pstat[15]
      >> pstat[16] >> pstat[17] >> pstat[18] >> pstat[19] >> pstat[20] >> pstat[21];
    
    return std::stol(pstat[13]) + std::stol(pstat[14]) + std::stol(pstat[15]) + std::stol(pstat[16]);
  }
  return 0; 
}

/**
 * Function to calulate the active cpu ticks as:
 * user + nice + system + irq + softirq + steal times
 * 
 * @return long, the total active cpu ticks
 */
long LinuxParser::ActiveJiffies() { 
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[0]) + std::stol(cpureadings[1]) 
    + std::stol(cpureadings[2]) + std::stol(cpureadings[5])
    + std::stol(cpureadings[6]) + std::stol(cpureadings[7]);  
}

/**
 * Function to calulate the idle cpu ticks as:
 * idle + iowait times
 * 
 * @return long, the total idle cpu ticks
 */
long LinuxParser::IdleJiffies() {
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[3]) + std::stol(cpureadings[4]); 
}

/**
 * Extract the CPU Utilization information from the /proc/stat file
 * 
 * Sample layout of the file:
 *      user   nice  system  idle    iowait  irq  softirq  steal   guest   guest_nice
 * cpu  12684  1928  5297    265504  1343    0    134      0       0       0
 * 
 * @return vector of all the readings for all the cpus in aggregate 
 */
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpureadings(10);
  string key, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
      if (key == "cpu") {
        cpureadings[0] = user;
        cpureadings[1] = nice;
        cpureadings[2] = system;
        cpureadings[3] = idle;
        cpureadings[4] = iowait;
        cpureadings[5] = irq;
        cpureadings[6] = softirq;
        cpureadings[7] = steal;
        cpureadings[8] = guest;
        cpureadings[9] = guest_nice;
        return cpureadings;
      }
    }
  }
  return {}; 
}

/**
 * Function to retrieve the total number of processes from /proc/stat file
 * 
 * @return int, the total number of processes
 */
int LinuxParser::TotalProcesses() { 
  string key, totalProcesses;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> totalProcesses;
      if (key == "processes") return std::stoi(totalProcesses);
    }
  }
  return 0; 
}

/**
 * Function to retrieve the number of currently running preocesses from /proc/stat file
 * 
 * @return int, the number of currently running processes
 */
int LinuxParser::RunningProcesses() { 
  string key, runningProcesses;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while(std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> runningProcesses;
      if (key == "procs_running") return std::stoi(runningProcesses);
    }
  }
  return 0; 
}

/**
 * Function to retrieve the command which spawned a process from /proc/[pid]/cmdline file
 * 
 * @param pid, process id
 * @return a string, the command line executable that spawned the processess
 */
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    return line;
  }
  return string(); 
}

/**
 * Function to retrieve the RAM used by a process from /proc/[pid]/status file
 * 
 * @param pid, process id
 * @return string, the ram used by the process in mega bytes
 */
string LinuxParser::Ram(int pid) { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmData:") {
        string size, unit;
        std::istringstream memvaluestream(value);
        memvaluestream >> size >> unit;
        return std::to_string(std::stoi(size)/1024);
      }
    }
  }
  return string(); 
}

/**
 * Function to retrieve the user id that spawned the process from /proc/[pid]/status file
 * 
 * @param pid, process id
 * @return string, the user id
 */
string LinuxParser::Uid(int pid) { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:") return value;
    }
  }
  return string(); 
}

/**
 * Function to extract the user name of the user that spawned a process from the /etc/passwd file
 * 
 * Example: games:x:5:60:games:/usr/games:/usr/sbin/nologin
 * 
 * @param pid, the process id
 * @return string, the user name
 */
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string user, x, id;
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      // std::replace(line.begin(), line.end(), ":x:", ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> id;
      if(id == uid) return user;
    }
  }
  return string(); 
}

/**
 * Function to extract and calcuate the uptime of a process using the information from /proc/[pid]/stat file
 * 
 * @param pid, the process id
 * @return long, process uptime
 */
long LinuxParser::UpTime(int pid) { 
  vector<string> pstat{22};
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream>> pstat[0] >> pstat[1] >> pstat[2] >> pstat[3] >> pstat[4] >> pstat[5] >> pstat[6] >> pstat[7]
      >> pstat[8] >> pstat[9] >> pstat[10] >> pstat[11] >> pstat[12] >> pstat[13] >> pstat[14] >> pstat[15]
      >> pstat[16] >> pstat[17] >> pstat[18] >> pstat[19] >> pstat[20] >> pstat[21];
    int processUptime = UpTime() - std::stol(pstat[21]) / sysconf(_SC_CLK_TCK);
    return processUptime;
  }
  return 0; 
}