# example usage:
# include(tools/resource_embedder.cmake)
# embed_resources(
#     ${CMAKE_CURRENT_BINARY_DIR}/include/resources.hpp
#     ${CMAKE_CURRENT_SOURCE_DIR}/resources
#     Resources
#     ${CMAKE_PROJECT_NAME}
# )
# target_include_directories(YOUR_PROJECT PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/include)

function(embed_resources header_output resource_directory namespace_name target)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/tools/resource_embedder.exe
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/tools
    COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CURRENT_SOURCE_DIR}/tools/resource_embedder.cpp -o ${CMAKE_CURRENT_BINARY_DIR}/tools/resource_embedder.exe
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/tools/resource_embedder.cpp
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Compiling resource_embedder"
)

add_custom_command(
    OUTPUT bogus_file_that_will_never_exist
    BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/include/resources.hpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/include
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/tools/resource_embedder.exe ${header_output} ${resource_directory} ${namespace_name}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/tools/resource_embedder.exe
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating embedded resources header"
)

add_custom_target(embed_resources_target ALL DEPENDS bogus_file_that_will_never_exist)
add_dependencies(${target} embed_resources_target)

endfunction()