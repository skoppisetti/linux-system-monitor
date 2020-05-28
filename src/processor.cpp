#include <vector>
#include <string>
#include <cassert>

#include "processor.h"
#include "linux_parser.h"

/**
 * Function to caluclate the processor utilization using the tick information
 * 
 *      user   nice  system  idle    iowait  irq  softirq  steal   guest   guest_nice
 * cpu  12684  1928  5297    265504  1343    0    134      0       0       0
 * 
 * @return float, the CPU utilization
 */
float Processor::Utilization() { 
    long int total = LinuxParser::Jiffies();
    long int cpu_inactive = LinuxParser::IdleJiffies();
    long int cpu_active = LinuxParser::ActiveJiffies();
    
    long int delta_tot = total - (prev_cpu_active + prev_cpu_inactive);
    long int delta_idle = cpu_inactive - prev_cpu_inactive;

    prev_cpu_active = cpu_active;
    prev_cpu_inactive = cpu_inactive;

    return (delta_tot - delta_idle)/(float)delta_tot; 
}