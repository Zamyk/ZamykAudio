
add_library(external INTERFACE)


if(ZAUDIO_USE_OWN_PUGIXML)
  add_subdirectory(pugixml)  
endif()

if(ZAUDIO_USE_OWN_FFTW3)

endif()

target_link_libraries(external INTERFACE pugixml)

# target_include_directories(external PUBLIC
# pugixml/src
# fftw3/api
# )