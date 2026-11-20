#include <ZAudio/ThreadTools.h>

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

bool ZAudio::ThreadTools::setCurrentThreadHighPriority() {
  return SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) != 0;
}

#elif defined(__APPLE__)

#include <pthread.h>
#include <pthread/qos.h>

bool ZAudio::ThreadTools::setCurrentThreadHighPriority() {
  return pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0) == 0;
}

#elif defined(__ANDROID__) || defined(__linux__)

#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

bool ZAudio::ThreadTools::setCurrentThreadHighPriority() {
  const pid_t tid = static_cast<pid_t>(syscall(SYS_gettid));
  return setpriority(PRIO_PROCESS, static_cast<id_t>(tid), -16) == 0;
}

#else

bool ZAudio::ThreadTools::setCurrentThreadHighPriority() {
  return false;
}

#endif
