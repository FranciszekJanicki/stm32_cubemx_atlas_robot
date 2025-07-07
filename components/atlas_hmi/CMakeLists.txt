add_subdirectory(common)
add_subdirectory(ui_task)
add_subdirectory(system_task)
add_subdirectory(packet_task)
add_subdirectory(sd_task)
add_subdirectory(kinematics_task)

add_library(atlas_hmi INTERFACE)

target_link_libraries(atlas_hmi INTERFACE 
    common
    ui_task
    kinematics_task
    packet_task
    system_task
    sd_task
)
