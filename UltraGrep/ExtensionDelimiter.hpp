#pragma once
#include "Delimiter.hpp"
#include "UltraGrep.hpp"

#ifndef EXTENSIONDELIMITER_HPP
#define EXTENSIONDELIMITER_HPP

class ExtensionDelimiter : public IDelimiter
{
	UltraGrep&  uGrep_ref;
	char DELIMITER = '.';
public:
	ExtensionDelimiter(UltraGrep& ulg) : uGrep_ref(ulg) {}
	~ExtensionDelimiter() {}
	virtual void execute(std::wstring&) override;
};

#endif // !EXTENSIONDELIMITER_HPP