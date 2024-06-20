#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_stdinc.h>

class Game {
public:
    ~Game();

    static Game &getInstance()
    {
        static Game g;
        return g;
    }

    int play();

private:
    Game();

    enum MoveDirections {
        MD_NONE,
        MD_LEFT  = 1 << 0,
        MD_RIGHT = 2 << 0,
        MD_UP    = 1 << 2,
        MD_DOWN  = 2 << 2,
    };

    struct Color_t {
        int r = 0, g = 0, b = 0, a = 0;
        Color_t() { }
        Color_t(int r, int g, int b, int a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        };
    };

    struct Entity_t {
        int          x = 0, y = 0, w = 0, h = 0;
        unsigned int move_direction = 0;
        int          velocity       = 0;
        int          move_speed     = 0;
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

    struct GameWindow {
        static constexpr const char *window_title = "Pong";
        static constexpr int         window_w = 800, window_h = 600;

        static Color_t bg_Color;
        static Color_t fg_Color;

        static SDL_Window   *window;
        static SDL_Renderer *renderer;
    };

    const static Color_t m_clr_Black;
    const static Color_t m_clr_White;

    static bool     m_game_reset, m_vs_com;
    static size_t   m_player_1_points, m_player_2_points;
    static Entity_t m_player_1, m_player_2, m_ball;

    static void drawBackground();
    static void drawEntities();
    static void initEntities();
    static void movePlayers();
    static void moveBall();
    static void calculateBallDeflection(const Entity_t &);
    static void moveEntity(Entity_t *const);
    static void drawScore(size_t score, int pixel_size, int x_offset);
    static void drawEntity(const Entity_t &);
    static bool hasCollision(const Entity_t &, const Entity_t &);
};

#endif    // !GAME_H
