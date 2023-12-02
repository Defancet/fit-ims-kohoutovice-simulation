#include <cstring>

#include "simlib.h"
#include <iostream>
#include <bits/getopt_core.h>

#define WaitUntil(condition) while(_WaitUntil(condition))

Store lockerRoom("locker Room", 200);
Store pool("swimming Pools", 180);
Store sauna("sauna", 20);

Facility waterSlide("water Slide");
int visitors = 0;
double workHours = 0;
double visitorsArivalTime = 0;

class Visitor : public Process {
public:
    void Behavior() {
        Enter(lockerRoom, 1);
        Wait(Exponential(5));   //changing time in locker room

        double choice = Random();
        if (choice <= 0.7) {    //go swimming
        pool: // bazen
            Enter(pool, 1);
            double nChoice = Random();

            if (nChoice <= 0.65) {   //continue swimming
            swimming:
                Wait(Exponential(25));
                double eChoice = Random();
                if (eChoice <= 0.7) {
                    goto swimming;
                }
                else if (eChoice <= 0.85) {
                    // change to 0.85
                    goto exitPool;
                }
                else {
                    goto toboggan;
                }
            }
            else {  //go from pool to toboggan
            toboggan:
                Seize(waterSlide);
                Wait(Exponential(1));
                Release(waterSlide);
                double tChoice = Random();
                if (tChoice <= 0.7) {
                    goto swimming;
                }
                else if (tChoice <= 0.8) {
                    goto exitPool;
                }
                else {
                    goto toboggan;
                }
            }
        }
        else {  //go to sauna
        saunaLabel:
            double time_start = Time;
            WaitUntil(!sauna.Full() || Time - time_start > Exponential(7));
            if (sauna.Full()) { // if sauna is full, go to pool
                goto pool;
            }

            Enter(sauna, 1);

        saunaInside:
            Wait(Exponential(20));  //chilling in sauna

            double lChoice = Random();
            if (lChoice <= 0.7) {   // go to pool
                Leave(sauna, 1);
                goto pool;
            }
            else if (lChoice <= 0.8) {  // leave
                Leave(sauna, 1);
                Leave(lockerRoom, 1);
                Wait(Exponential(5)); // prevlek (mb delete)
                std::cout << "SAUNA EXIT" << std::endl;
                return;
            }
            else {
                goto saunaInside;
            }
        }

    exitPool:
        Leave(pool, 1);
        double qChoice = Random();
        if (qChoice <= 0.3) {   // leave
            Leave(lockerRoom, 1);
            Wait(Exponential(5)); // prevlel delete ?
            std::cout << "BASIK EXIT" << std::endl;
            return;
        }
        else {
            goto saunaLabel;   // go to sauna
        }
    }
};


class Generator : public Event {
public:
    void Behavior() {
        (new Visitor)->Activate();
        visitors++;
        Activate(Time + Exponential(visitorsArivalTime));    //arrival time
    }
};

void printUsage(const char *programName) {
    std::cout << "Usage: " << programName
              << " ./kohoutovice -d <day_type>\n";
    std::cout << "Options:\n";
    std::cout << "  -d <day_type> - type of the day (weekday, weekend, holiday)\n";
}

void ParseArguments(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    int opt;

    while ((opt = getopt(argc, argv, "d:")) != -1) {
        switch (opt) {
            case 'd':
                if (strcmp(optarg, "weekday") == 0) {
                    workHours = 12 * 60;
                    visitorsArivalTime = 3;
                }
                else if (strcmp(optarg, "weekend") == 0) {
                    workHours = 14 * 60;
                    visitorsArivalTime = 2.5;
                }
                else if (strcmp(optarg, "holiday") == 0) {
                    workHours = 14 * 60;
                    visitorsArivalTime = 2;
                }
                else {
                    printUsage(argv[0]);
                    exit(EXIT_FAILURE);
                }
            break;

            case '?':
                    printUsage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (workHours == 0 || visitorsArivalTime == 0) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        std::cerr << "Error: Unexpected argument after options\n";
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[]) {
    ParseArguments(argc, argv);

    Init(0, workHours);

    (new Generator)->Activate();
    Run();
    lockerRoom.Output();
    sauna.Output();
    pool.Output();
    waterSlide.Output();
    std::cout << visitors << std::endl;

    return 0;
}
