#include <cstring>
#include <iostream>
#include <bits/getopt_core.h>
#include <vector>
#include "simlib.h"

using namespace std;

#define WaitUntil(condition) while(_WaitUntil(condition))

const int LOCKER_ROOM_CAPACITY = 200;
const int POOL_CAPACITY = 180;
const int SAUNA_CAPACITY = 20;

Store lockerRoom("Locker Room", LOCKER_ROOM_CAPACITY);
Store pool("Swimming Pools", POOL_CAPACITY);
Store sauna("Sauna", SAUNA_CAPACITY);

Facility waterSlide("Water Slide");

int totalVisitors = 0;
int totalWaiters = 0;

struct SimulationParameters {
    double changingTime;
    double workHours;
    double saunaWaitTime;
    double saunaTime;
    double tobogganTime;
    double visitorsArrivalTime;
    double swimmingTime;
};

SimulationParameters params = {5, 0, 15, 30, 1, 0, 40};

vector<double> transactionTimes;

class Visitor : public Process {
public:
    double startTime;

    void Behavior() {
        startTime = Time;
        EnterLockerRoom();
        ChooseActivity();
        LeaveLockerRoom();
    }

private:
    void EnterLockerRoom() {
        Enter(lockerRoom, 1);
        Wait(Exponential(params.changingTime));
    }

    void LeaveLockerRoom() {
        Leave(lockerRoom, 1);
        Wait(Exponential(params.changingTime));
        RecordTransactionTime();
    }

    void ChooseActivity() {
        double choice = Random();
        if (choice <= 0.7) {
            GoSwimming();
        } else {
            GoToSauna();
        }
    }

    void GoSwimming() {
        Enter(pool, 1);
        //SwimUntilDone();
        Swim();
        DecideAfterSwim();
    }

//    void SwimUntilDone() {
//        while (Random() <= 0.6) {
//            Swim();
//        }
//    }

    void Swim() {
        Wait(Exponential(params.swimmingTime));
    }

    void DecideAfterSwim() {
        double random = Random();
        if (random <= 0.3) {
            Leave(pool, 1);
        }
        else if(random <= 0.65){
            Leave(pool, 1);
            GoToSauna();
        }
        else {
            UseWaterSlide();
        }
    }

    void UseWaterSlide() {
        Seize(waterSlide);
        Wait(Exponential(params.tobogganTime));
        Release(waterSlide);
        DecideAfterSlide();
    }

    void DecideAfterSlide() {
        double random = Random();
        if (random <= 0.45) {
            Leave(pool, 1); // ?))))))))))))))))
            GoSwimming();
        } else if (random <= 0.5) {
            Leave(pool, 1);
        } else {
            UseWaterSlide();
        }
    }

    void GoToSauna() {
        WaitUntilSaunaIsAvailable();
        if (sauna.Full()) {
            totalWaiters++;
            GoSwimming();
        } else {
            Enter(sauna, 1);
            //ChillUntilDone();
            ChillInSauna();
            DecideAfterSauna();
        }
    }

    void WaitUntilSaunaIsAvailable() {
        double time_start = Time;
        WaitUntil(!sauna.Full() || Time - time_start > Exponential(params.saunaWaitTime));
    }

//    void ChillUntilDone() {
//        while (Random() <= 0.55) {
//            ChillInSauna();
//        }
//    }

    void ChillInSauna() {
        Wait(Exponential(params.saunaTime));
    }

    void DecideAfterSauna() {
        double random = Random();
        if (random <= 0.5) {
            Leave(sauna, 1);
        }
        else{
            Leave(sauna, 1);
            GoSwimming();
        }
//        else {
//            GoToSauna();
//        }
    }

    void RecordTransactionTime() {
        double transactionTime = Time - startTime;
        std::cout << "Transaction time: " << transactionTime << std::endl;
        transactionTimes.push_back(transactionTime);
    }
};

class Generator : public Event {
public:
    void Behavior() {
        (new Visitor)->Activate();
        totalVisitors++;
        Activate(Time + Exponential(params.visitorsArrivalTime));    //arrival time
    }
};

void printUsage(const char *programName) {
    std::cout << "Usage: " << programName
              << " ./kohoutovice -d <day_type>\n";
    std::cout << "Options:\n";
    std::cout << "  -d <day_type> - type of the day (weekday, weekend, holiday)\n";
}

void ParseArguments(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int opt;

    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                if (strcmp(optarg, "weekday") == 0) {
                    params.workHours = 12 * 60;
                    params.visitorsArrivalTime = 1.42;
                } else if (strcmp(optarg, "weekend") == 0) {
                    params.workHours = 14 * 60;
                    params.visitorsArrivalTime = 1.30;
                } else if (strcmp(optarg, "holiday") == 0) {
                    params.workHours = 14 * 60;
                    params.visitorsArrivalTime = 2.1;
                } else {
                    printUsage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case '?':
                printUsage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (params.workHours == 0 || params.visitorsArrivalTime == 0) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        std::cerr << "Error: Unexpected argument after options\n";
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    ParseArguments(argc, argv);

    Init(0, params.workHours);

    (new Generator)->Activate();
    Run();
    lockerRoom.Output();
    sauna.Output();
    pool.Output();
    waterSlide.Output();
    std::cout << totalVisitors << std::endl;

    double sum = 0;
    for (int i = 0; i < transactionTimes.size(); i++) {
        sum += transactionTimes[i];
    }
    std::cout << "Average transaction time: " << sum / transactionTimes.size() << std::endl;
    std::cout << "Waiters: " << totalWaiters << std::endl;

    return 0;
}
