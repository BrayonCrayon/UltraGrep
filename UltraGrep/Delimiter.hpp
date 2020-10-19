#pragma once
#include <string>
#include <algorithm>

#ifndef IDELIMITER_HPP
#define IDELIMITER_HPP

class IDelimiter 
{
public:
	IDelimiter() {}
	~IDelimiter() {}

	virtual void execute(std::wstring&) = 0;
};


#endif // !IDELIMITER_HPP