#pragma once

class Process
{
private:
    int Burst_Time = 0;
    int Arrival_Time = 0;
    int Priority = 0;

public:
    int Pid;
    int Remaining_Time = 0;
    int Waiting_Time = 0, Turnaround_Time = 0, Completion_Time = 0;
    Process() = default;

    int getBurst_Time() const
    {
        return Burst_Time;
    }
    int getArrival_Time() const
    {
        return Arrival_Time;
    }
    int getPriority() const
    {
        return Priority;
    }

    void setBurst_Time(int burst_time)
    {
        Burst_Time = burst_time;
        Remaining_Time = burst_time;
    }

    void setArrival_Time(int arrival_time)
    {
        Arrival_Time = arrival_time;
    }
    void setPriority(int priority)
    {
        Priority = priority;
    }
};
