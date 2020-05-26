#include <string>
#include <stdexcept>

#include "process.h"

using std::string;
using std::to_string;

Process::Process(
    int pid,
    string user, 
    string command,
    float cpuUtilization,
    string ram,
    long int uptime) 
    : pid_(pid), cpuUtilization_(cpuUtilization), command_(command), ram_(ram), user_(user), uptime_(uptime) {}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return cpuUtilization_;  }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    // return a.cpuUtilization_ < cpuUtilization_? true : false;
    bool pred = true;
    try {
        int lram = std::stoi(ram_);
        int rram = std::stoi(a.ram_);
        pred = lram < rram;
    } catch (std::invalid_argument& e) {
        pred = false;
    }
    return pred;
} 