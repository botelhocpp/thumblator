#ifndef THUMBLATOR_H
#define THUMBLATOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>

using namespace std;

string stringHex(int number);

string decodeInstructions(int numeric_opcode, int line_number);

#endif 
