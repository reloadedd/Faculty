/*********************************************************************************
 *          Copyright (c) 2021 reloadedd <reloadedd@protonmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *********************************************************************************/

/**********************
 * --=== INCLUDES ===--
 *********************/
#include "../lib/P1.h"


/******************************
 * --=== GLOBAL VARIABLES ===--
 *****************************/
HANDLE G_HANDLE;


/*************************
 * --=== MAIN DRIVER ===--
 ************************/
int _tmain() {
    /* Create a global writable handle for the file that will be used to print into */
    if ((G_HANDLE = CreateFile(OUT_FILE,
                               GENERIC_WRITE,
                               0,
                               NULL,
                               CREATE_ALWAYS,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL)) == INVALID_HANDLE_VALUE) {
        handleError(__MESSAGE("Unable to open file for writing"));
    }

    /* Get the processor info */
    _tprintf(_T("INFO\tGetting processor info...\n"));
    getProcessorInfo();

    /* Get the NUMA info */
    _tprintf(_T("INFO\tGetting NUMA info...\n"));
    getNUMAInfo();

    /* Get info about the default CPU Sets */
    _tprintf(_T("INFO\tGetting CPU Sets info...\n"));
    getCPUSetsInfo();

    CloseHandle(G_HANDLE);
    return 0;
}


/**********************************
 * --=== FUNCTION DEFINITIONS ===--
 *********************************/
void getCPUSetsInfo(void) {
    PULONG cpuSetsIds = NULL;
    ULONG requiredCapacity;
    ULONG cpuSetIdCount = 0;
    enum { maxLength = 10000 };
    TCHAR output[maxLength];

    if (!GetProcessDefaultCpuSets(GetCurrentProcess(),
                                  cpuSetsIds,
                                  cpuSetIdCount,
                                  &requiredCapacity)) {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            cpuSetIdCount = requiredCapacity;

            if (!GetProcessDefaultCpuSets(GetCurrentProcess(),
                                          cpuSetsIds,
                                          cpuSetIdCount,
                                          &requiredCapacity)) {
                handleError(__MESSAGE("Unable to get process default CPU Sets"));
            }
        }
    }
    snprintf(output, maxLength, _T("\n[+] CPU Sets Information:\n"));

    /*
     * From M$ docs:
     *      Retrieves the list of CPU Sets in the process default set that was set by SetProcessDefaultCpuSets.
     *      If no default CPU Sets are set for a given process, then the RequiredIdCount is set to 0 and the function
     *      succeeds.
     */
    if (requiredCapacity == 0) {
        snprintf(output + strlen(output),
                 maxLength - strlen(output),
                 _T("\t[~] No *default* CPU Sets were set.\n"));
    } else {
        for (ULONG i = 0; i < cpuSetIdCount; ++i) {
            snprintf(output + strlen(output),
                     maxLength - strlen(output),
                     _T("\t[~] Default CPU Set #%lu: %lu\n"),
                     i,
                     cpuSetsIds[i]);
        }
    }

    PSYSTEM_CPU_SET_INFORMATION info = NULL;
    ULONG bufferLength = 0;

    if (!GetSystemCpuSetInformation(info,
                                    bufferLength,
                                    &requiredCapacity,
                                    GetCurrentProcess(),
                                    (ULONG) 0)) {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            bufferLength = requiredCapacity;
            info = malloc(requiredCapacity);

            if (!GetSystemCpuSetInformation(info,
                                            bufferLength,
                                            &requiredCapacity,
                                            GetCurrentProcess(),
                                            (ULONG) 0)) {
                handleError(__MESSAGE("Unable to get system CPU Set information"));
            }
        }
    }

    DWORD byteOffset = 0;
    DWORD i = 0;

    while (byteOffset + sizeof(SYSTEM_CPU_SET_INFORMATION) <= requiredCapacity) {
        snprintf(output + strlen(output),
                 maxLength - strlen(output),
                 _T("\t[~] System CPU Set #%lu\n"
                    "\t\t[-] Id: %lu\n"
                    "\t\t[-] Group: %d\n"
                    "\t\t[-] Logical Processor Index: %u\n"
                    "\t\t[-] Core Index: %u\n"
                    "\t\t[-] Last Level CacheIndex: %u\n"
                    "\t\t[-] Numa Node Index: %u\n"
                    "\t\t[-] Efficiency Class: %u\n"
                    "\t\t[-] Scheduling Class: %u\n"),
                 i,
                 info->CpuSet.Id,
                 info->CpuSet.Group,
                 info->CpuSet.LogicalProcessorIndex,
                 info->CpuSet.CoreIndex,
                 info->CpuSet.LastLevelCacheIndex,
                 info->CpuSet.NumaNodeIndex,
                 info->CpuSet.EfficiencyClass,
                 info->CpuSet.SchedulingClass);

        byteOffset += sizeof(SYSTEM_CPU_SET_INFORMATION);
        info++;
        i++;
    }
    snprintf(output + strlen(output),
             maxLength - strlen(output),
             _T("\t[~] Total number of CPU Sets: %lu.\n"), i);

    if (!WriteFile(G_HANDLE,
                   output,
                   strlen(output),
                   (void *) 0x0,
                   NULL)) {
        handleError(__MESSAGE("Unable to write to file"));
    }

    if (!info) free(info);
}

void getNUMAInfo(void) {
    ULONG highestNodeNumber;
    DWORD64 processorMask;

    if (!GetNumaHighestNodeNumber(&highestNodeNumber)) {
        handleError(__MESSAGE("Unable to get the highest NUMA node number"));
    }

    enum { maxLength = 5000};
    TCHAR output[maxLength];
    snprintf(output, maxLength, _T("\n[+] NUMA Information:\n"));

    if (highestNodeNumber == 0) {
        snprintf(output + strlen(output),
                 maxLength - strlen(output),
                 _T("\t[-] Not a NUMA system.\n"));
        goto End;
    }

    if (!GetNumaNodeProcessorMask(highestNodeNumber, &processorMask)) {
        handleError(__MESSAGE("Unable to get processor mask"));
    }

    UCHAR LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    for (UCHAR i = 0; i <= LSHIFT; ++i) {
        if (processorMask & ((ULONG_PTR) 1 << i)) {
            snprintf(output + strlen(output),
                     maxLength - strlen(output),
                     _T("\t[-] Processor #%d is in NUMA node #%lu\n"),
                     i,
                     highestNodeNumber);
        }
    }

End:
    if (!WriteFile(G_HANDLE,
                   output,
                   strlen(output),
                   (void *) 0x0,
                   NULL)) {
        handleError(__MESSAGE("Unable to write to file"));
    }
}

/* Mostly inspired after the official docs */
void getProcessorInfo(void) {
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
    DWORD returnedLength = 0;

    if (GetLogicalProcessorInformation(buffer, &returnedLength) == FALSE) {
        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            buffer = malloc(returnedLength);

            if (GetLogicalProcessorInformation(buffer, &returnedLength) == FALSE) {
                _tprintf(_T("returnedLength: %ld\n"), returnedLength);
                handleError(__MESSAGE("Unable to get processor information"));
            }
        }
    }

    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
    DWORD logicalProcessorCount = 0;
    DWORD numaNodeCount = 0;
    DWORD processorCoreCount = 0;
    DWORD processorL1CacheCount = 0;
    DWORD processorL2CacheCount = 0;
    DWORD processorL3CacheCount = 0;
    DWORD processorPackageCount = 0;
    DWORD byteOffset = 0;
    PCACHE_DESCRIPTOR Cache;

    while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnedLength)
    {
        switch (ptr->Relationship)
        {
            case RelationNumaNode:
                // Non-NUMA systems report a single record of this type.
                numaNodeCount++;
                break;

            case RelationProcessorCore:
                processorCoreCount++;

                // A hyperthreaded core supplies more than one logical processor.
                logicalProcessorCount += CountSetBits(ptr->ProcessorMask);
                break;

            case RelationCache:
                // Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
                Cache = &ptr->Cache;
                if (Cache->Level == 1)
                {
                    processorL1CacheCount++;
                }
                else if (Cache->Level == 2)
                {
                    processorL2CacheCount++;
                }
                else if (Cache->Level == 3)
                {
                    processorL3CacheCount++;
                }
                break;

            case RelationProcessorPackage:
                // Logical processors share a physical package.
                processorPackageCount++;
                break;

            default:
                handleError(__MESSAGE("\nError: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n"));
                break;
        }
        byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }

    enum { maxLength = 300 };
    TCHAR output[maxLength];
    snprintf(output,
             maxLength,
             _T("[+] Processor Information:\n"
                "\t[-] Number of NUMA nodes: %ld\n"
                "\t[-] Number of physical processor packages: %ld\n"
                "\t[-] Number of processor cores: %ld\n"
                "\t[-] Number of logical processors: %ld\n"
                "\t[-] Number of processor L1/L2/L3 caches: %ld/%ld/%ld\n"),
                numaNodeCount,
                processorPackageCount,
                processorCoreCount,
                logicalProcessorCount,
                processorL1CacheCount,
                processorL2CacheCount,
                processorL3CacheCount);

    if (!WriteFile(G_HANDLE,
                   output,
                   strlen(output),
                   (void *) 0x0,
                   NULL)) {
        handleError(__MESSAGE("Unable to write to file"));
    }

    free(buffer);
}

// Helper function to count set bits in the processor mask.
DWORD CountSetBits(ULONG_PTR bitMask)
/* Taken from official docs */
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
    DWORD i;

    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
