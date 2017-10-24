#ifndef CPUAFIINITYINTERFACE_H
#define CPUAFIINITYINTERFACE_H
#ifdef __linux__
#include <sched.h>
#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>
#endif


inline int setAffinity()
{
#ifdef __linux__
    qDebug()<< "Setting Linux cpu affinity";
    long int ncpus = sysconf(_SC_NPROCESSORS_ONLN);
    cpu_set_t my_set;
    CPU_ZERO(&my_set);
    for (int i = 0; i < ncpus; ++i) {
        CPU_SET(i, &my_set);
    }
    sched_setaffinity(0, sizeof(my_set), &my_set);
    int NO_OF_CPUS = CPU_COUNT(&my_set);
    std::cout<< "Number of CPU's on current machine :" + std::to_string(ncpus) <<std::endl;
    std::cout<< "Number of Active CPUS for this process : " + std::to_string(NO_OF_CPUS)<< std::endl;
    return NO_OF_CPUS;
#endif
    return -1;
}

#endif // CPUAFIINITYINTERFACE_H
