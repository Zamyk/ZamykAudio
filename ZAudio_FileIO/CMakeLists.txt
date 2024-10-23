add_library(ZAudio_FileIO STATIC
source/FileIO.cpp
)

target_include_directories(ZAudio_FileIO PUBLIC include dr_libs)
target_link_libraries(ZAudio_FileIO PRIVATE ZamykAudio)
