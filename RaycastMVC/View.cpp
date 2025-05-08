#include <SDL.h>
#include <stdio.h>
#include <iostream>

#include "Model.h"

#define screenWidth 640
#define screenHeight 640

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int bpp;

SDL_Texture* frameBuffer;
uint32_t* pixels = nullptr;

Player& player = getPlayer();
auto& level = getLevel();

int pitch = 0;

SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

SDL_Color getPixelColor(SDL_Surface* surface, int x, int y) {
    SDL_Color color = { 0, 0, 0, 255 }; // Default color (black)

    // Check if the surface is valid
    if (surface == nullptr) {
        std::cerr << "Surface is null!" << std::endl;
        return color;
    }

    // Lock the surface if necessary
    if (SDL_MUSTLOCK(surface)) {
        SDL_LockSurface(surface);
    }

    Uint8* pixel = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

    // Get the color based on the pixel format
    switch (bpp) {
    case 1: // 8-bit
        color.r = color.g = color.b = *pixel; // Grayscale
        break;
    case 2: // 16-bit
    {
        Uint16 p = *(Uint16*)pixel;
        SDL_GetRGB(p, surface->format, &color.r, &color.g, &color.b);
    }
    break;
    case 3: // 24-bit
        color.b = *pixel++;
        color.g = *pixel++;
        color.r = *pixel++;
        break;
    case 4: // 32-bit
        Uint32 p = *(Uint32*)pixel;
        SDL_GetRGBA(p, surface->format, &color.r, &color.g, &color.b, &color.a);
        break;
    }

    // Unlock the surface if it was locked
    if (SDL_MUSTLOCK(surface)) {
        SDL_UnlockSurface(surface);
    }

    return color;
}

void initView()
{
    // Init
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        window = SDL_CreateWindow("Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        }
        else if (renderer == NULL)
        {
            printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        }
    }

    frameBuffer = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenWidth,
        screenHeight);

    if (!frameBuffer) {
        std::cerr << "Texture creation failed: " << SDL_GetError() << std::endl;
    }    
}

void draw()
{
    // Lock the texture for pixel manipulation
    if (SDL_LockTexture(frameBuffer, nullptr, (void**)&pixels, &pitch) != 0) {
        std::cerr << "SDL_LockTexture failed: " << SDL_GetError() << std::endl;
    }

    // Clear screen
    for (int i = 0; i < screenWidth * screenHeight; ++i) {
        pixels[i] = SDL_MapRGB(format, 0, 0, 0); // Set each pixel to black
    }

    // Raycast
    for (int x = 0; x < screenWidth; x++)
    {
        double cameraX = 2 * x / (double)screenWidth - 1;
        double rayDirX = player.dirX + player.planeX * cameraX;
        double rayDirY = player.dirY + player.planeY * cameraX;

        int mapX = int(player.posX);
        int mapY = int(player.posY);

        double sideDistX;
        double sideDistY;

        double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

        double perpWallDist = 0;

        int stepX;
        int stepY;

        int hit = 0;
        int side;

        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (player.posX - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player.posX) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (player.posY - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - player.posY) * deltaDistY;
        }

        int distfade = 0;

        // Digital Differential Analysis (DDA)
        while (hit == 0)
        {
            int distfade = std::min(255, int(perpWallDist * 32));
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }

            if (level[mapX][mapY] > 0)
            {
                hit = level[mapX][mapY];
            }
        }

        if (side == 0) perpWallDist = (sideDistX - deltaDistX);
        else           perpWallDist = (sideDistY - deltaDistY);

        int lineHeight = (int)(screenHeight / perpWallDist);

        int drawStart = -lineHeight / 2 + screenHeight / 2;
        if (drawStart < 0) drawStart = 0;
        int drawEnd = lineHeight / 2 + screenHeight / 2;
        if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

        double wallX; // Exactly where the wall was hit
        if (side == 0) wallX = player.posY + perpWallDist * rayDirY;
        else           wallX = player.posX + perpWallDist * rayDirX;
        wallX -= floor((wallX));

        int fadeValue = 12;
        for (int y = drawStart; y < drawEnd; y++)
        {
            SDL_Color rgb = { hit*255, hit*255, hit*255 };
            
            int distfade = std::min(255, int(perpWallDist * fadeValue));
            rgb.r = std::max(0, rgb.r - distfade);
            rgb.g = std::max(0, rgb.g - distfade);
            rgb.b = std::max(0, rgb.b - distfade);

            if (side == 1)
            {
                rgb.r >>= 1;
                rgb.g >>= 1;
                rgb.b >>= 1;
            }

            if (x >= 0 && x < screenWidth && y >= 0 && y < screenHeight) {
                int index = y * (pitch / 4) + x;
                pixels[index] = SDL_MapRGB(format, rgb.r, rgb.g, rgb.b);
            }
        }
    }

    SDL_UnlockTexture(frameBuffer);

    SDL_RenderCopy(renderer, frameBuffer, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}