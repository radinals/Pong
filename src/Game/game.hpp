#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>
#include <cstdint>
#include <iostream>

class Game {
    struct GameObjects_t;

public:
    ~Game();
    Game();

    int play();

private:
    enum MoveDirections {
        MD_NONE  = 0,
        MD_UP    = 1 << 2,
        MD_DOWN  = 2 << 2,
        MD_LEFT  = 3 << 2,
        MD_RIGHT = 4 << 2,
    };

    struct GameObject_t {
        SDL_Rect       rect;
        MoveDirections move_direction;

        void move(unsigned int direction, int speed = 10)
        {
            switch (direction) {
                case MD_UP:
                    move_direction = MD_UP;
                    rect.y -= speed;
                    break;
                case MD_DOWN:
                    move_direction = MD_DOWN;
                    rect.y += speed;
                    break;
                case MD_LEFT:
                    rect.x -= speed;
                    break;
                case MD_RIGHT:
                    rect.x += speed;
                    break;
                default:
                    break;
            }
        };
    };

    struct Color_t {
        int r = 0, g = 0, b = 0, a = 0;
    };

    struct BallInfo_t {
        static inline MoveDirections y_axis_direction = MD_NONE,
                                     x_axis_direction = MD_LEFT;
    } m_ball_info;

    struct GameWindow_t {
        static inline const char *window_title = "Pong";
        static constexpr int      window_w = 800, window_h = 600;

        static inline SDL_Window   *window   = nullptr;
        static inline SDL_Renderer *renderer = nullptr;
        static inline SDL_Surface  *surface  = nullptr;

        static const inline Color_t window_bg
            = { .r = 0, .g = 0, .b = 0, .a = 255 };

        static const inline Color_t window_fg
            = { .r = 255, .g = 255, .b = 255, .a = 255 };

    } m_game_window;

    bool m_game_reset = false;
    int  m_ball_speed = 1;

    static inline MoveDirections m_ball_movement_direction
        = MoveDirections::MD_RIGHT;

    static inline GameObject_t m_player_1, m_player_2, m_ball;

    static inline const Uint8 *m_keystates;

    void       drawBackground();
    void       drawObject(GameObject_t *const);
    void       drawFrame();
    void       configureObjects();
    void       moveBall();
    void       calculateBallYMovement(GameObject_t *);
    static int randomRange(int min, int max);

    // void checkWinState();

    // void reset();

    static bool hasCollision(GameObject_t *, GameObject_t *);
};

#endif    // !GAME_H
