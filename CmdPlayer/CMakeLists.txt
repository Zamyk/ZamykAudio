add_executable(CmdPlayer CmdPlayer.cpp)

target_compile_features(CmdPlayer PUBLIC cxx_std_17)

target_link_libraries(CmdPlayer PRIVATE ZamykAudio ZAudio_FileIO ZAudio_SDL_IO)
