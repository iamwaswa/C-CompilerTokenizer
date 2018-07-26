//============================================================================
// Name        : dirTree_Test.cpp
// Author      : Mohammad Akbari
// Description : Course Project Part 3 (Dictionary Tree) - Testing Scheme
//============================================================================

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE Course Project Part 3
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
using namespace boost;

#include <sstream>
#include <fstream>
#include <iostream>

#include "makeDirectoryTree.hpp"
#include "Directory.hpp"

#define DONT_RUN_MAIN

using namespace std;

void check(std::string outputFile)
{
	ifstream fin(outputFile);
	std::string strDesired;
	stringstream sstr(OUT.str());
	cout << OUT.str();
	std::string line_str;
	if (fin.is_open())
	{
		while ( getline (fin,strDesired) && getline(sstr, line_str) )
		{
			trim(line_str); trim(strDesired);
			if(strDesired!=line_str)
			  BOOST_ERROR(line_str + "   !=   " + strDesired);
		}
		fin.close();
	}
}

BOOST_AUTO_TEST_CASE(test1) {
	std::cout << "***** test1 *****" << endl;
	auto root_dir1P = make_directory_tree("./testDirectory");  // function to make directory tree
	if (root_dir1P) {
		root_dir1P->print_traverse();
		check("test1.txt");
	}
}

BOOST_AUTO_TEST_CASE(test2) {
	std::cout << "***** test2 *****" << endl;
	OUT.str(""); // Clear the stream for the next test case!
	auto root_dir1P = make_directory_tree("./testDirectory/Notes");  // function to make directory tree
	if (root_dir1P) {
		root_dir1P->print_traverse();
		check("test2.txt");
	}
}

// copy construction
BOOST_AUTO_TEST_CASE(test3) {
	std::cout << "***** test3 *****" << endl;
	OUT.str(""); // Clear the stream for the next test case!
	auto root_dir1P = make_directory_tree("./testDirectory");  // function to make directory tree

	if (root_dir1P) {
		auto root_dir2 = Directory(*root_dir1P);
		root_dir2.print_traverse();
		check("test3.txt");
	}
}

// overloaded copy assignment operation
BOOST_AUTO_TEST_CASE(test4) {
	std::cout << "***** test4 *****" << endl;
	OUT.str(""); // Clear the stream for the next test case!
	auto root_dir1P = make_directory_tree("./testDirectory");  // function to make directory tree
	if (root_dir1P) {
		auto root_dir2 = Directory(*root_dir1P); // copy construction
		auto root_dir3P = make_directory_tree("./testDirectory/Lectures");
		if (root_dir3P) {
			root_dir2 = *root_dir3P; // overloaded copy assignment operation
		}
		root_dir2.print_traverse();
		check("test4.txt");
	}
}

BOOST_AUTO_TEST_CASE(test5) {
	std::cout << "***** test5 *****" << endl;

	Directory dirTest("test");

	dirTest.set_dir_name("test2");
	if(dirTest.get_dir_name()!="test2")
		BOOST_ERROR("Failed in (set_dir_name)!");

	dirTest.add_file("fileTest.txt");
	vector<string> files = dirTest.get_files();
	if(!files.size() || files.back()!="fileTest.txt")
		BOOST_ERROR("Failed in (add_file)!");

	Directory dirTest2("test2");
	files.clear();
	files.push_back("fileTest1.txt");
	files.push_back("fileTest2.txt");
	files.push_back("fileTest3.txt");
	dirTest2.add_multiple_files(files);
	vector<string> userFiles = dirTest2.get_files();
	if(files!=userFiles)
		BOOST_ERROR("Failed in (add_multiple_files)!");

}
