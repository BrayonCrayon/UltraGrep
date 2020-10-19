#pragma once
#include "Delimiter.hpp"
#include "UltraGrep.hpp"

#ifndef PATHDELIMITER_HPP
#define PATHDELIMITER_HPP

class PathDelimiter : public IDelimiter
{
	char dDelimiter = '\\';
	UltraGrep&  uGrep_ref;
public: 
	PathDelimiter(UltraGrep& ug) : uGrep_ref(ug) {}
	~PathDelimiter() {}
	virtual void execute(std::wstring&) override;
};

#endif // !PATHDELIMITER_HPP