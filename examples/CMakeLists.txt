add_executable(examples example.cpp)

target_compile_features(examples PUBLIC cxx_std_17)

target_link_libraries(examples PRIVATE ZAudio_PortAudioIO ZamykAudio ZAudio_FileIO ZAudio_SDL_IO)

include_directories(include ${SDL2_INCLUDE_DIRS})
target_link_libraries(examples PRIVATE ${SDL2_LIBRARIES}) 
