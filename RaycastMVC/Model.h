#ifndef MODEL_H
#define MODEL_H

const int mapWidth = 25;
const int mapHeight = 25;

struct Player {
    double posX, posY;
    double dirX, dirY;
    double planeX, planeY;
    double moveSpeed;
    double rotSpeed;
};

int(&getLevel())[mapWidth][mapHeight];

Player& getPlayer();

#endif
