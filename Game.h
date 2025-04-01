#pragma once

#include <iostream>
#include <chrono>
#include <thread>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Board.h"
#include "Config.h"
#include "Bot/Bot.cpp"

void Hello() {
    if (freopen("/Users/romankrecetov/Desktop/SekiWithoutQueue/txt/Hello.txt", "r", stdin) == NULL) {
        throw std::runtime_error("Ошибка открытия файла Hello.txt\n");
    }
    std::string hello;
    while (std::cin >> hello) {
        std::cout << hello << " ";
    }
}

const int CELL_SIZE = 100; // Увеличенный размер ячейки в пикселях (было 50)
const int FPS = 30; // Ограничение частоты кадров

// Функция для отрисовки текста в SDL
void DrawText(SDL_Renderer *renderer, TTF_Font *font, const char *text, int x, int y, SDL_Color color) {
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// Функция для отрисовки и обработки событий
void Display(Board &board, SDL_Renderer *renderer, TTF_Font *font) {
    const int N = static_cast<int>(board.GetBoard().size()); // Размер доски
    bool running = true;
    SDL_Event event;

    // Переменные для контроля FPS
    const int frameDelay = 1000 / FPS; // Время задержки между кадрами
    Uint32 frameStart;
    int frameTime;

    Bot bot(board, DIFFICULTY);

    std::thread bot_thread(&Bot::Play, std::ref(bot), std::ref(running));


    while (running) {
        frameStart = SDL_GetTicks(); // Время начала кадра

        // Обработка событий
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                // Преобразуем координаты мыши в индексы ячейки
                int cellX = mouseX / CELL_SIZE;
                int cellY = mouseY / CELL_SIZE;

                // Проверяем, что индексы находятся в пределах доски
                if (cellX >= 0 && cellX < N && cellY >= 0 && cellY < N) {
                    // Уменьшаем число в ячейке
                    board.Decrement(cellX, cellY, true); // true для строк, false для столбцов

                    // Проверяем, выиграл ли кто-то
                    auto winner = board.DidSomebodyWin();
                    if (winner) {
                        std::cout << *winner << std::endl;
                        running = false; // Завершаем игру
                    }
                }
            }
        }

        // Очистка экрана (заливаем белым цветом)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Белый цвет
        SDL_RenderClear(renderer);

        // Отрисовка чисел и градиентного фона
        SDL_Color textColor = {0, 0, 0, 255}; // Чёрный цвет текста
        auto boardData = board.GetBoard(); // Получаем текущее состояние доски
        auto columnsTime = board.GetColumnsTime(); // Время блокировки столбцов
        auto rowsTime = board.GetRowsTime(); // Время блокировки строк

        auto now = std::chrono::steady_clock::now(); // Текущее время

        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                // Рассчитываем оставшееся время блокировки
                auto columnTime = columnsTime[j];
                auto rowTime = rowsTime[i];
                auto maxTime = std::max(columnTime, rowTime);

                if (maxTime > now) {
                    // Вычисляем процент оставшегося времени
                    auto remainingTime = std::chrono::duration_cast<std::chrono::milliseconds>(maxTime - now).count();
                    double percent = 1 - static_cast<double>(remainingTime) / (HOLDUP * 1000);

                    // Вычисляем цвет фона (градиент от красного к белому)
                    int red = 255;
                    int green = static_cast<int>(255 * percent);
                    int blue = static_cast<int>(255 * percent);

                    // Отрисовка фона ячейки
                    SDL_Rect cellRect = {
                        j * CELL_SIZE, // X
                        i * CELL_SIZE, // Y
                        CELL_SIZE, // Ширина
                        CELL_SIZE // Высота
                    };
                    SDL_SetRenderDrawColor(renderer, red, green, blue, 255); // Градиентный цвет
                    SDL_RenderFillRect(renderer, &cellRect);
                }

                // Отрисовка числа в ячейке
                char buffer[10];
                snprintf(buffer, sizeof(buffer), "%d", boardData[i][j]); // Преобразуем число в строку
                int textX = j * CELL_SIZE + CELL_SIZE / 4; // Позиция текста по X
                int textY = i * CELL_SIZE + CELL_SIZE / 4; // Позиция текста по Y
                DrawText(renderer, font, buffer, textX, textY, textColor);
            }
        }

        // Отрисовка сетки (чёрным цветом)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Чёрный цвет
        for (int i = 0; i <= N; ++i) {
            // Вертикальные линии
            SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, N * CELL_SIZE);
            // Горизонтальные линии
            SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, N * CELL_SIZE, i * CELL_SIZE);
        }

        // Обновление экрана
        SDL_RenderPresent(renderer);

        // Ограничение FPS
        frameTime = SDL_GetTicks() - frameStart; // Время, затраченное на отрисовку кадра
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime); // Задержка для стабилизации FPS
        }
    }
    bot_thread.join();
}

void game(Board &board) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("");
    }

    if (TTF_Init()) {
        std::cerr << "TTF could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        throw std::runtime_error("");
    }

    // Создаём окно и рендерер
    SDL_Window *window = SDL_CreateWindow("Board Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, N * CELL_SIZE,
                                          N * CELL_SIZE, 0);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("");
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("");
    }

    // Загружаем шрифт
    TTF_Font *font = TTF_OpenFont("/Users/romankrecetov/Desktop/SekiWithoutQueue/arial.ttf", 48);
    // Увеличенный размер шрифта (было 24)
    if (!font) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        throw std::runtime_error("");
    }

    // Запуск основного цикла отрисовки и обработки событий
    Display(board, renderer, font);

    // Освобождение ресурсов
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}
