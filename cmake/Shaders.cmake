function (add_shaders TARGET_NAME)
    set(SHADER_SOURCE_FILES ${ARGN})
    list(LENGTH SHADER_SOURCE_FILES FILE_COUNT)
    if(FILE_COUNT EQUAL 0)
        message(FATAL_ERROR "Cannot add shaders target without shader files")
        endif()

    set(SHADER_PRODUCTS)

    foreach(SHADER_SOURCE IN LISTS SHADER_SOURCE_FILES)
        cmake_path(ABSOLUTE_PATH SHADER_SOURCE NORMALIZE)
        cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)
        set(SHADER_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Shaders/${SHADER_NAME}.spv")

        add_custom_command(
            OUTPUT "${SHADER_OUTPUT}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
            COMMAND Vulkan::glslc "${SHADER_SOURCE}" -o "${SHADER_OUTPUT}"
            DEPENDS "${SHADER_SOURCE}"
            COMMENT "Compiling shader ${SHADER_NAME}"
            VERBATIM
        )

        list(APPEND SHADER_PRODUCTS "${SHADER_OUTPUT}")
    endforeach()

    add_custom_target(${TARGET_NAME} ALL
        DEPENDS ${SHADER_PRODUCTS}
        SOURCES ${SHADER_SOURCE_FILES}
    )


endfunction()