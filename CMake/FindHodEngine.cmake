cmake_minimum_required(VERSION 3.10)

function(FindHodEngine)
  if (DEFINED HodEngine_DIR)
    set(Config Debug)
    find_path(HodEngine_INCLUDE_DIRS NAMES HodEngine/HodEngine.hpp HINTS ${HodEngine_DIR}/Sources)
    if (WIN32)       
      find_library(HodEngine_CORE_LIBRARY NAMES Core.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_WINDOW_LIBRARY NAMES Window.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_INPUT_LIBRARY NAMES Input.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_RENDERER_LIBRARY NAMES Renderer.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_PHYSICS_LIBRARY NAMES Physics.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_IMGUI_LIBRARY NAMES ImGui.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_EDITOR_LIBRARY NAMES Editor.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_UI_LIBRARY NAMES UI.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_GAME_LIBRARY NAMES Game.lib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
    elseif (APPLE)
      find_library(HodEngine_CORE_LIBRARY NAMES libCore.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_WINDOW_LIBRARY NAMES libWindow.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_INPUT_LIBRARY NAMES libInput.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_RENDERER_LIBRARY NAMES libRenderer.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_PHYSICS_LIBRARY NAMES libPhysics.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_IMGUI_LIBRARY NAMES libImGui.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_EDITOR_LIBRARY NAMES libEditor.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_UI_LIBRARY NAMES libUI.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_GAME_LIBRARY NAMES libGame.dylib HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
    elseif(ANDROID)
      find_library(HodEngine_CORE_LIBRARY NAMES libCore.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_WINDOW_LIBRARY NAMES libWindow.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_INPUT_LIBRARY NAMES libInput.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_RENDERER_LIBRARY NAMES libRenderer.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_PHYSICS_LIBRARY NAMES libPhysics.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_IMGUI_LIBRARY NAMES libImGui.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_GAME_LIBRARY NAMES libGame.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_APPLICATION_LIBRARY NAMES libApplication.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_AUDIO_LIBRARY NAMES libAudio.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_UI_LIBRARY NAMES libUI.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_HOD_APPLICATION_LIBRARY NAMES libHodApplication.so HINTS ${HodEngine_DIR}/build_android_x64/${Config} PATH_SUFFIXES lib bin)
    else()
      find_library(HodEngine_CORE_LIBRARY NAMES libCore.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_WINDOW_LIBRARY NAMES libWindow.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_INPUT_LIBRARY NAMES libInput.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_RENDERER_LIBRARY NAMES libRenderer.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_PHYSICS_LIBRARY NAMES libPhysics.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_IMGUI_LIBRARY NAMES libImGui.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_EDITOR_LIBRARY NAMES libEditor.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_UI_LIBRARY NAMES libUI.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
      find_library(HodEngine_GAME_LIBRARY NAMES libGame.so HINTS ${HodEngine_DIR}/build/${Config} PATH_SUFFIXES lib bin)
    endif()

    message(STATUS "HodEngine_INCLUDE_DIRS = ${HodEngine_INCLUDE_DIRS}")
    message(STATUS "HodEngine_CORE_LIBRARY = ${HodEngine_CORE_LIBRARY}")
  else()
    message(FATAL_ERROR "HodEngine_DIR is not defined")
  endif()
endfunction()
