#ifndef CONTROLLER_H
#define CONTROLLER_H

enum Movement
{
    none,
    forward,
    backward,
    turningLeft,
    turningRight
};

Movement& getMovement();
void control(double deltaTime);

#endif