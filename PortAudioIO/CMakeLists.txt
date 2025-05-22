add_library(ZAudio_PortAudioIO STATIC
source/ZAudio_PortAudioIO.cpp
)

option(ZAUDIO_USE_OWN_PORTAUDIO "Library will use fetched portaudio; if OFF, assumes portaudio is already in the project" ON)

if(ZAUDIO_USE_OWN_PORTAUDIO)
  add_subdirectory(portaudio)
  # FetchContent_Declare(
  #   portaudio
  #   GIT_REPOSITORY https://github.com/PortAudio/portaudio
  #   GIT_TAG        ccd16c85e64d8c1a5462541388b6fbcaedbb1cad
  # )
  # FetchContent_MakeAvailable(portaudio)  
endif()

option(PA_BUILD_SHARED "Build shared/dynamic library" OFF)
target_include_directories(ZAudio_PortAudioIO PUBLIC include ${portaudio_SOURCE_DIR}/include)
target_link_libraries(ZAudio_PortAudioIO PRIVATE ZamykAudio portaudio)
