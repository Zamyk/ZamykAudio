add_library(ZAudio_SDL_IO STATIC
source/SDL_IO.cpp
)

option(ZAUDIO_USE_OWN_SDL "Library will use fetched SDL3; if OFF, assumes SDL3 is already in the project" ON)

if(ZAUDIO_USE_OWN_SDL)
  add_subdirectory(SDL)
endif()

target_include_directories(ZAudio_SDL_IO PUBLIC include)
target_link_libraries(ZAudio_SDL_IO PUBLIC ZamykAudio external SDL3::SDL3)