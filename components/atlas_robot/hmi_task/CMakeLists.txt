add_subdirectory(button_task)
add_subdirectory(display_task)

add_library(hmi_task STATIC)

target_sources(hmi_task PRIVATE 
    hmi_manager.c
    hmi_task.c
    hmi_screen.c
)

target_include_directories(hmi_task PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(hmi_task PUBLIC 
    button_task
    display_task
)

target_compile_options(hmi_task PUBLIC
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
