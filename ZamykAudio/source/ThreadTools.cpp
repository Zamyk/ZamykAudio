#include <ZAudio/ThreadTools.h>


//#define THREADS_POSIX

#ifdef THREADS_POSIX

#include <pthread.h>

void ZAudio::ThreadTools::setHighPriority(std::thread& thread) {
  pthread_t thId = thread.native_handle();
  pthread_attr_t thAttr;
  int policy = 0;
  int maxPriority = 0;
  pthread_attr_init(&thAttr);
  pthread_attr_getschedpolicy(&thAttr, &policy);
  maxPriority = sched_get_priority_max(policy);
  pthread_setschedprio(thId, maxPriority);
  pthread_attr_destroy(&thAttr);
}

#elif THREADS_WINDOWS

#include <windows.h>

void ZAudio::ThreadTools::setHighPriority(std::thread& thread) {
  DWORD dwPriorityClass = 0;
  int nPriorityNumber = 0;
  tasks::getWinPriorityParameters(setPriority, dwPriorityClass, nPriorityNumber);
  int result = SetPriorityClass(
          reinterpret_cast<HANDLE>(mainThread.native_handle()),
          dwPriorityClass);
  if(result != 0) {
        std::cerr << "Setting priority class failed with " << GetLastError() << std::endl;
  }
  result = SetThreadPriority(
          reinterpret_cast<HANDLE>(mainThread.native_handle()),
          nPriorityNumber);
  if(result != 0) {
        std::cerr << "Setting priority number failed with " << GetLastError() << std::endl;
  }
}

#else

void ZAudio::ThreadTools::setHighPriority(std::thread& thread) {

}

#endif

