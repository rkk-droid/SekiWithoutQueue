#pragma once

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>
#include <optional>
#include <mutex>
#include <tuple>

#include "Config.h"

class Board {
public:
    // Конструкторы
    // // Случайная доска
    Board(int N) {
        std::random_device rd;
    	std::mt19937 gen(rd()); // Генератор
    	std::uniform_int_distribution<int> dist(1, 9); // Равномерное распределение
        board_ = std::vector<std::vector<int>>(N, std::vector<int>(N, 0));

		for (int y = 0; y < N; ++y) {
			for (int x = 0; x < N; ++x) {
				board_[y][x] = dist(gen);
			}
		}

        auto t = std::chrono::steady_clock::now() - std::chrono::seconds(HOLDUP);
		columns_time_ = std::vector<std::chrono::time_point<std::chrono::steady_clock>>(N, t);
        rows_time_ = std::vector<std::chrono::time_point<std::chrono::steady_clock>>(N, t);
    }

    // // Доска по данной
  	explicit Board(std::vector<std::vector<int>>& b) : board_(b) {
		auto t = std::chrono::steady_clock::now() - std::chrono::seconds(HOLDUP);
		columns_time_ = std::vector<std::chrono::time_point<std::chrono::steady_clock>>(b.size(), t);
        rows_time_ = std::vector<std::chrono::time_point<std::chrono::steady_clock>>(b.size(), t);
  	}
	//  // Доска по другой доске
	Board(const Board& b) : board_(b.board_), columns_time_(b.columns_time_), rows_time_(b.rows_time_) {};

    // Методы
    // // Получить всю доску
	std::tuple<std::vector<std::vector<int>>,
	           std::vector<std::chrono::time_point<std::chrono::steady_clock>>,
	           std::vector<std::chrono::time_point<std::chrono::steady_clock>>> GetAll() {
		std::lock_guard<std::mutex> lock(mutex_);
		return {board_, columns_time_, rows_time_};
	}

	// // Получить доску
	const std::vector<std::vector<int>> GetBoard() {
		std::lock_guard<std::mutex> lock(mutex_);
        return board_;
    }

    // // Получить времена по столбцам
	const std::vector<std::chrono::time_point<std::chrono::steady_clock>> GetColumnsTime() {
		std::lock_guard<std::mutex> lock(mutex_);
        return columns_time_;
    }

	// // Получить времена по строкам
	const std::vector<std::chrono::time_point<std::chrono::steady_clock>> GetRowsTime() {
		std::lock_guard<std::mutex> lock(mutex_);
        return rows_time_;
    }

    // // Изменить ячейку
	void Decrement(int x, int y, bool rows = true) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (std::max(columns_time_[x], rows_time_[y]) <= std::chrono::steady_clock::now()) {
            if (board_[y][x]) {
                --board_[y][x];
                if (rows) {
                    rows_time_[y] = std::chrono::steady_clock::now() + std::chrono::seconds(HOLDUP);
                } else {
                    columns_time_[x] = std::chrono::steady_clock::now() + std::chrono::seconds(HOLDUP);
                }
            }
		}
	}

    // // Выиграл ли кто-то
    std::optional<std::string> DidSomebodyWin() {
		std::lock_guard<std::mutex> lock(mutex_);
		std::optional<std::string> result;
        for (int x = 0; x < board_.size(); ++x) {
            bool flag = true;
        	for (int y = 0; y < board_.size(); ++y) {
				if (board_[y][x]) {
                    flag = false;
                    break;
				}
        	}
            if (flag) {
                result = "Игрок B выиграл";
                break;
            }
        }
        for (int y = 0; y < board_.size(); ++y) {
            bool flag = true;
        	for (int x = 0; x < board_.size(); ++x) {
				if (board_[y][x]) {
                    flag = false;
                    break;
				}
        	}
            if (flag) {
              	if (result) {
                    result = "Ничья";
              	} else {
                    result = "Игрок A выиграл";
                }
                break;
            }
        }
        return result;
    }

	void Print() const {
		for (int y = 0; y < board_.size(); ++y) {
			for (int x = 0; x < board_.size(); ++x) {
				std::cout << board_[y][x] << " ";
			}
			std::cout << "\n";
		}
	}

private:
    std::vector<std::vector<int>> board_;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> columns_time_;
    std::vector<std::chrono::time_point<std::chrono::steady_clock>> rows_time_;
    std::mutex mutex_;
};

