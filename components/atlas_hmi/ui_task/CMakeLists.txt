add_subdirectory(button_task)
add_subdirectory(display_task)

add_library(ui_task INTERFACE)

target_link_libraries(ui_task INTERFACE 
    button_task
    display_task
)