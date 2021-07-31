cmake_minimum_required(VERSION 3.5)

project(HodEngine-Editor LANGUAGES CXX)

function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source_rel}")
    endforeach()
endfunction(assign_source_group)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "C:\\Qt\\6.0.2\\msvc2019_64")

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Network REQUIRED)

file(GLOB_RECURSE Srcs
        "${PROJECT_SOURCE_DIR}/Src/*.cpp"
        "${PROJECT_SOURCE_DIR}/Src/*.h"
		"${PROJECT_SOURCE_DIR}/Src/*.inl"
		"${PROJECT_SOURCE_DIR}/Src/*.ui"
		"${PROJECT_SOURCE_DIR}/Src/*.qrc"
        )
		
assign_source_group(${Srcs})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HodEngine-Editor
        ${Srcs}
    )
else()
    if(ANDROID)
        add_library(HodEngine-Editor SHARED
            ${Srcs}
        )
    else()
        add_executable(HodEngine-Editor
            ${Srcs}
        )
    endif()
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT HodEngine-Editor)

target_link_libraries(HodEngine-Editor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

add_custom_command(TARGET HodEngine-Editor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/bin/Qt6Cored.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/bin/Qt6Core.dll"> $<TARGET_FILE_DIR:HodEngine-Editor>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/bin/Qt6Widgetsd.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/bin/Qt6Widgets.dll"> $<TARGET_FILE_DIR:HodEngine-Editor>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/bin/Qt6Guid.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/bin/Qt6Gui.dll"> $<TARGET_FILE_DIR:HodEngine-Editor>
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/bin/Qt6Networkd.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/bin/Qt6Network.dll"> $<TARGET_FILE_DIR:HodEngine-Editor>
	
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:HodEngine-Editor>/platforms"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/plugins/platforms/qminimald.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/plugins/platforms/qminimal.dll"> "$<TARGET_FILE_DIR:HodEngine-Editor>/platforms"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/plugins/platforms/qwindowsd.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/plugins/platforms/qwindows.dll"> "$<TARGET_FILE_DIR:HodEngine-Editor>/platforms"
	
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:HodEngine-Editor>/styles"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<$<CONFIG:debug>:"${CMAKE_PREFIX_PATH}/plugins/styles/qwindowsvistastyled.dll">$<$<CONFIG:release>:"${CMAKE_PREFIX_PATH}/plugins/styles/qwindowsvistastyle.dll"> "$<TARGET_FILE_DIR:HodEngine-Editor>/styles"
)
