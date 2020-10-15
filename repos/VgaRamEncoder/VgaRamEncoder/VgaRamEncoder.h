#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

using namespace std;

int displayUsage(char* name);
int displayUsage(char* name, int page);
void writeHexLine(ofstream* file, int& addr, char curChar, char color);
int parseEscapeSequence(char& toWrite, int& escLength, const string& line, int loc);
int display(string file);

enum error_codes {
	SUCCESS,
	HELP_INDEX_OUT_OF_BOUNDS,
	HELP_REQUIRES_ARGUMENT,
	HELP_PAGE_INVALID,
	FLAG_REQUIRES_ARGUMENT,
	UNKNOWN_ARGUMENT,
	TOO_MANY_ARGUMENTS,
	TOO_FEW_ARGUMENTS,
	HDATA_NULL,
	TEMPLATE_FILE_NOT_OPEN,
	SOURCE_FILE_NOT_OPEN,
	HEX_FILE_NOT_OPEN,
	HRES_NULL,

	ESCAPE_SEQUENCE_DATA_VALID,
	ESCAPE_SEQUENCE_INVALID,
	ESCAPE_SEQUENCE_HEX_INVALID,
	ESCAPE_SEQUENCE_BIN_OUT_OF_RANGE,
	ESCAPE_SEQUENCE_BIN_EMPTY,
	ESCAPE_SEQUENCE_BIN_INVALID,
	ESCAPE_SEQUENCE_DEC_OUT_OF_RANGE,
	ESCAPE_SEQUENCE_DEC_EMPTY,
	ESCAPE_SEQUENCE_COLOR_VALID,
	ESCAPE_SEQUENCE_DEC_INVALID
};