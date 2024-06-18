#include "game.hpp"
#include "letters.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <cassert>
#include <stdexcept>

Game::Game()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
        m_window.window = SDL_CreateWindow(m_window.window_title,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           m_window.window_w,
                                           m_window.window_h,
                                           SDL_WINDOW_SHOWN);
        m_window.renderer
            = SDL_CreateRenderer(m_window.window, -1, SDL_RENDERER_ACCELERATED);
    } else {
        throw std::runtime_error("Game::Game(): Failed to Create Window");
    }

    m_player_2.color = m_player_1.color = m_ball.color
        = { .r = 255, .g = 255, .b = 255, .a = 255 };
    initEntities();
}

Game::~Game()
{
    SDL_DestroyWindow(m_window.window);
    SDL_Quit();
}

int
Game::play()
{
    SDL_Event e;

    while (true) {
        m_player_1.setYDirection(MD_NONE);
        m_player_2.setYDirection(MD_NONE);

        SDL_PumpEvents();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { return 0; }
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP:
                        m_player_1.setYDirection(MD_UP);
                        break;
                    case SDLK_DOWN:
                        m_player_1.setYDirection(MD_DOWN);
                        break;
                    case SDLK_j:
                        m_player_2.setYDirection(MD_UP);
                        break;
                    case SDLK_k:
                        m_player_2.setYDirection(MD_DOWN);
                        break;
                    case SDLK_F1:
                        m_vs_com = !m_vs_com;
                        break;
                    case SDLK_F2:
                        m_ball.move_speed++;
                        if (m_ball.move_speed >= m_ball.w) {
                            m_ball.move_speed = m_ball.w;
                        }
                        break;
                    case SDLK_F3:
                        m_ball.move_speed--;
                        if (m_ball.move_speed <= 0) { m_ball.move_speed = 1; }
                        break;
                    case SDLK_F4:
                        m_player_1_points = m_player_2_points = 0;
                        initEntities();
                        break;
                }
            }
        }

        movePlayers();
        moveBall();

        if (m_game_reset) {
            initEntities();
            m_game_reset = false;
            continue;
        }

        drawBackground();
        drawEntities();

        SDL_RenderPresent(m_window.renderer);
        SDL_Delay(50);

        if (m_player_2_points >= 3 || m_player_1_points >= 3) {
            m_player_1_points = m_player_2_points = 0;
            SDL_Delay(500);
            initEntities();
            continue;
        }
    }

    return -1;
}

void
Game::initEntities()
{
    m_ball.setYDirection(MD_NONE);
    m_ball.setXDirection(MD_LEFT);

    m_player_1.move_direction = m_player_2.move_direction = MD_NONE;

    m_ball.w = m_ball.h = m_window.window_h * 0.05f;
    m_ball.y            = (m_window.window_h * 0.5f) - m_ball.h * 0.5f;
    m_ball.x            = (m_window.window_w * 0.5f) - m_ball.w * 0.5f;

    m_player_1.w = m_player_2.w = (m_ball.w);
    m_player_1.h = m_player_2.h = (m_ball.h * 4);

    m_player_1.x = m_ball.w * 2;
    m_player_1.y = (m_window.window_h * 0.5f) - (m_player_1.h * 0.5f);

    m_player_2.x = (m_window.window_w - m_player_2.w) - m_ball.w * 2;
    m_player_2.y = (m_window.window_h * 0.5f) - (m_player_2.h * 0.5f);

    m_player_1.move_speed = m_player_2.move_speed = m_player_1.h * 0.2f;
    m_ball.move_speed                             = m_ball.h * 0.5f;
}

void
Game::drawScore(size_t score, int pixel_size, int x_offset)
{
    const int y_offset = (pixel_size * Letters::LETTER_H);
    SDL_Rect  p;

    p.w = p.h = pixel_size;
    p.y       = y_offset;

    for (size_t i = 0; i < Letters::LETTER_H; i++) {
        p.x = x_offset;
        for (size_t j = 0; j < Letters::LETTER_W; j++) {
            if (Letters::letters[score][i][j]) {
                SDL_RenderFillRect(m_window.renderer, &p);
            }
            p.x += pixel_size;
        }
        p.y += pixel_size;
    }
}

void
Game::drawBackground()
{
    SDL_SetRenderDrawColor(m_window.renderer,
                           m_window.bg_Color.r,
                           m_window.bg_Color.g,
                           m_window.bg_Color.b,
                           m_window.bg_Color.a);

    SDL_RenderClear(m_window.renderer);

    SDL_SetRenderDrawColor(m_window.renderer, 255, 255, 255, 255);
    SDL_Rect separator;

    separator.h = m_window.window_h;
    separator.w = m_window.window_w * 0.01f;
    separator.x = (m_window.window_w * 0.5f) - (separator.w * 0.5f);
    separator.y = m_window.window_h - separator.h;

    const int px      = separator.w;
    const int spacing = separator.w * 4;

    drawScore(m_player_1_points,
              px,
              (separator.x + (separator.w * 0.5f))
                  - ((Letters::LETTER_W * px) + spacing));
    drawScore(m_player_2_points,
              px,
              (separator.x + (separator.w * 0.5f)) + spacing);

    SDL_RenderFillRect(m_window.renderer, &separator);
}

void
Game::drawEntity(const Entity_t &entity)
{
    const SDL_Rect r = entity.toRect();
    SDL_SetRenderDrawColor(m_window.renderer,
                           entity.color.r,
                           entity.color.g,
                           entity.color.b,
                           entity.color.a);
    SDL_RenderFillRect(m_window.renderer, &r);
}

void
Game::drawEntities()
{
    drawEntity(m_ball);
    drawEntity(m_player_1);
    drawEntity(m_player_2);
}

bool
Game::hasCollision(const Entity_t &a, const Entity_t &b)
{
    const float aMinX = a.x;
    const float aMaxY = a.y + a.h;
    const float aMaxX = a.x + a.w;
    const float aMinY = a.y;

    const float bMinX = b.x;
    const float bMaxY = b.y + b.h;
    const float bMaxX = b.x + b.w;
    const float bMinY = b.y;

    return !(aMaxX <= bMinX || aMinX >= bMaxX || aMaxY <= bMinY
             || aMinY >= bMaxY);
}

void
Game::calculateBallYMovement(Entity_t *p)
{
    m_ball.setYDirection(MD_NONE);
    m_ball.setYDirection(MoveDirections((p->move_direction >> 2) << 2));
}

void
Game::moveEntity(Entity_t *const entity)
{
    switch (entity->move_direction) {
        case MD_NONE:
            return;
        case MD_LEFT:

            entity->x -= entity->move_speed;
            break;
        case MD_RIGHT:

            entity->x += entity->move_speed;
            break;
        case MD_UP:

            entity->y -= entity->move_speed;
            break;
        case MD_DOWN:

            entity->y += entity->move_speed;
            break;
        case (MD_LEFT | MD_UP):

            entity->x -= entity->move_speed;
            entity->y -= entity->move_speed;
            break;
        case (MD_LEFT | MD_DOWN):

            entity->x -= entity->move_speed;
            entity->y += entity->move_speed;
            break;
        case (MD_RIGHT | MD_UP):

            entity->x += entity->move_speed;
            entity->y -= entity->move_speed;
            break;
        case (MD_RIGHT | MD_DOWN):

            entity->x += entity->move_speed;
            entity->y += entity->move_speed;
            break;
        default:
            return;
    }

    if (entity->y <= 0) {
        entity->y = 0;
    } else if ((entity->y + entity->h) >= m_window.window_h) {
        entity->y = m_window.window_h - entity->h;
    }
}

void
Game::movePlayers()
{
    moveEntity(&m_player_1);

    if (!m_vs_com) {
        moveEntity(&m_player_2);
    } else {
        if (m_player_2.y > m_ball.y) {
            m_player_2.setYDirection(MD_UP);
        } else if (m_player_2.y > m_ball.y) {
            m_player_2.setYDirection(MD_DOWN);
        } else {
            m_player_2.setYDirection(MD_NONE);
        }

        m_player_2.y = m_ball.y;
        if (m_player_2.y <= 0) {
            m_player_2.y = 0;
        } else if ((m_player_2.y + m_player_2.h) >= m_window.window_h) {
            m_player_2.y = m_window.window_h - m_player_2.h;
        }
    }
}

void
Game::moveBall()
{
    if (hasCollision(m_player_1, m_ball)) {
        calculateBallYMovement(&m_player_1);
        m_ball.setXDirection(MD_RIGHT);
    } else if (hasCollision(m_player_2, m_ball)) {
        calculateBallYMovement(&m_player_2);
        m_ball.setXDirection(MD_LEFT);
    } else if ((m_ball.x + m_ball.w) >= (m_player_2.x + m_player_2.w)) {
        m_player_1_points++;
        m_game_reset = true;
    } else if (m_ball.x < m_player_1.x) {
        m_player_2_points++;
        m_game_reset = true;
    } else if (m_ball.y <= 0) {
        m_ball.setYDirection(MD_DOWN);
    } else if ((m_ball.y + m_ball.h) >= m_window.window_h) {
        m_ball.setYDirection(MD_UP);
    }

    moveEntity(&m_ball);
}
