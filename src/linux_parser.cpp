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

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
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
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  float totalMem, availableMem;
  bool readTotalMem, readAvailableMem = false;
  string key, value;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      //std::replace(line.begin(), line.end(), ':', ' ');
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
      if(readTotalMem && readAvailableMem) return (totalMem - availableMem) / totalMem;
    }
  }
  return 0.0; 
}

// TODO: Read and return the system uptime
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

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[0]) + std::stol(cpureadings[1]) 
    + std::stol(cpureadings[2]) + std::stol(cpureadings[3]) 
    + std::stol(cpureadings[4]) + std::stol(cpureadings[5])
    + std::stol(cpureadings[6]) + std::stol(cpureadings[7]); 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  // std::cout << "Calculating active jiffies for : " << pid << std::endl;
  vector<string> pstat{22};
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> pstat[0] >> pstat[1] >> pstat[2] >> pstat[3] >> pstat[4] >> pstat[5] >> pstat[6] >> pstat[7]
      >> pstat[8] >> pstat[9] >> pstat[10] >> pstat[11] >> pstat[12] >> pstat[13] >> pstat[14] >> pstat[15]
      >> pstat[16] >> pstat[17] >> pstat[18] >> pstat[19] >> pstat[20] >> pstat[21];
    // std::cout << "|" << pstat[13] << "|" << pstat[14] << "|" << pstat[15] << "|" << pstat[16] << "|" <<std::endl;
    return std::stol(pstat[13]) + std::stol(pstat[14]) + std::stol(pstat[15]) + std::stol(pstat[16]);
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[0]) + std::stol(cpureadings[1]) 
    + std::stol(cpureadings[2]) + std::stol(cpureadings[5])
    + std::stol(cpureadings[6]) + std::stol(cpureadings[7]);  
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpureadings = LinuxParser::CpuUtilization();
  return std::stol(cpureadings[3]) + std::stol(cpureadings[4]); 
}

// TODO: Read and return CPU utilization
//      user   nice  system  idle    iowait  irq  softirq  steal   guest   guest_nice
// cpu  12684  1928  5297    265504  1343    0    134      0       0       0
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

// TODO: Read and return the total number of processes
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

// TODO: Read and return the number of running processes
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

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    return line;
  }
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string key, value;
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:") {
        string size, unit;
        std::istringstream memvaluestream(value);
        memvaluestream >> size >> unit;
        return std::to_string(std::stoi(size)/1024);
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
// Example: games:x:5:60:games:/usr/games:/usr/sbin/nologin
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

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
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
    return std::stol(pstat[21]) / sysconf(_SC_CLK_TCK);
  }
  return 0; 
}