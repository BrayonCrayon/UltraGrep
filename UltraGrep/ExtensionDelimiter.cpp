#include "ExtensionDelimiter.hpp"

using namespace std;

/*	Method Definition
	Method:			execute
	Params:			wstring&
	Purpose:		Checks to see if the Delimiter '.' is found with in the argument.
						Then it will parse out each extension provided and added it to the 
						list.
	Return:			Void

*/
void ExtensionDelimiter::execute(wstring& p) 
{
	wstring param = p;

	size_t ext_Count = p.find_first_of(DELIMITER, 0);

	if (ext_Count == p.npos)
		return;

	//loop through the wstring and find the first occurence of the delimiter.
	// if the delimiter is not found then add the argument to the list
	// If the delimiter was found then extract the substring of the delimiter positions found to the list
	// and set the new string to itself and repeat the process till finished.
	for (size_t beg_del = 0, end_del = 0;;)
	{
		beg_del = param.find_first_of(DELIMITER, 0);
		end_del = param.find_first_of(DELIMITER, (beg_del + 1));

		if (beg_del == param.npos || end_del == param.npos)
		{
			uGrep_ref.ext_list_->push_back(wstring(param.substr(beg_del, end_del)));
			break;
		}

		uGrep_ref.ext_list_->push_back(wstring(param.substr(beg_del, end_del)));
		param = param.substr(end_del, param.length());
	}
}