add_subdirectory(button_task)
add_subdirectory(display_task)

add_library(ui_task STATIC)

target_sources(ui_task PRIVATE 
    ui_manager.c
    ui_task.c
    ui_screen.c
)

target_include_directories(ui_task PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ui_task PUBLIC 
    button_task
    display_task
)

target_compile_options(ui_task PUBLIC
    -std=c23
    -Wall
    -Wextra
    -Wconversion
    -Wshadow
    -Wpedantic
    -Wnarrowing
    -Waddress
    -pedantic
    -Wdeprecated
    -Wsign-conversion
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wdouble-promotion
    -Wimplicit-fallthrough
    -Wcast-align
    -Wformat=2
    -Wformat-security
    -Wmissing-prototypes
    -Wmissing-declarations
    -Wstrict-prototypes
    -Wold-style-definition
    -Wundef
    -Wvla
    -Wpointer-arith
    -Wstrict-aliasing=2
)
