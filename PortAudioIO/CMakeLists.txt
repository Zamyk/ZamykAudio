add_library(ZAudio_PortAudioIO STATIC
source/ZAudio_PortAudioIO.cpp
)

add_subdirectory(portaudio)
option(PA_BUILD_SHARED "Build shared/dynamic library" OFF)
target_include_directories(ZAudio_PortAudioIO PUBLIC include)
target_include_directories(ZAudio_PortAudioIO PRIVATE portaudio/include)
target_link_libraries(ZAudio_PortAudioIO PRIVATE ZamykAudio portaudio_static)