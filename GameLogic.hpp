#pragma once
#define SDL_MAIN_HANDLED
#include <iostream>
#include <string>
#include <cmath>

#include <vector>
#include <unordered_map>

#include <random>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_timer.h>

#include "Settings.hpp"
#include "GraphicsEngine.hpp"


namespace GameLogic {
    class Game : public GraphicsEngine::Window {
    public:
        Game(int width, int height, std::string title)
             : GraphicsEngine::Window(width, height, title) {

            for (int i = 0; i < MAP_WIDTH; ++i) {
                for (int j = 0; j < MAP_HEIGHT; ++j) {
                    worldMap[i][j] = Settings::worldMap[i][j];
                }
            }
        }

        void on_update(double delta_time) override {
            //speed modifiers
            double moveSpeed = delta_time * 5.0; //the constant value is in squares/second
            double rotSpeed = delta_time * 3.0; //the constant value is in radians/second

            //move forward if no wall in front of you
            if (key_manager.is_key_hold(SDLK_w))
            {
                if (worldMap[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
                if (worldMap[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
            }
            //move backwards if no wall behind you
            if (key_manager.is_key_hold(SDLK_s))
            {
                if (worldMap[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
                if (worldMap[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
            }
            //rotate to the right
            if (key_manager.is_key_hold(SDLK_d))
            {
                //both camera direction and camera plane must be rotated
                double oldDirX = dirX;
                dirX = dirX * cos(-rotSpeed) - dirY * sin(-rotSpeed);
                dirY = oldDirX * sin(-rotSpeed) + dirY * cos(-rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(-rotSpeed) - planeY * sin(-rotSpeed);
                planeY = oldPlaneX * sin(-rotSpeed) + planeY * cos(-rotSpeed);
            }
            //rotate to the left
            if (key_manager.is_key_hold(SDLK_a))
            {
                //both camera direction and camera plane must be rotated
                double oldDirX = dirX;
                dirX = dirX * cos(rotSpeed) - dirY * sin(rotSpeed);
                dirY = oldDirX * sin(rotSpeed) + dirY * cos(rotSpeed);
                double oldPlaneX = planeX;
                planeX = planeX * cos(rotSpeed) - planeY * sin(rotSpeed);
                planeY = oldPlaneX * sin(rotSpeed) + planeY * cos(rotSpeed);
            }
        }

        void on_draw(SDL_Renderer* renderer) {
            for (int x = 0; x < width; x++) {
                double cameraX = 2 * x / (double)width - 1;
                double rayDirX = dirX + planeX * cameraX;
                double rayDirY = dirY + planeY * cameraX;

                int mapX = (int)posX, mapY = (int)posY;

                // Initialize step and sideDist based on ray direction
                int stepX = rayDirX < 0 ? -1 : 1;
                int stepY = rayDirY < 0 ? -1 : 1;
                double deltaDistX = std::abs(1 / rayDirX);
                double deltaDistY = std::abs(1 / rayDirY);
                double sideDistX = stepX == -1 ? (posX - mapX) * deltaDistX : (mapX + 1.0 - posX) * deltaDistX;
                double sideDistY = stepY == -1 ? (posY - mapY) * deltaDistY : (mapY + 1.0 - posY) * deltaDistY;

                int hit = 0, side;
                double totalDistance = 0.0;

                // DDA Algorithm
                while (!hit) {
                    side = sideDistX < sideDistY ? 0 : 1;
                    if (side == 0) {
                        sideDistX += deltaDistX;
                        mapX += stepX;
                        totalDistance += deltaDistX;
                    }
                    else {
                        sideDistY += deltaDistY;
                        mapY += stepY;
                        totalDistance += deltaDistY;
                    }
                    hit = worldMap[mapX][mapY] > 0;
                }

                // Calculate wall distance and line height
                double wallDist = side == 0 ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
                int lineHeight = height / wallDist;

                // Draw the line
                SDL_Point start = { x, std::max(-lineHeight / 2 + height / 2, 0) };
                SDL_Point end = { x, std::min(lineHeight / 2 + height / 2, height - 1) };
                draw->line(start, end, choose_color(worldMap[mapX][mapY], side));
            }
        }

        GraphicsEngine::Color choose_color(int wallType, int side) {
            GraphicsEngine::Color RGB_Red(255, 0, 0, 100);    // Red
            GraphicsEngine::Color RGB_Green(0, 255, 0, 100);  // Green
            GraphicsEngine::Color RGB_Blue(0, 0, 255, 100);   // Blue
            GraphicsEngine::Color RGB_White(255, 255, 255, 100); // White
            GraphicsEngine::Color RGB_Yellow(255, 255, 0, 100); // Yellow

            // Choose wall color based on worldMap values
            GraphicsEngine::Color color;
            switch (wallType)
            {
            case 1:  color.copy(RGB_Red);    break; // Red
            case 2:  color.copy(RGB_Green);    break; // Green
            case 3:  color.copy(RGB_Blue);    break; // Blue
            case 4:  color.copy(RGB_White); break; // White
            default: color.copy(RGB_Yellow);   break; // Yellow
            }

            // Give x and y sides different brightness
            return color / (side + 1);
        }

    private:
        // Map
        static const int MAP_WIDTH = Settings::MAP_WIDTH;
        static const int MAP_HEIGHT = Settings::MAP_HEIGHT;
        int worldMap[MAP_WIDTH][MAP_HEIGHT];

        // Player
        double posX = 22, posY = 12;  //x and y start position
        double dirX = -1, dirY = 0; //initial direction vector
        double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane
    };
}

