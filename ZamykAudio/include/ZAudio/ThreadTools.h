#pragma once

#include <thread>


namespace ZAudio::ThreadTools {


void setHighPriority(std::thread& thread);


} // namespace ZAudio::ThreadTools

