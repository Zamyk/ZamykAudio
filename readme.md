# ZamykAudio

# About
This library is a personal project created to explore audio programming, with a particular focus on audio effects. Most of the algorithms used in this library are based on the book Designing Audio Effects Plugins in C++ by Will Pirkle.
Main library is located in ZamykAudio directory.

# Documentation
For detailed documentation, please refer to the doc.md file.

# Examples

There are few examples present in the examples/ directory. \
There are examples of effects and their results in the EffectExamples directory. \
There is small CLI audio player example in CmdPlayer directory.

## Building

There are CMAKE options which should be set depending on which parts are to be build:
``` CMAKE
option(ZAUDIO_USE_ZAUDIO_SDL_IO "Enable the ZAudio_SDL_IO part of library" ON)
option(ZAUDIO_USE_ZAUDIO_PORTAUDIO_IO "Enable the PortAudioIO part of library" ON)
option(ZAUDIO_USE_ZAUDIO_FILE_IO "Enable the ZAudio_FileIO part of library" ON)
option(ZAUDIO_ENABLE_FFT "Enable the parts that require fft, require fftw3 library" OFF)
option(ZAUDIO_BUILD_EXAMPLES "Will add examples target" OFF)
option(ZAUDIO_BUILD_CMD_PLAYER "Will add cmd-player example target" OFF)
```

Now after setting these flags, there options for dependencies:

- for the ZAUDIO_SDL_IO either set this option ON and use git submodule to get the SDL sources, or add the subdirectory in the parent project.
```CMAKE 
option(ZAUDIO_USE_OWN_SDL "Library will use fetched SDL3; if OFF, assumes SDL3 is already in the project" ON)
```

- for the PORTAUDIO_IO either set this option ON and use git submodule to get the portaudio sources, or add the subdirectory in the parent project.
``` CMAKE
option(ZAUDIO_USE_OWN_PORTAUDIO "Library will use fetched portaudio; if OFF, assumes portaudio is already in the project" ON)
```

- for the ZAUDIO_FileIO either set this option ON and use git submodule to get the dr_libs sources, or add the subdirectory in the parent project.
``` CMAKE
option(ZAUDIO_USE_OWN_DR_LIBS "Library will use fetched dr_libs; if OFF, assumes dr_libs are already in the project" ON)
```

- pugixml is required for the main part of the library, either set this option ON and use git submodule to get pugixml sources, or add the pugixml subdirectory somewehre in the parent project.
``` CMAKE
option(ZAUDIO_USE_OWN_PUGIXML "Library will use fetched PugiXML; if OFF, assumes PugiXML is already in the project" ON)
```

- fftw is required if ZAUDIO_ENABLE_FFT is used(which is required for few effects). There is no submodule for the fftw3, so either download it manually to external/fftw3 and set option to ON or add fftw3 somewhere in the parent project.
``` CMAKE
option(ZAUDIO_USE_OWN_FFTW3 "Library will use fetched FFTW3; if OFF, assumes FFTW3 is already in the project(or enable fft is off)" ON)
```

After dealing with dependencies the library and examples (if on) should be ready to use.