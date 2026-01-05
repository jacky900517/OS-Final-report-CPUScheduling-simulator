#include "Process.h"
#include <algorithm>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

struct Segment
{
    int pid;
    int start;
    int end;
};

class Scheduling
{
public:
    Scheduling() = default;

    void InsertProcess()
    {
        processes.clear();
        processes.reserve(kProcessCount);

        ::std::cout << "PLZ insert 5 Process's info" << ::std::endl;
        for (int i = 0; i < kProcessCount; i++)
        {
            int at, bt, pr;
            ::std::cout << "\nProcess" << i << "'s Arrival time, Burst time, Priority: ";
            ::std::cin >> at >> bt >> pr;
            Process p;
            p.Pid = i;
            p.setBurst_Time(bt);
            p.setArrival_Time(at);
            p.setPriority(pr);
            processes.push_back(p);
        }
    }

    void FCFS()
    {
        ::std::cout << "\nAlgorithm: FCFS\n";
        auto timeline = buildFCFS();
        printGanttChart(timeline);
        printStats(timeline);
    }

    void SJF()
    {
        ::std::cout << "\nAlgorithm: SJF\n";
        auto timeline = buildSJF();
        printGanttChart(timeline);
        printStats(timeline);
    }

    void SRJF()
    {
        ::std::cout << "\nAlgorithm: SRJF\n";
        auto timeline = buildSRJF();
        printGanttChart(timeline);
        printStats(timeline);
    }

    void PS()
    {
        ::std::cout << "\nAlgorithm: Priority Scheduling\n";
        auto timeline = buildPriority();
        printGanttChart(timeline);
        printStats(timeline);
    }

    void RR()
    {
        ::std::cout << "\nAlgorithm: Round Robin\n";
        int quantum = 0;
        ::std::cout << "Time Quantum: ";
        ::std::cin >> quantum;
        if (quantum <= 0)
        {
            ::std::cout << "Invalid quantum. Skip.\n";
            return;
        }
        auto timeline = buildRR(quantum);
        printGanttChart(timeline);
        printStats(timeline);
    }

private:
    static constexpr int kProcessCount = 5;
    ::std::vector<Process> processes;

    static void appendSegment(::std::vector<Segment> &timeline, int pid, int start, int end)
    {
        if (start >= end)
        {
            return;
        }
        if (!timeline.empty() && timeline.back().pid == pid && timeline.back().end == start)
        {
            timeline.back().end = end;
            return;
        }
        timeline.push_back({pid, start, end});
    }

    void printGanttChart(const ::std::vector<Segment> &timeline) const
    {
        ::std::cout << "\nGantt Chart:\n";
        for (const auto &seg : timeline)
        {
            if (seg.pid < 0)
            {
                ::std::cout << "| IDLE ";
            }
            else
            {
                ::std::cout << "| P" << seg.pid << " ";
            }
        }
        ::std::cout << "|\n";

        if (timeline.empty())
        {
            return;
        }

        ::std::cout << timeline.front().start;
        for (const auto &seg : timeline)
        {
            ::std::cout << ::std::setw(5) << seg.end;
        }
        ::std::cout << "\n";
    }

    void printStats(const ::std::vector<Segment> &timeline) const
    {
        ::std::vector<Process> stats = processes;
        for (auto &p : stats)
        {
            p.Completion_Time = 0;
        }

        for (const auto &seg : timeline)
        {
            if (seg.pid >= 0)
            {
                stats[seg.pid].Completion_Time = ::std::max(stats[seg.pid].Completion_Time, seg.end);
            }
        }

        int sumTT = 0, sumWT = 0;
        for (auto &p : stats)
        {
            p.Turnaround_Time = p.Completion_Time - p.getArrival_Time();
            p.Waiting_Time = p.Turnaround_Time - p.getBurst_Time();
            sumTT += p.Turnaround_Time;
            sumWT += p.Waiting_Time;
        }

        ::std::cout << "\nProcess Waiting Time:\n";
        for (const auto &p : stats)
        {
            ::std::cout << " P_" << p.Pid << " : " << p.Waiting_Time;
        }

        ::std::cout << "\nProcess Turnaround Time:\n";
        for (const auto &p : stats)
        {
            ::std::cout << " P_" << p.Pid << " : " << p.Turnaround_Time;
        }

        double AWT = static_cast<double>(sumWT) / kProcessCount;
        double ATT = static_cast<double>(sumTT) / kProcessCount;
        ::std::cout << "\nAVG Waiting Time : " << AWT << ::std::endl;
        ::std::cout << "AVG Turnaround Time : " << ATT << ::std::endl;
    }

    ::std::vector<Process> cloneProcesses() const
    {
        return processes;
    }

    ::std::vector<Segment> buildFCFS() const
    {
        auto procs = cloneProcesses();
        ::std::stable_sort(procs.begin(), procs.end(), [](const Process &a, const Process &b)
                           {
                               if (a.getArrival_Time() != b.getArrival_Time())
                                   return a.getArrival_Time() < b.getArrival_Time();
                               return a.Pid < b.Pid;
                           });

        ::std::vector<Segment> timeline;
        int current = 0;
        for (const auto &p : procs)
        {
            if (current < p.getArrival_Time())
            {
                appendSegment(timeline, -1, current, p.getArrival_Time());
                current = p.getArrival_Time();
            }
            appendSegment(timeline, p.Pid, current, current + p.getBurst_Time());
            current += p.getBurst_Time();
        }
        return timeline;
    }

    ::std::vector<Segment> buildSJF() const
    {
        auto procs = cloneProcesses();
        ::std::vector<bool> done(kProcessCount, false);
        ::std::vector<Segment> timeline;

        int completed = 0;
        int current = 0;
        while (completed < kProcessCount)
        {
            int nextPid = -1;
            int bestBurst = ::std::numeric_limits<int>::max();
            int bestArrival = ::std::numeric_limits<int>::max();

            for (int i = 0; i < kProcessCount; ++i)
            {
                if (done[i])
                    continue;
                if (procs[i].getArrival_Time() <= current)
                {
                    int burst = procs[i].getBurst_Time();
                    int arrival = procs[i].getArrival_Time();
                    if (burst < bestBurst || (burst == bestBurst && arrival < bestArrival))
                    {
                        bestBurst = burst;
                        bestArrival = arrival;
                        nextPid = i;
                    }
                }
            }

            if (nextPid < 0)
            {
                int nextArrival = ::std::numeric_limits<int>::max();
                for (int i = 0; i < kProcessCount; ++i)
                {
                    if (!done[i])
                    {
                        nextArrival = ::std::min(nextArrival, procs[i].getArrival_Time());
                    }
                }
                appendSegment(timeline, -1, current, nextArrival);
                current = nextArrival;
                continue;
            }

            appendSegment(timeline, procs[nextPid].Pid, current, current + procs[nextPid].getBurst_Time());
            current += procs[nextPid].getBurst_Time();
            done[nextPid] = true;
            completed++;
        }

        return timeline;
    }

    ::std::vector<Segment> buildSRJF() const
    {
        auto procs = cloneProcesses();
        ::std::vector<int> remaining(kProcessCount);
        ::std::vector<bool> done(kProcessCount, false);
        for (int i = 0; i < kProcessCount; ++i)
        {
            remaining[i] = procs[i].getBurst_Time();
        }

        ::std::vector<Segment> timeline;
        int completed = 0;
        int current = 0;

        while (completed < kProcessCount)
        {
            int nextPid = -1;
            int bestRemain = ::std::numeric_limits<int>::max();
            int bestArrival = ::std::numeric_limits<int>::max();

            for (int i = 0; i < kProcessCount; ++i)
            {
                if (done[i])
                    continue;
                if (procs[i].getArrival_Time() <= current && remaining[i] > 0)
                {
                    int arrival = procs[i].getArrival_Time();
                    if (remaining[i] < bestRemain || (remaining[i] == bestRemain && arrival < bestArrival))
                    {
                        bestRemain = remaining[i];
                        bestArrival = arrival;
                        nextPid = i;
                    }
                }
            }

            if (nextPid < 0)
            {
                int nextArrival = ::std::numeric_limits<int>::max();
                for (int i = 0; i < kProcessCount; ++i)
                {
                    if (!done[i])
                    {
                        nextArrival = ::std::min(nextArrival, procs[i].getArrival_Time());
                    }
                }
                appendSegment(timeline, -1, current, nextArrival);
                current = nextArrival;
                continue;
            }

            int nextArrival = ::std::numeric_limits<int>::max();
            for (int i = 0; i < kProcessCount; ++i)
            {
                if (!done[i] && procs[i].getArrival_Time() > current)
                {
                    nextArrival = ::std::min(nextArrival, procs[i].getArrival_Time());
                }
            }

            int runFor = remaining[nextPid];
            if (nextArrival != ::std::numeric_limits<int>::max())
            {
                runFor = ::std::min(runFor, nextArrival - current);
            }

            appendSegment(timeline, procs[nextPid].Pid, current, current + runFor);
            current += runFor;
            remaining[nextPid] -= runFor;

            if (remaining[nextPid] == 0)
            {
                done[nextPid] = true;
                completed++;
            }
        }

        return timeline;
    }

    ::std::vector<Segment> buildPriority() const
    {
        auto procs = cloneProcesses();
        ::std::vector<bool> done(kProcessCount, false);
        ::std::vector<Segment> timeline;

        int completed = 0;
        int current = 0;
        while (completed < kProcessCount)
        {
            int nextPid = -1;
            int bestPriority = ::std::numeric_limits<int>::max();
            int bestArrival = ::std::numeric_limits<int>::max();

            for (int i = 0; i < kProcessCount; ++i)
            {
                if (done[i])
                    continue;
                if (procs[i].getArrival_Time() <= current)
                {
                    int pr = procs[i].getPriority();
                    int arrival = procs[i].getArrival_Time();
                    if (pr < bestPriority || (pr == bestPriority && arrival < bestArrival))
                    {
                        bestPriority = pr;
                        bestArrival = arrival;
                        nextPid = i;
                    }
                }
            }

            if (nextPid < 0)
            {
                int nextArrival = ::std::numeric_limits<int>::max();
                for (int i = 0; i < kProcessCount; ++i)
                {
                    if (!done[i])
                    {
                        nextArrival = ::std::min(nextArrival, procs[i].getArrival_Time());
                    }
                }
                appendSegment(timeline, -1, current, nextArrival);
                current = nextArrival;
                continue;
            }

            appendSegment(timeline, procs[nextPid].Pid, current, current + procs[nextPid].getBurst_Time());
            current += procs[nextPid].getBurst_Time();
            done[nextPid] = true;
            completed++;
        }

        return timeline;
    }

    ::std::vector<Segment> buildRR(int quantum) const
    {
        auto procs = cloneProcesses();
        ::std::vector<int> remaining(kProcessCount);
        for (int i = 0; i < kProcessCount; ++i)
        {
            remaining[i] = procs[i].getBurst_Time();
        }

        ::std::vector<int> order(kProcessCount);
        for (int i = 0; i < kProcessCount; ++i)
        {
            order[i] = i;
        }
        ::std::stable_sort(order.begin(), order.end(), [&](int a, int b)
                           {
                               if (procs[a].getArrival_Time() != procs[b].getArrival_Time())
                                   return procs[a].getArrival_Time() < procs[b].getArrival_Time();
                               return procs[a].Pid < procs[b].Pid;
                           });

        ::std::deque<int> ready;
        ::std::vector<bool> done(kProcessCount, false);
        ::std::vector<Segment> timeline;

        int completed = 0;
        int current = 0;
        int nextIdx = 0;

        auto pushArrivalsUpTo = [&](int time)
        {
            while (nextIdx < kProcessCount && procs[order[nextIdx]].getArrival_Time() <= time)
            {
                ready.push_back(order[nextIdx]);
                nextIdx++;
            }
        };

        while (completed < kProcessCount)
        {
            pushArrivalsUpTo(current);

            if (ready.empty())
            {
                if (nextIdx < kProcessCount)
                {
                    int nextArrival = procs[order[nextIdx]].getArrival_Time();
                    appendSegment(timeline, -1, current, nextArrival);
                    current = nextArrival;
                    continue;
                }
                break;
            }

            int pid = ready.front();
            ready.pop_front();
            int runFor = ::std::min(quantum, remaining[pid]);
            appendSegment(timeline, procs[pid].Pid, current, current + runFor);
            current += runFor;
            remaining[pid] -= runFor;

            pushArrivalsUpTo(current);

            if (remaining[pid] > 0)
            {
                ready.push_back(pid);
            }
            else if (!done[pid])
            {
                done[pid] = true;
                completed++;
            }
        }

        return timeline;
    }
};
