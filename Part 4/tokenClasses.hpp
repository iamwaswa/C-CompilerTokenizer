//============================================================================
//
//% Student Name 1: Waswa Olunga
//% Student 1 #: 301307966
//% Student 1 userid (email): wolunga (wolunga@sfu.ca)
//
//% Student Name 2: student2
//% Student 2 #: 123456782
//% Student 2 userid (email): stu2 (stu2@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put NONE if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put �none�)__
//
// Also, list any resources beyond the course textbook and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Enter the above information in tokenClasses.cpp  too.
//% * Edit the "Helpers" line and "Resources" line.
//% * Your group name should be "P5_<userid1>_<userid2>" (eg. P5_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// Name        : tokenClasses.hpp
// Description : Course Project Part 4
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSES_HPP_
#define TOKENCLASSES_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include "TreeNode.hpp"

namespace ensc251 {

class Token;
char *demangle(const char *typeName);
class constant;
extern std::vector<Token*> tokenObjectPs; // objects for abstract syntax tree
extern unsigned int tracker; // tracker index

class Token : public TreeNode {    // The Base Token Class
private:
    int lineNumber;
    int indexNumber;
public:
    int getLineNumber() { return lineNumber; }
    int getIndexNumber() { return indexNumber; }

    Token();
    Token(int lineNum, int indexNum) : lineNumber(lineNum), indexNumber(indexNum) {}

    virtual bool has_string_value(const std::string& aString) { return false; };
    virtual std::string getStringValue() = 0;
    void print_ast() { print_traverse(); OUT << ";" << std::endl;}

    friend std::ostream& operator <<(std::ostream& outs, const Token& token){
        token.print(outs);
        return outs;
    }

    void print(std::ostream& str) const { str << demangle(typeid(*this).name()) << ": "; }

    virtual Token* process_punctuator(const std::string &puncValue) { return nullptr; }
    virtual Token* process_declaration() { return nullptr; }

    virtual Token* process_id() { return nullptr; }
    virtual Token* process_primary_exp() { return nullptr; }

    virtual Token* process_pointer_primary_exp() { return nullptr; }

    virtual Token* process_numeric_id() { return nullptr; }
    virtual Token* process_numeric_primary_exp() { return nullptr; }

    virtual Token* process_int_id() { return nullptr; }
    virtual Token* process_int_primary_exp() { return nullptr; }

    virtual Token* process_int_comp() { return nullptr; }
    virtual Token* process_postfix_operator() { return nullptr; }
    virtual Token* advance_past_conditional_operator() { return nullptr; }
    virtual Token* advance_past_shift_operator() { return nullptr; }

    // ***** you may need to add code here *****
    virtual Token* advance_past_additive_operator() {return nullptr; }

    virtual Token* advance_past_div_operator() { return nullptr; }

    virtual Token* advance_past_mod_operator() { return nullptr; }

    virtual Token* advance_past_gen_assignment_operator() { return nullptr; }
    virtual Token* advance_past_int_assignment_operator() { return nullptr; }

    Token* process_token()
    {
        childPVector.clear();

        tracker++;
        if (tracker < tokenObjectPs.size()){
            return this;
        }
        else
            // our grammar must end in ';'. If *this is the end
            //  then it is a premature end which we will report now.
            return nullptr;
    }

    virtual void print_action() = 0;
    virtual void print_traverse()
    {
        if (childPVector.size() > 2)
            traverse_children_post_order(
                    static_cast<ensc251::traverse_func>(&Token::print_traverse),
                    static_cast<ensc251::action_func>(&Token::print_action));
        else if(childPVector.size() == 2)
            traverse_children_in_order(
                    static_cast<ensc251::traverse_func>(&Token::print_traverse),
                    static_cast<ensc251::action_func>(&Token::print_action));
        else
            traverse_children_pre_order(
                    static_cast<ensc251::traverse_func>(&Token::print_traverse),
                    static_cast<ensc251::action_func>(&Token::print_action));
    }

    virtual ~Token() {}
};

class TokenException : public std::exception {
    std::string ErrMsg;
public:
    TokenException(Token* tkn = nullptr, std::string msg=""){
        if(tkn)
            ErrMsg = "\nSyntax Error around {" + tkn->getStringValue() +
                "}, line " + std::to_string(tkn->getLineNumber()) +
                ", index " + std::to_string(tkn->getIndexNumber()) +
                "\n" + msg + "\n";
        else
            ErrMsg = "Syntax Error!\n" + msg + "\n";
    }
    const char * what () const throw () {
        return ErrMsg.c_str();
    }
};

template<typename T>
class numeric_const;

template<typename T>
class numeric_int_const;

class constant : public Token {
public:
    constant(int lineNum, int indexNum): Token(lineNum, indexNum) {};

    virtual std::string getStringValue() = 0;
    Token* process_primary_exp() { return process_token(); }
};

// the below functions are defined in tokenClasses.cpp
Token* recursive_parser();
Token* assignment_exp();
Token* int_assignment_exp();
Token* int_comp_exp();

class StringBasedToken:public Token
{
protected:
    std::string stringValue;
public:
    StringBasedToken(const std::string &aStringValue, int lineNum, int indexNum): Token(lineNum, indexNum), stringValue (aStringValue) {};
    void print(std::ostream& str) const { Token::print(str); str << ": " << stringValue; }
    bool has_string_value(const std::string& aString) { return stringValue == aString; }
    std::string getStringValue() { return stringValue; }
    void print_action() { OUT << stringValue << " "; };
};

class punctuator:public StringBasedToken {
    // e.g., ';', '*', '(', ')', ':', etc.
public:
    punctuator(const std::string &puncValue, int lineNum, int indexNum): StringBasedToken(puncValue, lineNum, indexNum){ };

    void print_traverse()
    {
        if (stringValue == "(") {
            traverse_children_pre_order(
                    static_cast<ensc251::traverse_func>(&punctuator::print_traverse),
                    static_cast<ensc251::action_func>(&punctuator::print_action));

            OUT << ") ";
        }
        else
            traverse_children_in_order(
                    static_cast<ensc251::traverse_func>(&punctuator::print_traverse),
                    static_cast<ensc251::action_func>(&punctuator::print_action));
    }

    void print_action()
    {
        StringBasedToken::print_action();
        if (stringValue==";")
            OUT << std::endl;
    };

    Token* process_primary_exp()
    {
        if(stringValue == "(" && process_token())
        {
            Token* tObjP = assignment_exp();
            if(tObjP)
            {
                if(tokenObjectPs[tracker]->process_punctuator(")"))
                    {
                    add_childP(tObjP);
                    return this;
                    }
            }

        }
        return nullptr;
    }

    Token* process_int_primary_exp()
    {
        if(stringValue == "(" && process_token())
        {
            Token* tObjP = int_assignment_exp();
            if(tObjP)
            {
                if(tokenObjectPs[tracker]->process_punctuator(")"))
                    {
                    add_childP(tObjP);
                    return this;
                    }
            }

        }
        return nullptr;
    }

    Token* process_punctuator(const std::string &puncValue)
    {
        if (puncValue == stringValue)
            return process_token();
        return nullptr;
    }

};

class type_spec:public StringBasedToken
{ // 'char' | 'int' | 'float'
public:
    type_spec(): StringBasedToken("int", 0, 0) { };
    type_spec(const std::string &typeValue, int lineNum, int indexNum): StringBasedToken(typeValue, lineNum, indexNum){ };
    // ***** Complete this class
    Token* process_declaration()
    {
    	int string_tracker = tracker;
        //production: declarator id
        if(tokenObjectPs[string_tracker]->has_string_value("char") || tokenObjectPs[string_tracker]->has_string_value("int") || tokenObjectPs[string_tracker]->has_string_value("float"))
        {
        	Token* type_spec = tokenObjectPs[tracker]->process_token();
        	int asterix_tracker = tracker;
			// production: type_spec - char & int
        	if (!(tokenObjectPs[asterix_tracker]->has_string_value("*")))
        	{
				if (tokenObjectPs[string_tracker]->has_string_value("char") || tokenObjectPs[string_tracker]->has_string_value("int"))
				{
					if(Token* int_id = tokenObjectPs[tracker]->process_int_id())
						type_spec->add_childP(int_id);
					else
						return nullptr;
				}
				// production: type_spec - float
				if (tokenObjectPs[string_tracker]->has_string_value("float"))
				{
					if(Token* float_id = tokenObjectPs[tracker]->process_numeric_id())
						type_spec->add_childP(float_id);
					else
						return nullptr;
				}
				return type_spec;
        	}
            //production: type_spec *
        	if (Token* asterix = tokenObjectPs[tracker]->process_punctuator("*"))
            {
        		Token* pointer_id = tokenObjectPs[tracker]->process_id();
        		asterix->add_childP(type_spec);
                asterix->add_childP(pointer_id);
				return asterix;
			}
        }
        return nullptr;
    }
};

class id:public StringBasedToken
{    // identifiers -- example:  sum
public:
    id(const std::string &idValue, int lineNum, int indexNum): StringBasedToken(idValue, lineNum, indexNum) {  };
    Token* process_id()    { return process_token(); }
    Token* process_primary_exp() { return process_id(); }
};

class pointer_id:public id {
    // char* identifier
public:
    pointer_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
};

class numeric_id:public id
{    // char, int, and float identifiers
public:
    numeric_id(const std::string &idValue, int lineNum, int indexNum): id(idValue, lineNum, indexNum) {  };
    Token* process_numeric_id()    { return process_token(); }
    Token* process_numeric_primary_exp() { return process_numeric_id(); }
};

class int_id:public numeric_id
{    // char and int identifiers
public:
    int_id(const std::string &idValue, int lineNum, int indexNum): numeric_id(idValue, lineNum, indexNum) {  };
    Token* process_int_id()    { return process_id(); } // a different pattern is being followed here.
    Token* process_int_primary_exp() { return process_int_id(); }
};

class gen_assignment_operator:public StringBasedToken
{
    // '=' | '/=' | '+=' | '-='
public:
    gen_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
    Token* advance_past_gen_assignment_operator() { return process_token();    }
};

class int_assignment_operator:public StringBasedToken
{
    // '%=' | '<<=' | '>>='
public:
    int_assignment_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
    Token* advance_past_int_assignment_operator() { return process_token();    }
};

class conditional_operator:public StringBasedToken
{    // '?'
public:
    conditional_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
    Token* advance_past_conditional_operator() {
        return process_token();
    }
    void print_traverse()
    {
        static_cast<ensc251::Token*>(this->childPVector[0])->print_traverse();
        this->print_action();
        static_cast<ensc251::Token*>(this->childPVector[1])->print_traverse();
        OUT << ": " ;
        static_cast<ensc251::Token*>(this->childPVector[2])->print_traverse();
    }
};

class shift_operator:public StringBasedToken
{
    // '<<' | '>>'
public:
    shift_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
    Token* advance_past_shift_operator() { return process_token(); }
};

class additive_operator:public StringBasedToken
{
    // '+' | '-'
public:
    additive_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
    // ***** Complete this class if needed
    Token* advance_past_additive_operator() { return process_token(); }
};

class div_operator:public StringBasedToken
{    // '/'
public:
    div_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
    Token* advance_past_div_operator() { return process_token(); }
};

class mod_operator:public StringBasedToken
{    // '%'
public:
    mod_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum) { };
    Token* advance_past_mod_operator() { return process_token(); }
};

class comp_operator:public StringBasedToken
{
    //  '~'
public:
    comp_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };

    Token* process_int_comp()
    {
        Token* subTreeP;
        if(process_token() && (subTreeP = int_comp_exp()))
        {
            add_childP(subTreeP);
            return this;
        }
        return nullptr;
    }
};

class postfix_operator:public StringBasedToken
{    // '++' | '--'
public:
    postfix_operator(const std::string &opValue, int lineNum, int indexNum): StringBasedToken(opValue, lineNum, indexNum){ };
    // ***** Complete this class if needed
    Token* process_postfix_operator() { return process_token(); }
    // necessary so that ++ is printed before id given that ++ is the root of id
    void print_traverse()
    {
        traverse_children_post_order(
                        static_cast<ensc251::traverse_func>(&punctuator::print_traverse),
                        static_cast<ensc251::action_func>(&punctuator::print_action));
    }
};

class string:public StringBasedToken
{
    // e.g., "sample string"
public:
    string(const std::string &aStringValue, int lineNum, int indexNum) : StringBasedToken(aStringValue, lineNum, indexNum) {};

    Token* process_primary_exp()
    {
        return process_token();
    }
};

class incorrect:public StringBasedToken
{    // e.g., &
public:
    incorrect(const std::string &aStringValue, int lineNum, int indexNum) : StringBasedToken(aStringValue, lineNum, indexNum) {
    };
};

template<typename T>
class numeric_const:public constant
{    // e.g., 123 | 'a' | 12.5
private:
    T value;
public:
    numeric_const(const T& constValue, int lineNum, int indexNum): constant(lineNum, indexNum), value(constValue)  {}

    std::string getStringValue() { return std::to_string(value); }
    T& getValue() { return value; }
    void print(std::ostream& str) const {Token::print(str); str << ": " << value; }
    void print_action() {
        if (std::is_same<T, char>::value)
            OUT << "'" << value << "'" << " ";
        else
            OUT << value << " ";
    };

    Token* process_numeric_primary_exp() { return this->process_primary_exp(); }
};

template<typename T>
class numeric_int_const:public numeric_const<T>
{    // e.g., 123 | 'a'
public:
    numeric_int_const(const T& constValue, int lineNum, int indexNum): numeric_const<T>(constValue, lineNum, indexNum) {}

    Token* process_int_primary_exp() { return this->process_primary_exp(); }
};

} // namespace ensc251
#endif /* TOKENCLASSES_HPP_ */
