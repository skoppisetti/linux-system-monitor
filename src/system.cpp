#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    for (int pid : pids) {
        // 1. Get User
        string user = LinuxParser::User(pid);

        // 2. Get CommandLine
        string commandLine = LinuxParser::Command(pid);

        // 3. Get CPU Utilization
        float cpuUtilization = 0.0; //LinuxParser::CpuUtilization(pid);

        // 4. Get RAM
        string ram = LinuxParser::Ram(pid);

        // 5. Get Uptime
        long int uptime = LinuxParser::UpTime(pid);

        // 6. Create instance of the Process
        Process p(pid, user, commandLine, cpuUtilization, ram, uptime);

        // 7. Add process to the processes vector
        processes_.push_back(p);
    }
    return processes_; 
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }