#include <vector>
#include <string>
#include <cassert>

#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
//      user   nice  system  idle    iowait  irq  softirq  steal   guest   guest_nice
// cpu  12684  1928  5297    265504  1343    0    134      0       0       0
float Processor::Utilization() { 
    std::vector<std::string> cpu_utilization = LinuxParser::CpuUtilization();
    
    // Something is wrong if the cpu_utilization vector is not atleast 8 items long
    if(cpu_utilization.size() < 8) return 0.0;

    // Parse the vector for current utilizations
    user = std::stol(cpu_utilization[0]);
    nice = std::stol(cpu_utilization[1]);
    system = std::stol(cpu_utilization[2]);
    idle = std::stol(cpu_utilization[3]);
    iowait = std::stol(cpu_utilization[4]);
    irq = std::stol(cpu_utilization[5]);
    softirq = std::stol(cpu_utilization[6]);
    steal = std::stol(cpu_utilization[7]);

    long int tot_previdle = previdle + previowait;
    long int tot_idle = idle + iowait;
    long int tot_prevnonidle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal;
    long int tot_nonidle = user + nice + system + irq + softirq + steal;

    long int total_prev = tot_previdle + tot_prevnonidle;
    long int total = tot_idle + tot_nonidle;

    long int delta_tot = total - total_prev;
    long int delta_idle = tot_idle - tot_previdle;

    // Assign the current reading to the prev variables
    prevuser = user;
    prevnice = nice;
    prevsystem = system;
    previdle = idle;
    previowait = iowait;
    previrq = irq;
    prevsoftirq = softirq;
    prevsteal = steal;

    return (delta_tot - delta_idle)/(float)delta_tot; 
}