/*
 * TestPart4.cpp
 *
 *  Created on: Summer 2017
 *      Author: Mohammad Akbari
 *
 *  Copyright (c) 2017 School of Engineering Science, Simon Fraser University
 */

#include <iostream>
#include <sstream>

#include "tokenClassifier.hpp"

using namespace std;

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 4
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

void check(std::istream& input, std::istream& expectedOutput)
{
    std::cout <<  "\n*** " << boost::unit_test::framework::current_test_case().p_name << " ***\n";
    std::string strExpectedOutput;
    try{
        ensc251::tokenObjectPs = tokenClassifier(input);
        ensc251::Token* ast_root = ensc251::recursive_parser();
        if(ast_root)
        {
            ast_root->print_ast();
            std::string strOuput;
            istringstream output(OUT.str());
            while(output >> strOuput && expectedOutput >> strExpectedOutput)
                if(strExpectedOutput!=strOuput){
                    BOOST_ERROR("FAILED! \noutput not matched in ("+strOuput+")");
                    break;
                }
        }
    }
    catch(ensc251::TokenException& e)     {
        expectedOutput >> strExpectedOutput;
        if(strExpectedOutput=="SyntaxError")
            std::cout<<e.what();
        else
        {
            std::cout<<"FAILED!\n";
            BOOST_ERROR(e.what());
        }
    }
    OUT.str(""); // Clear the stream for the next test case!
    ensc251::tracker = 0; // Reset the tracker for the next case!
}

BOOST_AUTO_TEST_CASE(test1)
{
    istringstream input("errno=5;");
    istringstream expectedOutput ("errno = 5 ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test2)
{
    istringstream input ("int a; a=5;");
    istringstream expectedOutput ("int a ; "
                                  "a = 5 ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test3)
{
    istringstream input ("errno ++;" );
    istringstream expectedOutput ("errno ++ ;" );
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test4)
{
    istringstream input("float f; f=6/4.5/3+4;");
    istringstream expectedOutput ("float f ; "
                                  "f = 6 / 4.5 / 3 + 4 ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test5)
{
    istringstream input ("errno>>=10+~errno;");
    istringstream expectedOutput ("errno >>= 10 + ~ errno ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test6)
{
    istringstream input ("char *c;errno>> 2 <<(errno/2 ) ;");
    istringstream expectedOutput ("char * c ; "
                                  "errno >> 2 << ( errno / 2 ) ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test7)
{
    istringstream input ("char chr;errno = errno-'2'+chr;");
    istringstream expectedOutput ("char chr ; "
                                  "errno = errno - '2' + chr ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test8)
{
    istringstream input ("errno = ((25+errno)-1) / (errno+++2);");
    istringstream expectedOutput ("errno = ( ( 25 + errno ) - 1 ) / ( errno ++ + 2 ) ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test9)
{
    istringstream input ("int a; int c; errno %= a?1:c ;");
    istringstream expectedOutput ("int a ; "
                                  "int c ; "
                                  "errno %= a ? 1 : c ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test10)
{
    istringstream input ("float f; f = errno ? errno ? 5.5:2:3 ;");
    istringstream expectedOutput ("float f ; "
                                  "f = errno ? errno ? 5.5 : 2 : 3 ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test11)
{
    istringstream input ("int i ; i %= (i=2.5)+(i<<=2) ;");
    istringstream expectedOutput ("int i ; "
                                  "i %= ( i = 2.5 ) + ( i <<= 2 ) ;");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(test12)
{
    istringstream input ("int b; \na=10;");
    istringstream expectedOutput ("int b ; a = 10 ;");
    check(input, expectedOutput);
}

// Syntax Error
BOOST_AUTO_TEST_CASE(testE1)
{
    istringstream input ("errno 10;");
    istringstream expectedOutput ("SyntaxError");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE2)
{
    istringstream input ("float a; errno >>= a?a:2 ;");
    istringstream expectedOutput ("SyntaxError");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE3)
{
    istringstream input ("float f; f = 5.. ;");
    istringstream expectedOutput ("SyntaxError");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testE4)
{
    istringstream input ("float ;");
    istringstream expectedOutput ("SyntaxError");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine1) //production: declaration
{
    istringstream input ("char a; char* b; int c; int* d; float e; float* e;");
    istringstream expectedOutput ("char a ; char * b ; int c ; int * d ; float e ; float * e ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine2) //production: assignment_expression_1
// including int_assignment_expression_1
{
    istringstream input ("a %= b = c; d <<= e = f; g >>= h = i;");
    istringstream expectedOutput ("a %= b = c ; d <<= e = f ; g >>= h = i ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine3) //production: int_assignment_expression_2
{
    istringstream input ("a %= b <<= c; d <<= e >>= f;");
    istringstream expectedOutput ("a %= b <<= c ; d <<= e >>= f ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine4) //production: int_assignment_expression_3
// including int_ternary_expression_1
{
    istringstream input ("a %= b ? c <<= d : e;");
    istringstream expectedOutput ("a %= b ? c <<= d : e ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine5) //production: int_ternary_expression_2
// including int_comp_expression_1
// including int_postfix_expression_1
{
    istringstream input ("a %= ~~~~~b--;");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ b -- ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine6) //production: int_ternary_expression_2
// including int_comp_expression_1
// including int_postfix_expression_2
// including int_primary_expression_1
{
    istringstream input ("a %= ~~~~~b;");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ b ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine7) //production: int_ternary_expression_2
// including int_comp_expression_1
// including int_postfix_expression_2
// including int_primary_expression_2
{
    istringstream input ("a %= ~~~~~'c';");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ 'c' ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine8) //production: int_ternary_expression_2
// including int_comp_expression_1
// including int_postfix_expression_2
// including int_primary_expression_3
{
    istringstream input ("a %= ~~~~~(b<<=c);");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ ( b <<= c ) ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine9) //production: int_ternary_expression_2
// including int_comp_expression_1 with added int_comp_expression(s)
// including int_postfix_expression_2
// including int_primary_expression_3
{
    istringstream input ("a %= ~~~~~(b<<=c) / ~~~~~(b<<=c) % ~~~~~(b<<=c) / ~~~~~(b<<=c) % ~~~~~(b<<=c) % ~~~~~(b<<=c);");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ ( b <<= c ) / ~ ~ ~ ~ ~ ( b <<= c ) % ~ ~ ~ ~ ~ ( b <<= c ) /  ~ ~ ~ ~ ~ ( b <<= c ) % ~ ~ ~ ~ ~ ( b <<= c ) % ~ ~ ~ ~ ~ ( b <<= c ) ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine10) //production: int_ternary_expression_2
// including int_comp_expression_1 with added div_expression(s)
// including int_postfix_expression_2
// including int_primary_expression_2
{
    istringstream input ("a %= ~~~~~'c'/~~~~~'c' + ~~~~~'c'%~~~~~'c' - ~~~~~'c'%~~~~~'c' - ~~~~~'c'%~~~~~'c' - ~~~~~'c'%~~~~~'c';");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ 'c' / ~ ~ ~ ~ ~ 'c' + ~ ~ ~ ~ ~ 'c' % ~ ~ ~ ~ ~ 'c' - ~ ~ ~ ~ ~ 'c' % ~ ~ ~ ~ ~ 'c' - ~ ~ ~ ~ ~ 'c' % ~ ~ ~ ~ ~ 'c' - ~ ~ ~ ~ ~ 'c' % ~ ~ ~ ~ ~ 'c' ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine11) //production: int_ternary_expression_2
// including int_comp_expression_1 with added additive_expression(s)
// including int_postfix_expression_2
// including int_primary_expression_1
{
    istringstream input ("a %= ~~~~~b << ~~~~~b >> ~~~~~b << ~~~~~b >> ~~~~~b;");
    istringstream expectedOutput ("a %= ~ ~ ~ ~ ~ b << ~ ~ ~ ~ ~ b >> ~ ~ ~ ~ ~ b << ~ ~ ~ ~ ~ b >> ~ ~ ~ ~ ~ b ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine12) //production: assignment_expression_2
{
    istringstream input ("a /= b += c -= d = e;");
    istringstream expectedOutput ("a /= b += c -= d = e ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine13) //production: assignment_expression_3
// including ternary_exp_1
{
    istringstream input ("a ? b = 2.5 : g;");
    istringstream expectedOutput ("a ? b = 2.5 : g ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine14) //production: declaration
{
    istringstream input ("char* a; ");
    istringstream expectedOutput ("char * a ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine15) //production: declaration
{
    istringstream input ("int* b; ");
    istringstream expectedOutput ("int * b ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine16) //production: declaration
{
    istringstream input ("float* c; ");
    istringstream expectedOutput ("float * c ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine17) //production: int_assignment_exp 1
{
    istringstream input ("a %= e++; ");
    istringstream expectedOutput ("a %= e ++ ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine18) //production: int_assignment_exp 2
{
    istringstream input ("a %= ~e++; ");
    istringstream expectedOutput ("a %= ~ e ++ ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine19) //production: int_assignment_exp 3
{
    istringstream input ("a%=b;");
    istringstream expectedOutput ("a %= b ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine20) //production: int_assignment_exp 4
{
    istringstream input ("a%=5;");
    istringstream expectedOutput ("a %= 5 ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine21) //production: int_assignment_exp 5
{
    istringstream input ("a%=(a<<=b);");
    istringstream expectedOutput ("a %= ( a <<= b ) ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine22) //production: int_assignment_exp 6
{
    istringstream input ("a %= ~e++/~e++%~e++/e--; ");
    istringstream expectedOutput ("a %= ~ e ++ / ~ e ++ % ~ e ++ / e -- ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine23) //production: int_assignment_exp 7
{
    istringstream input ("float e; float a; a = e++/e++ + e++/e++ - e++/e++ + e++/e++; ");
    istringstream expectedOutput (" float e ; float a ; a = e ++ / e ++ + e ++ / e ++ - e ++ / e ++ + e ++ / e ++ ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine24) //production: int_assignment_exp 8
{
    istringstream input ("a %= ~e++/5 + ~e++%~e++ << ~e++/~e++ - 2%~e++ >> ~e++/~e++ + ~e++%~e++ << ~e++/~e++ - ~e++%~(a >>= b);");
    istringstream expectedOutput ("a %= ~ e ++ / 5 + ~ e ++ % ~ e ++ << ~ e ++ / ~ e ++ - 2 % ~ e ++ >> ~ e ++ / ~ e ++ + ~ e ++ % ~ e ++ << ~ e ++ / ~ e ++ - ~ e ++ % ~ ( a >>= b ) ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine25) //production: additive_exp 1
{
    istringstream input ("char b; b = 'c';");
    istringstream expectedOutput ("char b ; b = 'c' ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine26) //production: additive_exp 2
{
    istringstream input ("char b; (b = 'f');");
    istringstream expectedOutput ("char b ; ( b = 'f' ) ; ");
    check(input, expectedOutput);
}

BOOST_AUTO_TEST_CASE(testMine27) //production: additive_exp 3
{
    istringstream input ("float b; b = 5.67;");
    istringstream expectedOutput ("float b ; b = 5.67 ; ");
    check(input, expectedOutput);
}
