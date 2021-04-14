cmake_minimum_required(VERSION 3.17)
project(HuaweiCodeCraft2021)

set(CMAKE_CXX_STANDARD 11)

# headers directory
include_directories(include)


file(GLOB include_glob ./include/*.h)
file(GLOB source_glob ./src/*.cpp)

# ${} 与 ${} 之间不能有逗号
add_executable(HuaweiCodeCraft2021 ${source_glob} ${include_glob})