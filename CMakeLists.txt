cmake_minimum_required(VERSION 3.20)
project(SekiWithoutQueue)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 17)

# Указываем путь к SDL2_image
set(SDL2_IMAGE_DIR /opt/homebrew/Cellar/sdl2_image/2.8.8)
set(SDL2_TTF_DIR /opt/homebrew/Cellar/sdl2_ttf/2.24.0)

# Ищем SDL2 и SDL2_image
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED PATHS ${SDL2_IMAGE_DIR})
find_package(SDL2_ttf REQUIRED PATHS ${SDL2_TTF_DIR})

# Подключаем GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Указываем исходные файлы
set(SOURCES main.cpp)

# Создаем исполняемый файл
add_executable(SekiWithoutQueue ${SOURCES})

# Подключаем SDL2 и SDL2_image
target_link_libraries(SekiWithoutQueue PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)

# Добавляем тесты
add_executable(SekiTests
        tests/BoardTest.cpp
        tests/BotTest.cpp
        tests/TestMain.cpp
)

target_link_libraries(SekiTests PRIVATE gtest gtest_main)