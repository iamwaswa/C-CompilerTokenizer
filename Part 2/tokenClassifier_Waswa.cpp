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
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: _everybody helped us/me with the assignment (list names or put 'None')__
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
// Name        : tokenClassifier.cpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#include <vector>
#include <sstream>
#include <algorithm>
#include "tokenClasses.hpp"
#include <string>

using namespace std;

string delim3[] =  {">>=", "<<="};
string delim2[] =  {"+=","-=", "/=", "%=", "<<", ">>", "++", "--"};
string delim1andWhite = "\r\t\v\f/()*%:;=+-~?";
string white = "\r\t\v\f ";

// Checks for a collection of digits and returns true if at least a single digit is found
// Returns false if there is a dot
bool processDigits(string line, string::size_type &index)
{
    bool DigiFound=false;
    while (index < line.length() && isdigit(line.at(index)))
        {
            index++;
            DigiFound=true;
        }
    return DigiFound;
}

using namespace ensc251;

// Construct token objects categorized into appropriate types
// Input: a stream
// Output: a vector containing the list of Token objects properly categorized
std::vector<ensc251::Token*> tokenClassifier(istream& code)
{
    //TODO: Write Code for classifying each token into an object of the proper class
    // store a pointer to the object in vector tokenObjectPs

    // For example, if you want to classify the number '10' in the input "errno = 10+2.5;":
    //tokenObjectPs.push_back(new ensc251::numeric_int_const<int>(10, 1 /* lineNumber*/, 8 /*indexNumber*/));
    // where the 2nd argument is the line Number and the 3rd argument is the
    //    indexNumber (the position of the start of the token in that line).

    std::vector<ensc251::Token*> tokenObjectPs;
    int current_line_number = 0, current_index_number = 0, this_is_the_line_number = -1;
    std::string current_string, line, save_string;
    std::string::size_type float_find = std::string::npos, int_star_find = std::string::npos;
    std::string::size_type char_find = std::string::npos, int_find = std::string::npos;
    std::string::size_type char_star_find = std::string::npos, dot_find = std::string::npos;
    while (getline (code, line))
    {
        // Excludes lines that are library declarations
        if (line.at(0) == '#')
            continue;
        // Position on the line
        std::string::size_type index2 = 0;
        // Length of the line
        auto length = line.length();
        // Runs as long as the index position is for some entity on the line
        while(index2 < length )
        {
            // Excludes lines that have whitespace characters
            auto index1 = line.find_first_not_of(white, index2);
            if (index1 == std::string::npos)
            {
                current_index_number = 0;
                current_line_number++;
                break;
            }
            /// Dealing with int and float consts
            if (isdigit(line.at(index1)) || line.at(index1) == '.')
            {
                index2 = index1;
                // Ensures the start and end of a number are identified before any decimal points
                bool preDigitFound = processDigits(line, index2);  // integer consts
                // floats
                if (index2 < length && line.at(index2) == '.')
                {
                    index2++;
                    // Ensures the start and end of a number after a decimal point are identified
                    bool postDigitFound = processDigits(line, index2);
                    // Deals with incorrect in the form of unclassifiable characters
                    if(!preDigitFound && !postDigitFound) // detecting the single "." as an invalid token!
                    {
                        current_string = line.substr(index1, index2 - index1);
                        std::cout << "Error -- invalid token in input: " << current_string << std::endl;
                        tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
                        current_index_number = line.find_first_not_of(white, index2);
                        continue;
                    }
                }
                // exponentials
                if (index2 < length && (line.at(index2) == 'e' || line.at(index2) == 'E'))
                {
                    // Checks for e+, e-, E+ or E-
                    if(line.at(index2+1) == '+' || line.at(index2+1) == '-')
                        index2+=2;
                    else
                        index2++;
                    // Ensures index2 is on the last number of the exponential
                    // Ignores the possiblity of an exponential if false is returned
                    if(!processDigits(line, index2)) // invalid float -> roll back ...
                    {
                        if(line.at(index2-1) == 'e' || line.at(index2-1) == 'E')
                            index2--;
                        else
                            index2-=2;
                    }
                    //Deals with exponentials only
                    // numeric_const<float>
                    if (line.at(index2) != 'f' || line.at(index2) != 'F')
                    {
                        current_string = line.substr(index1, index2 - index1);
                        tokenObjectPs.push_back(new ensc251::numeric_const<float>(std::stof(current_string), current_line_number, current_index_number));
                        current_index_number = line.find_first_not_of(white, index2);
                        continue;
                    }
                }
                // suffix f and F;
                // Deals with floats and mixtures of floats and exponentials with suffix f or F
                // numeric_const<float>
                if (index2 < length && (line.at(index2) == 'f' || line.at(index2) == 'F'))
                {
                    index2++;
                    current_string = line.substr(index1, index2 - index1);
                    tokenObjectPs.push_back(new ensc251::numeric_const<float>(std::stof(current_string), current_line_number, current_index_number));
                    current_index_number = line.find_first_not_of(white, index2);
                    continue;
                }
                current_string = line.substr(index1, index2 - index1);
                dot_find = current_string.find(".");
                // Deals with numeric_const<float>
                if (dot_find != std::string::npos)
                {
                    tokenObjectPs.push_back(new ensc251::numeric_const<float>(std::stof(current_string), current_line_number, current_index_number));
                    current_index_number = line.find_first_not_of(white, index2);
                    continue;
                }
                // Deals with numeric_int_const<int>
                else
                {
                    tokenObjectPs.push_back(new ensc251::numeric_int_const<int>(std::stoi(current_string), current_line_number, current_index_number));
                    current_index_number = line.find_first_not_of(white, index2);
                    continue;
                }
            }
            // Equivalent to for loop initializing delim to each value of delim 3 until all values are exhausted
            for (auto delim : delim3)
            {
                // Checking if the substring is equal to the left/right shift operator and the assignment operator combined
                // Deals with int_assignment_operator excluding %=
                if (line.substr(index1, 3) == delim)
                {
                    current_string = delim;
                    tokenObjectPs.push_back(new ensc251::int_assignment_operator(current_string, current_line_number, current_index_number));
                    index2 = index1 + 3;
                    current_index_number = line.find_first_not_of(white, index2);
                    break;
                }
            }
            // Goes to next iteration if the particular delimiter has been found
            if (index2 > index1)
                continue;
            // the below follows the pattern from the lines above.
            for (auto delim : delim2)
            {
                // Deals with %= from int_assignment_operator, shift_operator, postfix_operator
                // and gen_assignment_operator excluding "="
                if (line.substr(index1, 2) == delim)
                {
                    current_string = delim;
                    if (current_string == "%=")
                        tokenObjectPs.push_back(new ensc251::int_assignment_operator(current_string, current_line_number, current_index_number));
                    else if (current_string == "++" || current_string == "--")
                        tokenObjectPs.push_back(new ensc251::postfix_operator(current_string, current_line_number, current_index_number));
                    else if (current_string == "<<" || current_string == ">>")
                        tokenObjectPs.push_back(new ensc251::shift_operator(current_string, current_line_number, current_index_number));
                    else
                        tokenObjectPs.push_back(new ensc251::gen_assignment_operator(current_string, current_line_number, current_index_number));
                    index2 = index1 + 2;
                    current_index_number = line.find_first_not_of(white, index2);
                    break;
                }
            }
            if (index2 > index1)
                continue;
            // Ensures strings are added
            if (line[index1] == '"')
            {
                index2 = index1 + 1;
                while (index2 < length)
                {
                    if (line[index2] == '\\')
                    {
                        index2++; // skip over escape character
                    }
                    else
                        // Deals with string
                        if (line.at(index2) == '"')
                        {
                            current_string = line.substr(index1, index2 + 1 - index1);
                            tokenObjectPs.push_back(new ensc251::string(current_string, current_line_number, current_index_number));
                            current_index_number = line.find_first_not_of(white, index2 + 1);
                            break;
                        }
                    index2++;
                }
                // String is not terminated!
                // Deals with incorrect in the form of unterminated strings
                if (index2 >= length)
                {
                    // Unterminated strings of the form:
                    // "_space_, "_space_ ;, "_something__space_;, "_something__space_
                    if (line[length - 1] == '\r ' || line[length - 1] == '\t ' || line[length - 1] == '\v ' || line[length - 1] == '\f ' || line[length - 2] == '\r ' || line[length - 2] == '\t ' || line[length - 2] == '\v ' || line[length - 2] == '\f ')
                    {
                        std::cout << "Error:  string is not terminated!" << std::endl;
                        current_string = line.substr(index1, line.find_last_of(white) - index1);
                        tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
                        break;
                    }
                    // Unterminated strings of the form:
                    // "_something_;
                    else
                    {
                        std::cout << "Error:  string is not terminated!" << std::endl;
                        current_string = line.substr(index1, length - index1);
                        tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
                        break;
                    }
                    current_line_number++;
                }
                index2++; // non-empty string was pushed on the result vector
                continue;
            }
            // Ensures characters are added
            if (line[index1] == '\'')
            {
                index2 = index1 + 1;
                if (index2 < length && line[index2] == '\\')
                {
                    index2++; // skip over escape character
                }
                // skip over character
                index2++;
                // Character constant is not terminated!
                // Deals with incorrect in the form of unterminated character constants
                if (index2 >= length || line.at(index2) != '\'')
                {
                    // Unterminated character constants of the form:
                    // '_space_, '_space_ ;, '_something__space_;, '_something__space_
                    if (line[length - 1] == '\r ' || line[length - 1] == '\t ' || line[length - 1] == '\v ' || line[length - 1] == '\f ' || line[length - 2] == '\r ' || line[length - 2] == '\t ' || line[length - 2] == '\v ' || line[length - 2] == '\f ')
                    {
                        std::cout << "Error:  character constant is not terminated!" << std::endl;
                        current_string = line.substr(index1, line.find_last_of(white) - index1);
                        tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
                        break;
                    }
                    // Unterminated character constants of the form:
                    // '_something_;
                    else
                    {
                        std::cout << "Error:  character constant is not terminated!" << std::endl;
                        current_string = line.substr(index1, length - index1);
                        tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
                        break;
                    }
                }
                // Deals with numeric_int_const<char>
                else
                {
                    index2++;
                    current_string = line.substr(index1, index2 - index1);
                    char into_character = current_string[1];
                    tokenObjectPs.push_back(new ensc251::numeric_int_const<char>(into_character, current_line_number, current_index_number));
                    current_index_number = line.find_first_not_of(white, index2);
                }
                continue;
            }
            // Ensures all alphanumeric characters are added
            // Deals with identifiers and type_spec
            if(line[index1]=='_' || isalpha(line[index1]))
            {
                //std::string neccesary_binary_op = delim3 + delim2;
                index2 = index1 + 1;
                while (index2 < length && (line[index2]=='_' || isalnum(line[index2])))
                    index2 ++;
                current_string = line.substr(index1, index2 - index1);
                float_find = line.find("float"), int_star_find = line.find("int*"), char_star_find = line.find("char*"), char_find = line.find("char "), int_find = line.find("int ");
                // Deals with type_spec
                if (current_string == "float" || current_string == "int" || current_string == "char")
                    tokenObjectPs.push_back(new ensc251::type_spec(current_string, current_line_number, current_index_number));
                // Deals with id
                else if (this_is_the_line_number == -1)
                {
                    this_is_the_line_number = current_line_number;
                    bool id = false;
                    if (float_find != std::string::npos || char_star_find != std::string::npos || int_star_find != std::string::npos)
                        id = true;
                    // Deals with all id apart from int_id
                    if (id)
                    {
                        // Deals with numeric_id
                        if (float_find != std::string::npos && float_find < index2 && index1 - 6 == float_find)
                        {
                            tokenObjectPs.push_back(new ensc251::numeric_id(current_string, current_line_number, current_index_number));
                            save_string = "float";
                        }
                        // Deals with pointer_id for type char
                        if(char_star_find != std::string::npos && char_star_find < index2 && line.at(char_star_find + 4) == '*')
                        {
                            tokenObjectPs.push_back(new ensc251::pointer_id(current_string, current_line_number, current_index_number));
                            save_string = "char*";
                        }
                        // Deals with pointer_id for type int
                        if(int_star_find != std::string::npos && int_star_find < index2 && line.at(char_star_find + 3) == '*')
                        {
                            tokenObjectPs.push_back(new ensc251::pointer_id(current_string, current_line_number, current_index_number));
                            save_string = "int*";
                        }
                    }
                    // Deals with int_id
                    else
                    {
                        // Deals with int_id for type char
                        if (char_find != std::string::npos && char_find < index2)
                        {
                            tokenObjectPs.push_back(new ensc251::int_id(current_string, current_line_number, current_index_number));
                            save_string = "char";
                        }
                        // Deals with int_id for type int
                        else
                        {
                            tokenObjectPs.push_back(new ensc251::int_id(current_string, current_line_number, current_index_number));
                            save_string = "int";;
                        }
                    }
                }
                // Deals with id, situations where an identifier is mentioned again in a newline
                else
                {
                    // Deals with numeric_id
                    if (save_string == "float")
                        tokenObjectPs.push_back(new ensc251::numeric_id(current_string, current_line_number, current_index_number));
                    // Deals with pointer_id for type int and type char
                    else if (save_string == "char*" || save_string == "int*")
                        tokenObjectPs.push_back(new ensc251::pointer_id(current_string, current_line_number, current_index_number));
                    // Deals with int_id for type int and type char
                    else if (save_string == "char" || save_string == "int")
                        tokenObjectPs.push_back(new ensc251::int_id(current_string, current_line_number, current_index_number));
                }
                current_index_number = line.find_first_not_of(white, index2);
                continue;
            }

            index2 = line.find_first_of(delim1andWhite, index1);
            if (index2 == index1)
            {
                // we found a 1-character token
                index2++;
                current_string = line.substr(index1, index2 - index1);
                // Deals with punctuator
                if(current_string == "(" || current_string == ")" || current_string == ":" || current_string == ";" || current_string == "*")
                    tokenObjectPs.push_back(new ensc251::punctuator(current_string, current_line_number, current_index_number));
                // Deals with the final gen_assignment_operator "="
                else if(current_string == "=")
                    tokenObjectPs.push_back(new ensc251::gen_assignment_operator(current_string, current_line_number, current_index_number));
                // Deals with conditional_operator
                else if (current_string == "?")
                    tokenObjectPs.push_back(new ensc251::conditional_operator(current_string, current_line_number, current_index_number));
                // Deals with additive_operator
                else if (current_string == "+" || current_string == "-")
                    tokenObjectPs.push_back(new ensc251::additive_operator(current_string, current_line_number, current_index_number));
                // Deals with div_operator
                else if (current_string == "/")
                    tokenObjectPs.push_back(new ensc251::div_operator(current_string, current_line_number, current_index_number));
                // Deals with mod_operator
                else if (current_string == "%")
                    tokenObjectPs.push_back(new ensc251::mod_operator(current_string, current_line_number, current_index_number));
                // Deals with comp_operator
                else
                    tokenObjectPs.push_back(new ensc251::comp_operator(current_string, current_line_number, current_index_number));
                current_index_number = line.find_first_not_of(white, index2);
                continue;
            }
            // Deals with unclassified single character tokens
            current_string = line.substr(index1, index2 - index1);
            std::cout << "Error -- invalid token in input: " << current_string << std::endl;
            tokenObjectPs.push_back(new ensc251::incorrect(current_string, current_line_number, current_index_number));
            current_index_number = line.find_first_not_of(white, index2);
        }
    }
    return tokenObjectPs;
}