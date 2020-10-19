#include "ExpressionDelimiter.hpp"

using namespace std;

/*	Method Definition
	Method:			execute
	Purpose:		To check to see if the sent in string contains 
					any opposing delimiters ex('.', '//', '\\', '-') for other functionality purposes.
	Return:			Void

*/
void ExpressionDelimiter::execute(wstring& p)
{
	for (auto x : opposing_delimiters)
		if (count(p.begin(), p.end(), x) > 0)
			return;
	
	this->uGrep_ref.expression_str_ = regex("(" + string(p.begin(), p.end()) + ")");
}