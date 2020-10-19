#pragma once
#include "Delimiter.hpp"
#include "UltraGrep.hpp"

#ifndef VERBOSEDELIMITER_HPP
#define VERBOSEDELIMITER_HPP

class VerboseDelimiter : public IDelimiter
{
	char fDELIMITER = '-';
	UltraGrep&  uGrep_ref;
public:
	VerboseDelimiter(UltraGrep& ug) : uGrep_ref(ug){}
	~VerboseDelimiter() {}
	
	virtual void execute(std::wstring&) override;
};

#endif // !VERBOSEDELIMITER_HPP