#include <SDL.h>
#include <stdio.h>
#include <iostream>

#include "Model.h"
#include "View.h"

int main(int argc, char* args[])
{
    Player& player = getPlayer();
    auto& level = getLevel();

    bool movingForward = false;
    bool movingBackward = false;
    bool turningRight = false;
    bool turningLeft = false;
    bool moving = false;

    SDL_Event event;

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0;

    initView();

    // Main loop
    static bool done = false;
    while (!done)
    {
        // std::cout << "Player at (" << player.posX << ", " << player.posY << ")\n"; // Debug player pos
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = ((NOW - LAST) * 3 / (double)SDL_GetPerformanceFrequency());

        double oldDirX = player.dirX;
        double oldPlaneX = player.planeX;

        // Input
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN)
            {
                /* Check the SDLKey values and move change the coords */
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    turningLeft = true;
                    break;
                case SDLK_RIGHT:
                    turningRight = true;
                    break;
                case SDLK_UP:
                    movingForward = true;
                    moving = true;
                    break;
                case SDLK_DOWN:
                    movingBackward = true;
                    moving = true;
                    break;
                case SDLK_ESCAPE:
                    done = true;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_KEYUP)
            {
                /* Check the SDLKey values and move change the coords */
                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    turningLeft = false;
                    break;
                case SDLK_RIGHT:
                    turningRight = false;
                    break;
                case SDLK_UP:
                    movingForward = false;
                    moving = false;
                    break;
                case SDLK_DOWN:
                    movingBackward = false;
                    moving = false;
                    break;
                default:
                    break;
                }
            }
            if (event.type == SDL_QUIT) done = true;
        }

        // Applying input
        if (movingForward)
        {
            if (level[int(player.posX + player.dirX * player.moveSpeed * 4 * deltaTime)][int(player.posY)] == 0) player.posX += player.dirX * player.moveSpeed * deltaTime;
            if (level[int(player.posX)][int(player.posY + player.dirY * player.moveSpeed * 4 * deltaTime)] == 0) player.posY += player.dirY * player.moveSpeed * deltaTime;
        }
        if (movingBackward)
        {
            if (level[int(player.posX - player.dirX * player.moveSpeed * 4 * deltaTime)][int(player.posY)] == 0) player.posX -= player.dirX * player.moveSpeed * deltaTime;
            if (level[int(player.posX)][int(player.posY - player.dirY * player.moveSpeed * 4 * deltaTime)] == 0) player.posY -= player.dirY * player.moveSpeed * deltaTime;
        }
        if (turningRight)
        {
            player.dirX = player.dirX * cos(-player.rotSpeed * deltaTime) - player.dirY * sin(-player.rotSpeed * deltaTime);
            player.dirY = oldDirX * sin(-player.rotSpeed * deltaTime) + player.dirY * cos(-player.rotSpeed * deltaTime);
            player.planeX = player.planeX * cos(-player.rotSpeed * deltaTime) - player.planeY * sin(-player.rotSpeed * deltaTime);
            player.planeY = oldPlaneX * sin(-player.rotSpeed * deltaTime) + player.planeY * cos(-player.rotSpeed * deltaTime);
        }
        if (turningLeft)
        {
            player.dirX = player.dirX * cos(player.rotSpeed * deltaTime) - player.dirY * sin(player.rotSpeed * deltaTime);
            player.dirY = oldDirX * sin(player.rotSpeed * deltaTime) + player.dirY * cos(player.rotSpeed * deltaTime);
            player.planeX = player.planeX * cos(player.rotSpeed * deltaTime) - player.planeY * sin(player.rotSpeed * deltaTime);
            player.planeY = oldPlaneX * sin(player.rotSpeed * deltaTime) + player.planeY * cos(player.rotSpeed * deltaTime);
        }

        draw();
    }

    SDL_Quit();
    return 0;
}