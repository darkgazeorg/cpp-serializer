# ---- Dependencies ----

FIND_PACKAGE(Doxygen)
IF(NOT DOXYGEN_FOUND)
    MESSAGE(FATAL_ERROR "Doxygen is required to build the documentation.")
ENDIF()

set(
    DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/docs"
    CACHE PATH "Path for the generated Doxygen documentation"
)

set(working_dir "${PROJECT_BINARY_DIR}/docs")

configure_file("${CMAKE_SOURCE_DIR}/docs/Doxyfile.in" "${working_dir}/Doxyfile" @ONLY)

find_program(opener NAMES xdg-open start open)

SET(DOXYGEN_HTML ${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html)
	

add_custom_command(OUTPUT ${DOXYGEN_HTML}
    COMMAND "${CMAKE_COMMAND}" -E remove_directory "${DOXYGEN_OUTPUT_DIRECTORY}/html"
    COMMAND ${DOXYGEN_EXECUTABLE} ${working_dir}/Doxyfile
    SOURCES ${working_dir}/Doxyfile
    MAIN_DEPENDENCY ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in ${working_dir}/Doxyfile
    DEPENDS ${All}
)

add_custom_target(docs ALL DEPENDS ${DOXYGEN_HTML})

add_custom_target(
    docs-open
    COMMAND ${opener} "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html"
    DEPENDS ${DOXYGEN_HTML}
)
