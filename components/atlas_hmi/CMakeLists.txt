add_subdirectory(common)

add_library(atlas_hmi INTERFACE)

target_link_libraries(atlas_hmi INTERFACE 
    common
)
