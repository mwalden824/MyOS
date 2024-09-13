#include <multitasking.h>

using namespace myos;
using namespace myos::common;

Task::Task(GlobalDescriptorTable* gdt, void entrypoint())
{
    cpuState = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpuState->eax = 0;
    cpuState->ebx = 0;
    cpuState->ecx = 0;
    cpuState->edx = 0;

    cpuState->esi = 0;
    cpuState->edi = 0;
    cpuState->ebp = 0;

    // cpuState->gs = 0;
    // cpuState->fs = 0;
    // cpuState->es = 0;
    // cpuState->ds = 0;

    // cpuState->error = 0;

    cpuState->eip = (uint32_t)entrypoint;
    cpuState->cs = gdt->CodeSegmentSelector();
    cpuState->eflags = 0x202;
    // cpuState->esp = 0;
    // cpuState->ss = 0;
}

Task::~Task()
{

}

TaskManager::TaskManager()
{
    numTasks = 0;
    currentTask = -1;
}

TaskManager::~TaskManager()
{

}

bool TaskManager::AddTask(Task* task)
{
    if (numTasks >= 256)
        return false;

    tasks[numTasks++] = task;
    return true;
}

CPUState* TaskManager::Scheduler(CPUState* cpuState)
{
    if (numTasks <= 0)
        return cpuState;

    if (currentTask >= 0)
        tasks[currentTask]->cpuState = cpuState;

    if (++currentTask >= numTasks)
        currentTask %= numTasks;

    return tasks[currentTask]->cpuState;
}

