#include "thumblator.hpp"
#include "cmath"

string stringHex(int number){
	string hex_string = "0000";
	int step = 3;
	while(number != 0){
		hex_string[step--] = (number % 0x10 <= 0x09) ? 
            (number % 0x10) + 0x30: 
            (number % 0x10) + 0x57;
		number /= 0x10;
	}
	return hex_string;
}

string decodeInstructions(int numeric_opcode, int line_number){
    string opcode;
    switch((numeric_opcode >> 12) & 0xF){
    case 0x0:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "LSR" : "LSL";
        opcode += " r" + to_string(numeric_opcode & 0x7) 
               + ", r" + to_string((numeric_opcode >> 3) & 0x7) 
               + ", #" + to_string((numeric_opcode >> 6) & 0x1F);
        break;

    case 0x01:
        if ((numeric_opcode >> 11) & 0x1){
            opcode = ((numeric_opcode >> 9) & 0x1) ? "SUB" : "ADD";
            opcode += " r" + to_string(numeric_opcode & 0x7)
                   + ", r" + to_string((numeric_opcode >> 3) & 0x7);
            opcode += ((numeric_opcode >> 10) & 0x1) ? 
                   ", #" + to_string((numeric_opcode >> 6) & 0x7):
                   ", r" + to_string((numeric_opcode >> 6) & 0x7);
        }
        else{
            opcode = "ASR r" + to_string(numeric_opcode & 0x7) 
                   + ", r" + to_string((numeric_opcode >> 3) & 0x7) 
                   + ", #" + to_string((numeric_opcode >> 6) & 0x1F);
        }
        break;

    case 0x2:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "CMP" : "MOV";
        opcode += " r" + to_string((numeric_opcode >> 8) & 0x7) 
               + ", #" + to_string(numeric_opcode & 0xFF);
        break;

    case 0x3:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "SUB" : "ADD";
        opcode += " r" + to_string((numeric_opcode >> 8) & 0x7) 
               + ", #" + to_string(numeric_opcode & 0xFF);
        break;

    case 0x5:
        switch((numeric_opcode >> 9) & 0x7){
            case 0:
                opcode = "STR";
                break;
            case 1:
                opcode = "STRH";
                break;
            case 2:
                opcode = "STRB";
                break;
            case 3:
                opcode = "LDRSB";
                break;
            case 4:
                opcode = "LDR";
                break;
            case 5:
                opcode = "LDRH";
                break;
            case 6:
                opcode = "LDRB";
                break;
            case 7:
                opcode = "LDRSH";
                break;
        }
        opcode += " r" + to_string(numeric_opcode & 0x7) 
               + ", [r" + to_string((numeric_opcode >> 3) & 0x7) 
               + ", r" + to_string((numeric_opcode >> 6) & 0x7) + "]";
        break;

    case 0x6:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "LDR" : "STR";
        opcode += " r" + to_string(numeric_opcode & 0x7) 
               + ", [r" + to_string((numeric_opcode >> 3) & 0x7) 
               + ", #" + to_string(((numeric_opcode >> 6) & 0x1F)*4) + "]";
        break;

    case 0x7:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "LDRB" : "STRB";
        opcode += " r" + to_string(numeric_opcode & 0x7) 
               + ", [r" + to_string((numeric_opcode >> 3) & 0x7) 
               + ", #" + to_string((numeric_opcode >> 6) & 0x1F) + "]";
        break;

    case 0x8:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "LDRH" : "STRH";
        opcode += " r" + to_string(numeric_opcode & 0x7) 
               + ", [r" + to_string((numeric_opcode >> 3) & 0x7) 
               + ", #" + to_string(((numeric_opcode >> 6) & 0x1F)*2) + "]";
        break;

    case 0x9:
        opcode = ((numeric_opcode >> 11) & 0x1) ? "LDR" : "STR";
        opcode += " r" + to_string((numeric_opcode >> 8) & 0x7) 
               + ", [sp, #" + to_string((numeric_opcode & 0xFF)*4) + "]"; 
        break;

    case 0xa:
        opcode = "ADD r" + to_string((numeric_opcode >> 8) & 0x7);
        opcode += ((numeric_opcode >> 11) & 0x1) ? 
            + ", sp, #" + to_string((numeric_opcode & 0xFF)*4): 
            + ", pc, #" + to_string((numeric_opcode & 0xFF)*4); 
        break;

    case 0xb:
        switch ((numeric_opcode >> 9) & 0x3){
            case 0x0:
                opcode = ((numeric_opcode >> 7) & 0x1) ? "SUB " : "ADD ";
                opcode += "pc, #" + to_string((numeric_opcode & 0x7F) * 4);
                break;
            case 0x1:
                switch((numeric_opcode >> 6) & 0x23){
                    case 0x0:
                        opcode = "SXTH ";
                        break;
                    case 0x1:
                        opcode = "SXTB ";
                        break;
                    case 0x2:
                        opcode = "UXTH ";
                        break;
                    case 0x3:
                        opcode = "UXTB ";
                        break;
                    case 0x20:
                        opcode = "REV ";
                        break;
                    case 0x21:
                        opcode = "REV16 ";
                        break;
                    case 0x22:
                        opcode = "REVSH ";
                        break;
                    default: 
                        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: invalid instruction " << stringHex(numeric_opcode) << " at line " << line_number << ".\nAborting.\n";
                        opcode.clear();
                        return opcode;
                }
                opcode += "r" + to_string(numeric_opcode & 0x7) + ", r" + to_string((numeric_opcode >> 3) & 0x7);
                break;

            case 0x2:
                opcode = ((numeric_opcode >> 11) & 0x1) ? "POP{" : "PUSH{";
                for(int loop = 0; loop < 8; loop++){
                    if(numeric_opcode & 1) opcode += "r" + to_string(loop) + ", "; 
                    numeric_opcode >>= 1;
                }
                opcode.erase(opcode.size() - 2, 2);
                if((numeric_opcode >> 8) & 0x1){
                    opcode += ((numeric_opcode >> 11) & 0x1) ? ", pc}" : ", lr}";
                }
                else{
                    opcode += ((numeric_opcode >> 11) & 0x1) ? "}" : "}";
                }
                break;

            case 0x3:
                if ((numeric_opcode >> 11) & 0x1)
                    opcode = "BKPT #" + to_string((numeric_opcode)&0xFF);
                else{
                    if ((numeric_opcode >> 5) & 0x1){
                        opcode = ((numeric_opcode >> 4) & 0x1) ? "CPSID " : "CPSIE ";
                        if(numeric_opcode & 4) opcode += "a";
                        if(numeric_opcode & 1) opcode += "i";
                        if(numeric_opcode & 2) opcode += "f";
                    }
                    else{
                        opcode = "SETEND ";
                        opcode += ((numeric_opcode >> 3) & 0x1) ? "BE" : "LE";
                    }
                }
                break;
        }
        break;

    default:
        cout << "\033[1;37mThumblator:\033[0m \033[1;31mFatal error\033[0m: invalid instruction " << stringHex(numeric_opcode) << " at line " << line_number << ".\nAborting.\n";
        opcode.clear();
        return opcode;
    }
    return opcode;
}

/*
1011 X10R YYYY YYYY

PUSH
1011 0100 0101 0101 PUSH{r0,r2,r4,r6} 
1011 0101 1111 1111 PUSH{r0-r7,lr}    

POP
1011 1100 0101 0101 POP{r0,r2,r4,r6} 
1011 1101 1111 1111 POP{r0-r7,pc}

00000111
00001111
00011111
00111111
01111111
11111111
11111110
11111100
11111000
11110000
11100000


                
                bool f;
                f = false;
                opcode = ((numeric_opcode >> 11) & 0x1) ? "POP{" : "PUSH{";
                if((numeric_opcode >> 11) & 0x1) f = true;
                short itb;
                itb = 0;
                while(itb < 8){
                    if(((numeric_opcode >> itb) & 0x1)){
                        opcode += "r" + to_string(itb) + ", "; 
                    }
                    itb++;
                }        
                opcode.erase(opcode.size() - 2, 2); 
                if((numeric_opcode >> 8) & 0x1) f ? opcode += ", pc" : opcode += ", lr";
                opcode += "}";
                break; 
                


int minRegister(int number){
    int step = 0;
    while(!(number & 1)){
        number >>= 1;
        step++;
    }
    return step;
}

int maxRegister(int number){
    int step = 7;
    while(!(number & 0x80)){
        number <<= 1;
        step--;
    }
    return step;
}
00000111
00001111
00011111
00111111
01111111
11111111
11111110
11111100
11111000
11110000
11100000

string registerList(int number){
    string list_of_registers;
    short counter = 0;
    short minRegister = 0;
    short maxRegister = 0;
    for(int step = 0; step < 8; step++){
        
    }
    if(counter > 2)
        list_of_registers = "r" + to_string(minRegister) + "-r" + to_string(maxRegister);
    for(int loop = 0; loop < 8; loop++){
        if(number & 1) list_of_registers += "r" + to_string(loop) + ", "; 
        number >>= 1;
    }
    return list_of_registers.erase(list_of_registers.size() - 2, 2);
    //return list_of_registers;
}
*/

