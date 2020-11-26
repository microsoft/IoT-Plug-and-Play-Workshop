#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

struct cpu_part_name {
    const int id;
    const char* name;
};

static const struct cpu_part_name arm_part[] = {
    { 0x810, "ARM810" },
    { 0x920, "ARM920" },
    { 0x922, "ARM922" },
    { 0x926, "ARM926" },
    { 0x940, "ARM940" },
    { 0x946, "ARM946" },
    { 0x966, "ARM966" },
    { 0xa20, "ARM1020" },
    { 0xa22, "ARM1022" },
    { 0xa26, "ARM1026" },
    { 0xb02, "ARM11 MPCore" },
    { 0xb36, "ARM1136" },
    { 0xb56, "ARM1156" },
    { 0xb76, "ARM1176" },
    { 0xc05, "Cortex-A5" },
    { 0xc07, "Cortex-A7" },
    { 0xc08, "Cortex-A8" },
    { 0xc09, "Cortex-A9" },
    { 0xc0d, "Cortex-A17" },	/* Originally A12 */
    { 0xc0f, "Cortex-A15" },
    { 0xc0e, "Cortex-A17" },
    { 0xc14, "Cortex-R4" },
    { 0xc15, "Cortex-R5" },
    { 0xc17, "Cortex-R7" },
    { 0xc18, "Cortex-R8" },
    { 0xc20, "Cortex-M0" },
    { 0xc21, "Cortex-M1" },
    { 0xc23, "Cortex-M3" },
    { 0xc24, "Cortex-M4" },
    { 0xc27, "Cortex-M7" },
    { 0xc60, "Cortex-M0+" },
    { 0xd01, "Cortex-A32" },
    { 0xd03, "Cortex-A53" },
    { 0xd04, "Cortex-A35" },
    { 0xd05, "Cortex-A55" },
    { 0xd07, "Cortex-A57" },
    { 0xd08, "Cortex-A72" },
    { 0xd09, "Cortex-A73" },
    { 0xd0a, "Cortex-A75" },
    { 0xd0b, "Cortex-A76" },
    { 0xd0c, "Neoverse-N1" },
    { 0xd13, "Cortex-R52" },
    { 0xd20, "Cortex-M23" },
    { 0xd21, "Cortex-M33" },
    { 0xd4a, "Neoverse-E1" },
    { -1, "unknown" },
};

static const struct cpu_part_name intel_part[] = {
    { -1, "unknown" },
};

struct cpu_implementer {
   const int    id; // from /proc/cpuinfo
   const struct cpu_part_name  *cpuName;
   const char   *vendorName;
};

static const struct cpu_part_name unknown_part[] = {
    { -1, "unknown" },
};

static const struct cpu_implementer cpu_implementer[] = {
    { 0x41, arm_part,     "ARM" },
    { 0x69, intel_part,   "Intel" },
    { -1,   unknown_part, "unknown" },
};

int getModel(char* buffer, size_t bufferSize);
int getSwVersion(char* buffer, size_t bufferSize);
int getOsName(char* buffer, size_t bufferSize);
int getCpuArchitecture(char* buffer, size_t bufferSize);
int getCpuManufacturer(char* buffer, size_t bufferSize);
double getMemory(void);
double getDisk(void);
int getDeviceInforProperty(char* buffer, size_t bufferSize);