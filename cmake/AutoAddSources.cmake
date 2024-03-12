function(add_sources_ex target extra)
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
    target_sources(${target} PRIVATE ${sources} ${extra})
endfunction()

function(add_sources target)
    add_sources_ex(${target} "")
endfunction()