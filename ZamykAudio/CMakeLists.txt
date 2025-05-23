add_library(ZamykAudio STATIC
source/AnalogFilter.cpp
source/AudioDecoder.cpp
source/AudioDelay.cpp
source/AudioDetector.cpp
source/AudioEngine.cpp
source/AudioEncoder.cpp
source/AutoWahEffect.cpp
source/BitCrusherEffect.cpp
source/BufferDecoder.cpp
source/BufferEncoder.cpp
source/BypassEffect.cpp
source/CallbackIO.cpp
source/DelayEffect.cpp
source/DuckDelayEffect.cpp
source/DynamicsProcessorEffect.cpp
source/EffectSerializer.cpp
source/FFT.cpp
source/FilterEffect.cpp
source/FIR_Filter.cpp
source/FlangerEffect.cpp
source/FrameFormat.cpp
source/LfoWahEffect.cpp
source/LooperEffect.cpp
source/LowFrequencyOscillator.cpp
source/ModulatedDelay.cpp
source/MonoToStereoAdapter.cpp
source/ParallelEffect.cpp
source/PhaserEffect.cpp
source/PhaseShifter.cpp
source/PhaseVocoder.cpp
source/PingPongDelayEffect.cpp
source/PitchShiftEffect.cpp
source/ReverseDelayEffect.cpp
source/RobotEffect.cpp
source/SampleRateConversion.cpp
source/SequenceFilterEffect.cpp
source/SerialEffect.cpp
source/SimpleDelay.cpp
source/SoundBuffer.cpp
source/SoundCache.cpp
source/StereoChorusEffect.cpp
source/StereoFlangerEffect.cpp
source/StereoPhaserEffect.cpp
source/ThreadTools.cpp
source/TreeDatabase.cpp
source/TremoloEffect.cpp
source/TubePreampEffect.cpp
source/Spatial2dEffect.cpp
source/StringTools.cpp
source/VibratoEffect.cpp
source/WaveShapers.cpp
source/WhisperEffect.cpp
source/WindowFunction.cpp
source/VolumeControlEffect.cpp
source/Math.cpp
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
target_compile_features(ZamykAudio PUBLIC cxx_std_20)
target_link_libraries(ZamykAudio PUBLIC ZAudio_external)
target_include_directories(ZamykAudio PUBLIC include )
