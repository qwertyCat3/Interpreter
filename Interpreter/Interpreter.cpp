#include "Interpreter.h"

std::map<std::string, Operator> Interpreter::_operators = {
	{"+", Operator{[](Type* a, Type* b) { return a->add(b); }, 16} },
	{"-", Operator{[](Type* a, Type* b) { if (b == nullptr) throw SyntaxException(INVALID_OPERATOR_USE(std::string("-"))); else return a == nullptr ? b->negative() : a->sub(b); }, 16, BINARY_INFIX, true} },
	{"*", Operator{[](Type* a, Type* b) { return a->mul(b); }, 17} },
	{"/", Operator{[](Type* a, Type* b) { return a->div(b); }, 17} },
	{"%", Operator{[](Type* a, Type* b) { return a->mod(b); }, 17} },
	{"**", Operator{[](Type* a, Type* b) { return a->exp(b); }, 18} },

	{"++", Operator{[](Type* a, Type* b) { if (a == nullptr) return b->increment(false); else return a->increment(true); }, 20, BINARY_INFIX, true} },
	{"--", Operator{[](Type* a, Type* b) { if (a == nullptr) return b->decrement(false); else return a->decrement(true); }, 20, BINARY_INFIX, true} },

	{"|", Operator{[](Type* a, Type* b) { return a->bitOr(b); }, 13} },
	{"^", Operator{[](Type* a, Type* b) { return a->bitXor(b); }, 14} },
	{"&", Operator{[](Type* a, Type* b) { return a->bitAnd(b); }, 15} },
	{"~", Operator{[](Type* a, Type* b) { return b->bitNot(); }, 19, UNARY_PREFIX} },

	{"(^)", Operator{[](Type* a, Type* b) { return a->call(b); }, 21, false, true} },

	{"->", Operator{[](Type* a, Type* b) { return b->extend(a); }, 8} },
	{"<-", Operator{[](Type* a, Type* b) { return a->extend(b); }, 8} },
	{":", Operator{[](Type* a, Type* b) { return (Type*)new Pair(a->isVariable() ? a : a->copy(), b->isVariable() ? b : b->copy()); }, 7}},
	{",", Operator{(operation)Interpreter::sequenceExtension, 6} },
	{"=>", Operator{[](Type* a, Type* b) { return (Type*)new Function(a, (Block*)b); }, 9} },

	{"=", Operator{(operation)Interpreter::assign, 5, BINARY_INFIX, false, false} },
	{"+=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->addAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"-=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->subAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"*=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->mulAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"/=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->divAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"%=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->modAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"**=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->expAssign(b); }, 5, BINARY_INFIX, false, false} },

	{"<-=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->extendAssign(b); }, 5, BINARY_INFIX, false, false} },

	{"^=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->xorAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"&=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->andAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"|=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->orAssign(b); }, 5, BINARY_INFIX, false, false} },
	{"<<=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->leftShiftAssign(b); }, 5, BINARY_INFIX, false, false} },
	{">>=", Operator{[](Type* a, Type* b) { Interpreter::checkAssign(a); return a->rightShiftAssign(b); }, 5, BINARY_INFIX, false, false} },

	{"?", Operator{[](Type* a, Type* b) { return a->ternary(b); }, 7}},

	{"if", Operator{[](Type* a, Type* b) { return (Type*)new If(b); }, 4, UNARY_PREFIX}},
	{"else", Operator{[](Type* a, Type* b) { return If::elseCheck(a, b); }, 2}},
	{"{^}", Operator{[](Type* a, Type* b) { return a->block(b); }, 3} },

	{"catch", Operator{[](Type* a, Type* b) { return Interpreter::catchBlock(a, b); }, 2}},

	{".", Operator{[](Type* a, Type* b) { return a->point(b); }, 22} },
	{"[^]", Operator{[](Type* a, Type* b) { return a->index(b); }, 21} },
	{"while", Operator{[](Type* a, Type* b) { return (Type*)new While(b); }, 4, UNARY_PREFIX}},
	{"foreach", Operator{[](Type* a, Type* b) { if (b == nullptr) throw SyntaxException(INVALID_OPERATOR_USE(std::string("-"))); else return a == nullptr ? (Type*)new Foreach(b) : Foreach::comprehension(a, b); }, 4, BINARY_INFIX, true} },
	{"for", Operator{[](Type* a, Type* b) { return (Type*)new For(b); }, 4, UNARY_PREFIX}},

	// logic operators
	{"==", Operator{[](Type* a, Type* b) { return a->equal(b); }, 12} },
	{"!=", Operator{[](Type* a, Type* b) { return a->notEqual(b); }, 12} },
	{">", Operator{[](Type* a, Type* b) { return a->greater(b); }, 12} },
	{"<", Operator{[](Type* a, Type* b) { return a->less(b); }, 12} },
	{">=", Operator{[](Type* a, Type* b) { return a->greaterEqual(b); }, 12} },
	{"<=", Operator{[](Type* a, Type* b) { return a->lessEqual(b); }, 12} },
	{"||", Operator{[](Type* a, Type* b) { return a->logicOr(b); }, 11} },
	{"&&", Operator{[](Type* a, Type* b) { return a->logicAnd(b); }, 10} },

	{";", Operator{[](Type* a, Type* b) { return (Type*)new Undefined(); }, 1, BINARY_INFIX, true} },

	// casting
	{"(" STRING ")", Operator{[](Type* a, Type* b) { return (Type*)new String(b->toString()); }, 20, UNARY_PREFIX}},
	{"(" FLOAT ")", Operator{[](Type* a, Type* b) { return b->toFloat(); } , 20, UNARY_PREFIX }},
	{"(" INT ")", Operator{[](Type* a, Type* b) { return b->toInt(); }, 20, UNARY_PREFIX}},
	{"(" CHAR ")", Operator{[](Type* a, Type* b) { return b->toChar(); }, 20, UNARY_PREFIX}},
	{"(" _BOOL ")", Operator{[](Type* a, Type* b) { return b->toBool(); }, 20, UNARY_PREFIX}},
};
std::map<std::string, Type*> Interpreter::_variables;
std::vector<std::vector<ScopeVariable>> Interpreter::_variableScope = std::vector<std::vector<ScopeVariable>>({ std::vector<ScopeVariable>() });

Interpreter::Interpreter() : Parser(Interpreter::_operators)
{
	// predefined
	initVariables(this->_variables);
}

Interpreter::~Interpreter()
{
	for (const std::pair<std::string, Type*>& pair : this->_variables)
		delete pair.second;
}

std::string Interpreter::run(const std::string& code)
{
	Type* result = this->value(code);
	std::string resultStr = result->toString();
	if(!result->isVariable())
		delete result;
	return resultStr;
}

void Interpreter::importFunctions(const std::map<std::string, staticFunction>& functions)
{
	for (const std::pair<std::string, staticFunction>& pair : functions)
	{
		this->_variables[pair.first] = new StaticFunction(pair.second);
		this->_variables[pair.first]->setVariable(pair.first);
	}
}

void Interpreter::importVariables(const std::map<std::string, Type*>& variables)
{
	for (const std::pair<std::string, Type*>& pair : variables)
	{
		this->_variables[pair.first] = pair.second;
		this->_variables[pair.first]->setVariable(pair.first);
	}
}

Type* Interpreter::valueOf(const std::string& str)
{
	// is a variable
	if (this->_variables.find(str) != this->_variables.end())	// old variable
		return Interpreter::_variables[str];
	// is a type
	else if (Void::isType(str))
		return new Void();
	else if (Undefined::isType(str))
		return new Undefined();
	else if (Int::isType(str))
		return new Int(str);
	else if (Float::isType(str))
		return new Float(str);
	else if (Bool::isType(str))
		return new Bool(str);
	else if (String::isType(str))
		return new String(str.substr(1, str.size() - 2));
	else if (Char::isType(str))
		return new Char(str[1]);
	
	// check new variable
	Type* newVar = this->checkNewVariable(str);
	if (newVar)
		return newVar;
	// invalid expression
	else
		throw TypeErrorException("Value \"" + str + "\" cannot be parsed");
}

Type* Interpreter::evaluateBlock(Node* node)
{
	return new Block(*this, node);
}

std::string Interpreter::getValue(const std::string& expression)
{
	std::regex r(R"~(^((?=[\d\.]*?\d)\d*\.?\d*(e-?\d+)?|[a-zA-Z_](\w*( \w)?)*|""|".*?[^\\]"|'.'))~");	// int, float, name, string or char
	std::smatch match;
	return std::regex_search(expression, match, r) ? match.str() : "";
}

Type* Interpreter::handleParentheses(Type* value, char parenthesesType)
{
	if (value->getType() == TEMP_SEQUENCE)
	{
		Type* ret = value;
		if (parenthesesType == '(')
			ret = new Tuple(((TempSequence*)value)->begin(), ((TempSequence*)value)->end());
		else if (parenthesesType == '[')
			ret = new List(((TempSequence*)value)->begin(), ((TempSequence*)value)->end());
		else if (parenthesesType == '{')
			ret = new Object(((TempSequence*)value)->begin(), ((TempSequence*)value)->end());
		else return ret;
		((TempSequence*)value)->clear();
		delete value;
		return ret;
	}
	else if (parenthesesType == '[')	// short list
	{
		if (value->getType() == UNDEFINED)
			return new List();
		else
			return new List(std::vector<Type*>{value->isVariable() ? value->copy() : value});
	}
	else if (parenthesesType == '{' && value->getType() == PAIR)
	{
		// single value object
		return new Object((Pair*)value);
	}
	return value;
}

void Interpreter::handleTempTypes(Type* a, Type* b, Type* res, const std::string& op)
{
	// if not variables, delete after being used in operator
	bool flag = res->getType() == TEMP_SEQUENCE && op == ",";	// sequence creation
	if (a && !a->isVariable() && !flag && a != res)
		delete a;
	if (b && !b->isVariable() && !flag && b != res)
		delete b;
}

Type* Interpreter::assign(Type* a, Type* b)
{
	if (!a->isVariable())
	{
		if (a->getType() == TUPLE && ((Tuple*)a)->isVariableTuple())
		{
			return a->assign(b);
		}
		else if (a->getType() == REFERENCE)
		{
			return a->assign(b);
		}
		else
		{
			throw InvalidOperationException("assigning to a non-variable value");
		}
	}
	// regular cases
	if (a->isStaticType() || a->getType() == REFERENCE)
		return a->assign(b);	// assign operator
	// replace
	return Interpreter::addVariable(a->getVariable(), b->copy());
}

void Interpreter::removeVariable(const std::string& name, bool deleteValue)
{
	if (deleteValue)
		delete Interpreter::_variables[name];
	else
		Interpreter::_variables[name]->setVariable("");
	Interpreter::_variables.erase(name);
}

void Interpreter::openScope()
{
	Interpreter::_variableScope.push_back(std::vector<ScopeVariable>());
}

void Interpreter::closeScope()
{
	for (const ScopeVariable& var : Interpreter::_variableScope.back())
	{
		if (var.previousValue == nullptr)
			Interpreter::removeVariable(var.name);
		else
		{
			delete Interpreter::_variables[var.name];
			Interpreter::_variables[var.name] = var.previousValue;
		}
	}
	Interpreter::_variableScope.pop_back();
}

Type* Interpreter::addVariable(std::string variableName, Type* variable, bool isNew, bool setScope)
{
	Helper::trim(variableName);
	// check if variable already exists in this scope
	bool inThisScope = false;
	if (Interpreter::_variables.find(variableName) != Interpreter::_variables.end())
	{
		for (const ScopeVariable& var : Interpreter::_variableScope.back())
			if (var.name == variableName)
				inThisScope = true;
	}

	if (isNew)
	{
		if (Interpreter::_variables.find(variableName) != Interpreter::_variables.end())
			throw VariableException('"' + variableName + "\" already exists");
		else if (!isVariableNameValid(variableName))
			throw VariableException('"' + variableName + "\" is not a valid name");
		Interpreter::_variableScope.back().push_back(ScopeVariable{ variableName, nullptr });
	}
	else if(setScope)
		Interpreter::_variableScope.back().push_back(ScopeVariable{ variableName, nullptr });

	variable->setVariable(variableName);
	// if variable already exists
	if (Interpreter::_variables.find(variableName) != Interpreter::_variables.end())
	{
		if(inThisScope)
			delete Interpreter::_variables[variableName];
		else
			Interpreter::_variableScope.back().back().previousValue = Interpreter::_variables[variableName];
	}
	return Interpreter::_variables[variableName] = variable;
}

void Interpreter::checkAssign(Type* type)
{
	if (!type->isVariable() && type->getType() != REFERENCE)
		throw InvalidOperationException("Assigning to a non-variable value");
}

Type* Interpreter::catchBlock(Type* a, Type* b)
{
	try
	{
		if (a->getType() == BLOCK)
			delete ((Block*)a)->run();
		return new Void();
	}
	catch (Type* e)
	{
		if (b->getType() == BLOCK)
		{
			Interpreter::addVariable("e", e, true);
			delete ((Block*)b)->run();
			Interpreter::removeVariable("e");
		}
		return new Void();
	}
}

Type* Interpreter::checkNewVariable(const std::string& str)
{
	if (str.empty())
		return new Undefined();
	Type* staticType = nullptr;
	if (str.rfind(ANY " ", 0) == 0)	// new dynamic variable
		return Interpreter::addVariable(str.substr(strlen(ANY " ")), new Undefined(), true);
	else if (str.rfind(INT " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(INT " ")), new Int(), true);
	else if (str.rfind(FLOAT " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(FLOAT " ")), new Float(), true);
	else if (str.rfind(FUNCTION " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(FUNCTION " ")), new Function(*this), true);
	else if (str.rfind(_BOOL " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(_BOOL " ")), new Bool());
	else if (str.rfind(LIST " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(LIST " ")), new List());
	else if (str.rfind(STRING " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(STRING " ")), new String());
	else if (str.rfind(CHAR " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(CHAR " ")), new Char());
	else if (str.rfind(REFERENCE " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(REFERENCE " ")), new Reference());
	else if (str.rfind(CLASS " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(CLASS " ")), new Class());
	else if (str.rfind(OBJECT " ", 0) == 0)
		staticType = Interpreter::addVariable(str.substr(strlen(OBJECT " ")), new Object());
	else if(this->isVariableNameValid(str))
		return new Name(str);
	else
		return nullptr;
	staticType->setStatic();
	return staticType;
}

TempSequence* Interpreter::sequenceExtension(Type* a, Type* b)
{
	if (a && a->getType() == TEMP_SEQUENCE)
	{
		((TempSequence*)a)->sequenceExtend(b ? b : new Undefined());
		return (TempSequence*)a;
	}
	TempSequence* sequence = new TempSequence();
	sequence->sequenceExtend(a ? a : new Undefined());
	sequence->sequenceExtend(b ? b : new Undefined());
	return sequence;
}

bool Interpreter::isVariableNameValid(const std::string& name)
{
	return std::regex_match(name, std::regex("[a-zA-Z_][a-zA-Z0-9_]*"));
}
