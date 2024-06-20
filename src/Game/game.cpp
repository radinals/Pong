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

SDL_Window   *Game::GameWindow::window   = nullptr;
SDL_Renderer *Game::GameWindow::renderer = nullptr;

const Game::Color_t Game::m_clr_Black = Game::Color_t(0, 0, 0, 255);
const Game::Color_t Game::m_clr_White = Game::Color_t(255, 255, 255, 255);

Game::Color_t Game::GameWindow::bg_Color = Game::m_clr_Black;
Game::Color_t Game::GameWindow::fg_Color = Game::m_clr_White;

Game::Entity_t Game::m_player_1, Game::m_player_2, Game::m_ball;

bool Game::m_game_reset = false, Game::m_vs_com = false;

size_t Game::m_player_1_points = 0, Game::m_player_2_points = 0;

Game::Game()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
        GameWindow::window   = SDL_CreateWindow(GameWindow::window_title,
                                              SDL_WINDOWPOS_UNDEFINED,
                                              SDL_WINDOWPOS_UNDEFINED,
                                              GameWindow::window_w,
                                              GameWindow::window_h,
                                              SDL_WINDOW_SHOWN);
        GameWindow::renderer = SDL_CreateRenderer(GameWindow::window,
                                                  -1,
                                                  SDL_RENDERER_ACCELERATED);
    } else {
        throw std::runtime_error("Game::Game(): Failed to Create Window");
    }

    // initialize entities colors
    m_player_1.color = m_player_2.color = m_ball.color = m_clr_White;

    initEntities();
}

Game::~Game()
{
    SDL_DestroyRenderer(GameWindow::renderer);
    SDL_DestroyWindow(GameWindow::window);
    SDL_Quit();
}

// game loop
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

        SDL_RenderPresent(GameWindow::renderer);
        SDL_Delay(33);

        if (m_player_2_points >= Letters::LETTER_AMOUNT - 1
            || m_player_1_points >= Letters::LETTER_AMOUNT - 1) {
            m_player_1_points = m_player_2_points = 0;
            SDL_Delay(2000);
            initEntities();
            continue;
        }
    }

    return -1;
}

// set the starting values
void
Game::initEntities()
{
    m_ball.setYDirection(MD_NONE);
    m_ball.setXDirection(MD_LEFT);

    m_player_1.move_direction = m_player_2.move_direction = MD_NONE;

    m_ball.w = m_ball.h = GameWindow::window_h * 0.025f;
    m_ball.y            = (GameWindow::window_h * 0.5f) - (m_ball.h * 0.5f);
    m_ball.x            = (GameWindow::window_w * 0.5f) - (m_ball.w * 0.5f);

    m_player_1.w = m_player_2.w = (m_ball.w * 1.1f);
    m_player_1.h = m_player_2.h = (m_ball.h * 8);

    m_player_1.x = m_ball.w * 2;
    m_player_1.y = (GameWindow::window_h * 0.5f) - (m_player_1.h * 0.5f);

    m_player_2.x = (GameWindow::window_w - m_player_2.w) - m_ball.w * 2;
    m_player_2.y = (GameWindow::window_h * 0.5f) - (m_player_2.h * 0.5f);

    m_player_1.move_speed = m_player_2.move_speed = m_player_1.h * 0.5f;

    m_ball.move_speed = m_ball.w * 0.9f;
    m_ball.velocity   = 0;
}

// draw the score numbers on the game
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
            if (score < Letters::LETTER_AMOUNT
                && Letters::letters[score][i][j]) {
                SDL_RenderFillRect(GameWindow::renderer, &p);
            }
            p.x += pixel_size;
        }
        p.y += pixel_size;
    }
}

// draw thins behind the main game entity
void
Game::drawBackground()
{
    SDL_SetRenderDrawColor(GameWindow::renderer,
                           GameWindow::bg_Color.r,
                           GameWindow::bg_Color.g,
                           GameWindow::bg_Color.b,
                           GameWindow::bg_Color.a);
    SDL_RenderClear(GameWindow::renderer);

    SDL_SetRenderDrawColor(GameWindow::renderer,
                           GameWindow::fg_Color.r,
                           GameWindow::fg_Color.g,
                           GameWindow::fg_Color.b,
                           GameWindow::fg_Color.a);

    SDL_Rect separator;
    separator.h = GameWindow::window_h;
    separator.w = GameWindow::window_w * 0.01f;
    separator.x = (GameWindow::window_w * 0.5f) - (separator.w * 0.5f);
    separator.y = GameWindow::window_h - separator.h;

    SDL_RenderFillRect(GameWindow::renderer, &separator);

    const int px      = separator.w;
    const int spacing = separator.w * 4;

    drawScore(m_player_1_points,
              px,
              (separator.x + (separator.w * 0.5f))
                  - ((Letters::LETTER_W * px) + spacing));
    drawScore(m_player_2_points,
              px,
              (separator.x + (separator.w * 0.5f)) + spacing);
}

// draw a single entity
void
Game::drawEntity(const Entity_t &entity)
{
    const SDL_Rect r = entity.toRect();
    SDL_SetRenderDrawColor(GameWindow::renderer,
                           entity.color.r,
                           entity.color.g,
                           entity.color.b,
                           entity.color.a);
    SDL_RenderFillRect(GameWindow::renderer, &r);
}

// draw the games' entities
void
Game::drawEntities()
{
    drawEntity(m_ball);
    drawEntity(m_player_1);
    drawEntity(m_player_2);
}

// check collision between two entities (AABB)
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

// move a entity
void
Game::moveEntity(Entity_t *const entity)
{
    switch (entity->move_direction) {
        case MD_NONE:
            return;
        case MD_LEFT:
            entity->x -= entity->move_speed + entity->velocity;
            break;
        case MD_RIGHT:
            entity->x += entity->move_speed + entity->velocity;
            break;
        case MD_UP:
            entity->y -= entity->move_speed + entity->velocity;
            break;
        case MD_DOWN:
            entity->y += entity->move_speed + entity->velocity;
            break;
        case (MD_LEFT | MD_UP):
            entity->x -= entity->move_speed + entity->velocity;
            entity->y -= entity->move_speed + entity->velocity;
            break;
        case (MD_LEFT | MD_DOWN):
            entity->x -= entity->move_speed + entity->velocity;
            entity->y += entity->move_speed + entity->velocity;
            break;
        case (MD_RIGHT | MD_UP):
            entity->x += entity->move_speed + entity->velocity;
            entity->y -= entity->move_speed + entity->velocity;
            break;
        case (MD_RIGHT | MD_DOWN):
            entity->x += entity->move_speed + entity->velocity;
            entity->y += entity->move_speed + entity->velocity;
            break;
        default:
            return;
    }

    if (entity->y < 0) {
        entity->y = 0;
    } else if ((entity->y + entity->h) >= GameWindow::window_h) {
        entity->y = GameWindow::window_h - entity->h;
    }
}

// move  player 1/2
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
        if (m_player_2.y < 0) {
            m_player_2.y = 0;
        } else if ((m_player_2.y + m_player_2.h) >= GameWindow::window_h) {
            m_player_2.y = GameWindow::window_h - m_player_2.h;
        }
    }
}

// set the correct deflection direction and speed
void
Game::calculateBallDeflection(const Entity_t &paddle)
{
    if ((paddle.move_direction >> 2) != MD_NONE) {
        m_ball.setYDirection(MoveDirections((paddle.move_direction >> 2) << 2));
        return;
    }

    const int paddle_center_y = paddle.y + (paddle.h * 0.5f),
              paddle_y_area = paddle.h * ((float) 1 / 3), y_speed = 0;

    const int ball_max = m_ball.y, ball_min = m_ball.y + m_ball.h;

    const int paddle_middle_max = paddle_center_y - (paddle_y_area * 0.5f),
              paddle_middle_min = paddle_center_y + (paddle_y_area * 0.5f);

    const int paddle_upper_max = paddle_middle_max - paddle_y_area,
              paddle_upper_min = paddle_middle_max;

    const int paddle_lower_max = paddle_middle_min,
              paddle_lower_min = paddle_middle_min + paddle_y_area;

    if (ball_min >= paddle_lower_max || ball_max >= paddle_lower_min) {
        m_ball.setYDirection(MD_DOWN);
    } else if (ball_min >= paddle_middle_max || ball_max >= paddle_middle_min) {
        m_ball.setYDirection(MD_NONE);
    } else if (ball_min >= paddle_upper_max || ball_max >= paddle_upper_min) {
        m_ball.setYDirection(MD_UP);
    }

    m_ball.velocity = m_ball.move_speed * 1.1f;
}

// handles the ball movement, deflection, and bounds checking
void
Game::moveBall()
{
    if (hasCollision(m_player_1, m_ball)) {
        calculateBallDeflection(m_player_1);
        m_ball.setXDirection(MD_RIGHT);
    } else if (hasCollision(m_player_2, m_ball)) {
        calculateBallDeflection(m_player_2);
        m_ball.setXDirection(MD_LEFT);
    } else if ((m_ball.x + m_ball.w) >= (m_player_2.x + m_player_2.w)) {
        m_player_1_points++;
        m_game_reset = true;
    } else if (m_ball.x < m_player_1.x) {
        m_player_2_points++;
        m_game_reset = true;
    } else if (m_ball.y <= 0) {
        m_ball.setYDirection(MD_DOWN);
    } else if ((m_ball.y + m_ball.h) >= GameWindow::window_h) {
        m_ball.setYDirection(MD_UP);
    }

    moveEntity(&m_ball);

    if (m_ball.velocity > 0) { m_ball.velocity--; }
}
