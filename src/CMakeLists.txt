# /src/CMakeLists.txt file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set(RC_FILE "${CMAKE_SOURCE_DIR}/rc/index.html")
    set(OUT_FILE "${CMAKE_SOURCE_DIR}/src/index_html.h")

    add_custom_command(
        OUTPUT ${OUT_FILE}
        # COMMAND echo "\n\necho\n\n" # not working
        # COMMAND xxd -i ${RC_FILE} > ${OUT_FILE}
        COMMAND touch {CMAKE_SOURCE_DIR}/pupazalupa.txt
        DEPENDS ${RC_FILE}
        COMMENT "\n\n\nwriting that file\n\n\n"
        VERBATIM
    )

    list(APPEND app_sources ${OUT_FILE})
    add_custom_target(generate_index_html ALL DEPENDS ${OUT_FILE})
endif()
    
idf_component_register(SRCS ${app_sources})

add_dependencies(__idf_src generate_index_html)

set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
     ADDITIONAL_CLEAN_FILES index_html.h)

# set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
#   ADDITIONAL_CLEAN_FILES index_html.h)