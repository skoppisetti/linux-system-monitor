#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { 
    vector<int> pids = LinuxParser::Pids();
    for (int pid : pids) {
        // 1. Get User
        string user = LinuxParser::User(pid);

        // 2. Get CommandLine
        string commandLine = LinuxParser::Command(pid);

        // 3. Get RAM
        string ram = LinuxParser::Ram(pid);

        // 4. Get Uptime
        long int process_uptime = LinuxParser::UpTime(pid);

        // 5. Get CPU Utilization
        long system_uptime = LinuxParser::UpTime();
        long total_time = LinuxParser::ActiveJiffies(pid);
        long time_elapsed = system_uptime - process_uptime;
        float cpuUtilization;
        if(time_elapsed <= 0) cpuUtilization = 0;
        else cpuUtilization = ((total_time / sysconf(_SC_CLK_TCK)) / (float)time_elapsed);
    
        // 6. Create instance of the Process
        Process p(pid, user, commandLine, cpuUtilization, ram, process_uptime);

        // 7. Add process to the processes vector
        processes_.push_back(p);
    }
    std::sort (processes_.begin(), processes_.end());
    return processes_; 
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }