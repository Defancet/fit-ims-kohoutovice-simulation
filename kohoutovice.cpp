/**
 * Kohoutovice aquapark simulation
 * @authors
 *  xkalut00, Maksim Kalutski
 *  xrasst00, Sergei Rasstrigin
 *
 * @file   kohooutovice.cpp
 * @brief  Implementation of Kohoutovice aquapark simulation
 * @date   30.11.2023
 */

#include <cstring>
#include <iostream>
#include <bits/getopt_core.h>
#include <vector>
#include "getopt.h"
#include "simlib.h"
#include <fstream>

using namespace std;

#define WaitUntil(condition) while(_WaitUntil(condition))

/** Constants defining capacities */
const int LOCKER_ROOM_CAPACITY = 192;
const int POOL_CAPACITY = 180;
const int SAUNA_CAPACITY = 12;

/** Facilities and Stores representing different areas in the aquapark */
Store lockerRoom("Locker Room", LOCKER_ROOM_CAPACITY);
Store pool("Swimming Pools", POOL_CAPACITY);
Store sauna("Sauna", SAUNA_CAPACITY);

Facility waterSlide("Water Slide");

/** Global variables to track statistics */
int totalVisitors = 0;
int totalWaiters = 0;

/** Structure of simulation parameters */
struct SimulationParameters {
    double changingTime;
    double workHours;
    double saunaWaitTime;
    double saunaTime;
    double tobogganTime;
    double visitorsArrivalTime;
    double swimmingTime;
};

SimulationParameters params = {5, 0, 15, 29, 1, 0, 45};

/** Vector to store transaction times for visitors */
vector<double> transactionTimes;

/** Class representing a Visitor in the aquapark */
class Visitor : public Process {
public:
    double startTime;

    /**
     * @brief Behavior of a visitor in the aquapark
     */
    void Behavior() {
        startTime = Time;
        EnterLockerRoom();
        ChooseActivity();
        LeaveLockerRoom();
    }

private:
    /**
     * @brief Visitor enters the locker room
     */
    void EnterLockerRoom() {
        Enter(lockerRoom, 1);
        Wait(Exponential(params.changingTime));
    }

    /**
     * @brief Visitor leaves the locker room
     */
    void LeaveLockerRoom() {
        Leave(lockerRoom, 1);
        Wait(Exponential(params.changingTime));
        RecordTransactionTime();
    }

    /**
     * @brief Visitor chooses where to go next (swimming pool or sauna)
     */
    void ChooseActivity() {
        double choice = Random();
        if (choice <= 0.7) {
            GoSwimming();
        } else {
            GoToSauna();
        }
    }

    /**
     * @brief Visitor goes swimming in the pool and decides what to do after swimming
     */
    void GoSwimming() {
        Enter(pool, 1);
        Swim();
        DecideAfterSwim();
    }

    /**
     * @brief Visitor swims in the pool
     */
    void Swim() {
        Wait(Exponential(params.swimmingTime));
    }

    /**
     * @brief Visitor decides what to do after swimming (leave, go to sauna or use water slide)
     */
    void DecideAfterSwim() {
        double random = Random();
        if (random <= 0.4) {
            Leave(pool, 1);
        } else if (random <= 0.7) {
            Leave(pool, 1);
            GoToSauna();
        } else {
            UseWaterSlide();
        }
    }

    /**
     * @brief Visitor uses the water slide and decides what to do after (leave, go to sauna or swim)
     */
    void UseWaterSlide() {
        Seize(waterSlide);
        Wait(Exponential(params.tobogganTime));
        Release(waterSlide);
        DecideAfterSlide();
    }

    /**
     * @brief Visitor decides what to do after using the water slide (leave, go to sauna or swim)
     */
    void DecideAfterSlide() {
        double random = Random();
        if (random <= 0.45) {
            Leave(pool, 1);
            GoSwimming();
        } else if (random <= 0.5) {
            Leave(pool, 1);
        } else {
            UseWaterSlide();
        }
    }

    /**
     * @brief Visitor goes to the sauna and decides what to do after (leave or go swimming)
     */
    void GoToSauna() {
        WaitUntilSaunaIsAvailable();
        if (sauna.Full()) {
            totalWaiters++;
            GoSwimming();
        } else {
            Enter(sauna, 1);
            ChillInSauna();
            DecideAfterSauna();
        }
    }

    /**
     * @brief Visitor waits until the sauna is available
     */
    void WaitUntilSaunaIsAvailable() {
        double time_start = Time;
        WaitUntil(!sauna.Full() || Time - time_start > Exponential(params.saunaWaitTime));
    }

    /**
     * @brief Visitor spends time in the sauna
     */
    void ChillInSauna() {
        Wait(Exponential(params.saunaTime));
    }

    /**
     * @brief Visitor decides what to do after spending time in the sauna (leave or go swimming)
     */
    void DecideAfterSauna() {
        double random = Random();
        if (random <= 0.5) {
            Leave(sauna, 1);
        } else {
            Leave(sauna, 1);
            GoSwimming();
        }
    }

    /**
     * @brief Records transaction time for the visitor
     */
    void RecordTransactionTime() {
        double transactionTime = Time - startTime;
        transactionTimes.push_back(transactionTime);
    }
};

/** Class representing a generator that creates new visitors at specified intervals */
class Generator : public Event {
public:
    /**
     * @brief Behavior of the generator event
     */
    void Behavior() {
        (new Visitor)->Activate();
        totalVisitors++;
        Activate(Time + Exponential(params.visitorsArrivalTime));
    }
};

/**
 * @brief Prints the usage information for the program
 * @param programName Name of the program
 */
void printUsage(const char *programName) {
    cout << "Usage: " << programName
              << " -d --day <day_type>\n";
    cout << "Options:\n";
    cout << "  -h --help - prints help\n";
    cout << "  -d --day <day_type> - type of the day (weekday, weekend, holiday)\n";
}

/**
 * @brief Parses command line arguments and sets simulation parameters
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 */
void ParseArguments(int argc, char *argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    static struct option long_options[] = {
            {"help", no_argument,       0, 'h'},
            {"day",  required_argument, 0, 'd'},
            {0,      0,                 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hd:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printUsage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'd':
                if (strcmp(optarg, "weekday") == 0) {
                    params.workHours = 15 * 60 + 30;
                    params.visitorsArrivalTime = 1.42;
                } else if (strcmp(optarg, "weekend") == 0) {
                    params.workHours = 13 * 60 + 30;
                    params.visitorsArrivalTime = 1.30;
                } else if (strcmp(optarg, "holiday") == 0) {
                    params.workHours = 13 * 60 + 30;
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
        cerr << "Error: Unexpected argument after options\n";
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    ParseArguments(argc, argv);
    SetOutput("kohoutovice.dat");
    Init(0, params.workHours);

    (new Generator)->Activate();
    Run();
    lockerRoom.Output();
    sauna.Output();
    pool.Output();
    waterSlide.Output();

    double sum = 0;
    for (int i = 0; i < transactionTimes.size(); i++) {
        sum += transactionTimes[i];
    }

    ofstream file("kohoutovice.dat", ios::app);

    if (file.is_open()) {

        file << "Sauna waiters: " << totalWaiters << endl;
        file << "Total visitors: " << totalVisitors << endl;
        file.close();
    } else {
        cout << "Cant open file" << endl;
    }

    return 0;
}
