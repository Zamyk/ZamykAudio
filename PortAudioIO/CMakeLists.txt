add_library(ZAudio_PortAudioIO STATIC
source/ZAudio_PortAudioIO.cpp
)

option(ZAUDIO_USE_OWN_PORTAUDIO "Library will use fetched portaudio; if OFF, assumes portaudio is already in the project" ON)

if(ZAUDIO_USE_OWN_PORTAUDIO)
  add_subdirectory(portaudio)  
endif()

option(PA_BUILD_SHARED "Build shared/dynamic library" OFF)
target_include_directories(ZAudio_PortAudioIO PUBLIC include ${portaudio_SOURCE_DIR}/include)
target_link_libraries(ZAudio_PortAudioIO PRIVATE ZamykAudio portaudio)
