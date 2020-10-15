#include "VgaRamEncoder.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include "VGADisplay.h"
#include "resource.h"

using namespace std;

int displayUsage(char* name) {
	return displayUsage(name, 1);
}

int displayUsage(char* name, int page) {
	int status = SUCCESS;
	if (page < 1 || page > 2) {
		cerr << "Error: Help page index '" << page << "' out of range. Must be from 1 - 2\n";
		status = HELP_INDEX_OUT_OF_BOUNDS;
	}
	if (page == 2) {
		cout << "Command Detail:\n" //----------------------------------------------------------|
			<< "    The VGA Ram Encoder converts an 80 by 30 character, UTF-8 encoded text file\n"
			<< "    into an Intel format .HEX file which follows the encoding:\n\n"
			<< "            +0   +0    +1\n"
			<< "           _______________\n"
			<< "    0x00  | FG | BG | CHAR\n"
			<< "    0x02  | FG | BG | CHAR\n\n"
			<< "    FG and BG are the 4-bit foreground and background colors respectively. CHAR\n"
			<< "    is the 8-bit character code. The resulting .HEX file contains 2400 words.\n\n"
			<< "    The produced .HEX file is therefore made up of 16-bit (2 byte) words. Using\n"
			<< "    a wordsize other than 16-bits to initialize the VGA text mode memory may\n"
			<< "    lead to unexpected behavior. A .HEX formatted memory initialization file\n"
			<< "    created outside this utility can always be verified by specifying the '-d'\n"
			<< "    flag (See help page 1).\n\n"
			<< "Page (2 / 2)\n";
		return status;
	}
	char* displayname = strrchr(name, '\\');
	cout << "Usage:\n" //93 <- is the last char
		<< "    " << (displayname ? (displayname + 1) : name) << " [-h:<page> | --help:<page>] [-g | --template] [-v | --verbose] [(-o | --output) out_file] [-d (d_file)] in_file\n\n"
		<< "    VGA Ram Encoder - Converts an 80 by 30 character, UTF-8 encoded text file\n"
		<< "    into an Intel format .HEX file (See help page 2 for more information)\n\n"
		<< "Where\n"
		<< "    in_file               Specifies the text file to convert\n\n"
		<< "Options:\n"
		<< "    -h, --help            Display this help message\n"
		<< "    -h:<page>             Display page <page> of the help. For example, -h:2 for\n"
		<< "                          page 2. This is page 1.\n"
		<< "    -g, --template        Output a template.txt file containing the proper\n"
		<< "                          number of rows and columns instead of converting an\n"
		<< "                          input file\n"
		<< "    -v, --verbose         Enable verbose output\n"
		<< "    -o, --output out_file Specify the output filename\n"
		<< "    -d (d_file)           Display a visual showing how the contents of d_file should\n"
		<< "                          look when displayed via VGA. If d_file is not specified,\n"
		<< "                          in_file is displayed once the encoding is complete. If d_file\n"
		<< "                          is specified, ENCODING WILL NOT TAKE PLACE. Instead, the\n"
		<< "                          contents of d_file will be displayed.\n\n"
		<< "Page (1 / 2)\n";
	return status;
}

void writeHexLine(ofstream* file, int &addr, char curChar, char color) {
	*file << ":02"; //Start code and byte count
	*file << setw(4) << addr; //addr in 2 byte hex
	*file << "00"; //Type is data
	*file << setw(2) << (+color & 0xFF) << setw(2) <<  (+curChar & 0xFF); //data
	int checksum = (~(0x02 + (addr & 0xFF) + ((addr >> 8) & 0xFF) + 0x00 + +color + +curChar) + 1) & 0xFF;
	*file << setw(2) << checksum << "\n"; //checksum and CR + LF
	++addr;
}

int parseEscapeSequence(char &toWrite, int &escLength, const string &line, int loc) {
	//Format:
	//Escape sequences can be used to define raw bytes:
	// \0xFF or \0d255 or \0d5 or \0b10101010
	//Escape sequences can be used to define color:
	// \^FF or \^0xFF
	escLength = 0;
	if (line[loc] == '^') {
		if (line[loc + 1] == '0' && (line[loc + 2] == 'x' || line[loc + 2] == 'X')) {
			loc = loc + 2;
			escLength += 2;
		}
		escLength += 3;
		try {
#pragma warning( suppress : 4244)
			toWrite = stoul(line.substr(loc+1, 2), nullptr, 16);
			return ESCAPE_SEQUENCE_COLOR_VALID;
		}
		catch (invalid_argument) {
			return ESCAPE_SEQUENCE_HEX_INVALID;
		}
	}
	else if (line[loc] == '0') {
		escLength += 1;
		if (line[loc + 1] == 'x' || line[loc + 1] == 'X') {
			escLength += 3;
			try {
#pragma warning( suppress : 4244)
				toWrite = stoul(line.substr(loc + 2, 2), nullptr, 16);
				return ESCAPE_SEQUENCE_DATA_VALID;
			}
			catch (invalid_argument) {
				return ESCAPE_SEQUENCE_HEX_INVALID;
			}
		}
		else if (line[loc + 1] == 'b' || line[loc + 1] == 'B') {
			escLength += 1;
			stringstream ss;
			int binLength = 0;
			for (unsigned int i = loc + 2; i < line.length(); ++i, ++binLength) {
				if (line[i] == '0' || line[i] == '1') {
					ss << line[i];
				}
				else {
					if (binLength > 0) {
						break;
					}
					else {
						return ESCAPE_SEQUENCE_BIN_EMPTY;
					}
				}
			}
			escLength += binLength;
			try {
				int binNum = stoul(ss.str(), nullptr, 2);
				if (binNum > 255) {
					return ESCAPE_SEQUENCE_BIN_OUT_OF_RANGE;
				}
#pragma warning( suppress : 4244)
				toWrite = binNum;
				return ESCAPE_SEQUENCE_DATA_VALID;
			}
			catch (invalid_argument) {
				return ESCAPE_SEQUENCE_BIN_INVALID;
			}
		}
		else if (line[loc + 1] == 'd' || line[loc + 1] == 'D') {
			escLength += 1;
			stringstream ss;
			int decLength = 0;
			for (unsigned int i = loc + 2; i < line.length(); ++i, ++decLength) {
				if (decLength == 4) {
					return ESCAPE_SEQUENCE_DEC_OUT_OF_RANGE;
				}
				if (line[i] - '0' >= 0 && line[i] - '9' <= 9) {
					ss << line[i];
				}
				else {
					if (decLength > 0) {
						break;
					}
					else {
						return ESCAPE_SEQUENCE_DEC_EMPTY;
					}
				}
			}
			escLength += decLength;
			try {
				int toWriteDec;
				toWriteDec = stoul(ss.str(), nullptr, 10);
				if (toWriteDec > 255) {
					return ESCAPE_SEQUENCE_DEC_OUT_OF_RANGE;
				}
#pragma warning( suppress : 4244)
				toWrite = toWriteDec;
				return ESCAPE_SEQUENCE_DATA_VALID;
			}
			catch (invalid_argument) {
				return ESCAPE_SEQUENCE_DEC_INVALID;
			}
		}
		else {
			return ESCAPE_SEQUENCE_INVALID;
		}
	}
	else {
		return ESCAPE_SEQUENCE_INVALID;
	}
}

int display(string file) {
	VGADisplay display(file, GetStdHandle(STD_OUTPUT_HANDLE));
	int row;
	int condition = display.display(row, cout);
	switch (condition) {
	case VGADisplay::error_codes::VGADISPLAY_SUCCESS:
		break;
	case VGADisplay::error_codes::VGADISPLAY_EMPTY_LINE:
		cerr << "Error: Line " << (row + 1) << " is empty\n";
		break;
	case VGADisplay::error_codes::VGADISPLAY_MISSING_START_CODE:
		cerr << "Error: Missing start code on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_BYTE_COUNT_NOT_2:
		cerr << "Error: Byte should be 2 on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_DUPLICATE_ADDRESS:
		cerr << "Error: Duplicate address on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_DECENDING_ADDRESS:
		cerr << "Error: Unexpected decending address on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_UNEXPECTED_RECORD_TYPE:
		cerr << "Error: Unexpected record type on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_PREMATURE_EOF:
		cerr << "Error: Premature EOF on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_INVALID_ADDRESS:
		cerr << "Error: Invalid address on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_INVALID_COLOR_CODE:
		cerr << "Error: Invalid color code on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_INVALID_CHARACTER_CODE:
		cerr << "Error: Invalid character code on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_INVALID_CHECKSUM_FORMAT:
		cerr << "Error: Invalid checksum format on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_CHECKSUM_INVALID:
		cerr << "Error: Checksum provided does not match calculated on line " << (row + 1) << '\n';
		break;
	case VGADisplay::error_codes::VGADISPLAY_CANNOT_OPEN_FILE:
		cerr << "Error: Could not open file '" << file << "' for display\n";
		break;
	case VGADisplay::error_codes::VGADISPLAY_CANNOT_OBTAIN_CON_INFO:
		cerr << "Could not obtain console information. (consoleapi2.h returned " << row << ")\n";
		break;
	default:
		cerr << "Error: Unknown error on line " << (row + 1) << '\n';
		break;
	}
	return condition;
}

int main(int argc, char* argv[]) {
	bool generate = false;
	bool verbose = false;
	bool displayAfter = false;
	string source;
	string output;
	
	//Parse arguments
	//first check if help flag exists at all. If it does, ignore everything else, print usage and exit
	//Better way to parse help pages? This feels inefficient
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			displayUsage(argv[0]);
			return SUCCESS;
		}
		if (strcmp(argv[i], "--help") > 0 && argv[i][6] == ':') { // > 0 means first arg longer
			//Greater than 0 guarentees there's a 6th char. If it's 0, the check for ':' fails
			//So, checking for 7th is ok, but it might be the null char
			if (argv[i][7] == 0) {
				cerr << "Error: Parameter '--help:' requires an argument\n";
				displayUsage(argv[0]);
				return HELP_REQUIRES_ARGUMENT;
			}
			try {
				displayUsage(argv[0], stoul(&argv[i][7], nullptr, 10));
				return SUCCESS;
			}
			catch (invalid_argument) {
				cerr << "Error: Invalid help page '" << argv[i][7] << "'\n";
				displayUsage(argv[0], 1);
				return HELP_PAGE_INVALID;
			}
			return SUCCESS;
		}
		else if (strcmp(argv[i], "-h") > 0 && argv[i][2] == ':') {
			if (argv[i][3] == 0) {
				cerr << "Error: Parameter '-h:' requires an argument\n";
				displayUsage(argv[0]);
				return HELP_REQUIRES_ARGUMENT;
			}
			try {
				displayUsage(argv[0], stoul(&argv[i][3], nullptr, 10));
				return SUCCESS;
			}
			catch (invalid_argument) {
				cerr << "Error: Invalid help page '" << argv[i][3] << "'\n";
				displayUsage(argv[0], 1);
				return HELP_PAGE_INVALID;
			}
			return SUCCESS;
		}
	}

	//iterate through each argument
	for (int i = 1; i < argc; ++i) {
		//check if arg starts with a hyphen and is thus a flag
		if (argv[i][0] == '-') {
			//This is a flag. Let's check each possible argument to see which one it is
			if (strcmp(argv[i], "-g") == 0 || strcmp(argv[i], "--template") == 0) {
				generate = true;
			}
			else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
				verbose = true;
			}
			else if (strcmp(argv[i], "-d") == 0) {
				if (i == argc - 1 || argv[i + 1][0] == '-') {
					displayAfter = true;
				}
				else {
					return display(argv[i + 1]);
				}
			}
			else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
				if (i == argc - 1 || argv[i + 1][0] == '-') {
					cout << "Error: '" << argv[i] << "' requires an argument\n";
					displayUsage(argv[0]);
					return FLAG_REQUIRES_ARGUMENT;
				}
				++i;
				output.assign(argv[i]);
			}
			else {
				cout << "Error: Unknown argument '" << argv[i] << "'\n";
				displayUsage(argv[0]);
				return UNKNOWN_ARGUMENT;
			}
		}
		else {
			if (source.empty()) {
				source.assign(argv[i]);
			}
			else {
				cout << "Error: Too many arguments\n"
					<< "Here---> " << argv[i] << "\n\n";
				displayUsage(argv[0]);
				return TOO_MANY_ARGUMENTS;
			}
		}
	}

	if (generate) {
		string result;
		char* data = NULL;
		HRSRC hRes = FindResource(NULL, MAKEINTRESOURCE(IDR_TEXT1), MAKEINTRESOURCE(TEXTFILE));
		if (hRes != NULL) {
			HGLOBAL hData = LoadResource(NULL, hRes);
			if (hData) {
				DWORD dataSize = SizeofResource(NULL, hRes);
				data = (char*)LockResource(hData);
			}
			else {
				cerr << "Error: Could not find template file to generate. Additional information:\n";
				cerr << "hData is NULL\n";
				return HDATA_NULL;
			}
			ofstream exFile;
			exFile.open("template.txt", ios::binary);
			if (exFile.is_open()) {
				exFile << data;
				exFile.close();
				return SUCCESS;
			}
			else {
				cerr << "Error: Could not create file 'template.txt'\n";
				return TEMPLATE_FILE_NOT_OPEN;
			}
		}
		else {
			cerr << "Error: Could not find template file to generate. Additional information:\n";
			cerr << "hRes is NULL\n";
			return HRES_NULL;
		}
	}

	if (source.empty()) {
		cout << "Error: Too few arguments. Input file must be specified\n\n";
		displayUsage(argv[0]);
		return TOO_FEW_ARGUMENTS;
	}
	if (output.empty()) {
		output = "out.hex";
	}

	ofstream hexFile;
	hexFile.open(output, ios::out | ios::trunc);

	ifstream sourceFile(source);
	string line;
	int addr = 0;

	if (hexFile.is_open()) {
		if (sourceFile.is_open()) {
			hexFile << hex << setfill('0'); //Set up stream manipulators
			for (int i = 0; i < 30; ++i) {
				if (verbose) {
					cout << "Writing line " << i << '\n';
				}
				unsigned int curChar = 0;
				unsigned int curCharWrite = 0;
				unsigned char color = 0x70;
				if (getline(sourceFile, line)) {
					while (curChar < line.length()) {
						if (line[curChar] == '\\') {
							++curChar;
							if (line[curChar] == '\\') {
								writeHexLine(&hexFile, addr, line[curChar], color);
								++curChar;
								++curCharWrite;
							}
							else {
								int type, escLength;
								char toWrite;
								type = parseEscapeSequence(toWrite, escLength, line, curChar);
								switch (type) {
								case ESCAPE_SEQUENCE_DATA_VALID:
									writeHexLine(&hexFile, addr, toWrite, color);
									++curCharWrite;
									break;
								case ESCAPE_SEQUENCE_INVALID:
									cerr << "Error: Invalid escape sequence at column " << curChar << ", row " << (i + 1) << '\n';
									cerr << "Must be '\\0x', '\\0b', or '\\0d' to define a HEX, binary, or decimal literal\n";
									return type;
								case ESCAPE_SEQUENCE_HEX_INVALID:
									cerr << "Error: Invalid HEX literal at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_BIN_OUT_OF_RANGE:
									cerr << "Error: Binary literal exceeds 1 byte at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_BIN_EMPTY:
									cerr << "Error: Empty binary literal at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_BIN_INVALID:
									cerr << "Error: Invalid binary literal at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_DEC_OUT_OF_RANGE:
									cerr << "Error: Decimal literal exceeds 1 byte at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_DEC_EMPTY:
									cerr << "Error: Empty decimal literal at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								case ESCAPE_SEQUENCE_COLOR_VALID:
									color = toWrite;
									break;
								case ESCAPE_SEQUENCE_DEC_INVALID:
									cerr << "Error: Invalid decimal literal at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								default:
									cerr << "Error: Unknown error occured while parsing escape sequence: '" << type << "' at column " << curChar << ", row " << (i + 1) << '\n';
									return type;
								};
								curChar += escLength;
							}
						}
						else {
							writeHexLine(&hexFile, addr, line[curChar], color);
							++curChar;
							++curCharWrite;
						}

						if (curCharWrite == 80) {
							if (line[curChar] != 0) {
								cout << "Warning: Line " << (i + 1) << " exceeds 80 characters and will be truncated\n";
							}
							break;
						}
					}
					while (curCharWrite < 80) {
						writeHexLine(&hexFile, addr, 0, 0);
						++curCharWrite;
					}
				} else {
					while (curCharWrite < 80) {
						writeHexLine(&hexFile, addr, 0, 0);
						++curCharWrite;
					}
				}
			}
			if (getline(sourceFile, line) && !(line.empty() || line == "\n")) {
				cout << "Warning: File exceeds 30 rows. Extra rows will be ignored\n";
			}
			hexFile << ":00000001FF\n";
			cout << "Success! File saved to '" << output << "'\n";
		} else {
			cerr << "Error: Could not read file '" << source << "'\n";
			return SOURCE_FILE_NOT_OPEN;
		}
	} else {
		cerr << "Error: Could not create file '" << output << "'\n";
		return HEX_FILE_NOT_OPEN;
	}
	hexFile.close();

	if (displayAfter) {
		return display(output);
	}
	return SUCCESS;
}