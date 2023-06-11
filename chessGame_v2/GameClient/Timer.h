#pragma once
#include <time.h>
class Timer
{
public:
    Timer() {}
    ~Timer() {}

        void init(time_t temp_duration) {

            initial_time = time(NULL);
            temp = temp_duration;

        }

        void update() {

            static time_t lastTime = initial_time;
            time_t deltaTime;
            //Sacamos la diferencia de tiempo desde la ultima iteracion y la actual
            deltaTime = time(NULL) - lastTime;
            //Actualizamos el tiempo restante
            temp -= deltaTime;
            //Almacenamos el tiempo de la iteracion actual
            lastTime += deltaTime;
        }

        time_t temp;
private:
        time_t initial_time;
};

