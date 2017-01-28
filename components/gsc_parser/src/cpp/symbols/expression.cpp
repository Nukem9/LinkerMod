#include "expression.h"
#include "symbol.h"

Expression::Expression(void) : op_type(OP_TYPE_NONE)
{
	this->type = S_TYPE_EXPRESSION;
}

Expression::Expression(YYLTYPE loc) : op_type(OP_TYPE_NONE)
{
	this->type = S_TYPE_EXPRESSION;
	this->location = loc;
}

Expression::Expression(OPERATOR_TYPE prefix, Expression* expr, YYLTYPE range)
{
	this->type = S_TYPE_EXPRESSION;
	
	this->op_type = prefix;
	
	this->location = range;
	this->AddChild(expr);
}

Expression::Expression(Expression* left, OPERATOR_TYPE mid_op, Expression* right, YYLTYPE range)
{
	this->type = S_TYPE_EXPRESSION;
	
	this->op_type = mid_op;
	
	this->location = range;
	this->AddChild(left);
	this->AddChild(right);
}

Expression::Expression(Expression* expr, OPERATOR_TYPE postfix, YYLTYPE range)
{
	this->type = S_TYPE_EXPRESSION;
	
	this->op_type = postfix;
	
	this->location = range;
	this->AddChild(expr);
}

Expression::~Expression()
{
}

OPERATOR_TYPE Expression::Operator(void) const
{
	return this->op_type;
}

void Expression::PrintInfo() const
{
	printf("%s with %d children at %d(%d) - %d(%d), op '%s'\n",
		SYMBOL_TYPE_STRING(type),
		this->children ? this->children->Size() + 1 : 0,
		location.start.line,
		location.start.character,
		location.end.line,
		location.end.character,
		OPERATOR_TYPE_STRING(this->op_type));
}