add_library(ZAudio_SDL_IO STATIC
source/SDL_IO.cpp
)

option(ZAUDIO_USE_OWN_SDL "Library will use fetched SDL3; if OFF, assumes SDL3 is already in the project" ON)

if(ZAUDIO_USE_OWN_SDL)
  add_subdirectory(SDL)
  # FetchContent_Declare(
  #   SDL
  #   GIT_REPOSITORY https://github.com/libsdl-org/SDL
  #   GIT_TAG        ab114490fcbb218a2794673851220f16aab6ae1c
  # )
  # FetchContent_MakeAvailable(SDL)  
endif()

target_include_directories(ZAudio_SDL_IO PUBLIC include)
target_link_libraries(ZAudio_SDL_IO PUBLIC ZamykAudio external SDL3::SDL3)