# Exercise the actual application implementations without changing their API.
# Compile support once for the separate, process-isolated regression programs.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
if(NOT TARGET Qt6::QOffscreenIntegrationPlugin)
    message(FATAL_ERROR "Regression tests require the Qt offscreen platform plugin")
endif()
get_target_property(regression_sources Crosimage SOURCES)
list(REMOVE_ITEM regression_sources main.cpp)
list(TRANSFORM regression_sources PREPEND "${PROJECT_SOURCE_DIR}/Crosimage/")
add_library(regression_support OBJECT ${regression_sources})
target_include_directories(regression_support PUBLIC "${PROJECT_SOURCE_DIR}/Crosimage")
target_compile_definitions(regression_support PUBLIC CROSIMAGE_PROJECT_ROOT="${PROJECT_SOURCE_DIR}")
target_link_libraries(regression_support PUBLIC
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql Qt6::Svg Qt6::Xml
)
target_precompile_headers(regression_support PRIVATE "${PROJECT_SOURCE_DIR}/Crosimage/pch.h")
set_target_properties(regression_support PROPERTIES FOLDER "Tests")
if(MSVC)
    target_compile_options(regression_support PRIVATE /utf-8 /MP)
endif()

foreach(regression IN ITEMS worker view storage_sort)
    add_executable(${regression}_regression ${regression}_regression.cpp)
    target_link_libraries(${regression}_regression PRIVATE regression_support)
    set_target_properties(${regression}_regression PROPERTIES
        FOLDER "Tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>"
    )
    if(MSVC)
        target_compile_options(${regression}_regression PRIVATE /utf-8)
    endif()
    # The app's deployment step supplies the Qt runtime in the shared bin dir.
    add_dependencies(${regression}_regression Crosimage)
    if(regression STREQUAL "worker" OR regression STREQUAL "view")
        add_test(NAME ${regression}_regression COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/run_regression.py"
            "$<TARGET_FILE:${regression}_regression>")
    else()
        add_test(NAME ${regression}_regression COMMAND ${regression}_regression)
    endif()
    set_tests_properties(${regression}_regression PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_FORCE_STDERR_LOGGING=1;QT_QPA_PLATFORM_PLUGIN_PATH=$<TARGET_FILE_DIR:Qt6::QOffscreenIntegrationPlugin>"
        TIMEOUT 300
    )
endforeach()
