add_subdirectory(common)
add_subdirectory(hmi_task)
add_subdirectory(system_task)
add_subdirectory(packet_task)
add_subdirectory(kinematics_task)

add_library(atlas_robot STATIC)

target_sources(atlas_robot PRIVATE 
    atlas_robot.c
)

target_include_directories(atlas_robot PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(atlas_robot PUBLIC 
    common
    hmi_task
    kinematics_task
    packet_task
    system_task
    uart_task
)

target_compile_options(atlas_robot PUBLIC
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