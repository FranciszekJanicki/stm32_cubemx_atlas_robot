add_subdirectory(common)
add_subdirectory(ui_task)
add_subdirectory(system_task)
add_subdirectory(packet_task)
add_subdirectory(sd_task)
add_subdirectory(kinematics_task)

add_library(atlas_hmi STATIC)

target_sources(atlas_hmi PRIVATE 
    atlas_hmi.c
    nvic.c
    hooks.c
    syscalls.c
)

target_include_directories(atlas_hmi PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(atlas_hmi PUBLIC 
    common
    ui_task
    kinematics_task
    packet_task
    system_task
    sd_task
    uart_task
)

target_compile_options(atlas_hmi PUBLIC
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