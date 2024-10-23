cmake_minimum_required(VERSION 3.5.1)
project(ZAudio VERSION 1.0 LANGUAGES CXX C)

include(CTest)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_definitions(-DZAUDIO_USE_FFT)

add_subdirectory(external)
add_subdirectory(ZamykAudio)
add_subdirectory(ZAudio_FileIO)
add_subdirectory(PortAudioIO)
add_subdirectory(ZAudio_SDL_IO)
add_subdirectory(examples)
add_subdirectory(CmdPlayer)

enable_testing()
add_subdirectory(tests)
add_test(NAME tests COMMAND tests)
