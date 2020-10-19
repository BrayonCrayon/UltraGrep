#include "Win32_Thread_Pool.hpp"

using namespace std;

void Win32_Thread_Pool::SetupThreads(VOID(CALLBACK *f)(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK), bool isVerbose, regex& expr, Report& report, bool& isDirDone, mutex& c_lock)
{
	for(size_t size = 0; size < si.dwNumberOfProcessors; ++size)
		threads.push_back(std::make_pair(PTP_WORK(), FileTasks(taskLock, tasks, isVerbose, expr, report, isDirDone, c_lock)));

	for (auto &thr : threads)
	{
		thr.first = CreateThreadpoolWork(f, &thr.second, &call_back_ev);
		SubmitThreadpoolWork(thr.first);
	}
}



