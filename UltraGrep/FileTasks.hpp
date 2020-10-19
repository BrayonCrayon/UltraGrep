#pragma once
#include <mutex>
#include <string>
#include <queue>
#include <regex>
#include "Report.hpp"

#ifndef FILE_TASKS_HPP
#define FILE_TASKS_HPP

struct FileTasks
{
	SRWLOCK& s_Lock;
	std::regex& expr;
	Report& report_;
	bool isV;
	std::mutex& c_lock;
	bool& isDirSearchDone;
	std::queue<std::wstring>& tasks;

	FileTasks (SRWLOCK& sl, std::queue<std::wstring>& t, bool v, std::regex& e, Report& r, bool& directoryDone, std::mutex& c_l) : 
		s_Lock(sl), tasks(t), isV(v), expr(e), report_(r), isDirSearchDone(directoryDone), c_lock(c_l) {}
};

#endif // !FILE_TASKS_HPP


