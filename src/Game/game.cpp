#include "game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <iostream>
#include <random>
#include <stdexcept>

Game::Game()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
        m_game_window.window   = SDL_CreateWindow(m_game_window.window_title,
                                                SDL_WINDOWPOS_UNDEFINED,
                                                SDL_WINDOWPOS_UNDEFINED,
                                                m_game_window.window_w,
                                                m_game_window.window_h,
                                                SDL_WINDOW_SHOWN);
        m_game_window.renderer = SDL_CreateRenderer(m_game_window.window,
                                                    -1,
                                                    SDL_RENDERER_ACCELERATED);
    } else {
        throw std::runtime_error("Game::Game(): Failed to Create Window");
    }

    configureObjects();
}

Game::~Game()
{
    SDL_DestroyWindow(m_game_window.window);
    SDL_Quit();
}

int
Game::play()
{
    SDL_Event e;
    bool      quit = false;

    while (!quit) {
        SDL_PumpEvents();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
                break;
            }
            m_player_1.move_direction = MD_NONE;
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    m_player_1.move_direction = MD_UP;
                    m_player_1.move(MoveDirections::MD_UP);
                    break;
                case SDLK_DOWN:
                    m_player_1.move_direction = MD_DOWN;
                    m_player_1.move(MoveDirections::MD_DOWN);
                    break;
                    // case SDLK_w:
                    //     m_player_2.move(MoveDirections::MD_UP);
                    //     break;
                    // case SDLK_s:
                    //     m_player_2.move(MoveDirections::MD_DOWN);
                    //     break;
            }
            if (m_player_1.rect.y <= 0) {
                m_player_1.rect.y = 0;
            } else if ((m_player_1.rect.y + m_player_1.rect.h)
                       >= m_game_window.window_h) {
                m_player_1.rect.y = m_game_window.window_h - m_player_1.rect.h;
            }
        }

        SDL_SetRenderDrawColor(m_game_window.renderer, 50, 100, 50, 255);
        SDL_RenderClear(m_game_window.renderer);

        moveBall();

        if (m_game_reset) {
            configureObjects();
            m_game_reset = false;
            continue;
        }

        float nw_y
            = (m_ball.rect.y + m_ball.rect.h) - (m_player_2.rect.h * 0.5f);

        if (nw_y > m_player_2.rect.y) {
            m_player_2.move_direction = MD_DOWN;
        } else if (nw_y == m_player_2.rect.y) {
            m_player_2.move_direction = MD_NONE;
        } else {
            m_player_2.move_direction = MD_UP;
        }

        // if (nw_y > m_player_1.rect.y) {
        //     m_player_1.move_direction = MD_DOWN;
        // } else if (nw_y == m_player_1.rect.y) {
        //     m_player_1.move_direction = MD_NONE;
        // } else {
        //     m_player_1.move_direction = MD_UP;
        // }

        m_player_2.rect.y = nw_y;
        // m_player_1.rect.y = nw_y;

        if (m_player_2.rect.y <= 0) {
            m_player_2.rect.y = 0;
        } else if ((m_player_2.rect.y + m_player_2.rect.h)
                   >= m_game_window.window_h) {
            m_player_2.rect.y = m_game_window.window_h - m_player_2.rect.h;
        }

        // if (m_player_1.rect.y <= 0) {
        //     m_player_1.rect.y = 0;
        // } else if ((m_player_1.rect.y + m_player_1.rect.h)
        //            >= m_game_window.window_h) {
        //     m_player_1.rect.y = m_game_window.window_h - m_player_1.rect.h;
        // }

        drawBackground();
        drawFrame();
        SDL_RenderPresent(m_game_window.renderer);
        SDL_Delay(50);
    }

    return 0;
}

void
Game::configureObjects()
{
    m_ball_info.y_axis_direction = MD_NONE;
    m_ball_info.x_axis_direction = MD_LEFT;

    m_ball.rect.w = m_ball.rect.h = m_game_window.window_h * 0.05f;
    m_ball.rect.y = (m_game_window.window_h * 0.5f) - m_ball.rect.h * 0.5f;
    m_ball.rect.x = (m_game_window.window_w * 0.5f) - m_ball.rect.w * 0.5f;

    m_ball_speed = m_ball.rect.w;

    m_player_1.move_direction = MD_NONE;
    m_player_2.move_direction = MD_NONE;

    m_player_1.rect.w = m_player_2.rect.w = (m_ball.rect.w);
    m_player_1.rect.h = m_player_2.rect.h = (m_ball.rect.h * 4);

    m_player_1.rect.x = m_ball.rect.w * 2;
    m_player_1.rect.y
        = (m_game_window.window_h * 0.5f) - (m_player_1.rect.h * 0.5f);

    m_player_2.rect.x
        = (m_game_window.window_w - m_player_2.rect.w) - m_ball.rect.w * 2;
    m_player_2.rect.y
        = (m_game_window.window_h * 0.5f) - (m_player_2.rect.h * 0.5f);
}

void
Game::drawBackground()
{
    SDL_SetRenderDrawColor(m_game_window.renderer, 255, 255, 255, 255);
    SDL_Rect separator;

    separator.h = m_game_window.window_h;
    separator.w = 10;

    separator.x = (m_game_window.window_w * 0.5f) - (separator.w * 0.5f);
    separator.y = m_game_window.window_h - separator.h;

    SDL_RenderFillRect(m_game_window.renderer, &separator);
}

void
Game::drawObject(GameObject_t *const obj)
{
    SDL_RenderFillRect(m_game_window.renderer, &obj->rect);
}

void
Game::drawFrame()
{
    SDL_SetRenderDrawColor(m_game_window.renderer, 255, 255, 0, 255);
    drawObject(&m_ball);
    // GameObject_t b(m_ball);
    // b.rect.h += m_ball.rect.h * 0.3f;
    // b.rect.w -= m_ball.rect.w * 0.3f;
    // b.rect.x = ((m_ball.rect.x) + (m_ball.rect.w * 0.5f)) - (b.rect.w *
    // 0.5f); b.rect.y = ((m_ball.rect.y) + (m_ball.rect.h * 0.5f)) - (b.rect.h
    // * 0.5f);
    drawObject(&m_ball);
    // drawObject(&b);
    SDL_SetRenderDrawColor(m_game_window.renderer, 0, 200, 200, 255);
    drawObject(&m_player_1);
    drawObject(&m_player_2);
}

bool
Game::hasCollision(GameObject_t *a, GameObject_t *b)
{
    const float aMinX = a->rect.x;
    const float aMaxY = a->rect.y + a->rect.h;
    const float aMaxX = a->rect.x + a->rect.w;
    const float aMinY = a->rect.y;

    const float bMinX = b->rect.x;
    const float bMaxY = b->rect.y + b->rect.h;
    const float bMaxX = b->rect.x + b->rect.w;
    const float bMinY = b->rect.y;

    return !(aMaxX <= bMinX || aMinX >= bMaxX || aMaxY <= bMinY
             || aMinY >= bMaxY);
}

int
Game::randomRange(int min, int max)
{
    std::random_device rd;
    std::mt19937       gen(rd());

    std::uniform_int_distribution<size_t> distr(min, max);

    return distr(gen);
}

void
Game::calculateBallYMovement(GameObject_t *p)
{
    GameObject_t *b = &m_ball;

    const float paddel_section_h = float(p->rect.h) / 3.0f;

    const float bMinX = b->rect.x;
    const float bMinY = b->rect.y;
    const float bMaxX = b->rect.x + b->rect.w;
    const float bMaxY = b->rect.y + b->rect.h;

    const float pMinX = p->rect.x;

    const float pMaxX = p->rect.x + p->rect.w;

    // upper paddel
    {
        const float pMaxY = (p->rect.y + p->rect.h) - (paddel_section_h * 2);
        const float pMinY = p->rect.y;

        if (!((pMaxX < bMinX || pMinX > bMaxX)
              || (pMaxY < bMinY || pMinY > bMaxY))) {
            std::cout << "UPPER PADDEL HIT\n";
            if (p->move_direction == MD_DOWN) {
                m_ball_info.y_axis_direction = MD_DOWN;
            } else {
                m_ball_info.y_axis_direction = MD_UP;
            }
            return;
        }
    }

    // middle paddel
    {
        const float pMaxY = (p->rect.y + p->rect.h) - (paddel_section_h * 1);
        const float pMinY = p->rect.y + (paddel_section_h * 1);

        if (!((pMaxX < bMinX || pMinX > bMaxX)
              || (pMaxY < bMinY || pMinY > bMaxY))) {
            std::cout << "MIDDLE PADDEL HIT\n";
            switch (randomRange(0, 2)) {
                case 0:
                    m_ball_info.y_axis_direction = MD_NONE;
                    break;
                case 1:
                    m_ball_info.y_axis_direction = MD_UP;
                    break;
                case 2:
                    m_ball_info.y_axis_direction = MD_DOWN;
                    break;
            }
            return;
        }
    }

    // lower paddel
    {
        const float pMaxY = (p->rect.y + p->rect.h);
        const float pMinY = p->rect.y + (paddel_section_h * 2);

        if (!((pMaxX < bMinX || pMinX > bMaxX)
              || (pMaxY < bMinY || pMinY > bMaxY))) {
            std::cout << "LOWER PADDEL HIT\n";
            if (p->move_direction == MD_DOWN) {
                m_ball_info.y_axis_direction = MD_DOWN;
            } else {
                m_ball_info.y_axis_direction = MD_UP;
            }
            return;
        }
    }
}

void
Game::moveBall()
{
    if (hasCollision(&m_player_1, &m_ball)) {
        calculateBallYMovement(&m_player_1);
        std::cout << "PLAYER 1 HIT\n";

        m_ball_info.x_axis_direction = MD_RIGHT;
    } else if (hasCollision(&m_player_2, &m_ball)) {
        calculateBallYMovement(&m_player_2);
        std::cout << "PLAYER 2 HIT\n";
        m_ball_info.x_axis_direction = MD_LEFT;
    } else if ((m_ball.rect.x + m_ball.rect.w)
               >= (m_player_2.rect.x + m_player_2.rect.w)) {
        std::cout << "RIGHT WALL HIT\n";
        std::cout << "RIGHT_WALL(X)" << m_player_2.rect.x + m_player_2.rect.w
                  << ' ';
        std::cout << "BALL(X,Y)" << m_ball.rect.x << '\n';
        m_game_reset = true;
    } else if (m_ball.rect.x < m_player_1.rect.x) {
        std::cout << "LEFT WALL HIT\n";
        std::cout << "LEFT_WALL(X)" << m_player_1.rect.x << ' ';
        std::cout << "BALL(X,Y)" << m_ball.rect.x << '\n';
        m_game_reset = true;
    } else if (m_ball.rect.y <= 0) {
        m_ball_info.y_axis_direction = MD_DOWN;
    } else if ((m_ball.rect.y + m_ball.rect.h) >= m_game_window.window_h) {
        m_ball_info.y_axis_direction = MD_UP;
    }

    m_ball.move(m_ball_info.x_axis_direction, m_ball_speed);
    m_ball.move(m_ball_info.y_axis_direction, m_ball_speed);
}
