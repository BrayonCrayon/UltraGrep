#include "VerboseDelimiter.hpp"

using namespace std;

/*	Method Definition
	Method:		execute
	Params:		wstring&
	Returns:	Void
	Purpose:	Check to see if the provided argument matches the delimiter '-'
*/
void VerboseDelimiter::execute(wstring& p)
{
	if (p.size() != 2)
		return;

	size_t idx = 0;

	if (p[idx] == this->fDELIMITER && p[(idx + 1)] == 'v' && p.length() >= 2)
	{
		uGrep_ref.isVerbose_ = true;
	}

} 