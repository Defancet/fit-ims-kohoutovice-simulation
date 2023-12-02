#include "simlib.h"
#include <iostream>

Store lockerRoom("locker Room", 250);
Store pool("swimming Pool", 230);
Store sauna("sauna", 20);

Facility waterSlide("water Slide");

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
                if(eChoice <= 0.4){
                    goto swimming;
                }
                else if(eChoice <= 0.6){
                    goto exitPool;
                }
                else{
                    goto toboggan;
                }

            }
            else{
                toboggan:
                Seize(waterSlide);
                Wait(Exponential(2));
                Release(waterSlide);
                double tChoice = Random();
                if(tChoice <= 0.7){
                    goto swimming;
                }
                else if(tChoice <= 0.75){
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
            Enter(sauna, 1);

            saunaInside:
            Wait(Exponential(30));

            double lChoice = Random();
            if(lChoice <= 0.7){
                Leave(sauna, 1);
                goto pool;
            }
            else if(lChoice <= 0.8){
                Leave(sauna, 1);
                Leave(lockerRoom, 1);
                Wait(Exponential(5)); // prevlek (mb delete)
                return;
            }
            else{
                goto saunaInside;
            }

        }

        exitPool:
        Leave(pool, 1);
        double qChoice = Random();
        if(qChoice <= 0.2){
            Leave(lockerRoom, 1);
            Wait(Exponential(5)); // prevlel delete ?
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
        Activate(Time+Exponential(5));
    }

};

int main()
{
    Init(0,100000);
    (new Generator)->Activate();
    Run();
    lockerRoom.Output();
    sauna.Output();
    pool.Output();
    waterSlide.Output();
}

