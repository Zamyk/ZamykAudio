#include <ZAudio/FrameFormat.h>

#include <cassert>
#include <span>

#include <ZAudio/CommonTypes.h>

namespace ZAudio::Tools {

void monoToStereo(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = in[0];
  out[1] = in[0];
}

void stereoToMono(std::span<const sample_t> in, std::span<sample_t> out) {
  out[0] = in[0] + in[1];  
}

size_t numberOfChannels(FrameFormat format) {
  switch (format) {
    case FrameFormat::Mono:
      return 1;    
    case FrameFormat::Stereo:
      return 2;
    default:
      assert(false);
      return 0;
  }
}

void convertFrames(std::span<const sample_t> in, FrameFormat inFormat, std::span<sample_t> out, FrameFormat outFormat) {
  if(inFormat == outFormat) {
    for(size_t i = 0; i < numberOfChannels(inFormat); i++) {
      out[i] = in[i];
    }
  }
  else {
    switch (inFormat) {
      case FrameFormat::Mono:
        monoToStereo(in, out);
        break;
      case FrameFormat::Stereo:
        stereoToMono(in, out);
        break;
      default:
        assert(false);
    }
  }
}


} // namespace ZAudio::Tools