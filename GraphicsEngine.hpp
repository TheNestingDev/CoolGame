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


namespace GraphicsEngine {
    class Timer {
    public:
        Timer() {
            freq = SDL_GetPerformanceFrequency();
            start = SDL_GetPerformanceCounter();
        }

        double get_elapsed_time() {
            Uint64 now = SDL_GetPerformanceCounter();
            return (now - start) / static_cast<double>(freq);
        }

        void reset() {
            start = SDL_GetPerformanceCounter();
        }

    private:
        Uint64 freq;
        Uint64 start;
    };

    class StateManager {
    public:
        enum class KeyState {
            NotObserved,
            Pressed,
            Released,
            NoChange
        };

        bool is_key_hold(int key_code) {
            KeyState state = get_state(key_code);
            if (state == KeyState::Pressed) {
                return true;
            }
            return false;
        }

        KeyState check_change(bool state, int key_code) {
            KeyState last_key_state = get_state(key_code);

            KeyState current_key_state;
            if (state) current_key_state = KeyState::Pressed;
            else current_key_state = KeyState::Released;

            if (last_key_state == KeyState::NotObserved) last_key_state = KeyState::Released;
            if (last_key_state == current_key_state) return KeyState::NoChange;  // Return the unchanged state

            if (last_key_state == KeyState::Pressed && current_key_state == KeyState::Released) {
                // Key got released
                state_map[key_code] = KeyState::Released;
                return KeyState::Released;
            }
            else if (last_key_state == KeyState::Released && current_key_state == KeyState::Pressed) {
                // Key got pressed
                state_map[key_code] = KeyState::Pressed;
                return KeyState::Pressed;
            }
            return KeyState::NoChange;  // Default return value
        }

    private:
        std::unordered_map<int, KeyState> state_map;

        // Get the state of a key
        KeyState get_state(int key) {
            auto it = state_map.find(key);
            if (it != state_map.end()) {
                return it->second;
            }
            return KeyState::NotObserved;
        }
    };

    class Color {
    public:
        int red;    // Red component (0-255)
        int green;  // Green component (0-255)
        int blue;   // Blue component (0-255)
        int gamma;  // Gamma component (0-100)

        // Constructor to initialize the color components
        Color(int r, int g, int b, int a)
            : red(r), green(g), blue(b), gamma(a) {
            // Ensure that the color components are within valid ranges
            red = clamp(red, 0, 255);
            green = clamp(green, 0, 255);
            blue = clamp(blue, 0, 255);
            gamma = clamp(gamma, 0, 100);
        }

        // Alternative constructor for uninitialized color
        Color()
            : red(0), green(0), blue(0), gamma(0) {}

        // Helper function to clamp a value within a specified range
        int clamp(int value, int min, int max) {
            if (value < min) return min;
            if (value > max) return max;
            return value;
        }

        void clampAll() {
            red = clamp(red, 0, 255);
            green = clamp(green, 0, 255);
            blue = clamp(blue, 0, 255);
            gamma = clamp(gamma, 0, 100);
        }

        // Getter methods to retrieve color components
        int getRed() const {
            return red;
        }

        int getGreen() const {
            return green;
        }

        int getBlue() const {
            return blue;
        }

        int getAlpha() const {
            return gamma;
        }

        // Setter method to set all color components simultaneously
        void set(int r, int g, int b, int a) {
            red = clamp(r, 0, 255);
            green = clamp(g, 0, 255);
            blue = clamp(b, 0, 255);
            gamma = clamp(a, 0, 100);
        }

        // Setter method to set all color components simultaneously
        void copy(const Color& other) {
            red = clamp(other.red, 0, 255);
            green = clamp(other.green, 0, 255);
            blue = clamp(other.blue, 0, 255);
            gamma = clamp(other.gamma, 0, 100);
        }

        // Divide operator overload
        Color operator/(int divisor) const {
            // Ensure that divisor is not zero to avoid division by zero
            if (divisor == 0) {
                // You can choose how to handle this case; here, we return a copy of the original color.
                return *this;
            }

            // Perform component-wise division and return a new Color
            return Color(red / divisor, green / divisor, blue / divisor, gamma);
        }
    };


    class Draw {
    public:
        // Constructor
        Draw(SDL_Renderer* renderer) : renderer(renderer) {}

        // Create an SDL_Rect
        SDL_Rect createRect(int x, int y, int width, int height) {
            SDL_Rect rect = { x, y, width, height };
            return rect;
        }

        // Create an SDL_Point
        SDL_Point createPoint(int x, int y) {
            SDL_Point point = { x, y };
            return point;
        }

        // Create a vector of SDL_Point objects
        std::vector<SDL_Point> createPoints(const std::vector<std::pair<int, int>>& coordinates) {
            std::vector<SDL_Point> points;
            for (const auto& coord : coordinates) {
                points.push_back(createPoint(coord.first, coord.second));
            }
            return points;
        }

        // Drawing Methods
        /**
         * Fills a rectangle with a specified color.
         * @param rect The SDL_Rect object defining the rectangle's dimensions.
         * @param color The Color object representing the fill color.
         */
        void rect(const SDL_Rect& rect, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderFillRect(renderer, &rect);
        }

        /**
         * Draws the outline of a rectangle with a specified color.
         * @param rect The SDL_Rect object defining the rectangle's dimensions.
         * @param color The Color object representing the outline color.
         */
        void rectOutline(const SDL_Rect& rect, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderDrawRect(renderer, &rect);
        }

        /**
         * Draws a line segment between two points with a specified color.
         * @param point1 The starting SDL_Point.
         * @param point2 The ending SDL_Point.
         * @param color The Color object representing the line color.
         */
        void line(const SDL_Point& point1, const SDL_Point& point2, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderDrawLine(renderer, point1.x, point1.y, point2.x, point2.y);
        }

        /**
         * Draws a point at a specified location with a specified color.
         * @param point The SDL_Point object defining the location of the point.
         * @param color The Color object representing the point color.
         */
        void point(const SDL_Point& point, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderDrawPoint(renderer, point.x, point.y);
        }

        /**
         * Draws multiple points with a specified color.
         * @param pointsArray A std::vector containing SDL_Point objects.
         * @param color The Color object representing the points color.
         */
        void points(const std::vector<SDL_Point>& pointsArray, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderDrawPoints(renderer, pointsArray.data(), static_cast<int>(pointsArray.size()));
        }

        /**
         * Draws multiple connected line segments with a specified color.
         * @param pointsArray A std::vector containing SDL_Point objects.
         * @param color The Color object representing the lines color.
         */
        void lines(const std::vector<SDL_Point>& pointsArray, const GraphicsEngine::Color& color) {
            SDL_SetRenderDrawColor(renderer, color.getRed(), color.getGreen(), color.getBlue(), color.getAlpha());
            SDL_RenderDrawLines(renderer, pointsArray.data(), static_cast<int>(pointsArray.size()));
        }

    private:
        SDL_Renderer* renderer = nullptr;
    };


    class Window {
    public:
        Draw* draw;

        Window(int width, int height, std::string title)
            : width(width), height(height), title(title), running(true) {

            if (!initSDL()) {
                throw std::runtime_error("Failed to initialize SDL!");
            }

            draw = new Draw(renderer);
        }

        // Cleanup
        ~Window() {
            delete draw;
            SDL_DestroyWindow(window);
            SDL_Quit();
        }

        // foolprove game exit...
        void exit_game() {
            set_running(false);
        }

        bool is_game_running() const {
            return running;
        }

        // game loop
        virtual void on_draw(SDL_Renderer* renderer) {}
        virtual void on_update(double delta_time) {}

        // key events
        virtual void on_key_press(SDL_Event e) {
            std::cout << "Key Pressed: " << e.key.keysym.sym << std::endl;
        }
        virtual void on_key_release(SDL_Event e) {
            std::cout << "Key Released: " << e.key.keysym.sym << std::endl;
        }

        // mouse events
        virtual void on_mouse_press(SDL_Event e) {
            std::cout << "Mouse Pressed: " << e.key.keysym.sym << std::endl;
        }
        virtual void on_mouse_release(SDL_Event e) {
            std::cout << "Mouse Released: " << e.key.keysym.sym << std::endl;
        }
        virtual void on_mouse_motion(SDL_Event e) {}

        void run() {
            Timer gameTickTimer;
            double delta_time;

            while (running) {
                delta_time = gameTickTimer.get_elapsed_time();
                gameTickTimer.reset();

                handle_events();
                on_update(delta_time);
                draw_frame();

                SDL_Delay(0);  // Roughly 60 frames per second
            }
        }

    private:
        bool initSDL() {
            window = SDL_CreateWindow(
                title.c_str(),
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                width,
                height,
                0
            );

            if (!window) {
                std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
                return false;
            }

            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

            if (!renderer) {
                std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
                return false;
            }

            return true;
        }

        void handle_key_events(SDL_Event e) {
            // Quick exit for debugging and testing while developing window class
            int key_code = e.key.keysym.sym;
            const int ESCAPE_KEY = 27;
            if (key_code == ESCAPE_KEY) {
                exit_game();
                return;  // Important to exit early.
            }

            bool key_pressed;
            if (e.type == SDL_KEYDOWN) {
                key_pressed = true;
            }
            else if (e.type == SDL_KEYUP) {
                key_pressed = false;
            }
            StateManager::KeyState key_action = key_manager.check_change(key_pressed, key_code);
            if (key_action == StateManager::KeyState::Pressed) {
                on_key_press(e);
            }
            else if (key_action == StateManager::KeyState::Released) {
                on_key_release(e);
            }
        }

        void handle_mouse_events(SDL_Event e) {
            if (e.type == SDL_MOUSEMOTION) {
                on_mouse_motion(e);
                return;
            }

            int button_code = e.button.button;;
            bool mouse_pressed;
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                mouse_pressed = true;
            }
            else if (e.type == SDL_MOUSEBUTTONUP) {
                mouse_pressed = false;
            }
            StateManager::KeyState mouse_action = mouse_manager.check_change(mouse_pressed, button_code);
            if (mouse_action == StateManager::KeyState::Pressed) {
                on_mouse_press(e);
            }
            else if (mouse_action == StateManager::KeyState::Released) {
                on_mouse_release(e);
            }
        }

        void handle_events() {
            SDL_Event e;
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    exit_game();
                }
                else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                    handle_key_events(e);
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN ||
                    e.type == SDL_MOUSEBUTTONUP ||
                    e.type == SDL_MOUSEMOTION) {
                    handle_mouse_events(e);
                }
            }
        }

        void draw_frame() {
            // clear screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            on_draw(renderer);

            SDL_RenderPresent(renderer);
        }

        void set_running(bool value) {
            if (!value || !running) {
                running = false;
            }
        }

        // window
        std::string title;
        SDL_Window* window = nullptr;
        SDL_Renderer* renderer = nullptr;

        // game loop
        bool running;

    protected:
        // window
        int width;
        int height;

        // evnet handling
        StateManager key_manager;
        StateManager mouse_manager;
    };
}
