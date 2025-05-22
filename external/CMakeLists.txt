
add_library(external INTERFACE)


if(ZAUDIO_USE_OWN_PUGIXML)
  # FetchContent_Declare(
  #   pugixml
  #   GIT_REPOSITORY https://github.com/zeux/pugixml.git
  #   GIT_TAG        5a1892b321a38997e3e359069aea3fa35d49a5c6
  # )
  # FetchContent_MakeAvailable(pugixml)
  add_subdirectory(pugixml)  
endif()

if(ZAUDIO_USE_OWN_FFTW3)

endif()

target_link_libraries(external INTERFACE pugixml)

# target_include_directories(external PUBLIC
# pugixml/src
# fftw3/api
# )