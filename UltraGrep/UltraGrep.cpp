/*
	Program Name:	Brayon's Grepper
	Programmer:		Brayon Alan Deroy
	Created:		10/3/2017
	it's Purpose:	A program to launch multiple processes based on certain localgroups.
					Each Local group has one or many processes to run concurrently, and runs each group serially.
					Output's process information from each groups ran processes.
*/
#include <iostream>
#include "ExpressionDelimiter.hpp"
#include "ExtensionDelimiter.hpp"
#include "PathDelimiter.hpp"
#include "VerboseDelimiter.hpp"

#define _CRTDBG_MAP_ALLOC  
#include <stdlib.h>  
#include <crtdbg.h> 

using namespace std;
using namespace experimental::filesystem::v1;

//Deconstructor
// Clean up the Delimiter Organiser
UltraGrep::~UltraGrep()
{
	for (organizer_type::iterator it = this->param_organizer.begin(); it != this->param_organizer.end(); ++it)
		it->second.reset();

	this->param_organizer.clear();
	this->ext_list_->clear();
	this->ext_list_.reset();
}

/*	Method Definition
	Method:		Setup
	Params:		void
	Returns:	Void
	Purpose:	To setup the some deffinitions and Delimiter processing
*/
void UltraGrep::Setup()
{
	QueryPerformanceFrequency(&Frequency);
	this->isVerbose_ = false;

	this->param_organizer[1] = shared_ptr<ExpressionDelimiter>(new ExpressionDelimiter(*this));
	this->param_organizer[2] = shared_ptr<VerboseDelimiter>(new VerboseDelimiter(*this));
	this->param_organizer[3] = shared_ptr<PathDelimiter>(new PathDelimiter(*this));
	this->param_organizer[4] = shared_ptr<ExtensionDelimiter>(new ExtensionDelimiter(*this));
}

/*	Method Definition
	Method:		ExtractCommandParams
	Params:		String
	Returns:	Void
	Purpose:	To extract the params and set them for the arguments of the program
*/
void UltraGrep::ExtractCommandParams(string arg)
{

	for (auto delimiter : param_organizer)
		delimiter.second->execute(wstring(arg.begin(), arg.end()));
}

/*	Method Definition
	Method:		StartGrep
	Params:		UltraGrep&
	Returns:	Void
	Purpose:	Add threads to all thread pools, and start the grepping at the starting
					directory given. Then wait for all directory threads, and file threads
					to finish and display results.
*/
void UltraGrep::StartGrep(UltraGrep& u_ref)
{
	//start Directory Searching with notifying one thread
	cout << "\n\t\t\t\tBrayon's Grepper" << endl;
	cout << "\n\t\tParsing: " << this->start_Folder_ << endl;
	cout << setw(90) << setfill('_') << "" << endl;

	if (!exists(path(this->start_Folder_)))
	{
		cout << "Directory Does not Exist!" << endl;
		return;
	}

	//setup Directory Pool
	dir_pool.SetupPool(&UltraGrep::DirectorySearch, u_ref, wstring(), u_ref);
	this->isDoneDirectorySearch = true;

	//if extension list is empty then add default extension lookup.
	if (this->ext_list_->empty())
		this->ext_list_->push_back(L".txt");

	//Add task to Directory pool and Start Grepping
	dir_pool.add_task(&UltraGrep::DirectorySearch, u_ref, this->start_Folder_, u_ref);

	(WIN32_THREADING_) ? SetupWIN32(u_ref) : SetupCPPEleven(u_ref);
	QueryPerformanceCounter(&ParsingStart);
	dir_pool.wake_cond.notify_one();

	//clean up directory threads
	dir_pool.~Thread_Pool();
	QueryPerformanceCounter(&ParsingStop);
	this->isDoneDirectorySearch = false;

	(WIN32_THREADING_) ? CloseWIN32() : CloseCPPEleven();

	//Report the results
	ultraGrep_report_.DisplayReport();
	ParsingResults();
}

/*	Method Definition
	Method:		ParsingResults
	Params:		Void
	Returns:	Void
	Purpose:	Print the results of the overall grepping process
*/
void UltraGrep::ParsingResults()
{
	double parsingLapsed = (ParsingStop.QuadPart - ParsingStart.QuadPart) / static_cast<double>(Frequency.QuadPart);
	cout << "\n\n\t" << "Parsing Results" << endl;
	cout << "\t" << setw(20) << setfill('-') << "" << endl;
	cout << "\t" << "Files with Matches: " << ultraGrep_report_.FilesMatched() << endl;
	cout << "\t" << "Number of Matches: " << ultraGrep_report_.NumberOfMatches() << endl;
	cout << "\t" << "Scan Completed in: " << parsingLapsed << "s\n" << endl;
}


//WIN32 Threading
/*	Method Definition
	Method:		FileWorker
	Params:		PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK
	Returns:	VOID CALLBACK
	Purpose:	Thread function to parse files and if a match is found with the 
					provided regex expresion record the line on the file on which it was 
					matched.
*/
VOID CALLBACK FileWorker(PTP_CALLBACK_INSTANCE instance, PVOID context, PTP_WORK work)
{
	//get thread info object
	FileTasks* f_task = (FileTasks*)context;
	wstring path_to_file;
	fstream fileS;
	bool isMoreWork = true;
	while(isMoreWork)
	{

		AcquireSRWLockExclusive(&f_task->s_Lock);
		if (!f_task->tasks.empty())
		{
			path_to_file = f_task->tasks.front();
			fileS.open(path_to_file);
			path fileP(path_to_file);
			f_task->tasks.pop();
			ReleaseSRWLockExclusive(&f_task->s_Lock);

			if (f_task->isV)
			{
				lock_guard<mutex> lk(f_task->c_lock);
				cout << "\n\tScanning File: " << fileP.filename().string() << " in " << fileP.parent_path().string() << '\n' << endl;
			}

			//extract the file
			string line;
			int lineNum = 0;
			while (getline(fileS, line))
			{
				++lineNum;

				ptrdiff_t matchCount(distance(sregex_iterator(line.begin(), line.end(), f_task->expr), sregex_iterator()));
				if (matchCount > 0)
				{
					if (f_task->isV)
					{
						lock_guard<mutex> lk(f_task->c_lock);
						cout << "\tFound Match. Matched : [" << matchCount << "] in File: " << fileP.filename() << " [" << lineNum << "] " << line << "\n" << endl;
					}
					AcquireSRWLockExclusive(&f_task->s_Lock);
					f_task->report_.addMatch(path_to_file, line, lineNum, static_cast<unsigned long>(matchCount));
					ReleaseSRWLockExclusive(&f_task->s_Lock);
				}
			}//end of while
		}
		else
			ReleaseSRWLockExclusive(&f_task->s_Lock);


		if (!f_task->isDirSearchDone && f_task->tasks.empty())
			isMoreWork = false;
	}
}


//C++ 11 Threading 
/*	Method Definition
	Method:		FileExtract
	Params:		wstring, UltraGrep& 
	Returns:	Void
	Purpose:	Thread function to extract the File and document the results 
					in the Report class
*/
void UltraGrep::FileExtract(wstring filePath, UltraGrep& c_ref)
{
	//bool moreWork = true;
	while (c_ref.isDoneDirectorySearch)
	{
		{
			unique_lock<mutex> lk(c_ref.file_pool.wake_lock);
			c_ref.file_pool.wake_cond.wait(lk);
		}
		while (!c_ref.file_pool.queued_Tasks.empty())
		{
			task_type* task;
			bool haveTask = false;
			
				{
					lock_guard<mutex> lk(c_ref.file_pool.task_lock);
					if (!c_ref.file_pool.queued_Tasks.empty())
					{
						//Get the task from the stack
						task = &c_ref.file_pool.queued_Tasks.front().GetArguments();
						filePath = get<0>(*task);
						task = nullptr;
						c_ref.file_pool.queued_Tasks.pop();
						haveTask = true;
					}
				}


				if (haveTask)
				{
					fstream fileS(filePath);
					path scanFile(filePath);
					int lineNum = 0;
					string line = "";

					if (c_ref.isVerbose_)
					{
						lock_guard<mutex> consoleLk(c_ref.cout_Lock);
						cout << "\n\tScanning File: " << scanFile.filename().string() << " in " << scanFile.parent_path().string() << '\n' << endl;
					}

					//get each line in the file 
					while (getline(fileS, line))
					{
						++lineNum;
						//get the match count of each line 
						ptrdiff_t matchCount(distance(sregex_iterator(line.begin(), line.end(), c_ref.expression_str_), sregex_iterator()));
						if (matchCount > 0)
						{
							if(c_ref.isVerbose_)
							{
								lock_guard<mutex> consoleLk(c_ref.cout_Lock);
								cout << "\tFound Match. Matched : [" << matchCount << "] in File: " << scanFile.filename();
								cout << " [" << lineNum << "] " << line << endl;
							}
							{
								lock_guard<mutex> lk(c_ref.file_pool.work_lock);
								c_ref.ultraGrep_report_.addMatch(scanFile.wstring(), line, lineNum, static_cast<unsigned long>(matchCount));
							}
						}
					}
				
			}
		}
	}
}//end of method

 /*	Method Definition
	 Method:	DirectorySearch
	 Params:	wstring, UltraGrep&
	 Returns:	Void
	 Purpose:	Thread function to parse through the starting directory recursivly by adding
					a found directory to the task stack
 */
void UltraGrep::DirectorySearch(wstring dir, UltraGrep& c_ref)
{
	bool moreWork = true;
	while (moreWork)
	{//fix bug where one thread stops 
		{
			unique_lock<mutex> lk(c_ref.dir_pool.wake_lock);
			c_ref.dir_pool.wake_cond.wait(lk);
		}
		while (!c_ref.dir_pool.queued_Tasks.empty())
		{
			task_type* task;
			bool haveTask = false;
			{
				lock_guard<mutex> lk(c_ref.dir_pool.task_lock);
				//Grab the task off the stack
				if (!c_ref.dir_pool.queued_Tasks.empty())
				{
					task = &c_ref.dir_pool.queued_Tasks.front().GetArguments();
					dir = get<0>(*task);
					task = nullptr;
					c_ref.dir_pool.queued_Tasks.pop();
					haveTask = true;
				}
			}


			if (haveTask)
			{	

				if (c_ref.isVerbose_)
				{
					lock_guard<mutex> consoleLk(c_ref.cout_Lock);
					cout << "\tParsing Directory: " << dir.substr(dir.find_last_of('\\'), dir.length()) << endl;
				}
				
				for (directory_iterator dirIter( dir ), dirEnd; dirIter != dirEnd; ++dirIter)
				{
					//if a file is found than add it to the file pool task then notify One.
					//if not then add it to the directory pool task
					wstring newDir(dirIter->path().c_str());
					if (!is_directory(dirIter->path()))
						for (auto file_ext : *c_ref.ext_list_)
						{
							if (dirIter->path().filename().extension() == file_ext)
							{
								if (!WIN32_THREADING_)
								{
									lock_guard<mutex> lk(c_ref.dir_pool.work_lock);
									c_ref.file_pool.add_task(&UltraGrep::FileExtract, c_ref, newDir, c_ref);
									c_ref.file_pool.wake_cond.notify_one();
								}
								else
								{
									lock_guard<mutex> lk(c_ref.dir_pool.work_lock);
									c_ref.m_file_pool.AddTask(newDir);
								}
							}

						}
					else 
					{
						lock_guard<mutex> lk(c_ref.dir_pool.work_lock);
						c_ref.dir_pool.add_task(&UltraGrep::DirectorySearch, c_ref, newDir, c_ref);
						c_ref.dir_pool.wake_cond.notify_one();
					}
					
				}
			}
		}

		{
			lock_guard<mutex> lk(c_ref.dir_pool.task_lock);
			if (c_ref.dir_pool.queued_Tasks.empty())
			{
				moreWork = false;
				c_ref.dir_pool.wake_cond.notify_all();
			}
		}

	}
}//end of method


int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (argc < 2)
	{
		cout << "\n\tProgram Arguments not Provieded." << endl;
		cout << "\tInterface Required: UltraGrep.exe -v C://directoryPath [gG]et .cpp.hpp" << endl;
		return EXIT_FAILURE;
	}
	
	unique_ptr<UltraGrep> uGrep( new UltraGrep());
	try {

		for (int idx = 1; idx < argc; ++idx)
		{
			uGrep->ExtractCommandParams(string(argv[idx]));
		}
		uGrep->StartGrep(*uGrep);
		
	}
	catch (exception ex)
	{
		cout << "\n\tProgram Arguments not Provieded." << endl;
		cout << "\tInterface Required: UltraGrep.exe -v C://directoryPath [gG]et .cpp.hpp" << endl;
	}
	uGrep.reset();

	_CrtDumpMemoryLeaks();
}