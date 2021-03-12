#include "pch.h"
#include "sysInfo.h"

#ifdef _MSC_VER

typedef struct _PROCESS_MEMORY_COUNTERS_EX {
    unsigned long cb;
    unsigned long PageFaultCount;
    size_t PeakWorkingSetSize;
    size_t WorkingSetSize;
    size_t QuotaPeakPagedPoolUsage;
    size_t QuotaPagedPoolUsage;
    size_t QuotaPeakNonPagedPoolUsage;
    size_t QuotaNonPagedPoolUsage;
    size_t PagefileUsage;
    size_t PeakPagefileUsage;
    size_t PrivateUsage;
} PROCESS_MEMORY_COUNTERS_EX;
typedef PROCESS_MEMORY_COUNTERS_EX* PPROCESS_MEMORY_COUNTERS_EX;

extern "C"
{
    __declspec(dllimport) void* __stdcall GetCurrentProcess(void);
    __declspec(dllimport) int __stdcall K32GetProcessMemoryInfo(void * Process, PROCESS_MEMORY_COUNTERS_EX* ppsmemCounters, unsigned long cb);
}
#endif


/*
TODO:
Общая доступная виртуальная память
Используемая в настоящее время виртуальная память
Виртуальная память, используемая в данный момент моим процессом

Всего доступно RAM
RAM в настоящее время используется
RAM в настоящее время используется моим процессом

% CPU используется в настоящее время
% CPU в настоящее время используется моим процессом

*/

const size_t get_cpuCount() {
    return 0;
}

const size_t get_process_privateMemory() {
    size_t total = 0;
#ifdef _MSC_VER
    PROCESS_MEMORY_COUNTERS_EX pm;
    K32GetProcessMemoryInfo(GetCurrentProcess(), &pm, sizeof(pm));
    total = pm.PrivateUsage;
#endif
    return total;
}

const size_t get_process_sizeMemory() {
    size_t total = 0;
#ifdef _MSC_VER
    PROCESS_MEMORY_COUNTERS_EX pm;
    K32GetProcessMemoryInfo(GetCurrentProcess(), &pm, sizeof(pm));
    total = pm.PeakWorkingSetSize;
#endif
    return total;
}

const int get_availableRAM() {
    return 0;
}

