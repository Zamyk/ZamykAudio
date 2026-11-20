#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <vector>

#include "catch/catch.hpp"
#include <ZAudio/AudioEngine.h>
#include <ZAudio/BypassEffect.h>


namespace AudioEngineTestsHelpers {

using namespace ZAudio;

constexpr Frequency TestSampleRate = Frequency::Hz(44100);

inline std::shared_ptr<std::atomic_bool> makeFlag(bool v) {
  return std::make_shared<std::atomic_bool>(v);
}

// Mono input producing a constant value while the shared `playing` flag is set.
// Sets `destroyed` (if given) in its destructor.
class FakeInput : public AudioInput {
public:
  static constexpr double OutputValue = 3.25;

  explicit FakeInput(std::shared_ptr<std::atomic_bool> playing_p, std::shared_ptr<std::atomic_bool> destroyed_p = nullptr) :
    playing(std::move(playing_p)),
    destroyed(std::move(destroyed_p)) {}

  ~FakeInput() override {
    if(destroyed) {
      destroyed->store(true);
    }
  }

  void get(std::span<sample_t> out) override {
    out[0] = playing->load() ? 1. : 0.;
  }
  void setSampleRate(Frequency) override {}
  ParameterValue getOutputValue(size_t) const override { return ParameterValue::nonInteger(OutputValue); }
  bool errorOccured() const override { return false; }
  bool isPlaying() const override { return playing->load(); }
  FrameFormat getFormat() const override { return FrameFormat::Mono; }

private:
  std::shared_ptr<std::atomic_bool> playing;
  std::shared_ptr<std::atomic_bool> destroyed;
};

// Shared state of a RecordingOutput, readable from the test thread.
// Keeps the first `capacity` non-zero samples; zero frames (nothing playing yet) are skipped
// so the result doesn't depend on how many frames the engine ran before a command arrived.
struct Recording {
  explicit Recording(size_t capacity) : samples(capacity) {}

  std::vector<sample_t> samples;
  std::atomic<size_t> count{0};
  std::atomic_bool ended{false};

  bool full() const {
    return count.load(std::memory_order_acquire) == samples.size();
  }
};

class RecordingOutput : public AudioOutput {
public:
  static constexpr double OutputValue = 42.5;

  explicit RecordingOutput(std::shared_ptr<Recording> recording_p) :
    recording(std::move(recording_p)) {}

  void send(std::span<const sample_t> in) override {
    const size_t c = recording->count.load(std::memory_order_relaxed);
    if(in[0] != 0. && c < recording->samples.size()) {
      recording->samples[c] = in[0];
      recording->count.store(c + 1, std::memory_order_release);
    }
  }
  void setSampleRate(Frequency) override {}
  ParameterValue getOutputValue(size_t) override { return ParameterValue::nonInteger(OutputValue); }
  bool errorOccured() const override { return false; }
  bool ended() const override { return recording->ended.load(); }
  FrameFormat getFormat() const override { return FrameFormat::Mono; }

private:
  std::shared_ptr<Recording> recording;
};

// Ignores its input and outputs 1, 2, 3, ... - one step per process() call.
class RampEffect : public BypassEffect {
public:
  static constexpr double OutputValue = 7.;

  RampEffect() : BypassEffect(FrameFormat::Mono, FrameFormat::Mono) {}

  void process(std::span<const sample_t>, std::span<sample_t> out) override {
    out[0] = static_cast<sample_t>(++counter);
  }
  ParameterValue getOutputValue(size_t) override { return ParameterValue::nonInteger(OutputValue); }

private:
  int64_t counter = 0;
};

// Bypass that sets `destroyed` in its destructor.
class DestructionTrackingEffect : public BypassEffect {
public:
  explicit DestructionTrackingEffect(std::shared_ptr<std::atomic_bool> destroyed_p) :
    BypassEffect(FrameFormat::Mono, FrameFormat::Mono),
    destroyed(std::move(destroyed_p)) {}

  ~DestructionTrackingEffect() override {
    destroyed->store(true);
  }

private:
  std::shared_ptr<std::atomic_bool> destroyed;
};

template<typename F>
bool waitUntil(F condition, std::chrono::milliseconds timeout = std::chrono::milliseconds(2000)) {
  const auto start = std::chrono::steady_clock::now();
  while(!condition()) {
    if(std::chrono::steady_clock::now() - start > timeout) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return true;
}

inline std::vector<sample_t> rampOf(size_t n) {
  std::vector<sample_t> v(n);
  for(size_t i = 0; i < n; i++) {
    v[i] = static_cast<sample_t>(i + 1);
  }
  return v;
}

} // namespace AudioEngineTestsHelpers


TEST_CASE("AudioEngine sends a playing input to the connected output", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  AudioEngine engine(TestSampleRate);
  auto mixer = engine.addMixer(FrameFormat::Mono);
  auto recording = std::make_shared<Recording>(16);
  auto output = engine.addOutput<RecordingOutput>(recording);
  engine.addMixerOutput(mixer, output);

  auto input = engine.addInput<FakeInput>(makeFlag(true));
  engine.play(mixer, input);

  REQUIRE(waitUntil([&] { return recording->full(); }));
  CHECK(recording->samples == std::vector<sample_t>(16, 1.));
}

TEST_CASE("AudioEngine isPlaying", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  AudioEngine engine(TestSampleRate);
  auto mixer = engine.addMixer(FrameFormat::Mono);
  auto playing = makeFlag(true);
  auto input = engine.addInput<FakeInput>(playing);

  SECTION("is false before play") {
    CHECK_FALSE(engine.isPlaying(input));
  }

  SECTION("is true after play") {
    engine.play(mixer, input);
    CHECK(engine.isPlaying(input));
  }

  SECTION("is false after stop") {
    engine.play(mixer, input);
    engine.stop(mixer, input);
    CHECK_FALSE(engine.isPlaying(input));
  }

  SECTION("is false after the input finished by itself") {
    engine.play(mixer, input);
    playing->store(false);
    CHECK_FALSE(engine.isPlaying(input));
  }

  SECTION("is false after stop when the input was played twice on the same mixer") {
    engine.play(mixer, input);
    engine.play(mixer, input);
    REQUIRE(engine.isPlaying(input));
    engine.stop(mixer, input);
    CHECK_FALSE(engine.isPlaying(input));
  }
}

TEST_CASE("AudioEngine releases finished sounds once the app dropped its handles", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  AudioEngine engine(TestSampleRate);
  auto mixer = engine.addMixer(FrameFormat::Mono);
  // finished sounds are cleaned up while the mixer is processed, which only happens when it has an output
  auto output = engine.addOutput<RecordingOutput>(std::make_shared<Recording>(1));
  engine.addMixerOutput(mixer, output);

  auto inputDestroyed = makeFlag(false);
  auto effectDestroyed = makeFlag(false);
  auto playing = makeFlag(true);
  {
    auto input = engine.addInput<FakeInput>(playing, inputDestroyed);
    auto effect = engine.addEffect<DestructionTrackingEffect>(effectDestroyed);
    engine.play(mixer, input, effect);
    REQUIRE(engine.isPlaying(input));
    playing->store(false);
  }

  SECTION("input") {
    CHECK(waitUntil([&] { return inputDestroyed->load(); }));
  }

  SECTION("effect") {
    CHECK(waitUntil([&] { return effectDestroyed->load(); }));
  }
}

TEST_CASE("AudioEngine processes a mixer connected to two outputs once per frame", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  constexpr size_t Frames = 64;

  AudioEngine engine(TestSampleRate);
  auto mixer = engine.addMixer(FrameFormat::Mono);
  auto recordingA = std::make_shared<Recording>(Frames);
  auto recordingB = std::make_shared<Recording>(Frames);
  auto outputA = engine.addOutput<RecordingOutput>(recordingA);
  auto outputB = engine.addOutput<RecordingOutput>(recordingB);
  engine.addMixerOutput(mixer, outputA);
  engine.addMixerOutput(mixer, outputB);

  auto input = engine.addInput<FakeInput>(makeFlag(true));
  auto effect = engine.addEffect<RampEffect>();
  engine.play(mixer, input, effect);

  REQUIRE(waitUntil([&] { return recordingA->full() && recordingB->full(); }));

  // every frame advances the effect by exactly one step and both outputs get that same frame
  CHECK(recordingA->samples == rampOf(Frames));
  CHECK(recordingB->samples == rampOf(Frames));
}

TEST_CASE("AudioEngine getOutputValue", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  AudioEngine engine(TestSampleRate);

  SECTION("of an input") {
    auto input = engine.addInput<FakeInput>(makeFlag(true));
    CHECK(engine.getOutputValue(input, 0).getNonInteger() == Approx(FakeInput::OutputValue));
  }

  SECTION("of an output") {
    auto output = engine.addOutput<RecordingOutput>(std::make_shared<Recording>(1));
    CHECK(engine.getOutputValue(output, 0).getNonInteger() == Approx(RecordingOutput::OutputValue));
  }

  SECTION("of an effect") {
    auto effect = engine.addEffect<RampEffect>();
    CHECK(engine.getOutputValue(effect, 0).getNonInteger() == Approx(RampEffect::OutputValue));
  }
}

TEST_CASE("AudioEngine hasEnded", "[AudioEngine]") {
  using namespace ZAudio;
  using namespace AudioEngineTestsHelpers;

  AudioEngine engine(TestSampleRate);
  auto recording = std::make_shared<Recording>(1);
  auto output = engine.addOutput<RecordingOutput>(recording);

  CHECK_FALSE(engine.hasEnded(output));
  recording->ended.store(true);
  CHECK(engine.hasEnded(output));
}
