add_library(ZAudio_FileIO STATIC
source/FileIO.cpp
)

option(ZAUDIO_USE_OWN_DR_LIBS "Library will use fetched dr_libs; if OFF, assumes dr_libs are already in the project" ON)

if(ZAUDIO_USE_OWN_DR_LIBS)
  # FetchContent_Declare(
  #   dr_libs
  #   GIT_REPOSITORY https://github.com/mackron/dr_libs
  #   GIT_TAG        9cb7092ac8c75a82b5c6ea72652ca8d0091d7ffa
  # )
  # FetchContent_MakeAvailable(dr_libs)
  add_subdirectory(dr_libs)
endif()

target_include_directories(ZAudio_FileIO PUBLIC include ${dr_libs_SOURCE_DIR})
target_link_libraries(ZAudio_FileIO PUBLIC ZamykAudio)