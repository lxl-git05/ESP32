#include <Arduino.h>
#include "src/Hardware/LED.h"
#include "Initial.h"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void Initial_All(void)
{
    LED_Init();
}


#if (configUSE_TRACE_FACILITY == 1) && (configGENERATE_RUN_TIME_STATS == 1)
// 任务数可能在查询后增加，预留空间，并在容量不足时重新采集。
static UBaseType_t Initial_GetTaskSnapshot(TaskStatus_t **snapshot,
                                         configRUN_TIME_COUNTER_TYPE *totalRunTime)
{
    *snapshot = NULL;

    for (unsigned int attempt = 0; attempt < 3; ++attempt)
    {
        UBaseType_t capacity = uxTaskGetNumberOfTasks() + 4;
        TaskStatus_t *buffer = pvPortMalloc((size_t)capacity * sizeof(*buffer));

        if (buffer == NULL)
        {
            printf("[FreeRTOS] Cannot allocate task snapshot.\n");
            return 0;
        }

        *totalRunTime = 0;
        UBaseType_t count = uxTaskGetSystemState(buffer, capacity, totalRunTime);
        if (count != 0)
        {
            *snapshot = buffer;
            return count;
        }

        vPortFree(buffer);
    }

    printf("[FreeRTOS] Task list changed too quickly; retry later.\n");
    return 0;
}

static const char *Initial_TaskStateName(eTaskState state)
{
    switch (state)
    {
        case eRunning:   return "Running";
        case eReady:     return "Ready";
        case eBlocked:   return "Blocked";
        case eSuspended: return "Suspended";
        case eDeleted:   return "Deleted";
        default:         return "Unknown";
    }
}
#endif

void print_FreeRtos_Task(void)
{
#if (configUSE_TRACE_FACILITY == 1) && (configGENERATE_RUN_TIME_STATS == 1)
    TaskStatus_t *before = NULL;
    TaskStatus_t *after = NULL;
    configRUN_TIME_COUNTER_TYPE startRunTime = 0;
    configRUN_TIME_COUNTER_TYPE endRunTime = 0;
    UBaseType_t beforeCount = Initial_GetTaskSnapshot(&before, &startRunTime);

    if (beforeCount == 0)
    {
        return;
    }

    // CPU占用使用两次计数之差，避免把开机以来的平均值当作当前负载。
    vTaskDelay(pdMS_TO_TICKS(1000));

    UBaseType_t afterCount = Initial_GetTaskSnapshot(&after, &endRunTime);
    if (afterCount == 0)
    {
        vPortFree(before);
        return;
    }

    // 使用配置中的无符号计数类型，短采样窗口内允许计数器发生一次回绕。
    configRUN_TIME_COUNTER_TYPE elapsed = endRunTime - startRunTime;

    printf("\n[FreeRTOS] Tasks: %u | CPU sample: ~1000ms\n", (unsigned int)afterCount);
    printf("CPU%%: estimate, 100%% = one core; NEW = no baseline, N/A = unavailable.\n");
    printf("Affinity: pinning, not the currently executing core. StackMin: bytes.\n");
    printf("%-5s %-16s %3s %4s %-9s %8s %11s %14s %7s\n",
           "ID", "Task", "Pri", "Base", "State", "Affinity", "StackMin(B)", "RunTime(cnt)", "CPU%");

    for (UBaseType_t i = 0; i < afterCount; ++i)
    {
        const TaskStatus_t *task = &after[i];
        char cpuText[16] = "NEW";
        char affinity[12] = "N/A";

        for (UBaseType_t j = 0; j < beforeCount; ++j)
        {
            // 同时匹配任务编号和句柄，避免把复用TCB地址的新任务当成旧任务。
            if (task->xHandle == before[j].xHandle &&
                task->xTaskNumber == before[j].xTaskNumber)
            {
                configRUN_TIME_COUNTER_TYPE used =
                    task->ulRunTimeCounter - before[j].ulRunTimeCounter;

                if (elapsed == 0 || used > elapsed)
                {
                    // 运行计数在切换任务时更新；未及时结算的样本不显示误导性百分比。
                    snprintf(cpuText, sizeof(cpuText), "N/A");
                }
                else
                {
                    snprintf(cpuText, sizeof(cpuText), "%.2f", (double)used * 100.0 / (double)elapsed);
                }
                break;
            }
        }

#if (configTASKLIST_INCLUDE_COREID == 1)
        if (task->xCoreID == tskNO_AFFINITY)
        {
            snprintf(affinity, sizeof(affinity), "ANY");
        }
        else
        {
            snprintf(affinity, sizeof(affinity), "%d", (int)task->xCoreID);
        }
#endif

        // 删除中的任务名称不再解引用；其他任务需遵守头文件中的生命周期约定。
        const char *name = task->eCurrentState == eDeleted ? "<deleted>" : task->pcTaskName;
        printf("%-5u %-16s %3u %4u %-9s %8s %11u %14llu %7s\n",
               (unsigned int)task->xTaskNumber,
               name != NULL ? name : "<unnamed>",
               (unsigned int)task->uxCurrentPriority,
               (unsigned int)task->uxBasePriority,
               Initial_TaskStateName(task->eCurrentState),
               affinity,
               (unsigned int)task->usStackHighWaterMark,
               (unsigned long long)task->ulRunTimeCounter,
               cpuText);
    }

    vPortFree(after);
    vPortFree(before);
    fflush(stdout);
#else
    printf("[FreeRTOS] Enable CONFIG_FREERTOS_USE_TRACE_FACILITY and "
           "CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS in the SDK build.\n");
#endif
}
