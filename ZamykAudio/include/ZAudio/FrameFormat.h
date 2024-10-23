#pragma once

#include <cassert>
#include <span>
#include <array>

#include <ZAudio/CommonTypes.h>


namespace ZAudio {



enum struct FrameFormat {
  None, Mono, Stereo
};

static constexpr uint32_t NumberOfFrameFormats = 2;

static inline const std::array<std::pair<FrameFormat, std::string>, NumberOfFrameFormats> frameFormatToString = {
  std::make_pair(FrameFormat::Mono, "Mono"),
  std::make_pair(FrameFormat::Stereo, "Stereo")
};


} // namespace ZAudio

namespace ZAudio::Tools {

static constexpr inline size_t MaxNumberOfChannels = 2;
void monoToStereo(std::span<const sample_t> in, std::span<sample_t> out);
void stereoToMono(std::span<const sample_t> in, std::span<sample_t> out);
size_t numberOfChannels(FrameFormat format);
void convertFrames(std::span<const sample_t> in, FrameFormat inFormat, std::span<sample_t> out, FrameFormat outFormat);


} // namespace ZAudio::Tools