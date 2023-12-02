#include "simlib.h"
#include <iostream>

#define WaitUntil(condition) while(_WaitUntil(condition))

Store lockerRoom("locker Room", 200);
Store pool("swimming Pools", 180);
Store sauna("sauna", 20);

Facility waterSlide("water Slide");
int visitors = 0;

class Visitor : public Process {
public:
    void Behavior() {

        Enter(lockerRoom, 1);
        Wait(Exponential(5));

        double choice = Random();
        if(choice <= 0.7){
            pool: // bazen
            Enter(pool,1);
            double nChoice = Random();

            if(nChoice <= 0.7){
                swimming:
                Wait(Exponential(25));
                double eChoice = Random();
                if(eChoice <= 0.7){
                    goto swimming;
                }
                else if(eChoice <= 0.85){ // change to 0.85
                    goto exitPool;
                }
                else{
                    goto toboggan;
                }

            }
            else{
                toboggan:
                Seize(waterSlide);
                Wait(Exponential(1));
                Release(waterSlide);
                double tChoice = Random();
                if(tChoice <= 0.7){
                    goto swimming;
                }
                else if(tChoice <= 0.8){
                    goto exitPool;
                }
                else{
                    goto toboggan;
                }

            }

        }
        else{
            // sauna
            saunaLabel:

            double time_start = Time;
            WaitUntil(!sauna.Full() || Time - time_start > Exponential(7));
            if(sauna.Full()){
                goto pool;
            }

            Enter(sauna, 1);

            saunaInside:
            Wait(Exponential(20));

            double lChoice = Random();
            if(lChoice <= 0.7){
                Leave(sauna, 1);
                goto pool;
            }
            else if(lChoice <= 0.8){
                Leave(sauna, 1);
                Leave(lockerRoom, 1);
                Wait(Exponential(5)); // prevlek (mb delete)
                std::cout << "SAUNA EXIT" << std::endl;
                return;
            }
            else{
                goto saunaInside;
            }

        }

        exitPool:
        Leave(pool, 1);
        double qChoice = Random();
        if(qChoice <= 0.3){
            Leave(lockerRoom, 1);
            Wait(Exponential(5)); // prevlel delete ?
            std::cout << "BASIK EXIT" << std::endl;
            return;
        }
        else{
            goto saunaLabel;
        }
    }
};


class Generator : public Event {
public:
    void Behavior() {
        (new Visitor)->Activate();
        visitors ++;
        Activate(Time+Exponential(3));
    }

};

int main()
{
    Init(0,60*13);
    (new Generator)->Activate();
    Run();
    lockerRoom.Output();
    sauna.Output();
    pool.Output();
    waterSlide.Output();
    std::cout << visitors << std::endl;
}

