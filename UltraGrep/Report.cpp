#include "Report.hpp"

using namespace std;

/*	Method Definition
	Method:		DisplayReport
	Params:		Void
	Returns:	void
	Purpose:	Displays the full report of all the matches that 
					were found.
*/
void Report::DisplayReport()
{
	cout << "\n\n\t\t" << setw(60) << setfill('-') << "" << endl;
	cout << "\t\t" << setw(15) << setfill('\t') << "Begin Report" << endl;
	cout << "\t\t" << setw(60) << setfill('-') << "" << endl;
	if (this->report_coll_.empty())
	{
		cout << "\n" << setw(24) << setfill('\t') << "No Matches Found....\n" << endl;
	}

	for (map_type::iterator it = report_coll_.begin(); it != report_coll_.end(); ++it)
	{
		wcout << "\n\t" << it->first << endl;
		for (auto re_item : it->second)
			re_item.report();
	}
}