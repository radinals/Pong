#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

class Game {
public:
    ~Game();
    Game();

    int play();

private:
    enum MoveDirections {
        MD_NONE,
        MD_LEFT  = 1 << 0,
        MD_RIGHT = 2 << 0,
        MD_UP    = 1 << 2,
        MD_DOWN  = 2 << 2,
    };

    struct Color_t {
        int r = 0, g = 0, b = 0, a = 0;
    };

    struct Entity_t {
        int          x = 0, y = 0, w = 0, h = 0;
        unsigned int move_direction   = 0;
        int          move_y_speed_mod = 0;
        int          move_x_speed_mod = 0;
        int          move_speed       = 0;
        Color_t      color;

        inline void setYDirection(const MoveDirections &direction)
        {
            move_direction
                = (move_direction ^ (move_direction & (3 << 2))) | direction;
        };

        inline void setXDirection(const MoveDirections &direction)
        {
            move_direction
                = (move_direction ^ (move_direction & 3)) | direction;
        }

        inline SDL_Rect toRect() const
        {
            return SDL_Rect { .x = x, .y = y, .w = w, .h = h };
        }
    };

    struct GameWindow_t {
        static constexpr const char *window_title = "Pong";
        static constexpr int         window_w = 800, window_h = 600;
        static const inline Color_t  bg_Color
            = { .r = 0, .g = 0, .b = 0, .a = 255 };

        SDL_Window   *window   = nullptr;
        SDL_Renderer *renderer = nullptr;
    } m_window;

    bool     m_game_reset = false, m_vs_com = false;
    size_t   m_player_1_points = 0, m_player_2_points = 0;
    Entity_t m_player_1, m_player_2, m_ball;

    void drawBackground();
    void moveEntity(Entity_t *const);
    void drawEntity(const Entity_t &);
    void drawEntities();
    void initEntities();
    void movePlayers();
    void moveBall();
    void calculateBallDeflection(const Entity_t &);
    void drawScore(size_t score, int pixel_size, int x_offset);

    static bool hasCollision(const Entity_t &, const Entity_t &);
};

#endif    // !GAME_H
