#pragma once

#include <random>
#include <iostream>
#include <optional>

#include "Bot.h"

Bot::Bot(Board& b, int d) : board_(b), difficulty_(d) {};

void Bot::Play(bool& running) {
    while (running) {
        if (board_.DidSomebodyWin()) {
            running = false;
            continue;
        }
        int x, y;
        std::tie(x, y) = MakeDecision();

        board_.Decrement(x, y, false); // true для строки

        // Проверяем, выиграл ли кто-то
        if (board_.DidSomebodyWin()) {
            running = false;
            break;
        }

        // Задержка между ходами
        std::random_device rd;
        std::mt19937 gen(rd()); // Генератор
        std::uniform_int_distribution<int> dist(1, 1000); // Равномерное распределение
        std::this_thread::sleep_for(std::chrono::seconds(4 - difficulty_) + std::chrono::milliseconds((4 - difficulty_) * dist(gen)));
    }
};

std::tuple<int, int> Bot::MakeDecision() {
    const int N = static_cast<int>(board_.GetBoard().size());
    std::vector<std::vector<int>> board;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> columns;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> rows;
    std::tie(board, columns, rows) = board_.GetAll();

    // Собираем доступные столбцы
    std::unordered_set<int> available_columns;
    for (int i = 0; i < N; i++) {
        if (columns[i] <= std::chrono::steady_clock::now()) {
            available_columns.insert(i);
        }
    }

    // Собираем доступные строки
    std::unordered_set<int> available_rows;
    for (int i = 0; i < N; i++) {
        if (rows[i] <= std::chrono::steady_clock::now()) {
            available_rows.insert(i);
        }
    }

    // Если нет доступных столбцов или строк, пропускаем ход
    if (available_columns.empty() || available_rows.empty()) {
        return std::make_tuple(0, 0);
    }

    // Выбираем столбец с минимальной суммой значений
    int min_column_index = -1;
    int min_column_sum = 100000; // Начальное значение для поиска минимума
    for (int column : available_columns) {
        int sum = 0;
        for (int y = 0; y < N; y++) {
            sum += board[y][column];
        }
        if (sum < min_column_sum) {
            min_column_index = column;
            min_column_sum = sum;
        }
    }

    // Выбираем строку с максимальной суммой значений, где значение в ячейке не равно 0
    int max_row_index = -1;
    int max_row_sum = -1;
    for (int row : available_rows) {
        if (board[row][min_column_index] == 0) {
            continue; // Пропускаем строки, где значение в выбранном столбце равно 0
        }

        int sum = 0;
        for (int x = 0; x < N; x++) {
            sum += board[row][x];
        }
        if (sum > max_row_sum) {
            max_row_index = row;
            max_row_sum = sum;
        }
    }
    // Если не удалось найти подходящую строку, пропускаем ход
    if (max_row_index == -1) {
        max_row_index = 0;
    }
    return std::make_tuple(min_column_index, max_row_index );
};
