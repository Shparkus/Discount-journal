cmake_minimum_required(VERSION 3.16)
project(discount_journal C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(record_array_list STATIC
    lib/record_array_list.c
    lib/journal.c
    lib/ui.c
)

target_include_directories(record_array_list PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_executable(discount_journal bin/main.c)
target_link_libraries(discount_journal PRIVATE record_array_list)