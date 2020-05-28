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
    : pid_(pid), user_(user), command_(command), cpuUtilization_(cpuUtilization), ram_(ram), uptime_(uptime) {}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpuUtilization_;  }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

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