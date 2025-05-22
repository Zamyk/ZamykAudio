add_library(ZAudio_FileIO STATIC
source/FileIO.cpp
source/stb_vorbis.c
)

option(ZAUDIO_USE_OWN_DR_LIBS "Library will use fetched dr_libs; if OFF, assumes dr_libs are already in the project" ON)

if(ZAUDIO_USE_OWN_DR_LIBS)
  add_subdirectory(dr_libs)
endif()

target_include_directories(ZAudio_FileIO PUBLIC include ${dr_libs_SOURCE_DIR})
target_link_libraries(ZAudio_FileIO PUBLIC ZamykAudio)
