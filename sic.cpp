#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <map>
#include "tables.h" // Include external header for helper functions and predefined tables

using namespace std;

// Function to remove extra spaces from a line of code
string removeExtraSpaces(const std::string& str) {
    istringstream iss(str);
    string word;
    ostringstream oss;

    bool firstWord = true;
    while (iss >> word) {
        if (!firstWord) {
            oss << " ";  // Add a single space before each word except the first
        }
        oss << word;
        firstWord = false;
    }
    return oss.str();
}

int main() {
    // Initializing OPTAB and SYMTAB maps for mnemonics and symbols
    map<string, string> OPTAB;
    map<string, string> SYMTAB;

    // Populate OPTAB with mnemonic opcodes from helper function
    createOptab(OPTAB);

    // Open input file containing assembly code
    ifstream infile("code1.txt"); 
    // ifstream infile("code.txt"); 

    if (!infile) {
        cerr << "Error opening file." << endl;
        return 1; 
    }

    string line;
    int lineCount = 0;

    // Count lines in input file
    while (getline(infile, line)) {
        lineCount++;
    }

    // Reset file read position
    infile.clear(); 
    infile.seekg(0); 

    // Initialize a 2D vector to store parsed lines from input file
    vector<vector<string>> words(lineCount, vector<string>(5)); 
    int currentLine = 0;

    // Read each line, split it into words, and store in 2D vector
    while (getline(infile, line) && currentLine < lineCount) {
        line = removeExtraSpaces(line);
        istringstream iss(line);
        string word;
        int wordCount = 1;
        int length = 0;

        // Extract up to the first two words and store in vector
        while (iss >> word && wordCount < 3) {
            words[currentLine][wordCount] = word;
            wordCount++;
            length += word.length() + 1; // +1 accounts for the space between words
        }

        // Store remaining line as third column if available
        if (length < line.size()) {
            words[currentLine][3] = line.substr(length);
        } else {
            words[currentLine][3] = "-"; // Assign "-" if no content remains
        }

        currentLine++;
    }

    infile.close();
    
    // Initialize starting address from the first line
    int Loc = stoi(words[0][3], nullptr, 16);   
    int startingAddress = Loc;
    words[1][0] = words[0][3];                 

    // Calculate address for each line, update SYMTAB with symbols and their addresses
    for (int i = 1; i < lineCount - 1; i++) {
        string instruction = words[i][2];
        
        // Calculate location counter based on instruction type
        if (OPTAB.find(instruction) != OPTAB.end() || instruction == "WORD") {
            Loc += 3;
        } else if (instruction == "BYTE") {
            string str = words[i][3];
            if (str[0] == 'X') {
                Loc += (str.length() - 3) / 2; // Hexadecimal length adjustment
            } else if (str[0] == 'C') {
                Loc += str.length() - 3; // Character length adjustment
            }
        } else if (instruction == "RESW") {
            Loc += 3 * stoi(words[i][3]);
        } else if (instruction == "RESB") {
            Loc += stoi(words[i][3]);
        } 

        // Format Loc as hex string with leading zeros and store in vector
        ostringstream oss;
        oss << hex << uppercase << setw(4) << setfill('0') << Loc;
        if (i + 1 < lineCount - 1)
            words[i + 1][0] = oss.str();

        // Insert symbol into SYMTAB if present
        if (words[i][1] != "-") {
            if (createSymtab(SYMTAB, words[i][0], words[i][1])){
                continue;
            } else {
                cout << "Error !!\nDuplicate Symbol";
            }
        }
    }

    // Generate object code for each line based on the instruction type
    for (int i = 1; i < lineCount - 1; i++) {
        string instruction = words[i][2];
        int x = 0;
        string objectCode = ""; 

        if (instruction == "RSUB") {
            objectCode += "4C0000";
        } else if (OPTAB.find(instruction) != OPTAB.end()) {
            // Handle instructions in OPTAB
            string opcode = OPTAB.at(instruction);
            string str = words[i][3];
            string label = "";

            for (char ch : str) {
                if (ch != ',') {
                    label += ch;
                } else {
                    x = 1; // Indexed addressing
                    break;
                }
            }

            // Retrieve label address from SYMTAB if available
            string add;
            if (SYMTAB.count(label)) {
                add = SYMTAB.at(label);
            } else {
                cerr << "Warning: Label " << label << " not found in SYMTAB." << endl;
                continue;
            }

            if (x == 1) { // Apply indexed addressing adjustment
                int address = stoi(add, nullptr, 16);
                address |= 0x8000; 

                ostringstream oss;
                oss << hex << uppercase << setw(4) << setfill('0') << address;
                add = oss.str();
            }

            objectCode += opcode;
            objectCode += add;
        } else if (instruction == "WORD") {
            // Handle WORD directive
            int value = stoi(words[i][3]);
            ostringstream oss;
            oss << hex << uppercase << setw(6) << setfill('0') << value;
            objectCode = oss.str();
        } else if (instruction == "BYTE") {
            // Handle BYTE directive
            string str = words[i][3];
            string obj = "";

            if (str[0] == 'X') {
                for (int j = 2; j < str.length() - 1; j++) {
                    obj += str[j];
                }
                objectCode = obj;
            } else if (str[0] == 'C') {
                for (int j = 2; j < str.length() - 1; j++) {
                    int ch = str[j];
                    ostringstream oss;
                    oss << hex << uppercase << setw(2) << setfill('0') << ch;
                    obj += oss.str();
                }
                objectCode = obj;
            } 
        }

        words[i][4] = objectCode; // Store object code in vector
    }

    // Display program with object codes
    cout << "The program with object codes is : \n";
    for (int i = 0; i < lineCount; ++i) {
        for (const auto& w : words[i]) {
            cout << w << " "; // Print each word in the line
        }
        cout << endl;
    }

    int lengthProgram = Loc - startingAddress;
    ostringstream oss;
    oss << hex << uppercase << setw(4) << setfill('0') << lengthProgram;
    string len = oss.str();

    // Generate and display object program
    cout << "\nObject Program:\n";

    // Header record
    string programName = words[0][1];
    cout << "H" << setw(6) << left << programName.substr(0, 6) 
         << setfill('0') << right << setw(6) << hex << uppercase << startingAddress
         << setw(6) << len << endl;

    // Text records
    int textRecordStart = startingAddress;
    int textRecordLength = 0;
    string objectCodeText = "";
    string textRecord = "T";

    for (int i = 1; i < lineCount - 1; i++) {
        if (!words[i][4].empty()) {
            int wordLength = words[i][4].length() / 2;

            if (textRecordLength + wordLength > 30) {
                // Output text record and reset
                cout << textRecord 
                     << setw(6) << setfill('0') << hex << textRecordStart
                     << setw(2) << textRecordLength << objectCodeText << endl;

                textRecordStart = stoi(words[i][0], nullptr, 16);
                textRecordLength = 0;
                objectCodeText = "";
            }

            textRecordLength += wordLength;
            objectCodeText += words[i][4];
        } else {
            if (textRecordLength != 0){
                cout << textRecord 
                        << setw(6) << setfill('0') << hex << textRecordStart
                        << setw(2) << textRecordLength << objectCodeText << endl;

                textRecordStart = stoi(words[i][0], nullptr, 16);
                textRecordLength = 0;
                objectCodeText = "";
            } else {
                textRecordStart = stoi(words[i+1][0], nullptr, 16);
            }
        }
    }

    if (!objectCodeText.empty()) {
        cout << textRecord 
             << setw(6) << setfill('0') << hex << textRecordStart
             << setw(2) << textRecordLength << objectCodeText << endl;
    }

    // End record
    cout << "E" << setw(6) << setfill('0') << hex << uppercase << startingAddress << endl;

    return 0;
}  



/*
g++ -std=c++11 -o sic sic.cpp tables.cpp
*/