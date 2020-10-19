#pragma once

#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#ifndef REPORT_HPP
#define REPORT_HPP


// A struct to hold the information each match found
struct ReportItem {
	using num_t = unsigned long;

	num_t lineNum_;
	num_t occurs_;
	std::string line_;
public:

	ReportItem(num_t lnum, num_t occ, std::string line) : lineNum_(lnum), occurs_(occ), line_(line) {}
	
	/*	Method Definition
		Method:		report
		Params:		void
		Returns:	Void
		Purpose:	Displays to console the information about the reportItem
	*/
	inline void report()
	{
		std::cout << std::setw(15) << std::setfill(' ') << "[" << lineNum_;
		if (occurs_ > 1)
			std::cout << ":" << occurs_;
		std::cout << "]" << std::setfill(' ') << line_ << std::endl;
	}
};

class Report
{
	//types
	using num_t = unsigned long;
	using map_type = std::map<std::wstring, std::vector<ReportItem>>;
	
	//Report Collection
	map_type report_coll_;

public:
	Report() {}
	~Report() {}

	/*	Method Definition
		Method:		addMatch
		Params:		string, string, num_t, num_t
		Returns:	Void
		Purpose:	Add match information to a ReportItem object and add it to 
						report collection
	*/
	inline void addMatch(std::wstring filePath, std::string line, num_t lineNum, num_t occurence)
	{
		report_coll_[filePath].push_back(ReportItem(lineNum, occurence, line));
	}

	void DisplayReport();

	//Reports the Report Collection Size
	inline num_t FilesMatched() { return static_cast<num_t>(report_coll_.size()); }

	/*	Method Definition
		Method:		NumberOfMatches
		Params:		Void
		Returns:	num_t
		Purpose:	Get the number of matches for each report recorded
						and return the number.
	*/
	inline num_t NumberOfMatches()
	{
		num_t numOfMatches = 0;
		for (auto report : report_coll_)
			for (auto reportItem : report.second)
				numOfMatches += reportItem.occurs_;

		return numOfMatches;
	}
};


#endif // !Report