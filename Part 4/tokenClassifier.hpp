// Name        : tokenClassifier.hpp
// Description : Course Project Part 2 (Token Classifier)
// Original portions Copyright (c) 2017 School of Engineering Science, Simon Fraser University
//============================================================================

#ifndef TOKENCLASSIFIER_HPP_
#define TOKENCLASSIFIER_HPP_

#include <vector>
#include <sstream>

#include "tokenClasses.hpp"

// Construct the token objects and categorize them into appropriate types
// Input: a string vector of tokens
// Output: a vector including the list of Token objects properly categorized
std::vector<ensc251::Token*> tokenClassifier(std::istream& code)
;

#endif /* TOKENCLASSIFIER_HPP_ */
