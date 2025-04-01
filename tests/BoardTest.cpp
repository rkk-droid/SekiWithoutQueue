#include <gtest/gtest.h>
#include "../Board.h"
#include <thread>

class BoardTest : public ::testing::Test {
protected:
    // Инициализация через список инициализации, а не через присваивание
    BoardTest() : testBoard(3, std::vector<int>(3, 1)), board(testBoard) {}

    void SetUp() override {
        // Теперь здесь нет присваивания board = Board(testBoard)
    }

    std::vector<std::vector<int>> testBoard;
    Board board;
};

// Тест инициализации
TEST_F(BoardTest, InitializationTest) {
    // Проверка размера доски
    auto currentBoard = board.GetBoard();
    ASSERT_EQ(currentBoard.size(), 3);
    ASSERT_EQ(currentBoard[0].size(), 3);

    // Проверка значений (все должны быть 1)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_EQ(currentBoard[i][j], 1);
        }
    }
}

// Тест случайной инициализации
TEST(BoardRandomTest, RandomInitialization) {
Board randomBoard(4); // Создаем случайную доску 4x4
auto board = randomBoard.GetBoard();

// Проверяем, что доска имеет правильный размер
ASSERT_EQ(board.size(), 4);
ASSERT_EQ(board[0].size(), 4);

// Проверяем, что все значения в допустимом диапазоне (1-9)
bool hasNonZero = false;
for (int i = 0; i < 4; i++) {
for (int j = 0; j < 4; j++) {
EXPECT_GE(board[i][j], 1);
EXPECT_LE(board[i][j], 9);
if (board[i][j] > 0) hasNonZero = true;
}
}

// Убеждаемся, что на доске есть хотя бы одно ненулевое значение
EXPECT_TRUE(hasNonZero);
}

// Тест метода Decrement
TEST_F(BoardTest, DecrementTest) {
// Уменьшаем значение в ячейке (0,0)
board.Decrement(0, 0, true);

auto currentBoard = board.GetBoard();
EXPECT_EQ(currentBoard[0][0], 0);

// Пытаемся уменьшить еще раз - значение не должно измениться
board.Decrement(0, 0, true);
currentBoard = board.GetBoard();
EXPECT_EQ(currentBoard[0][0], 0);
}

// Тест блокировки строк и столбцов
TEST_F(BoardTest, BlockingTest) {
// Уменьшаем значение, блокируя строку 0
board.Decrement(0, 0, true);

// Пытаемся уменьшить значение в той же строке сразу после
board.Decrement(1, 0, true);

// Значение не должно измениться из-за блокировки строки
auto currentBoard = board.GetBoard();
EXPECT_EQ(currentBoard[0][1], 1);

// Ждем окончания блокировки
std::this_thread::sleep_for(std::chrono::seconds(HOLDUP + 1));

// Теперь должны иметь возможность уменьшить значение
board.Decrement(1, 0, true);
currentBoard = board.GetBoard();
EXPECT_EQ(currentBoard[0][1], 0);
}

// Тест на проверку победы
TEST(BoardWinTest, PlayerAWins) {
// Создаем доску, где первая строка почти нулевая
std::vector<std::vector<int>> almostWinA = {
        {1, 0, 0},
        {1, 1, 1},
        {1, 1, 1}
};
Board board(almostWinA);

// Проверяем, что победителя пока нет
auto winner = board.DidSomebodyWin();
EXPECT_FALSE(winner.has_value());

// Делаем ход, который должен привести к победе игрока A
board.Decrement(0, 0, true);

// Проверяем победу
winner = board.DidSomebodyWin();
EXPECT_TRUE(winner.has_value());
EXPECT_EQ(*winner, "Игрок A выиграл");
}

TEST(BoardWinTest, PlayerBWins) {
// Создаем доску, где первый столбец почти нулевой
std::vector<std::vector<int>> almostWinB = {
        {1, 1, 1},
        {0, 1, 1},
        {0, 1, 1}
};
Board board(almostWinB);

// Проверяем, что победителя пока нет
auto winner = board.DidSomebodyWin();
EXPECT_FALSE(winner.has_value());

// Делаем ход, который должен привести к победе игрока B
board.Decrement(0, 0, false);

// Проверяем победу
winner = board.DidSomebodyWin();
EXPECT_TRUE(winner.has_value());
EXPECT_EQ(*winner, "Игрок B выиграл");
}

TEST(BoardWinTest, DrawGame) {
// Создаем доску, где одновременно будет и нулевая строка, и нулевой столбец
std::vector<std::vector<int>> drawBoard = {
        {1, 0, 0},
        {0, 1, 1},
        {0, 1, 1}
};
Board board(drawBoard);

// Делаем ход, который приведет к ничьей
board.Decrement(0, 0, true);

// Проверяем результат
auto winner = board.DidSomebodyWin();
EXPECT_TRUE(winner.has_value());
EXPECT_EQ(*winner, "Ничья");
}