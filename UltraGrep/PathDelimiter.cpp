#include "PathDelimiter.hpp"

using namespace std;

/*	Method Definition
	Method:		execute
	Params:		wstring&
	Returns:	Void
	Purpose:	Find the delimiter count of the delimiter '\\' and set the parameter
*/
void PathDelimiter::execute(wstring& p)
{
	size_t dir_Count = count(p.begin(), p.end(), this->dDelimiter);

	if (dir_Count == 0)
		return;

	uGrep_ref.start_Folder_ = p;
}