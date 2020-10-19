#pragma once
#include "Delimiter.hpp"
#include "UltraGrep.hpp"

#ifndef EXPRESSIONDELIMITER_HPP
#define EXPRESSIONDELIMITER_HPP

class ExpressionDelimiter : public IDelimiter
{
	//const delimiter value
	const char LEFT_DELIMITER = '[';
	const char RIGHT_DELIMITER = ']';
	
	//definitions for classes execute
	size_t expr_val = 0;
	std::wstring start_expr;
	std::wstring opposing_delimiters = (L"-.//\\");
	UltraGrep&  uGrep_ref;
public:
	ExpressionDelimiter(UltraGrep& ug) : uGrep_ref(ug) {}
	~ExpressionDelimiter() {}

	virtual void execute(std::wstring&) override;
};

#endif // !EXPRESSIONDELIMITER_HPP