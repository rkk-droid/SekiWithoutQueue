#include <gtest/gtest.h>
#include "../Bot/Bot.cpp"

#include <gtest/gtest.h>
#include "../Bot/Bot.cpp"

class BotTest : public ::testing::Test {
protected:
    // Инициализируем все в конструкторе через список инициализации
    BotTest() :
            testBoard({
                              {2, 1, 3},
                              {0, 2, 1},
                              {1, 0, 1}
                      }),
            board(testBoard),
            bot(new Bot(board, 2)) {}

    void SetUp() override {
        // Убираем присваивание board = Board(testBoard);
    }

    void TearDown() override {
        delete bot;
    }

    std::vector<std::vector<int>> testBoard;
    Board board;
    Bot* bot;
};

// Тест создания бота с разной сложностью
TEST(BotCreationTest, DifficultyLevels) {
std::vector<std::vector<int>> testBoard = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
};
Board board(testBoard);

Bot easyBot(board, 1);
Bot mediumBot(board, 2);
Bot hardBot(board, 3);

// Просто проверяем, что боты создались без ошибок
SUCCEED();
}

// Тест принятия решений ботом
TEST_F(BotTest, MakeDecisionTest) {
// Бот должен выбрать столбец с минимальной суммой
int x, y;
std::tie(x, y) = bot->MakeDecision();

// Проверяем, что выбран столбец 1 (с суммой 3)
EXPECT_EQ(x, 1);

// В этом столбце бот должен выбрать строку с максимальной суммой,
// где значение в данном столбце не равно 0
// В данном случае это строка 0 (сумма 6)
EXPECT_EQ(y, 0);
}

// Тест выбора хода, когда много нулей
TEST(BotDecisionTest, ZeroValuesTest) {
std::vector<std::vector<int>> zeroBoard = {
        {0, 1, 0},
        {0, 0, 1},
        {1, 0, 0}
};
Board board(zeroBoard);
Bot bot(board, 2);

int x, y;
std::tie(x, y) = bot.MakeDecision();

// Проверяем, что бот выбрал один из столбцов с минимальной суммой (0 или 2)
EXPECT_TRUE(x == 0 || x == 2);

// Если выбран столбец 0, то должна быть выбрана строка 2
if (x == 0) {
EXPECT_EQ(y, 2);
}
// Если выбран столбец 2, то должна быть выбрана строка 1
else if (x == 2) {
EXPECT_EQ(y, 1);
}
}