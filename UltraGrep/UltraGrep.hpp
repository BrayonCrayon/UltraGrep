#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <memory>
#include <regex>
#include <map>
#include <array>
#include <locale>
#include "Delimiter.hpp"
#include "Thread_Pool.hpp"
#include "Report.hpp"
#include "Win32_Thread_Pool.hpp"
#include "Constants.hpp"
#include "FileTasks.hpp"

#ifndef ULTRAGREP_HPP
#define ULTRAGREP_HPP

// Methods for Win32 Threading
VOID CALLBACK FileWorker(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK);

class UltraGrep
{
	//Command Parameter Declarations
	typedef std::shared_ptr<std::vector<std::wstring>> ultra_list;
	typedef std::map<int, std::shared_ptr<IDelimiter>> organizer_type;
	typedef std::tuple<std::wstring, UltraGrep&> task_type;
	bool isVerbose_;
	std::wstring start_Folder_;
	ultra_list ext_list_ = std::make_shared<std::vector<std::wstring>>( std::vector<std::wstring>() );
	std::regex expression_str_;
	organizer_type param_organizer;
	bool isDoneDirectorySearch = false;

	//Utility Declarations
	std::mutex cout_Lock;
	std::size_t strPos_;

	//Report
	Report ultraGrep_report_;
	LARGE_INTEGER ParsingStart, ParsingStop, Frequency;

	//File parsing
	std::experimental::filesystem::v1::path scanFile;

	std::fstream fileS;
	std::string line;
	int lineNum;

	void Setup();
	void ParsingResults();


	// Methods for C++ 11 Threading
	void DirectorySearch(std::wstring, UltraGrep&);
	void FileExtract(std::wstring, UltraGrep&);


	//Threading functions
	/*	Method Definition
		Method:		UseWinApi
		Params:		UltraGrep&
		Returns:	Void
		Purpose:	To start the Grepping with Win32 Threading
	*/
	inline void SetupWIN32(UltraGrep& u_ref)
	{
		//setup pools
		m_file_pool.SetupThreads(&FileWorker, this->isVerbose_, this->expression_str_, this->ultraGrep_report_, this->isDoneDirectorySearch, this->cout_Lock);
	}

	/*	Method Definition
		Method:		UseCppEleven
		Params:		UltraGrep&
		Returns:	Void
		Purpose:	To start the Grepping with C++ 11 Threading
	*/
	inline void SetupCPPEleven(UltraGrep& u_ref)
	{
		//setup Threads for work
		file_pool.SetupPool(&UltraGrep::FileExtract, u_ref, std::wstring(), u_ref);
	}

	/*	Method Definition
		Method:		CloseWIN32
		Params:		void
		Returns:	Void
		Purpose:	To Close remaining WIN32 Threads
	*/
	inline void CloseWIN32()
	{
		m_file_pool.~Win32_Thread_Pool();
	}

	/*	Method Definition
		Method:		CloseCPPEleven
		Params:		void
		Returns:	Void
		Purpose:	To Close remaining C++ 11 Threads
	*/
	inline void CloseCPPEleven()
	{
		//Clean wait for other threads to finish
		file_pool.wake_cond.notify_all();
		file_pool.~Thread_Pool();
	}

public:
	//Constructors
	UltraGrep() : isVerbose_(false), start_Folder_(std::wstring(L"")) 
	{
		Setup();
	}
	~UltraGrep();

	//methods
	void ExtractCommandParams(std::string);
	void StartGrep(UltraGrep&);
	
	//friend classes
	friend class ExtensionDelimiter;
	friend class PathDelimiter;
	friend class VerboseDelimiter;
	friend class ExpressionDelimiter;
private:
	//c++ 11 ThreadPool
	Thread_Pool<UltraGrep, void, std::wstring, UltraGrep&> file_pool;
	Thread_Pool<UltraGrep, void, std::wstring, UltraGrep&> dir_pool;

	////Win32 ThreadPool for File Parsing
	Win32_Thread_Pool m_file_pool;
};

#endif // !ULTRAGREP_HPP