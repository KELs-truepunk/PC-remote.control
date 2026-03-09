cmake_minimum_required(VERSION 3.10)
project(client_gui C)

set(CMAKE_C_STANDARD 11)

# Поиск pkg-config
find_package(PkgConfig REQUIRED)

# Поиск GTK4
pkg_search_module(GTK4 REQUIRED gtk4)

# Включение заголовков
include_directories(${GTK4_INCLUDE_DIRS})

add_executable(client_gui client_gui.c)

# Линковка библиотек
target_link_libraries(client_gui PRIVATE ${GTK4_LIBRARIES})
target_compile_options(client_gui PRIVATE ${GTK4_CFLAGS_OTHER})
