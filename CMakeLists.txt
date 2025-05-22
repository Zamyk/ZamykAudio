cmake_minimum_required(VERSION 3.5.1)
project(ZAudio VERSION 1.0 LANGUAGES CXX C)

include(CTest)
include(FetchContent)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -wall")



option(ZAUDIO_USE_OWN_PUGIXML "Library will use fetched PugiXML; if OFF, assumes PugiXML is already in the project" ON)
option(ZAUDIO_USE_OWN_FFTW3 "Library will use fetched FFTW3; if OFF, assumes FFTW3 is already in the project(or enable fft is off)" ON)

option(ZAUDIO_USE_ZAUDIO_SDL_IO "Enable the ZAudio_SDL_IO part of library" ON)
option(ZAUDIO_USE_ZAUDIO_PORTAUDIO_IO "Enable the PortAudioIO part of library" ON)
option(ZAUDIO_USE_ZAUDIO_FILE_IO "Enable the ZAudio_FileIO part of library" ON)
option(ZAUDIO_ENABLE_FFT "Enable the parts that require fft, require fftw3 library" OFF)
option(ZAUDIO_BUILD_EXAMPLES "Will add examples target" OFF)
option(ZAUDIO_BUILD_CMD_PLAYER "Will add cmd-player example target" OFF)

if(ZAUDIO_ENABLE_FFT)
  add_definitions(-DZAUDIO_USE_FFT)
endif()

# now add if BUILD_EXAMPLES and then add the examples and CmdPlayer and also make sure the SD:_IO and FILE_IO are then used

add_subdirectory(external)
add_subdirectory(ZamykAudio)

if(ZAUDIO_USE_ZAUDIO_FILE_IO)
  add_subdirectory(ZAudio_FileIO)
endif()

if(ZAUDIO_USE_ZAUDIO_PORTAUDIO_IO)  
  add_subdirectory(PortAudioIO)
endif()

if(ZAUDIO_USE_ZAUDIO_SDL_IO)
  add_subdirectory(ZAudio_SDL_IO)
endif()

if(ZAUDIO_BUILD_EXAMPLES AND (NOT ZAUDIO_USE_ZAUDIO_FILE_IO OR NOT ZAUDIO_USE_ZAUDIO_SDL_IO OR NOT ZAUDIO_USE_ZAUDIO_PORTAUDIO_IO))
    message(WARNING "BUILD_EXAMPLES requires USE_ZAUDIO_FILE_IO, USE_ZAUDIO_SDL_IO and USE_ZAUDIO_PORTAUDIO_IO to be enabled. Disabling BUILD_EXAMPLES.")
    set(ZAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "Will add examples target" FORCE)
endif()

if(ZAUDIO_BUILD_CMD_PLAYER AND (NOT ZAUDIO_USE_ZAUDIO_FILE_IO OR NOT ZAUDIO_USE_ZAUDIO_SDL_IO))
    message(WARNING "ZAUDIO_BUILD_CMD_PLAYER requires both USE_ZAUDIO_FILE_IO and USE_ZAUDIO_SDL_IO to be enabled. Disabling BUILD_EXAMPLES.")
    set(ZAUDIO_BUILD_EXAMPLES OFF CACHE BOOL "Will add examples target" FORCE)
endif()

if(ZAUDIO_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(ZAUDIO_BUILD_CMD_PLAYER)
  add_subdirectory(CmdPlayer)
endif()

enable_testing()
add_subdirectory(tests)
add_test(NAME tests COMMAND tests)
