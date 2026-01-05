#include <iostream>
#include "Scheduling.h"

int main()
{
    bool running = true;

    Scheduling S;
    S.InsertProcess();

    while (running)
    {

        int input;
        ::std::cout << "\nPlz input 0~5 to select which CPU Scheduling Algorithm simulator u need." << ::std::endl;
        ::std::cout << "0 : exit\n1 : FCFS\n2 : SJF\n3 : SRJF\n4 : Priority Scheduling\n5 : Round Robin" << ::std::endl;
        ::std::cin >> input;
        switch (input)
        {

        case 0:
            running = false;
            break;
        case 1:
            S.FCFS();
            break;
        case 2:
            S.SJF();
            break;
        case 3:
            S.SRJF();
            break;
        case 4:
            S.PS();
            break;
        case 5:
            S.RR();
            break;
        default:
            ::std::cout << "Invalid input. Please enter 0~5.\n";
            break;
        }
    }
}
