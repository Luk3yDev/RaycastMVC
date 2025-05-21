#include <SDL.h>
#include <stdio.h>
#include <iostream>

#include "Model.h"
#include "Controller.h"

Movement movement = none;
Movement& getMovement()
{
    return movement;
}

Player& player = getPlayer();
auto& level = getLevel();

void control(double deltaTime)
{
    double oldDirX = player.dirX;
    double oldPlaneX = player.planeX;
    
    if (movement == forward)
    {
        if (level[int(player.posX + player.dirX * player.moveSpeed * 4 * deltaTime)][int(player.posY)] == 0) player.posX += player.dirX * player.moveSpeed * deltaTime;
        if (level[int(player.posX)][int(player.posY + player.dirY * player.moveSpeed * 4 * deltaTime)] == 0) player.posY += player.dirY * player.moveSpeed * deltaTime;
    }
    if (movement == backward)
    {
        if (level[int(player.posX - player.dirX * player.moveSpeed * 4 * deltaTime)][int(player.posY)] == 0) player.posX -= player.dirX * player.moveSpeed * deltaTime;
        if (level[int(player.posX)][int(player.posY - player.dirY * player.moveSpeed * 4 * deltaTime)] == 0) player.posY -= player.dirY * player.moveSpeed * deltaTime;
    }
    if (movement == turningRight)
    {
        player.dirX = player.dirX * cos(-player.rotSpeed * deltaTime) - player.dirY * sin(-player.rotSpeed * deltaTime);
        player.dirY = oldDirX * sin(-player.rotSpeed * deltaTime) + player.dirY * cos(-player.rotSpeed * deltaTime);
        player.planeX = player.planeX * cos(-player.rotSpeed * deltaTime) - player.planeY * sin(-player.rotSpeed * deltaTime);
        player.planeY = oldPlaneX * sin(-player.rotSpeed * deltaTime) + player.planeY * cos(-player.rotSpeed * deltaTime);
    }
    if (movement == turningLeft)
    {
        player.dirX = player.dirX * cos(player.rotSpeed * deltaTime) - player.dirY * sin(player.rotSpeed * deltaTime);
        player.dirY = oldDirX * sin(player.rotSpeed * deltaTime) + player.dirY * cos(player.rotSpeed * deltaTime);
        player.planeX = player.planeX * cos(player.rotSpeed * deltaTime) - player.planeY * sin(player.rotSpeed * deltaTime);
        player.planeY = oldPlaneX * sin(player.rotSpeed * deltaTime) + player.planeY * cos(player.rotSpeed * deltaTime);
    }
}