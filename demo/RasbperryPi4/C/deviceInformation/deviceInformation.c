#include <sys/stat.h>
#include "deviceInformation.h"

int getManufacturer(char* buffer, size_t bufferSize)
{

    if (snprintf(buffer, bufferSize, "%s", "Raspberry Pi Foundation") < 0)
    {
        printf("ERR : snprintf %s failed\r\n", __func__);
        return -1;
    }
    return 0;    
}

int getModel(char* buffer, size_t bufferSize)
{
    struct stat st;
    size_t len;
    const char fileName[] = "/proc/device-tree/model"; // /sys/firmware/devicetree/base/model
    FILE* fFile = fopen(fileName, "r");

    stat(fileName, &st);    

    if (st.st_size > bufferSize)
    {
        printf("ERR : %s : Buffer too small\r\n", __func__);
    }
    else if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else if ((len = fread(buffer, 1, st.st_size, fFile)) < 0)
    {
        printf("ERR : %s : Failed to read from file %s\r\n", __func__, fileName);
    }

    printf("%s\r\n", buffer);

    if (fFile > 0)
        fclose(fFile);
}

int getSwVersion(char* buffer, size_t bufferSize)
{
    struct stat st;
    const char fileName[] = "/etc/os-release";
    char* localBuffer;

    FILE* fFile = fopen(fileName, "r");

    stat(fileName, &st);

    localBuffer = malloc(st.st_size + 1);

    if ((localBuffer = malloc(st.st_size + 1)) == NULL)
    {
        printf("ERR : %s : Faled to allocate buffer\r\n", __func__);
    }
    else if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else
    {
        while (fgets(localBuffer, st.st_size, fFile))
        {
            char *p, *value;
            char buf[512];

            // Split into key=value pair
            p = strchr(localBuffer, '=');
            if (!p)
            {
                continue;
            }

            *p++ = '\0';
            value = &buf[0];

            // Remove quotes
            while (*p)
            {
                if ((*p == '"') || (*p == '\n'))
                {
                    p++;
                }
                else{
                    *value = *p;
                    value++;
                    p++;
                }
            }
            *value = '\0';

            if (!strcmp(localBuffer, "VERSION"))
            {
                if (strlen(buf) > bufferSize)
                {
                     printf("ERR : %s : Buffer too small %s\r\n", __func__, buf);
                }

                strcpy(buffer, buf);
                break;
            }
        }
    }

    if (fFile > 0)
        fclose(fFile);
}

int getOsName(char* buffer, size_t bufferSize)
{
    struct stat st;
    const char fileName[] = "/etc/os-release";
    char* localBuffer;

    FILE* fFile = fopen(fileName, "r");

    stat(fileName, &st);

    localBuffer = malloc(st.st_size + 1);

    if ((localBuffer = malloc(st.st_size + 1)) == NULL)
    {
        printf("ERR : %s : Faled to allocate buffer\r\n", __func__);
    }
    else if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else
    {
        while (fgets(localBuffer, st.st_size, fFile))
        {
            char *p, *value;
            char buf[512];

            // Split into key=value pair
            p = strchr(localBuffer, '=');
            if (!p)
            {
                continue;
            }

            *p++ = '\0';
            value = &buf[0];

            // Remove quotes
            while (*p)
            {
                if ((*p == '"') || (*p == '\n'))
                {
                    p++;
                }
                else{
                    *value = *p;
                    value++;
                    p++;
                }
            }
            *value = '\0';

            if (!strcmp(localBuffer, "PRETTY_NAME"))
            {
                if (strlen(buf) > bufferSize)
                {
                     printf("ERR : %s : Buffer too small %s\r\n", __func__, buf);
                }

                strcpy(buffer, buf);
                break;
            }
        }
    }

    if (fFile > 0)
        fclose(fFile);

    if (localBuffer)
        free(localBuffer);
}

int getCpuManufacturer(char* buffer, size_t bufferSize)
{
    const char fileName[] = "/proc/cpuinfo";
    char localBuffer[128];

    FILE* fFile = fopen(fileName, "r");

    if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else
    {
        while (fgets(localBuffer, sizeof(localBuffer) - 1, fFile))
        {
            if (strstr(localBuffer, "Hardware"))
            {
                char *value;

                value = strstr(localBuffer, ": ");

                if (!value)
                {
                    continue;
                }

                if (strstr(value, "BCM"))
                {
                    strcpy(buffer, "Broadcom");
                    break;
                }
            }
        }
    }

    if (fFile > 0)
        fclose(fFile);
}

int getCpuArchitecture(char* buffer, size_t bufferSize)
{
    const char fileName[] = "/proc/cpuinfo";
    char localBuffer[128];

    FILE* fFile = fopen(fileName, "r");

    if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else
    {
        int implementer = 0;
        int cpuPart = 0;

        while (fgets(localBuffer, sizeof(localBuffer) - 1, fFile))
        {
            int i;
            char *value;
            char *p;
            char *end;

            if (strstr(localBuffer, "CPU implementer"))
            {
                value = strstr(localBuffer, ": ");

                if (!value)
                {
                    continue;
                }

                value += 2;
                p = value;
                while (*p)
                {
                    if (*p == '\n')
                    {
                        *p = '\0';
                    }
                    p++;
                }

                implementer = (int) strtol(value, &end, 0);
            }
            else if (strstr(localBuffer, "CPU part"))
            {
                value = strstr(localBuffer, ": ");

                if (!value)
                {
                    continue;
                }

                value += 2;
                p = value;
                while (*p)
                {
                    if (*p == '\n')
                    {
                        *p = '\0';
                    }
                    p++;
                }

                cpuPart = (int) strtol(value, &end, 0);
            }

            printf("Implementer 0x%x CPU part 0x%x\r\n", implementer, cpuPart);

            if (cpuPart != 0 && implementer != 0)
            {
                const struct cpu_part_name* cpuPartName = NULL;

                for (i = 0 ; cpu_implementer[i].id != -1; i++)
                {
                    if (cpu_implementer[i].id == implementer)
                    {
                        cpuPartName = cpu_implementer[i].cpuName;
                        break;
                    }
                }

                if (cpuPartName)
                {
                    for (i = 0 ; cpuPartName[i].id != -1; i++)
                    {
                        if (cpuPartName[i].id == cpuPart || cpuPartName[i].id == -1)
                        {
                            strcpy(buffer, cpuPartName[i].name);
                        }
                    }
                }
                break;
            }
        }
    }

    if (fFile > 0)
        fclose(fFile);
}

double getMemory(void)
{
    const char fileName[] = "/proc/meminfo";
    char localBuffer[128];
    char *end;
    char buf[32];
    char *value;
    char *kb;
    char *p;

    FILE* fFile = fopen(fileName, "r");

    if ((fFile = fopen(fileName, "r")) < 0)
    {
        printf("ERR : %s : Failed to open file %s\r\n", __func__, fileName);
    }
    else
    {
        while (fgets(localBuffer, sizeof(localBuffer) - 1, fFile))
        {
            if (strncmp(localBuffer, "MemTotal", 7) == 0)
            {
                value = strstr(localBuffer, ":");
                kb = strstr(localBuffer, "kB");

                if (!value)
                {
                    continue;
                }

                if (kb)
                {
                    *kb = '\0';
                }

                p = value;
                value = &buf[0];

                while(*p)
                {
                    if (*p == ' ' || *p == ':')
                    {
                        p++;
                    } 
                    else if (*p == '\n')
                    {
                        *p = '\0';
                    }
                    else
                    {
                        *value = *p;
                        p++;
                        value++;
                    }
                }
                *value = '\0';
                printf("Memory %s (%d)\r\n", buf, strlen(buf));
            }
        }
    }

    if (fFile > 0)
        fclose(fFile);

    return strtol(buf, &end, 0);
}

double getDisk(void)
{
    return 0.0;
}

int getDeviceInforProperty(char* buffer, size_t bufferSize)
{
    char strManufacturer[64];
    char strModel[64];
    char strSwVersion[64];
    char strOsName[64];
    char strProcessorArchitecture[64];
    char strProcessorManufacturer[64];
    double storage;
    double memory;
    
    getManufacturer(strManufacturer, sizeof(strManufacturer));
    getModel(strModel, sizeof(strModel));
    getSwVersion(strSwVersion, sizeof(strSwVersion));
    getOsName(strOsName, sizeof(strOsName));
    getCpuArchitecture(strProcessorArchitecture, sizeof(strProcessorArchitecture));
    getCpuManufacturer(strProcessorManufacturer, sizeof(strProcessorManufacturer));
    storage = getDisk();
    memory = getMemory();

    if (snprintf(buffer, bufferSize, "{\"%s\":{\"__t\":\"c\",\"manufacturer\":\"%s\", \"model\":\"%s\",  \"swVersion\":\"%s\", \"osName\":\"%s\", \"processorArchitecture\":\"%s\", \"processorManufacturer\":\"%s\", \"totalStorage\":%f, \"totalMemory\":%f}}", "deviceInfo", strManufacturer, strModel, strSwVersion, strOsName, strProcessorArchitecture, strProcessorManufacturer, storage, memory) < 0)
    {
        printf("ERR : snprintf building targetTemperature response failed");
        return -1;
    }

    return 0;
}