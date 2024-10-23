add_library(external STATIC pugixml/src/pugixml.cpp)

add_subdirectory(fftw3)

target_link_libraries(external PUBLIC fftw3)

target_include_directories(external PUBLIC
pugixml/src                                 
fftw3/api
)






