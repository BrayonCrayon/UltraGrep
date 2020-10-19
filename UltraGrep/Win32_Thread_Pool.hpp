#pragma once
#include <Windows.h>
#include <mutex>
#include <vector>
#include <memory>
#include <queue>
#include <Windows.h>
#include <thread>
#include "FileTasks.hpp"
#include "Report.hpp"
#include <regex>
#include <string>


#ifndef WIN32_THREAD_POOL_HPP
#define WIN32_THREAD_POOL_HPP

constexpr int CACHE_LINE_SIZE = 64;


class Win32_Thread_Pool
{
	//types
	typedef std::vector<std::pair<PTP_WORK, FileTasks>> thread_type;
	
	//thread pool definitions
	SRWLOCK taskLock;
	TP_CALLBACK_ENVIRON call_back_ev;

	SYSTEM_INFO si;
	thread_type threads;

	std::queue<std::wstring> tasks;

	friend class UltraGrep;
public:
	//Constructors to set up the ThreadPool Environment
	Win32_Thread_Pool()
	{
		InitializeSRWLock(&taskLock);
		InitializeThreadpoolEnvironment(&call_back_ev);
		GetSystemInfo(&si);
	}
	//Deconstructor deconstruct the threadpool environment
	~Win32_Thread_Pool()
	{
		for (auto& thr : threads)
		{
			WaitForThreadpoolWorkCallbacks(thr.first, FALSE);
			CloseThreadpoolWork(thr.first);
		}
		DestroyThreadpoolEnvironment(&call_back_ev);
	}

	/*	Method Definition :: Reference => (ref)
		Method:		SetupThreads
		Params:		WIN32 CALLBACK Function, Bool, Regex at ref, Report object at ref, bool at ref, mutex for console lock at ref
		Returns:	Void
		Purpose:	To setup threads with the length of the number of processors you contain on your system. Submit and Create the 
						Callback function of where each thread will run on.
	*/
	void SetupThreads(VOID(CALLBACK *f)(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK), bool, std::regex&, Report&, bool&, std::mutex&);

	/*	Method Definition
		Method:		AddTask
		Params:		Wstring
		Returns:	Void
		Purpose:	Push a task that contains the Path to a file.
	*/
	void AddTask(std::wstring t)
	{
		tasks.push(t);
	}
};


#endif // !WIN32_THREAD_POOL_HPP
