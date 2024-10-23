add_executable (tests 
Tests.cpp
)


target_link_libraries(tests PUBLIC ZamykAudio)

target_include_directories(tests PRIVATE /catch)
