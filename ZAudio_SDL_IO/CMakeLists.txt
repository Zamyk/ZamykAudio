add_library(ZAudio_SDL_IO STATIC
source/SDL_IO.cpp
)

add_subdirectory(SDL)

target_include_directories(ZAudio_SDL_IO PUBLIC include)
target_link_libraries(ZAudio_SDL_IO PUBLIC ZamykAudio external SDL3::SDL3)
