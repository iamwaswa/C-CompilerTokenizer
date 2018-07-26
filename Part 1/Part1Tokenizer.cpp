/*
 * Part1Tokenizer.cpp
 *  Final version
 *
 *  Copyright (c) 2017 School of Engineering Science, Simon Fraser University
 *
 */

#include <cctype>
#include "Part1Tokenizer.hpp"

using namespace std;

// \n is also whitespace, but should have been removed by getline()
#define WHITE "\r\t\v\f "

string delim3[] =  {">>=", "<<="}; // >> shifts bits right by specified number i.e.
								   // int a = 3; (0011) a = a >> 1 now a = (0001)
								   // int a = 5; (0101) a <<= 2 now a = is first (1010) then (0100) finally
string delim2[] =  {"+=","-=", "/=", "%=", "<<", ">>", "++", "--"};
string delim1andWhite =  WHITE "/()*%:;=+-~?";
string white = WHITE;

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

// Accepts a stream and returns a vector of tokens with any remaining preprocessing directives stripped out
// Input: a stream
// Output: a vector of tokens
vector<string> tokenizeCodeStrip(istream& code)
{

	vector<string> result;
	string line;
	while (getline (code, line))
	{	// Excludes lines that are library declarations
		if (line.at(0) == '#')
			continue;

		string::size_type index2 = 0; //position on the line
        auto length = line.length(); //length of the line
        // Runs as long as the index position is for some entity on the line
		while(index2 < length )
		{
			// Excludes lines that have whitespace characters
			auto index1 = line.find_first_not_of(white, index2);
			if (index1 == string::npos)
				break;

			/// Dealing with int and float consts
			if (isdigit(line.at(index1)) || line.at(index1) == '.')
			{
				index2 = index1;

				// integer consts
				bool preDigitFound = processDigits(line, index2); // Ensures the start and end of a number are identified before any decimal points

				// floats
				if (index2 < length && line.at(index2) == '.')
				{
					index2++;
					bool postDigitFound = processDigits(line, index2); // Ensures the start and end of a number after a decimal point are identified

					if(!preDigitFound && !postDigitFound) // detecting the single "." as an invalid token!
					{
						cout << "Error -- invalid token in input: " << line.substr(index1, index2 - index1) << endl;
						result.push_back("");
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
				}

				// suffix f and F
				if (index2 < length && (line.at(index2) == 'f' || line.at(index2) == 'F'))
					index2++;

				// Deals with integers, floats and exponentials
				auto subs = line.substr(index1, index2 - index1);
				result.push_back(subs);
				continue;
			}
			// Equivalent to for loop initializing delim to each value of delim 3 until all values are exhausted
			for (auto delim : delim3)
			{
				// Checking if the substring is equal to the left/right shift operator and the assignment operator combined
				if (line.substr(index1, 3) == delim)
				{
					result.push_back(delim);
					index2 = index1 + 3;
					break;
				}
			}
			// Goes to next iteration if the particular delimiter has been found
			if (index2 > index1)
				continue;

			// the below follows the pattern from the lines above.
			for (auto delim : delim2)
			{
				if (line.substr(index1, 2) == delim)
				{
					result.push_back(delim);
					index2 = index1 + 2;
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
						if (line.at(index2) == '"')
						{
							result.push_back(line.substr(index1, index2 + 1 - index1));
							break;
						}
					index2++;
				}
				if (index2 >= length)
				{ // String is not terminated!
					cout << "Error:  string is not terminated!" << endl;
					result.push_back("");
					break;
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
				index2++; // skip over character
				if (index2 >= length || line.at(index2) != '\'') // Character constant is not terminated!
				{
					cout << "Error:  character constant is not terminated!" << endl;
					result.push_back("");
				}
				else
				{
					index2++;
					result.push_back(line.substr(index1, index2 - index1));
				}
				continue;
			}
			// Ensures all alphanumeric characters are added
			if(line[index1]=='_' || isalpha(line[index1]))
			{
				index2 = index1 + 1;
				while (index2 < length && (line[index2]=='_' || isalnum(line[index2])))
					index2 ++;
				result.push_back(line.substr(index1, index2 - index1));
				continue;
			}

			index2 = line.find_first_of(delim1andWhite, index1);
			if (index2 == index1)
			{
				index2++; // we found a 1-character token
				result.push_back(line.substr(index1, index2 - index1));
				continue;
			}
			cout << "Error -- invalid token in input: " <<
					line.substr(index1, index2 - index1) << endl;
			result.push_back("");
		}
	}
	return result;
}

