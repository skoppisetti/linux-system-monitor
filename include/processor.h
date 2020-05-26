#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
    public:
    float Utilization();  // TODO: See src/processor.cpp

    // TODO: Declare any necessary private members
    private:
    long cpu_active, cpu_inactive, prev_cpu_active, prev_cpu_inactive;
};

#endif