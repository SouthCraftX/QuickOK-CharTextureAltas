cmake_minimum_required(VERSION 3.15)
project(QuickOK-CharTextureAltas)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(WRITE ${CMAKE_BINARY_DIR}/dummy.c "int main() { return 0; }")
add_executable(dummy ${CMAKE_BINARY_DIR}/dummy.c)

if(WIN32)
    set(DUMMY_PROGRAM_EXT ".exe")
else()
    set(DUMMY_PROGRAM_EXT "")
endif()

# --------- QuickOK-Zero ---------
if(NOT DEFINED QOZERO_SOURCE_ROOT)
    if(EXISTS ${CMAKE_SOURCE_DIR}/../QuickOK-Zero)
        set(QOZERO_SOURCE_ROOT ${CMAKE_SOURCE_DIR}/../QuickOK-Zero)
        message(STATUS "QOZERO_SOURCE_ROOT not set but QuickOK-Zero found in the parent directory")
    else()
        message(FATAL_ERROR "QOZERO_SOURCE_ROOT not set and QuickOK-Zero not found in the parent directory")
    endif()
endif()

target_include_directories(dummy PRIVATE ${QOZERO_SOURCE_ROOT}/include)
add_custom_command(
    TARGET dummy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/dummy.c
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/dummy${DUMMY_PROGRAM_EXT}
    COMMENT "Removing dummy.c and dummy${DUMMY_PROGRAM_EXT}"
)

add_library(QuickOK-CharTextureAltas INTERFACE)
target_include_directories(QuickOK-CharTextureAltas INTERFACE ${CMAKE_SOURCE_DIR}/include)