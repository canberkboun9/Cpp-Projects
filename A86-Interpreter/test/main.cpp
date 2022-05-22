#include <iostream>
#include <utility>
#include <sstream>
#include <fstream>
#include "List"
#include "math.h"

using namespace std;
//helper class to define variables
class variable{
public:
    string name;
    char dataType;
    int address;
    int dataLength;
};
//helper functions
int findLineCount(ifstream& inFile);
void parseInput(ifstream& inFileSecond, string lines[], int height);
string takeFirsts(string lines[], string firsts[], int height,string labels[], int& int20h, string ins[]);
string changeToHexa(string str);
int changeToDecimal(string str);
string setVariables(list<variable>& variables, int height, int int20h, string lines[]);
string doInstructions(string lines[], string firsts[],int int20h, string labels[]);
string doMov(string lines[], int i);
string doMovMemoryRegister(string lhs, string rhs, bool isVar);
string doMovMemoryImmediate(string lhs, string rhs, bool isVar);
string doMovRegisterMemory(string lhs, string rhs, bool isVar, int varSize, int varAdress);
string doMovRegisterRegister(string lhs, string rhs);
string doMovRegisterImmediate(string lhs, string rhs);
string doMul(string lines[], int i);
string doDiv(string lines[], int i);
string doInc(string lines[], int i);
string doDec(string lines[], int i);
pair<int,int> findAddress(string str);
string findFirst(string line);
string changeToBinary(int data, int length);
string doLogic(string lines[],int i, int op);
pair<string, string> findFirstSecond(string line);
int doXorOperation(string lhs, string rhs);
int doAndOperation(string lhs, string rhs);
int doOrOperation(string lhs, string rhs);
int findImmediateValue(string str);
int fromBinarytoDecimal(int n);
string doNot(string line);
string doPush(string line);
string doPop(string line);
string doCmp(string line);
string doShiftorRotate(string line, int shift);
int doJump(string line, string labels[], int int20h, int n);
string doint21h(string line);
string doAddanSub(string line, int multiplier);
//memory
pair<unsigned char, string> memory[2<<15];
//registers
unsigned short ax = 0 ;
unsigned short bx = 0 ;
unsigned short cx = 0 ;
unsigned short dx = 0 ;
unsigned short di = 0 ;
unsigned short si = 0 ;
unsigned short sp = 65534;
unsigned short bp = 0 ;
unsigned char ah = 0 ;
unsigned char al = 0 ;
unsigned char bh = 0 ;
unsigned char bl = 0 ;
unsigned char ch = 0 ;
unsigned char cl = 0 ;
unsigned char dh = 0 ;
unsigned char dl = 0 ;
int memoryPointer = 0 ;     //helper value to put variables to right places
int usableMemoryAfter = 0 ; //the pointer to usable memory

int of = 0;             //overflow flag
int cf = 0;               //carry flag
int zf = 0;              // zero flag
int sf = 0;              // sign flag
int af = 0;             //auxiliary flag


//helper list to check variables when needed
list<variable> variables;

//register names in order
string registers[] = {"ax","bx","cx","dx","di","sp","si","bp","ah","al","bh","bl","ch","cl","dh","dl"};


//the main function of this program. controls wheter there is an input file or not.
//checks if there is an error. organize the base functions in order.
int main(int argc, char* argv[]) {
    string instructions[] = {"mov", "add", "sub", "mul", "div", "xor", "or","and", "not", "rcl", "rcr", "shl",
                             "shr", "push", "pop", "nop", "cmp", "jz", "jnz", "je", "jne", "ja", "jae", "jb", "jbe",
                             "jnae", "jna", "jnb", "jnbe", "jnc", "jc", "int", "dec", "inc"};

    // Command line arguments are malformed
    if (argc != 2) {
        // cerr should be used for errors
        cerr << "please give an input file" << endl;
        return 1;
    }

    // Open the input and output files, check for failures
    ifstream inFile(argv[1]);
    ifstream inFileSecond(argv[1]);
    if (!inFile) { // operator! is synonymous to .fail(), checking if there was a failure
        cerr << "There was a problem opening \"" << argv[1] << "\" as input file" << endl;
        return 1;
    }

    int height = findLineCount(inFile);
    int int20h = -1;

    string lines[height];
    string firsts[height];
    string labels[height];

    parseInput(inFileSecond, lines, height);
    string test = takeFirsts(lines, firsts, height, labels, int20h, instructions);
    usableMemoryAfter = memoryPointer;


    if(test.compare("error")==0){
        cout << "error" << endl;
        return 0;
    }



    test = setVariables(variables, height, int20h, lines);

    if(test.compare("error")==0){
        cout << "error" << endl;
        return 0;
    }

    test = doInstructions(lines, firsts, int20h, labels);

    if(test.compare("error")==0){
        cout << "error" << endl;
        return 0;
    }

    return 0;

}

// Every instruction picked up from lines and directed to the appropriate function
string doInstructions(string lines[], string firsts[],int int20h, string labels[]){
    for(int i=1; i<int20h; i++){
        string instr = firsts[i];
        string str;
        int test = 0;
        if(instr.compare("mov")==0){
            str = doMov(lines,i);
        }else if(instr.compare("add")==0){
            str = doAddanSub(lines[i],1);
        }else if(instr.compare("sub")==0){
            str = doAddanSub(lines[i], -1);
        }else if(instr.compare("mul")==0){
            str = doMul(lines, i);
        }else if(instr.compare("div")==0){
            str = doDiv(lines, i);
        }else if(instr.compare("inc")==0){
            str = doInc(lines, i);
        }else if(instr.compare("dec")==0){
            str = doDec(lines, i);
        }else if(instr.compare("xor")==0){
            str = doLogic(lines, i,1);
        }else if(instr.compare("and")==0){
            str = doLogic(lines, i,2);
        }else if(instr.compare("or")==0){
            str = doLogic(lines, i,3);
        }else if(instr.compare("not")==0){
            str = doNot(lines[i]);
        }else if(instr.compare("push")==0){
            str = doPush(lines[i]);
        }else if(instr.compare("pop")==0){
            str = doPop(lines[i]);
        }else if(instr.compare("nop")==0){
            str = "";
        }else if(instr.compare("cmp")==0){
            str = doCmp(lines[i]);
        }else if(instr.compare("shl")==0){
            str = doShiftorRotate(lines[i],1);
        }else if(instr.compare("shr")==0){
            str = doShiftorRotate(lines[i],2);
        }else if(instr.compare("rcl")==0){
            str = doShiftorRotate(lines[i],3);
        }else if(instr.compare("rcr")==0){
            str = doShiftorRotate(lines[i],4);
        }else if(instr.compare("jz")==0){
            test = doJump(lines[i],labels, int20h,0);
        }else if(instr.compare("jnz")==0){
            test = doJump(lines[i],labels, int20h,1);
        }else if(instr.compare("je")==0){
            test = doJump(lines[i],labels, int20h,2);
        }else if(instr.compare("jne")==0){
            test = doJump(lines[i],labels, int20h,3);
        }else if(instr.compare("ja")==0){
            test = doJump(lines[i],labels, int20h,4);
        }else if(instr.compare("jae")==0){
            test = doJump(lines[i],labels, int20h,5);
        }else if(instr.compare("jb")==0){
            test = doJump(lines[i],labels, int20h,6);
        }else if(instr.compare("jbe")==0){
            test = doJump(lines[i],labels, int20h,7);
        }else if(instr.compare("jnae")==0){
            test = doJump(lines[i],labels, int20h,8);
        }else if(instr.compare("jnb")==0){
            test = doJump(lines[i],labels, int20h,9);
        }else if(instr.compare("jnbe")==0){
            test = doJump(lines[i],labels, int20h,10);
        }else if(instr.compare("jc")==0){
            test = doJump(lines[i],labels, int20h,11);
        }else if(instr.compare("jnc")==0){
            test = doJump(lines[i],labels, int20h,12);
        }else if(instr.compare("jmp")==0){
            test = doJump(lines[i],labels, int20h,13);
        }else if(instr.compare("int")==0){
            str = doint21h(lines[i]);
        }
        if(str.compare("error")==0){
            return "error";
        }
        if(test==-1){
            return "error";
        }else if(test!=0){
            i = test;
        }
    }
    return "";
}

//it controls print and read operation
string doint21h(string line){
    while(line.at(0)==' '){
        line.erase(0,1);
    }
    int space = line.find_first_of(' ');
    while(space!=-1){
        line.erase(space,1);
        space = line.find_first_of(' ');
    }
    if(line.compare("int21h")!=0){
        return "error";
    }
    int n = ah;
    if(n==1){
        unsigned char c;
        cin >> c;
        al = c;
        ax = ah*256+al;
    }else if(n==2){
        al = dl;
        ax = ah*256+al;
        cout << (char)(dl%128);
    }else return "error";
    return "";
}

//all jump instructions come to this function. after determining the parameters the process executes
int doJump(string line, string labels[], int int20h, int n){
    int target = -1;

    line = findFirst(line);
    for(int i=1; i<int20h;i++){
        if(line.compare(labels[i])==0){
            target = i;
            break;
        }
    }
    if(target == -1) return -1;


    if(n==0){  //ZJ
        if(zf==1) return target;
    }else if(n==1){ //JNZ
        if(zf==0) return target;
    }else if(n==2){ //JE
        if(zf==1) return target;
    }else if(n==3){ //JNE
        if(zf==0) return target;
    }else if(n==4){ //JA
        if(zf==0&&cf==0) return target;
    }else if(n==5){ //JAE
        if(cf==0 || zf==1) return target;
    }else if(n==6){ //JB
        if(cf==1&&zf==0) return target;
    }else if(n==7){ //JBE
        if(cf==1||zf==1) return target;
    }else if(n==8){ //JNAE
        if(cf==1&&zf==0) return target;
    }else if(n==9){ //JNB
        if(cf==0||zf==1) return target;
    }else if(n==10){ //JNBE
        if(cf==0&&zf==0) return target;
    }else if(n==11){ //JC
        if(cf==1) return target;
    }else if(n==12){ //JNC
        if(cf==0) return target;
    }else if(n==13){ //JMP
        return target;
    }else return -1;

    return 0;
}

//when we found that we will do either shift or rotate we can process them with this function
string doShiftorRotate(string line, int shift){
    string lhs = findFirstSecond(line).first;
    string rhs = findFirstSecond(line).second;
    if(lhs.compare("error")==0) return "error";
    int lhsRegOrder = -1;
    int rhsRegOrder = -1;
    int lhsValue = 0;
    int rhsValue = 0;
    int dataLength = 0;
    int address = -1;
    bool rhsVar = false;
    for(int i =0; i<16; i++){
        if(lhs.compare(registers[i])==0){
            lhsRegOrder = i;
        }
        if(rhs.compare(registers[i])==0){
            rhsRegOrder = i;
        }
    }
    if(rhsRegOrder!=-1){
        if(rhsRegOrder==0){
            rhsValue = ax;
        }else if(rhsRegOrder==1){
            rhsValue = bx;
        }else if(rhsRegOrder==2){
            rhsValue = cx;
        }else if(rhsRegOrder==3){
            rhsValue = dx;
        }else if(rhsRegOrder==4){
            rhsValue = di;
        }else if(rhsRegOrder==5){
            rhsValue = sp;
        }else if(rhsRegOrder==6){
            rhsValue = si;
        }else if(rhsRegOrder==7){
            rhsValue = bp;
        }else if(rhsRegOrder==8){
            rhsValue = ah;
        }else if(rhsRegOrder==9){
            rhsValue = al;
        }else if(rhsRegOrder==10){
            rhsValue = bh;
        }else if(rhsRegOrder==11){
            rhsValue = bl;
        }else if(rhsRegOrder==12){
            rhsValue = ch;
        }else if(rhsRegOrder==13){
            rhsValue = cl;
        }else if(rhsRegOrder==14){
            rhsValue = dh;
        }else if(rhsRegOrder==15){
            rhsValue = dl;
        }else return "error";
    }else{
        for(variable v: variables){
            if(rhs.compare(v.name)==0){
                rhsVar = true;
                break;
            }
        }
        if(rhsVar||rhs.find_first_of('[')!=-1){
            int rsAd = findAddress(rhs).first;
            if(rsAd==-1) return "error";
            int rsLt = findAddress(rhs).second;
            if(rsLt==0) rsLt = 1;
            if(rsLt==1){
                rhsValue = memory[rsAd].first;
            }else{
                rhsValue = memory[rsAd].first+memory[rsAd+1].first*256;
            }
        }else{
            rhsValue = findImmediateValue(rhs);
        }
    }
    int multiplier = pow(2,rhsValue%16);

    if(lhsRegOrder!=-1){
        if(lhsRegOrder==0){
            lhsValue = ax;
        }else if(lhsRegOrder==1){
            lhsValue = bx;
        }else if(lhsRegOrder==2){
            lhsValue = cx;
        }else if(lhsRegOrder==3){
            lhsValue = dx;
        }else if(lhsRegOrder==4){
            lhsValue = di;
        }else if(lhsRegOrder==5){
            return "error";
        }else if(lhsRegOrder==6){
            lhsValue = si;
        }else if(lhsRegOrder==7){
            lhsValue = bp;
        }else if(lhsRegOrder==8){
            lhsValue = ah;
        }else if(lhsRegOrder==9){
            lhsValue = al;
        }else if(lhsRegOrder==10){
            lhsValue = bh;
        }else if(lhsRegOrder==11){
            lhsValue = bl;
        }else if(lhsRegOrder==12){
            lhsValue = ch;
        }else if(lhsRegOrder==13){
            lhsValue = cl;
        }else if(lhsRegOrder==14){
            lhsValue = dh;
        }else if(lhsRegOrder==15){
            lhsValue = dl;
        }else return "error";
        if(lhsRegOrder<=7){
            dataLength=2;
        }else{
            dataLength=1;
        }
    }else{
        address=findAddress(lhs).first;
        if(address==-1) return "error";
        dataLength=findAddress(lhs).second;

        if(dataLength==1){
            lhsValue = memory[address].first;
        }else{
            lhsValue = memory[address].first+memory[address+1].first*256;
        }
    }

    if(shift==1){
        lhsValue= (lhsValue*multiplier)%(int)pow(2,(dataLength*8));
        cf = lhsValue/((int)pow(2,(dataLength*8-1)));
        sf = cf;
    }else if(shift==2){
        lhsValue= (lhsValue/multiplier);
        cf = lhsValue/((int)pow(2,(dataLength*8-1)));
        sf = cf;
    }else if(shift==3){
        lhsValue = cf*pow(2,dataLength*8) + lhsValue;
        rhsValue = rhsValue%(dataLength*8+1);
        int n = (dataLength*8+1)-rhsValue;
        int varOne = lhsValue/(int)pow(2,n);
        int varTwo = lhsValue%(int)pow(2,n);
        lhsValue = varOne + varTwo*pow(2,rhsValue);
        cf = lhsValue/pow(2,dataLength*8);
        lhsValue = lhsValue%(int)pow(2,dataLength*8);
    }else if(shift==4){
        lhsValue = cf*pow(2,dataLength*8) + lhsValue;
        rhsValue = rhsValue%(dataLength*8+1);
        int n = rhsValue;
        int varOne = lhsValue/(int)pow(2,n);
        int varTwo = lhsValue%(int)pow(2,n);
        lhsValue = varOne + varTwo*pow(2,(dataLength*8+1)-rhsValue);
        cf = lhsValue/pow(2,dataLength*8);
        lhsValue = lhsValue%(int)pow(2,dataLength*8);
    }else return "error";

    if(lhsRegOrder!=-1){
        if(lhsRegOrder==0){
            ax = lhsValue;
            ah = ax/256;
            al = ax%256;
        }else if(lhsRegOrder==1){
            bx= lhsValue;
            bh = bx/256;
            bl = bx%256;
        }else if(lhsRegOrder==2){
            cx= lhsValue;
            ch = cx/256;
            cl = cx%256;
        }else if(lhsRegOrder==3){
            dx= lhsValue;
            dh = dx/256;
            dl = dx%256;
        }else if(lhsRegOrder==4){
            di= lhsValue;
        }else if(lhsRegOrder==5){
            return "error";
        }else if(lhsRegOrder==6){
            si= lhsValue;
        }else if(lhsRegOrder==7){
            bp= lhsValue;
        }else if(lhsRegOrder==8){
            ah= lhsValue;
            ax = ah*256+al;
        }else if(lhsRegOrder==9){
            al= lhsValue;
            ax = ah*256+al;
        }else if(lhsRegOrder==10){
            bh= lhsValue;
            bx = bh*256+bl;
        }else if(lhsRegOrder==11){
            bl= lhsValue;
            bx = bh*256+bl;
        }else if(lhsRegOrder==12){
            ch= lhsValue;
            cx = ch*256+cl;
        }else if(lhsRegOrder==13){
            cl= lhsValue;
            cx = ch*256+cl;
        }else if(lhsRegOrder==14){
            dh= lhsValue;
            dx = dh*256+dl;
        }else if(lhsRegOrder==15){
            dl= lhsValue;
            dx = dh*256+dl;
        }else return "error";
    }else{
        if(dataLength==1){
            memory[address].first=lhsValue;
        }else{
            memory[address].first = lhsValue%256;
            memory[address+1].first = lhsValue/256;
        }
    }

    return "";
}

//does the comparison
string doCmp(string line){
    string lhs = findFirstSecond(line).first;
    string rhs = findFirstSecond(line).second;
    if(lhs.compare("error")==0) return "error";
    int address = -1;
    int dataLength = -1;
    int lhsValue = 0;
    int rhsValue = 0;
    int lhsRegOrder = -1;
    int rhsRegOrder = -1;
    int lhsLength = 0;
    int rhsLength = 0;
    for(int i=0; i<16; i++){
        if(lhs.compare(registers[i])==0){
            lhsRegOrder = i;
        }
        if(rhs.compare(registers[i])==0){
            rhsRegOrder = i;
        }
    }
    bool isLhsVar = false;
    bool isRhsVar = false;
    for(variable v: variables){
        if(lhs.compare(v.name)==0){
            isLhsVar = true;
        }
        if(rhs.compare(v.name)==0){
            isRhsVar = true;
        }
    }
    if(lhsRegOrder!=-1){
        if(lhsRegOrder==0){
            lhsValue = ax;
        }else if(lhsRegOrder==1){
            lhsValue = bx;
        }else if(lhsRegOrder==2){
            lhsValue = cx;
        }else if(lhsRegOrder==3){
            lhsValue = dx;
        }else if(lhsRegOrder==4){
            lhsValue = di;
        }else if(lhsRegOrder==5){
            lhsValue = sp;
        }else if(lhsRegOrder==6){
            lhsValue = si;
        }else if(lhsRegOrder==7){
            lhsValue = bp;
        }else if(lhsRegOrder==8){
            lhsValue = ah;
        }else if(lhsRegOrder==9){
            lhsValue = al;
        }else if(lhsRegOrder==10){
            lhsValue = bh;
        }else if(lhsRegOrder==11){
            lhsValue = bl;
        }else if(lhsRegOrder==12){
            lhsValue = ch;
        }else if(lhsRegOrder==13){
            lhsValue = cl;
        }else if(lhsRegOrder==14){
            lhsValue = dh;
        }else if(lhsRegOrder==15){
            lhsValue = dl;
        }else return "error";
        if(lhsRegOrder<=7){
            lhsLength = 2;
        }else{
            lhsLength = 1;
        }
    }else{
        if(isLhsVar||lhs.find_first_of('[')!=-1){
            address = findAddress(lhs).first;
            if(address==-1) return "error";
            dataLength = findAddress(lhs).second;
            if(dataLength==1){
                lhsValue = memory[address].first;
            }else if(dataLength==2){
                lhsValue = memory[address].first+memory[address+1].first*256;
            }else return "error";
            lhsLength = dataLength;
        }else{
            lhsValue = findImmediateValue(lhs);
            if(lhsValue==-1) return "error";
        }
    }


    if(rhsRegOrder!=-1){
        if(rhsRegOrder==0){
            rhsValue = ax;
        }else if(rhsRegOrder==1){
            rhsValue = bx;
        }else if(rhsRegOrder==2){
            rhsValue = cx;
        }else if(rhsRegOrder==3){
            rhsValue = dx;
        }else if(rhsRegOrder==4){
            rhsValue = di;
        }else if(rhsRegOrder==5){
            rhsValue = sp;
        }else if(rhsRegOrder==6){
            rhsValue = si;
        }else if(rhsRegOrder==7){
            rhsValue = bp;
        }else if(rhsRegOrder==8){
            rhsValue = ah;
        }else if(rhsRegOrder==9){
            rhsValue = al;
        }else if(rhsRegOrder==10){
            rhsValue = bh;
        }else if(rhsRegOrder==11){
            rhsValue = bl;
        }else if(rhsRegOrder==12){
            rhsValue = ch;
        }else if(rhsRegOrder==13){
            rhsValue = cl;
        }else if(rhsRegOrder==14){
            rhsValue = dh;
        }else if(rhsRegOrder==15){
            rhsValue = dl;
        }else return "error";
        if(rhsRegOrder<=7){
            rhsLength = 2;
        }else{
            rhsLength = 1;
        }
    }else{
        if(isRhsVar||rhs.find_first_of('[')!=-1){
            address = findAddress(rhs).first;
            if(address==-1) return "error";
            dataLength = findAddress(rhs).second;
            if(dataLength==1){
                rhsValue = memory[address].first;
            }else if(dataLength==2){
                rhsValue = memory[address].first+memory[address+1].first*256;
            }else return "error";
            rhsLength = dataLength;
        }else{
            rhsValue = findImmediateValue(rhs);
            if(rhsValue==-1) return "error";
        }
    }

    if(lhsLength==rhsLength||lhsLength==0||rhsLength==0){
        if(lhsValue>rhsValue){
            cf = 0;
            zf = 0;
        }else if(lhsValue<rhsValue){
            cf = 1;
            zf = 0;
        }else{
            cf = 0;
            zf = 1;
        }
    }else return "error";

    return "";
}

//Uses stack and pops the value
string doPop(string line){
    sp = sp+2;
    if(sp>=65536) return "error";
    line = findFirst(line);
    if(line.compare("error")==0) return "error";
    int regOrder = -1;
    for(int i =0; i<16; i++){
        if(line.compare(registers[i])==0){
            regOrder = i;
            break;
        }
    }
    if(regOrder!=-1){
        if(regOrder>7||regOrder==5){
            return "error";
        }else if(regOrder==0){
            al = memory[sp].first;
            ah = memory[sp+1].first;
            ax = ah*256+al;
        }else if(regOrder==1){
            bl = memory[sp].first;
            bh = memory[sp+1].first;
            bx = bh*256+bl;
        }else if(regOrder==2){
            cl = memory[sp].first;
            ch = memory[sp+1].first;
            cx = ch*256+cl;
        }else if(regOrder==3){
            dl = memory[sp].first;
            dh = memory[sp+1].first;
            dx = dh*256+dl;
        }else if(regOrder==4){
            di = memory[sp].first+memory[sp+1].first*256;
        }else if(regOrder==6){
            si = memory[sp].first+memory[sp+1].first*256;
        }else if(regOrder==7){
            bp = memory[sp].first+memory[sp+1].first*256;
        }else return "error";
    }else{
        int address = findAddress(line).first;
        if(address==-1) return "error";
        int dataLength = findAddress(line).second;
        if(dataLength!=2) return "error";
        memory[address].first = memory[sp].first;
        memory[address+1].first = memory[sp+1].first;
        memory[sp].first = 0;
        memory[sp+1].first = 0;
    }
    return "";
}

//Uses stack and pushes the value
string doPush(string line){
    line = findFirst(line);
    if(line.compare("error")==0) return "error";
    int regOrder = -1;
    for(int i =0; i<16; i++){
        if(line.compare(registers[i])==0){
            regOrder = i;
            break;
        }
    }
    if(regOrder!=-1){
        if(regOrder>7){
            return "error";
        }
        if(regOrder==0){
            memory[sp].first = al;
            memory[sp+1].first = ah;
        }else if(regOrder==1){
            memory[sp].first = bl;
            memory[sp+1].first = bh;
        }else if(regOrder==2){
            memory[sp].first = cl;
            memory[sp+1].first = ch;
        }else if(regOrder==3){
            memory[sp].first = dl;
            memory[sp+1].first = dh;
        }else if(regOrder==4){
            memory[sp].first = di%256;
            memory[sp+1].first = di/256;
        }else if(regOrder==5){
            memory[sp].first = sp%256;
            memory[sp+1].first = sp/256;
        }else if(regOrder==6){
            memory[sp].first = si%256;
            memory[sp+1].first = si/256;
        }else if(regOrder==7){
            memory[sp].first = bp%256;
            memory[sp+1].first = bp/256;
        }
    }else{
        bool isVar = false;
        for(variable v: variables){
            if(line.compare(v.name)==0){
                isVar = true;
                break;
            }
        }
        if(isVar||line.find_first_of('[')!=-1){
            int address = findAddress(line).first;
            if(address==-1) return "error";
            int dataLength = findAddress(line).second;
            if(dataLength!=2) return "error";
            memory[sp].first = memory[address].first;
            memory[sp+1].first = memory[address+1].first;
        }else{
            int value = findImmediateValue(line);
            if(value==-1) return "error";
            memory[sp].first = value%256;
            memory[sp+1].first = value/256;
        }
    }
    sp = sp - 2;

    return "";
}

//does the logical NOT operation
string doNot(string line){
    line = findFirst(line);
    if(line.compare("error")==0) return "error";
    int regOrder = -1;
    for(int i =0; i<16; i++){
        if(line.compare(registers[i])==0){
            regOrder = i;
            break;
        }
    }
    int address = 0;
    int dataLength = 0;
    int data;
    if(regOrder!=-1){
        if(regOrder==0){
            data = ax;
            dataLength = 2;
        }else if(regOrder==1){
            data = bx;
            dataLength = 2;
        }else if(regOrder==2){
            data = cx;
            dataLength = 2;
        }else if(regOrder==3){
            data = dx;
            dataLength = 2;
        }else if(regOrder==4){
            data = di;
            dataLength = 2;
        }else if(regOrder==5){
//            data = sp;
//            dataLength = 2;
        }else if(regOrder==6){
            data = si;
            dataLength = 2;
        }else if(regOrder==7){
            data = bp;
            dataLength = 2;
        }else if(regOrder==8){
            data = ah;
            dataLength = 1;
        }else if(regOrder==9){
            data = al;
            dataLength = 1;
        }else if(regOrder==10){
            data = bh;
            dataLength = 1;
        }else if(regOrder==11){
            data = bl;
            dataLength = 1;
        }else if(regOrder==12){
            data = ch;
            dataLength = 1;
        }else if(regOrder==13){
            data = cl;
            dataLength = 1;
        }else if(regOrder==14){
            data = dh;
            dataLength = 1;
        }else if(regOrder==15){
            data = dl;
            dataLength = 1;
        }
    }else{
        address = findAddress(line).first;
        if(address==-1) return "error";
        dataLength = findAddress(line).second;
        if(dataLength==1){
            data = memory[address].first;
        }else{
            data = memory[address].first + memory[address+1].first*256;
        }
    }
    string binaryData = "";
    binaryData = changeToBinary(data,dataLength);
    data = 0;
    for(int i=0; i<binaryData.length();i++){
        data = data*10;
        if(binaryData.at(i)=='0'){
            data += 1;
        }
    }
    data = fromBinarytoDecimal(data);
    string  dataForTest = changeToBinary(data,dataLength);
    if(dataForTest.at(0)==1){
        sf = 1;
    }else{
        sf = 0;
    }
    if(data==0){
        zf = 1;
    }else{
        zf = 0;
    }
    if(regOrder!=-1){
        if(regOrder==0){
            ax = data;
            ah = ax/256;
            al = ax%256;
        }else if(regOrder==1){
            bx = data;
            bh = bx/256;
            bl = bx%256;
        }else if(regOrder==2){
            cx = data;
            ch = cx/256;
            cl = cx%256;
        }else if(regOrder==3){
            dx = data;
            dh = dx/256;
            dl = dx%256;
        }else if(regOrder==4){
            di = data;
        }else if(regOrder==5){
            return "error";
        }else if(regOrder==6){
            si = data;
        }else if(regOrder==7){
            bp = data;
        }else if(regOrder==8){
            ah = data;
            ax = ah*256+al;
        }else if(regOrder==9){
            al = data;
            ax = ah*256+al;
        }else if(regOrder==10){
            bh = data;
            bx = bh*256+bl;
        }else if(regOrder==11){
            bl = data;
            bx = bh*256+bl;
        }else if(regOrder==12){
            ch = data;
            cx = ch*256+cl;
        }else if(regOrder==13){
            cl = data;
            cx = ch*256+cl;
        }else if(regOrder==14){
            dh = data;
            dx = dh*256+dl;
        }else if(regOrder==15){
            dl = data;
            dx = dh*256+dl;
        }
    }else{
        if(dataLength==1){
            memory[address].first = data;
        }else{
            memory[address].first = data%256;
            memory[address+1].first = data/256;
        }
    }

    of = 0;
    cf = 0;
    return "";
}

//does the logical operations with two parameter(OR, XOR, AND)
//first it takes parameters and directs them to the appropriate logic functions
string doLogic(string lines[],int i, int op){
    string lhs = findFirstSecond(lines[i]).first;
    if(lhs.compare("error")==0){
        return "error";
    }
    string rhs = findFirstSecond(lines[i]).second;
    bool isLhsReg = false;
    bool isRhsReg = false;
    int leftOrder = 0;
    int rightOrder = 0;
    for(int j = 0; j<16; j++){
        if(lhs.compare(registers[j])==0){
            isLhsReg = true;
            leftOrder = j;
        }
        if(rhs.compare(registers[j])==0){
            isRhsReg = true;
            rightOrder = j;
        }
    }

    int address= 0;
    int dataLength = 0;
    int result = 0;
    if(isRhsReg){
        if(rightOrder==0){
            rhs = changeToBinary((int)ax, 2);
        }else if(rightOrder==1){
            rhs = changeToBinary((int)bx, 2);
        }else if(rightOrder==2){
            rhs = changeToBinary((int)cx, 2);
        }else if(rightOrder==3){
            rhs = changeToBinary((int)dx, 2);
        }else if(rightOrder==4){
            rhs = changeToBinary((int)si, 2);
        }else if(rightOrder==5){
//            rhs = changeToBinary((int)sp, 2);
        }else if(rightOrder==6){
            rhs = changeToBinary((int)di, 2);
        }else if(rightOrder==7){
            rhs = changeToBinary((int)bp, 2);
        }else if(rightOrder==8){
            rhs = changeToBinary((int)ah, 1);
        }else if(rightOrder==9){
            rhs = changeToBinary((int)al, 1);
        }else if(rightOrder==10){
            rhs = changeToBinary((int)bh, 1);
        }else if(rightOrder==11){
            rhs = changeToBinary((int)bl, 1);
        }else if(rightOrder==12){
            rhs = changeToBinary((int)ch, 1);
        }else if(rightOrder==13){
            rhs = changeToBinary((int)cl, 1);
        }else if(rightOrder==14){
            rhs = changeToBinary((int)dh, 1);
        }else if(rightOrder==15){
            rhs = changeToBinary((int)dl, 1);
        }else return "error";
    }else{
        int rhsValue = 0;
        bool isVar = false;
        for(variable v: variables){
            if(rhs.compare(v.name)==0){
                isVar = true;
                break;
            }
        }
        if(rhs.find_first_of('[')!=-1||isVar){
            address = findAddress(rhs).first;
            dataLength = findAddress(rhs).second;
            if(dataLength==1){
                rhsValue = memory[address].first;
            }else if(dataLength==2){
                rhsValue = memory[address].first+ memory[address+1].first*256;
            }else return "error";
        }else{
            rhsValue = findImmediateValue(rhs);
            if(rhsValue==-1) return "error";
            if(rhsValue>65535){
                return "error";
            }else if(rhsValue>256){
                dataLength = 2;
            }else{
                dataLength = 1;
            }
        }
        rhs = changeToBinary(rhsValue,dataLength);
    }


    if(isLhsReg){
        if(leftOrder==0){
            lhs = changeToBinary((int)ax, 2);
        }else if(leftOrder==1){
            lhs = changeToBinary((int)bx, 2);
        }else if(leftOrder==2){
            lhs = changeToBinary((int)cx, 2);
        }else if(leftOrder==3){
            lhs = changeToBinary((int)dx, 2);
        }else if(leftOrder==4){
            lhs = changeToBinary((int)si, 2);
        }else if(leftOrder==5){
//            lhs = changeToBinary((int)sp, 2);
        }else if(leftOrder==6){
            lhs = changeToBinary((int)di, 2);
        }else if(leftOrder==7){
            lhs = changeToBinary((int)bp, 2);
        }else if(leftOrder==8){
            lhs = changeToBinary((int)ah, 1);
        }else if(leftOrder==9){
            lhs = changeToBinary((int)al, 1);
        }else if(leftOrder==10){
            lhs = changeToBinary((int)bh, 1);
        }else if(leftOrder==11){
            lhs = changeToBinary((int)bl, 1);
        }else if(leftOrder==12){
            lhs = changeToBinary((int)ch, 1);
        }else if(leftOrder==13){
            lhs = changeToBinary((int)cl, 1);
        }else if(leftOrder==14){
            lhs = changeToBinary((int)dh, 1);
        }else if(leftOrder==15){
            lhs = changeToBinary((int)dl, 1);
        }else return "error";
    }else{
        int lhsValue = 0;
        address = findAddress(lhs).first;
        if(address==-1) return "error";
        dataLength = findAddress(lhs).second;
        if(dataLength==1){
            lhsValue = memory[address].first;
        }else{
            lhsValue = memory[address].first+memory[address+1].first*256;
        }
        lhs = changeToBinary(lhsValue,dataLength);
    }

    result = 0;
    if(op == 1){
        result = doXorOperation(lhs,rhs);
    }else if(op == 2){
        result = doAndOperation(lhs,rhs);
    }else if(op == 3){
        result = doOrOperation(lhs,rhs);
    }
    if(result == -1) return "error";
    result = fromBinarytoDecimal(result);


    if(isLhsReg){
        if(leftOrder==0){
            ax = result;
            ah = ax/256;
            al = ax%256;
        }else if(leftOrder==1){
            bx = result;
            bh = bx/256;
            bl = bx%256;
        }else if(leftOrder==2){
            cx = result;
            ch = cx/256;
            cl = cx%256;
        }else if(leftOrder==3){
            dx = result;
            dh = dx/256;
            dl = dx%256;
        }else if(leftOrder==4){
            di = result;
        }else if(leftOrder==5){
//            doldur
        }else if(leftOrder==6){
            si = result;
        }else if(leftOrder==7){
            bp = result;
        }else if(leftOrder==8){
            ah = result;
            ax = ah*256+al;
        }else if(leftOrder==9){
            al = result;
            ax = ah*256+al;
        }else if(leftOrder==10){
            bh = result;
            bx = bh*256+bl;
        }else if(leftOrder==11){
            bl = result;
            bx = bh*256+bl;
        }else if(leftOrder==12){
            ch = result;
            cx = ch*256+cl;
        }else if(leftOrder==13){
            cl = result;
            cx = ch*256+cl;
        }else if(leftOrder==14){
            dh = result;
            dx = dh*256+dl;
        }else if(leftOrder==15){
            dl = result;
            dx = dh*256+dl;
        }
    }else{
        if(dataLength==1){
            memory[address].first= result;
        }else if(dataLength==2){
            memory[address].first= result%256;
            memory[address+1].first= result/256;
        }else return "error";
    }
    of = 0;
    cf = 0;
    return "";
}

//does the logical XOR operation
int doXorOperation(string lhs, string rhs){
    if(lhs.length()!=rhs.length()) return -1;
    int result = 0;
    for(int i=0; i<lhs.length(); i++){
        result*=10;
        if(lhs.at(i)!=rhs.at(i)){
            result += 1;
        }
    }
    int power = pow(10,lhs.length()-1);
    if(result/power==1){
        sf = 1;
    }else{
        sf = 0;
    }
    if(result == 0){
        zf = 1;
    }else{
        zf = 0;
    }
    return result;
}

//does the logical AND operation
int doAndOperation(string lhs, string rhs){
    if(lhs.length()!=rhs.length()) return -1;
    int result = 0;
    for(int i=0; i<lhs.length(); i++){
        result *=10;
        if(lhs.at(i)=='1'&&rhs.at(i)=='1'){
            result+=1;
        }
    }
    int power = pow(10,lhs.length()-1);

    if(result/power==1){
        sf = 1;
    }else{
        sf = 0;
    }
    if(result == 0){
        zf = 1;
    }else{
        zf = 0;
    }
    return result;
}

//does the logical OR operation
int doOrOperation(string lhs, string rhs){
    if(lhs.length()!=rhs.length()) return -1;
    int result = 0;
    for(int i=0; i<lhs.length(); i++){
        result *=10;
        if(lhs.at(i)=='1'||rhs.at(i)=='1'){
            result+=1;
        }
    }
    int power = pow(10,lhs.length()-1);
    if(result/power==1){
        sf = 1;
    }else{
        sf = 0;
    }
    if(result == 0){
        zf = 1;
    }else{
        zf = 0;
    }
    return result;
}

//does the decrement from the given argument
string doDec(string lines[], int i){
    string str = findFirst(lines[i]);
    int seven = pow(2,7);
    int fifteen = pow(2,15);
    if(str.compare("error")==0){
        return "error";
    }
    bool isReg = false;
    for(int j = 0; j< 16; j++){
        if(str.compare(registers[j])==0){
            isReg = true;
            break;
        }
    }
    if(isReg){
        if(str.compare("ax")==0){
            ax = ax -1;
            ah = ax/256;
            al = ax%256;
            if(ax==0) zf = 1;
            if(ax/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("bx")==0){
            bx = bx -1;
            bh = bx/256;
            bl = bx%256;
            if(bx==0) zf = 1;
            if(bx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("cx")==0){
            cx = cx -1;
            ch = cx/256;
            cl = cx%256;
            if(cx==0) zf = 1;
            if(cx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("dx")==0){
            dx = dx -1;
            dh = dx/256;
            dl = dx%256;
            if(dx==0) zf = 1;
            if(dx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("di")==0){
            di = di -1 ;
            if(di==0) zf = 1;
            if(di/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("si")==0){
            si = si -1 ;
            if(si==0) zf = 1;
            if(si/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("sp")==0){
            return "error";
        }else if(str.compare("bp")==0){
            bp = bp -1 ;
            if(bp==0) zf = 1;
            if(bp/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("ah")==0){
            ah--;
            if(ah==0) zf = 1;
            if(ah/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            ax = ah*256+al;
        }else if(str.compare("al")==0){
            al--;
            if(al==0) zf = 1;
            if(al/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            ax = ah*256+al;
        }else if(str.compare("bh")==0){
            bh--;
            if(bh==0) zf = 1;
            if(bh/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            bx = bh*256+bl;
        }else if(str.compare("bl")==0){
            bl--;
            if(bl==0) zf = 1;
            if(bl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            bx = bh*256+bl;
        }else if(str.compare("ch")==0){
            ch--;
            if(ch==0) zf = 1;
            if(ch/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            cx = ch*256+cl;
        }else if(str.compare("cl")==0){
            cl--;
            if(cl==0) zf = 1;
            if(cl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            cx = ch*256+cl;
        }else if(str.compare("dh")==0){
            dh--;
            if(dh==0) zf = 1;
            if(dh/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            dx = dh*256+dl;
        }else if(str.compare("dl")==0){
            dl--;
            if(dl==0) zf = 1;
            if(dl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            dx = dh*256+dl;
        }
//        if(result%16==15){
//            af = 1;
//        }else{
//            af = 0;
//        }
    }else{
        int address = findAddress(str).first;
        if(address==-1) return "error";
        int dataLength = findAddress(str).second;
        int value = dataLength==1? memory[address].first: memory[address].first+memory[address+1].first*256;
        value = value -1;
        if(value<0){
            value += pow(2,8*dataLength);
        }
        if(value==0) zf = 1;
//        if(result%16==15){
//            af = 1;
//        }else{
//            af = 0;
//        }
        if(value/pow(2,8*dataLength-1)==1){
            sf = 1;
            of = 1;
        }else{
            sf = 0;
            of = 0;
        }
        if(dataLength==1){
            memory[address].first = value;
        }else{
            memory[address].first = value%256;
            memory[address+2].first = value/256;
        }
    }


    return "";
}

//does the increment to the given argument
string doInc(string lines[], int i){
    string str = findFirst(lines[i]);
    int seven = pow(2,7);
    int fifteen = pow(2,15);
    if(str.compare("error")==0){
        return "error";
    }
    bool isReg = false;
    for(int j = 0; j< 16; j++){
        if(str.compare(registers[j])==0){
            isReg = true;
            break;
        }
    }
    if(isReg){
        if(str.compare("ax")==0){
            ax = ax + 1;
            ah = ax/256;
            al = ax%256;
            if(ax==0) zf = 1;
            if(ax/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("bx")==0){
            bx = bx + 1;
            bh = bx/256;
            bl = bx%256;
            if(bx==0) zf = 1;
            if(bx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("cx")==0){
            cx = cx + 1;
            ch = cx/256;
            cl = cx%256;
            if(cx==0) zf = 1;
            if(cx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("dx")==0){
            dx = dx + 1;
            dh = dx/256;
            dl = dx%256;
            if(dx==0) zf = 1;
            if(dx/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("di")==0){
            di = di + 1 ;
            if(di==0) zf = 1;
            if(di/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("si")==0){
            si = si + 1 ;
            if(si==0) zf = 1;
            if(si/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("sp")==0){
            return "error";
        }else if(str.compare("bp")==0){
            bp = bp +1 ;
            if(bp==0) zf = 1;
            if(bp/fifteen==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
        }else if(str.compare("ah")==0){
            ah++;
            if(ah==0) zf = 1;
            if(ah/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            ax = ah*256+al;
        }else if(str.compare("al")==0){
            al++;
            if(al==0) zf = 1;
            if(al/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            ax = ah*256+al;
        }else if(str.compare("bh")==0){
            bh++;
            if(bh==0) zf = 1;
            if(bh/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            bx = bh*256+bl;
        }else if(str.compare("bl")==0){
            bl++;
            if(bl==0) zf = 1;
            if(bl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            bx = bh*256+bl;
        }else if(str.compare("ch")==0){
            ch++;
            if(ch==0) zf = 1;
            if(ch/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            cx = ch*256+cl;
        }else if(str.compare("cl")==0){
            cl++;
            if(cl==0) zf = 1;
            if(cl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            cx = ch*256+cl;
        }else if(str.compare("dh")==0){
            dh++;
            if(dh==0) zf = 1;
            if(dh/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            dx = dh*256+dl;
        }else if(str.compare("dl")==0){
            dl++;
            if(dl==0) zf = 1;
            if(dl/seven==1){
                sf = 1;
                of = 1;
            }else{
                sf = 0;
                of = 0;
            }
            dx = dh*256+dl;

        }

//        if(result%16==0){
//            af = 1;
//        }else{
//            af = 0;
//        }
    }else{
        int address = findAddress(str).first;
        if(address==-1) return "error";
        int dataLength = findAddress(str).second;
        int value = dataLength==1? memory[address].first: memory[address].first+memory[address+1].first*256;
        value = value + 1;
        value = value%(int)pow(2,8*dataLength);
//        if(result%16==0){
//            af = 1;
//        }else{
//            af = 0;
//        }
        if(value==0) zf = 1;
        if(value/pow(2,8*dataLength-1)==1){
            sf = 1;
            of = 1;
        }else{
            sf = 0;
            of = 0;
        }
        if(dataLength==1){
            memory[address].first = value;
        }else{
            memory[address].first = value%256;
            memory[address+2].first = value/256;
        }
    }
    return "";
}

//divides appropriate register value to given value
string doDiv(string lines[], int i){
    string str = findFirst(lines[i]);
    if(str.compare("error")==0){
        return "error";
    }
    bool isReg = false;
    int order = 0;
    for(int j = 0; j< 16; j++){
        if(str.compare(registers[j])==0){
            isReg = true;
            order = j;
            break;
        }
    }

    if(isReg){
        int result = 0;
        int begin = order<=7 ? dx*65536+ax: ax;
        if(order==0){
            if(ax==0) return "error";
            result = begin/ax;
            dx = begin%ax;
        }else if(order==1){
            if(bx==0) return "error";
            result = begin/bx;
            dx = begin%bx;
        }else if(order==2){
            if(cx==0) return "error";
            result = begin/cx;
            dx = begin%cx;
        }else if(order==3){
            if(dx==0) return "error";
            result = begin/dx;
            dx = begin%dx;
        }else if(order==4){
            if(di==0) return "error";
            result = begin/di;
            dx = begin%di;
        }else if(order==5){
            if(sp==0) return "error";
            result = begin/sp;
            dx = begin%si;
        }else if(order==6){
            if(si==0) return "error";
            result = begin/si;
            dx = begin%si;
        }else if(order==7){
            if(bp==0) return "error";
            result = begin/bp;
            dx = begin%bp;

        }else if(order==8){
            if(ah==0) return "error";
            al = ax/ah;
            ah = ax%ah;
            result = al;
        }else if(order==9){
            if(al==0) return "error";
            al = ax/al;
            ah = ax%al;
            result = al;
        }else if(order==10){
            if(bh==0) return "error";
            al = ax/bh;
            ah = ax%bh;
            result = al;
        }else if(order==11){
            if(bl==0) return "error";
            al = ax/bl;
            ah = ax%bl;
            result = al;
        }else if(order==12){
            if(ch==0) return "error";
            al = ax/ch;
            ah = ax%ch;
            result = al;
        }else if(order==13){
            if(cl==0) return "error";
            al = ax/cl;
            ah = ax%cl;
            result = al;
        }else if(order==14){
            if(dh==0) return "error";
            al = ax/dh;
            ah = ax%dh;
            result = al;
        }else if(order==15){
            if(dl==0) return "error";
            al = ax/dl;
            ah = ax%dl;
            result = al;
        }else return "error";


        if(order<=7){
            if(result>65536){
                return "error";
            }
            ax = result;
            ah = ax/256;
            al = ax%256;
            dh = dx/256;
            dl = dx%256;
        }else{
            if(result>256){
                return "error";
            }
            al = result;
            ax = ah*256+al;
        }
    }else{
        int address = findAddress(str).first;
        int dataLength = findAddress(str).second;
        if(address==-1){
            return "error";
        }
        if(dataLength==0) dataLength = 1;
        int result = 0;
        int value = dataLength==1? memory[address].first: memory[address].first+memory[address+1].first*256;
        if(value==0) return "error";
        if(dataLength==2){
            result = (dx*65536+ax) / value;
            if(result>65536){
                of = 1;
                return "error";
            }
            dx = (dx*65536+ax) % value;
            ax = result;
            dh = dx/256;
            dl = dx%256;
            ah = ax/256;
            al = ax%256;
        }else if(dataLength==1){
            result = ax / value;
            if(result>256){
                of = 1;
                return "error";
            }
            al = result;
            ah = ax % value;
            ax = ah*256+al;
        }else return "error";
    }
    return "";
}

//multiplies appropriate register value with given value
string doMul(string lines[], int i){
    string str = findFirst(lines[i]);
    if(str.compare("error")==0){
        return "error";
    }
    bool isReg = false;
    int order = 0;
    for(int j = 0; j< 16; j++){
        if(str.compare(registers[j])==0){
            isReg = true;
            order = j;
            break;
        }
    }

    if(isReg){
        int result = 0;
        if(order==0){
            result = ax*ax;
        }else if(order==1){
            result = ax*bx;
        }else if(order==2){
            result = ax*cx;
        }else if(order==3){
            result = ax*dx;
        }else if(order==4){
            result = ax*di;
        }else if(order==5){
       //     result = ax*sp;
        }else if(order==6){
            result = ax*si;
        }else if(order==7){
            result = ax*bp;

        }else if(order==8){
            result = al*ah;
        }else if(order==9){
            result = al*al;
        }else if(order==10){
            result = al*bh;
        }else if(order==11){
            result = al*bl;
        }else if(order==12){
            result = al*ch;
        }else if(order==13){
            result = al*cl;
        }else if(order==14){
            result = al*dh;
        }else if(order==15){
            result = al*dl;
        }else return "error";
        if(order<=7){
            if(result>65535){
                dx = result/65536;
                ax = result%65536;
                dh = dx/256;
                dl = dx%256;
                ah = ax/256;
                al = ax%256;
            }else{
                ax = result;
                ah = ax/256;
                al = ax%256;
            }
            if(dx == 0){
                of = 0;
                cf = 0;
            }else{
                of = 1;
                cf = 1;
            }
        }else{
            ax = result;
            ah = ax/256;
            al = ax%256;
            if(ah==0){
                of = 0;
                cf = 0;
            }else{
                of = 1;
                cf = 1;
            }
        }

    }else{
        int address = findAddress(str).first;
        int dataLength = findAddress(str).second;
        if(address==-1) return "error";
        if(dataLength==0) dataLength = 1;
        int value = dataLength==1? memory[address].first: memory[address].first+memory[address+1].first*256;
        int result= 0;
        if(dataLength==2){
            result = ax * value;
            if(result>65535){
                dx = result/65536;
                ax = result%65536;
                dh = dx/256;
                dl = dx%256;
                ah = ax/256;
                al = ax%256;

            }else{
                ax = result;
                ah = ax/256;
                al = ax%256;
            }
            if(dx == 0){
                of = 0;
                cf = 0;
            }else{
                of = 1;
                cf = 1;
            }
        }else if(dataLength==1){
            result = al * value;
            ax = result;
            ah = ax/256;
            al = ax%256;
            if(ah==0){
                of = 0;
                cf = 0;
            }else{
                of = 1;
                cf = 1;
            }
        }else return "error";
    }
    return "";
}

//adds or substructs appropriate values
string doAddanSub(string line, int multiplier){
    string lhs = findFirstSecond(line).first;
    string rhs = findFirstSecond(line).second;

    if(lhs.compare("error")==0||rhs.compare("error")==0){
        return "error";
    }

    int lhsRegOrder = -1;
    int rhsRegOrder = -1;
    int lhsLength = 0;
    int rhsLength = 0;
    int lhsAddress = -1;
    int rhsAddress = -1;
    bool rhsVar = false;
    int rhsNumb = -1;
    int lhsNumb = 0;

    for(int i=0; i<16; i++){
        if(lhs.compare(registers[i])==0){
            lhsRegOrder = i;
            if(i<=7){
                lhsLength = 2;
            }else{
                lhsLength = 1;
            }
        }
        if(rhs.compare(registers[i])==0){
            rhsRegOrder = i;
            if(i<=7){
                rhsLength = 2;
            }else{
                rhsLength = 1;
            }
        }
    }

    if(lhsRegOrder==-1){
        lhsAddress = findAddress(lhs).first;
        if(lhsAddress==-1) return "error";
        lhsLength = findAddress(lhs).second;
        if(lhsLength==0) lhsLength = 1;
    }
    if(rhsRegOrder==-1){
        for(variable v: variables){
            if(rhs.compare(v.name)==0){
                rhsVar = true;
                break;
            }
        }
        if(rhsVar||rhs.find_first_of('[')!=-1){
            rhsAddress = findAddress(rhs).first;
            if(rhsAddress == -1) return "error";
            rhsLength = findAddress(rhs).second;
            if(rhsLength==0) rhsLength = lhsLength;
            if(rhsLength==1){
                rhsNumb = memory[rhsAddress].first;
            }else{
                rhsNumb = memory[rhsAddress].first + memory[rhsAddress+1].first*256;
            }
        }else{
            rhsNumb = findImmediateValue(rhs);
            rhsLength = 3;
        }
    }else{
        if(rhsRegOrder==0){
            rhsNumb = ax;
        }else if(rhsRegOrder==1){
            rhsNumb = bx;
        }else if(rhsRegOrder==2){
            rhsNumb = cx;
        }else if(rhsRegOrder==3){
            rhsNumb = dx;
        }else if(rhsRegOrder==4){
            rhsNumb = di;
        }else if(rhsRegOrder==5){
            rhsNumb = sp;
        }else if(rhsRegOrder==6){
            rhsNumb = si;
        }else if(rhsRegOrder==7){
            rhsNumb = bp;
        }else if(rhsRegOrder==8){
            rhsNumb = ah;
        }else if(rhsRegOrder==9){
            rhsNumb = al;
        }else if(rhsRegOrder==10){
            rhsNumb = bh;
        }else if(rhsRegOrder==11){
            rhsNumb = bl;
        }else if(rhsRegOrder==12){
            rhsNumb = ch;
        }else if(rhsRegOrder==13){
            rhsNumb = cl;
        }else if(rhsRegOrder==14){
            rhsNumb = dh;
        }else if(rhsRegOrder==15){
            rhsNumb = dl;
        }
        if(rhsRegOrder<=7){
            rhsLength = 2;
        }else {
            rhsLength = 1;
        }
    }
    rhsNumb = rhsNumb*multiplier;

    if(lhsRegOrder==-1){
        if(lhsLength==1){
            lhsNumb = memory[lhsAddress].first;
            memory[lhsAddress].first + rhsNumb;
        } else{
            lhsNumb = memory[lhsAddress].first + memory[lhsAddress+1].first*256;
            memory[lhsAddress].first  += rhsNumb%256;
            memory[lhsAddress+1].first += rhsNumb/256;
        }
    }else{
        if(lhsRegOrder==0){
            lhsNumb = ax;
            ax += rhsNumb;
            ah = ax/256;
            al = ax%256;
        }else if(lhsRegOrder==1){
            lhsNumb = bx;
            bx += rhsNumb;
            bh = bx/256;
            bl = bx%256;
        }else if(lhsRegOrder==2){
            lhsNumb = cx;
            cx += rhsNumb;
            ch = cx/256;
            cl = cx%256;
        }else if(lhsRegOrder==3){
            lhsNumb = dx;
            dx += rhsNumb;
            dh = dx/256;
            dl = dx%256;
        }else if(lhsRegOrder==4){
            lhsNumb = di;
            di += rhsNumb;
        }else if(lhsRegOrder==5){
            return "error";
        }else if(lhsRegOrder==6){
            lhsNumb = si;
            si += rhsNumb;
        }else if(lhsRegOrder==7){
            lhsNumb = bp;
            bp += rhsNumb;
        }else if(lhsRegOrder == 8){
            lhsNumb = ah;
            ah += rhsNumb;
            ax = ah*256+al;
        }else if(lhsRegOrder == 9){
            lhsNumb = al;
            al += rhsNumb;
            ax = ah*256+al;
        }else if(lhsRegOrder == 10){
            lhsNumb = bh;
            bh += rhsNumb;
            bx = bh*256+bl;
        }else if(lhsRegOrder == 11){
            lhsNumb = bl;
            bl += rhsNumb;
            bx = bh*256+bl;
        }else if(lhsRegOrder == 12){
            lhsNumb = ch;
            ch += rhsNumb;
            cx = ch*256+cl;
        }else if(lhsRegOrder == 13){
            lhsNumb = cl;
            cl += rhsNumb;
            cx = ch*256+cl;
        }else if(lhsRegOrder == 14){
            lhsNumb = dh;
            dh += rhsNumb;
            dx = dh*256+dl;
        }else if(lhsRegOrder == 15){
            lhsNumb = dl;
            dl += rhsNumb;
            dx = dh*256+dl;
        }
        if(lhsRegOrder<=7){
            lhsLength = 2;
        }else {
            lhsLength = 1;
        }
    }
    if(rhsLength!=lhsLength&&rhsLength!=3) {
        return "error";
    }
    rhsNumb = rhsNumb*multiplier;
    if(multiplier==1){
        int result = 0;
        int power = pow(2,8*lhsLength);
        result =(lhsNumb+rhsNumb);
        if(result>=power){
            cf = 1;
        }else{
            cf = 0;
        }
        if(result==power){
            zf = 1;
        }else{
            zf = 0;
        }
        if(lhsNumb/(power/2)==rhsNumb/(power/2)){
            if(lhsNumb/(power/2)!=result/(power/2)){
                of = 1;
            }
        }else{
            of = 1;
        }
        if(result/(power/2)==1){
            sf = 1;
        }else{
            sf = 0;
        }


    }else{
        int result = 0;
        int power = pow(2,8*lhsLength);
        result = lhsNumb - rhsNumb;
        if(lhsNumb<rhsNumb){
            cf = 1;
        }else {
            cf = 0;
        }
        if(result == 0){
            zf = 1;
        }
        if(result/(power/2)==1){
            sf = 1;
        }else{
            sf = 0;
        }
    }
    //toplama isleminde af:
//    if(multiplier == 1){
//        int result = lhsNumb%16 + rhsNumb%16;
//        if(result>15){
//            af = 1;
//        }else{
//            af = 0;
//        }
//    }
//
    //cikarma isleminde af:
//    if(multiplier== -1){
//        if(lhsNumb%16<rhsNumb%16){
//            af = 1;
//        }else{
//            af = 0;
//        }
//    }

    return "";
}

//mov instructions firstly come here and this function directs them to appropriate specialized function
string doMov(string lines[], int i){
    string movLine = lines[i];
    while(movLine.at(0)==' '){
        movLine.erase(0,1);
    }
    int space = movLine.find_first_of(' ');
    if(space==-1){
        return "error";
    }
    movLine = movLine.substr(space+1);
    space = movLine.find_first_of(' ');
    while(space!=-1){
        movLine.erase(space,1);
        space = movLine.find_first_of(' ');
    }
    int comma = movLine.find_first_of(',');
    if(comma == -1){
        return "error";
    }
    string lhs = movLine.substr(0,comma);
    string rhs = movLine.substr(comma+1);
    if(lhs==""){
        return "error";
    }else if(rhs==""){
        return "error";
    }

    string test;

    bool isLhsVar = false;
    bool isRhsVar = false;
    int rhsVarSize = 0;
    int rhsvarAddress = 0;
    for(variable v: variables){
        if(lhs.compare(v.name)==0){
            isLhsVar = true;
        }
        if(rhs.compare(v.name)==0){
            isRhsVar = true;
            rhsVarSize = v.dataLength;
            rhsvarAddress = v.address;
        }
    }

    bool isLhsReg = false;
    bool isRhsReg = false;
    for(int k=0; k<16; k++){
        if(lhs.compare(registers[k])==0){
            isLhsReg = true;
        }
        if(rhs.compare(registers[k])==0){
            isRhsReg = true;
        }
    }

    ///  Bu alan sadece register movlari icin ///
    if(isLhsReg){
        if(isRhsVar){
            test = doMovRegisterMemory(lhs, rhs, true, rhsVarSize, rhsvarAddress);
            return test;
        }else if(rhs.at(rhs.size()-1)==']'){
            test = doMovRegisterMemory(lhs, rhs, false, 0, 0);
            return test;
        }else if(isRhsReg){
            test = doMovRegisterRegister(lhs, rhs);
            return test;
        }else{
            test = doMovRegisterImmediate(lhs, rhs);
            return test;
        }
    }


//    /////  Bu alan sadece memory kisimlarini inceliyor ///
    if(isLhsVar){
        if(isRhsReg){
            test = doMovMemoryRegister(lhs, rhs, true);
            return test;
        }else{
            test = doMovMemoryImmediate(lhs, rhs, true);
            return test;
        }
    }else if(lhs.at(lhs.size()-1)==']'){
        ///buraya [ax] gibi durumlari eklemek gerekebilir
        if(isRhsReg){
            test = doMovMemoryRegister(lhs, rhs, false);
            return test;
        }else{
            test = doMovMemoryImmediate(lhs, rhs, false);
            return test;
        }
    }
//    /////  Bu alan sadece memory kisimlarini inceliyor ///



    return "";
}

//moves memory value to register
string doMovRegisterMemory(string lhs, string rhs, bool isVar, int varSize, int varAddress){
    int regSize = 0;
    int rhsNumb = 0;
    int dataLength ;
    if(lhs.compare("sp")==0) return "error";
    ///lhs operations
    if(lhs.at(1)=='x'||lhs.at(1)=='i'||lhs.compare("bp")==0){
        regSize = 2;
    }else{
        regSize = 1;
    }

    int address = findAddress(rhs).first;
    if(address==-1) return "error";
    int rhsLength = findAddress(rhs).second;
    if(rhsLength==0) rhsLength = regSize;
    if(rhsLength==1){
        rhsNumb = memory[address].first;
    }else{
        rhsNumb = memory[address].first + memory[address+1].first*256;
    }
    dataLength = rhsLength;

    /// rhs operations


    if(regSize!=dataLength) {
        return "error";
    }
    if((regSize==1&&rhsNumb>255)||(regSize==2&&rhsNumb>65535)) return "error";
    if(lhs.compare("ax")==0){
        ax = rhsNumb;
        al = rhsNumb%256;
        ah = rhsNumb/256;
    }else if(lhs.compare("bx")==0){
        bx = rhsNumb;
        bl = rhsNumb%256;
        bh = rhsNumb/256;
    }else if(lhs.compare("cx")==0){
        cx = rhsNumb;
        cl = rhsNumb%256;
        ch = rhsNumb/256;
    }else if(lhs.compare("dx")==0){
        dx = rhsNumb;
        dl = rhsNumb%256;
        dh = rhsNumb/256;
    }else if(lhs.compare("di")==0){
        di = rhsNumb;
    }else if(lhs.compare("si")==0){
        si = rhsNumb;
    }else if(lhs.compare("bp")==0){
        bp = rhsNumb;
    }else if(lhs.compare("ah")==0){
        ah = rhsNumb;
        ax = ah*256+al;
    }else if(lhs.compare("bh")==0){
        bh = rhsNumb;
        bx = bh*256+bl;
    }else if(lhs.compare("ch")==0){
        ch = rhsNumb;
        cx = ch*256+cl;
    }else if(lhs.compare("dh")==0){
        dh = rhsNumb;
        dx = dh*256+dl;
    }else if(lhs.compare("al")==0){
        al = rhsNumb;
        ax = ah*256+al;
    }else if(lhs.compare("bl")==0){
        bl = rhsNumb;
        bx = bh*256+bl;
    }else if(lhs.compare("cl")==0){
        cl = rhsNumb;
        cx = ch*256+cl;
    }else if(lhs.compare("dl")==0){
        dl = rhsNumb;
        dx = dh*256+dl;
    }else {
        return "error";
    }
    return "";
}

//moves register value to register
string doMovRegisterRegister(string lhs, string rhs){
    int rhsRegSize;
    int lhsRegSize;
    if(lhs.compare("sp")==0) return "error";
    if(lhs.at(1)=='x'||lhs.at(1)=='i'||lhs.compare("bp")==0){
        lhsRegSize = 2;
    }else{
        lhsRegSize = 1;
    }
    if(rhs.at(1)=='x'||rhs.at(1)=='i'||rhs.at(1)=='p'){
        rhsRegSize = 2;
    }else{
        rhsRegSize = 1;
    }

    if(rhsRegSize!=lhsRegSize) return "error";

    if(lhs.compare("ax")==0){
        if(rhs.compare("ax")==0) return "";
        else if(rhs.compare("bx")==0){
            ax = bx;
            ah = bh;
            al = bl;
        }else if(rhs.compare("cx")==0){
            ax = cx;
            ah = ch;
            al = cl;
        }else if(rhs.compare("dx")==0){
            ax = dx;
            ah = dh;
            al = dl;
        }else if(rhs.compare("di")==0){
            ax = di;
            ah = di/256;
            al = di%256;
        }else if(rhs.compare("sp")==0){
            ax = sp;
            ah = sp/256;
            al = sp%256;
        }else if(rhs.compare("si")==0){
            ax = si;
            ah = si/256;
            al = si%256;
        }else if(rhs.compare("bp")==0){
            ax = bp;
            ah = bp/256;
            al = bp%256;
        }
    }else if(lhs.compare("bx")==0){
        if(rhs.compare("bx")==0) return "";
        else if(rhs.compare("ax")==0){
            bx = ax;
            bh = ah;
            bl = al;
        }else if(rhs.compare("cx")==0){
            bx = cx;
            bh = ch;
            bl = cl;
        }else if(rhs.compare("dx")==0){
            bx = dx;
            bh = dh;
            bl = dl;
        }else if(rhs.compare("di")==0){
            bx = di;
            bh = di/256;
            bl = di%256;
        }else if(rhs.compare("sp")==0){
            bx = sp;
            bh = sp/256;
            bl = sp%256;
        }else if(rhs.compare("si")==0){
            bx = si;
            bh = si/256;
            bl = si%256;
        }else if(rhs.compare("bp")==0){
            bx = bp;
            bh = bp/256;
            bl = bp%256;
        }
    }else if(lhs.compare("cx")==0){
        if(rhs.compare("cx")==0) return "";
        else if(rhs.compare("ax")==0){
            cx = ax;
            ch = ah;
            cl = al;
        }else if(rhs.compare("bx")==0){
            cx = bx;
            ch = bh;
            cl = bl;
        }else if(rhs.compare("dx")==0){
            cx = dx;
            ch = dh;
            cl = dl;
        }else if(rhs.compare("di")==0){
            cx = di;
            ch = di/256;
            cl = di%256;
        }else if(rhs.compare("sp")==0){
            cx = sp;
            ch = sp/256;
            cl = sp%256;
        }else if(rhs.compare("si")==0){
            cx = si;
            ch = si/256;
            cl = si%256;
        }else if(rhs.compare("bp")==0){
            cx = bp;
            ch = bp/256;
            cl = bp%256;
        }
    }else if(lhs.compare("dx")==0){
        if(rhs.compare("dx")==0) return "";
        else if(rhs.compare("ax")==0){
            dx = ax;
            dh = ah;
            dl = al;
        }else if(rhs.compare("bx")==0){
            dx = bx;
            dh = bh;
            dl = bl;
        }else if(rhs.compare("cx")==0){
            dx = cx;
            dh = ch;
            dl = cl;
        }else if(rhs.compare("di")==0){
            dx = di;
            dh = di/256;
            dl = di%256;
        }else if(rhs.compare("sp")==0){
            dx = sp;
            dh = sp/256;
            dl = sp%256;
        }else if(rhs.compare("si")==0){
            dx = si;
            dh = si/256;
            dl = si%256;
        }else if(rhs.compare("bp")==0){
            dx = bp;
            dh = bp/256;
            dl = bp%256;
        }
    }else if(lhs.compare("di")==0){
        if(rhs.compare("ax")==0){
            di = ax;
        }else if(rhs.compare("bx")==0){
            di = bx;
        }else if(rhs.compare("cx")==0){
            di = cx;
        }else if(rhs.compare("dx")==0){
            di = dx;
        }else if(rhs.compare("di")==0){
            di = di;
        }else if(rhs.compare("sp")==0){
            di = sp;
        }else if(rhs.compare("si")==0){
            di = si;
        }else if(rhs.compare("bp")==0){
            di = bp;
        }
    }else if(lhs.compare("si")==0){
        if(rhs.compare("ax")==0){
            si = ax;
        }else if(rhs.compare("bx")==0){
            si = bx;
        }else if(rhs.compare("cx")==0){
            si = cx;
        }else if(rhs.compare("dx")==0){
            si = dx;
        }else if(rhs.compare("di")==0){
            si = di;
        }else if(rhs.compare("sp")==0){
            si = sp;
        }else if(rhs.compare("si")==0){
            si = si;
        }else if(rhs.compare("bp")==0){
            si = bp;
        }
    }else if(lhs.compare("bp")==0){
        if(rhs.compare("ax")==0){
            bp = ax;
        }else if(rhs.compare("bx")==0){
            bp = bx;
        }else if(rhs.compare("cx")==0){
            bp = cx;
        }else if(rhs.compare("dx")==0){
            bp = dx;
        }else if(rhs.compare("di")==0){
            bp = di;
        }else if(rhs.compare("sp")==0){
            bp = sp;
        }else if(rhs.compare("si")==0){
            bp = si;
        }else if(rhs.compare("bp")==0){
            bp = bp;
        }
    }else if(lhs.compare("ah")==0){
        if(rhs.compare("ah")==0){
            ah = ah;
        }else if(rhs.compare("al")==0){
            ah = al;
        }else if(rhs.compare("bh")==0){
            ah = bh;
        }else if(rhs.compare("bl")==0){
            ah = bl;
        }else if(rhs.compare("ch")==0){
            ah = ch;
        }else if(rhs.compare("cl")==0){
            ah = cl;
        }else if(rhs.compare("dh")==0){
            ah = dh;
        }else if(rhs.compare("dl")==0){
            ah = dl;
        }
        ax = ah*256 + al;
    }else if(lhs.compare("al")==0){
        if(rhs.compare("ah")==0){
            al = ah;
        }else if(rhs.compare("al")==0){
            al = al;
        }else if(rhs.compare("bh")==0){
            al = bh;
        }else if(rhs.compare("bl")==0){
            al = bl;
        }else if(rhs.compare("ch")==0){
            al = ch;
        }else if(rhs.compare("cl")==0){
            al = cl;
        }else if(rhs.compare("dh")==0){
            al = dh;
        }else if(rhs.compare("dl")==0){
            al = dl;
        }
        ax = ah*256 + al;
    }else if(lhs.compare("bh")==0){
        if(rhs.compare("ah")==0){
            bh = ah;
        }else if(rhs.compare("al")==0){
            bh = al;
        }else if(rhs.compare("bh")==0){
            bh = bh;
        }else if(rhs.compare("bl")==0){
            bh = bl;
        }else if(rhs.compare("ch")==0){
            bh = ch;
        }else if(rhs.compare("cl")==0){
            bh = cl;
        }else if(rhs.compare("dh")==0){
            bh = dh;
        }else if(rhs.compare("dl")==0){
            bh = dl;
        }
        bx = bh*256 + bl;
    }else if(lhs.compare("bl")==0){
        if(rhs.compare("ah")==0){
            bl = ah;
        }else if(rhs.compare("al")==0){
            bl = al;
        }else if(rhs.compare("bh")==0){
            bl = bh;
        }else if(rhs.compare("bl")==0){
            bl = bl;
        }else if(rhs.compare("ch")==0){
            bl = ch;
        }else if(rhs.compare("cl")==0){
            bl = cl;
        }else if(rhs.compare("dh")==0){
            bl = dh;
        }else if(rhs.compare("dl")==0){
            bl = dl;
        }
        bx = bh*256 + bl;
    }else if(lhs.compare("ch")==0){
        if(rhs.compare("ah")==0){
            ch = ah;
        }else if(rhs.compare("al")==0){
            ch = al;
        }else if(rhs.compare("bh")==0){
            ch = bh;
        }else if(rhs.compare("bl")==0){
            ch = bl;
        }else if(rhs.compare("ch")==0){
            ch = ch;
        }else if(rhs.compare("cl")==0){
            ch = cl;
        }else if(rhs.compare("dh")==0){
            ch = dh;
        }else if(rhs.compare("dl")==0){
            ch = dl;
        }
        cx = ch*256 + cl;
    }else if(lhs.compare("cl")==0){
        if(rhs.compare("ah")==0){
            cl = ah;
        }else if(rhs.compare("al")==0){
            cl = al;
        }else if(rhs.compare("bh")==0){
            cl = bh;
        }else if(rhs.compare("bl")==0){
            cl = bl;
        }else if(rhs.compare("ch")==0){
            cl = ch;
        }else if(rhs.compare("cl")==0){
            cl = cl;
        }else if(rhs.compare("dh")==0){
            cl = dh;
        }else if(rhs.compare("dl")==0){
            cl = dl;
        }
        cx = ch*256 + cl;
    }else if(lhs.compare("dh")==0){
        if(rhs.compare("ah")==0){
            dh = ah;
        }else if(rhs.compare("al")==0){
            dh = al;
        }else if(rhs.compare("bh")==0){
            dh = bh;
        }else if(rhs.compare("bl")==0){
            dh = bl;
        }else if(rhs.compare("ch")==0){
            dh = ch;
        }else if(rhs.compare("cl")==0){
            dh = cl;
        }else if(rhs.compare("dh")==0){
            dh = dh;
        }else if(rhs.compare("dl")==0){
            dh = dl;
        }
        dx = dh*256 + dl;
    }else if(lhs.compare("dl")==0){
        if(rhs.compare("ah")==0){
            dl = ah;
        }else if(rhs.compare("al")==0){
            dl = al;
        }else if(rhs.compare("bh")==0){
            dl = bh;
        }else if(rhs.compare("bl")==0){
            dl = bl;
        }else if(rhs.compare("ch")==0){
            dl = ch;
        }else if(rhs.compare("cl")==0){
            dl = cl;
        }else if(rhs.compare("dh")==0){
            dl = dh;
        }else if(rhs.compare("dl")==0){
            dl = dl;
        }
        dx = dh*256 + dl;
    }

    return "";
}

//moves immediate value to register
string doMovRegisterImmediate(string lhs, string rhs){
    int regSize = 0;
    int rhsNumb = 0;
    if(lhs.compare("sp")==0) return "error";
    ///lhs operations
    if(lhs.at(1)=='x'||lhs.at(1)=='i'||lhs.compare("bp")==0){
        regSize = 2;
    }else{
        regSize = 1;
    }

    ///rhs islemleri
    rhsNumb = findImmediateValue(rhs);
    if(rhsNumb==-1) return "error";



    if((regSize==1&&rhsNumb>255)||(regSize==2&&rhsNumb>65535)) return "error";
    if(lhs.compare("ax")==0){
        ax = rhsNumb;
        al = rhsNumb%256;
        ah = rhsNumb/256;
    }else if(lhs.compare("bx")==0){
        bx = rhsNumb;
        bl = rhsNumb%256;
        bh = rhsNumb/256;
    }else if(lhs.compare("cx")==0){
        cx = rhsNumb;
        cl = rhsNumb%256;
        ch = rhsNumb/256;
    }else if(lhs.compare("dx")==0){
        dx = rhsNumb;
        dl = rhsNumb%256;
        dh = rhsNumb/256;
    }else if(lhs.compare("di")==0){
        di = rhsNumb;
    }else if(lhs.compare("si")==0){
        si = rhsNumb;
    }else if(lhs.compare("bp")==0){
        bp = rhsNumb;
    }else if(lhs.compare("ah")==0){
        ah = rhsNumb;
        ax = ah*256+al;
    }else if(lhs.compare("bh")==0){
        bh = rhsNumb;
        bx = bh*256+bl;
    }else if(lhs.compare("ch")==0){
        ch = rhsNumb;
        cx = ch*256+cl;
    }else if(lhs.compare("dh")==0){
        dh = rhsNumb;
        dx = dh*256+dl;
    }else if(lhs.compare("al")==0){
        al = rhsNumb;
        ax = ah*256+al;
    }else if(lhs.compare("bl")==0){
        bl = rhsNumb;
        bx = bh*256+bl;
    }else if(lhs.compare("cl")==0){
        cl = rhsNumb;
        cx = ch*256+cl;
    }else if(lhs.compare("dl")==0){
        dl = rhsNumb;
        dx = dh*256+dl;
    }else {
        return "error";
    }
    return "";
}

//moves register value to memory
string doMovMemoryRegister(string lhs, string rhs, bool isVar){
    int lhsNumb;
    int rhsNumb;
    int memorySize = 0;
    int regSize = 1;
    int address = -1;

    ///rhs islemleri
    if(rhs.compare("ax")==0){
        regSize = 2;
        rhsNumb = ax;
    }else if(rhs.compare("bx")==0){
        regSize = 2;
        rhsNumb = bx;
    }else if(rhs.compare("cx")==0){
        regSize = 2;
        rhsNumb = cx;
    }else if(rhs.compare("dx")==0){
        regSize = 2;
        rhsNumb = dx;
    }else if(rhs.compare("di")==0){
        regSize = 2;
        rhsNumb = di;
    }else if(rhs.compare("si")==0){
        regSize = 2;
        rhsNumb = si;
    }else if(rhs.compare("bp")==0){
        regSize = 2;
        rhsNumb = bp;
    }else if(rhs.compare("sp")==0){
        regSize = 2;
        rhsNumb = sp;
    }else if(rhs.compare("ah")==0){
        rhsNumb = ah;
    }else if(rhs.compare("al")==0){
        rhsNumb = al;
    }else if(rhs.compare("bh")==0){
        rhsNumb = bh;
    }else if(rhs.compare("bl")==0){
        rhsNumb = bl;
    }else if(rhs.compare("ch")==0){
        rhsNumb = ch;
    }else if(rhs.compare("cl")==0){
        rhsNumb = cl;
    }else if(rhs.compare("dh")==0){
        rhsNumb = dh;
    }else if(rhs.compare("dl")==0){
        rhsNumb = dl;
    }else{
        return "error";
    }


    ///lhs islemleri
    address = findAddress(lhs).first;
    if(address==-1) return "error";
    memorySize = findAddress(lhs).second;
    if(memorySize==0) memorySize=regSize;


    if(address<usableMemoryAfter||address>=sp){
        return "error";
    }

    if(memorySize==2){
        memory[address].first = rhsNumb%256;
        memory[address+1].first = rhsNumb/256;
        memory[address].second = "";
        memory[address+1].second = "";
    }else{
        memory[address].first = rhsNumb;
        memory[address].second = "";
    }

    return "";
}

//moves immediate value to memory
string doMovMemoryImmediate(string lhs, string rhs, bool isVar){
    // mov memory , offset var1 || offset b var1
    //case ini de dusunmek lazim
    int lhsNumb;
    int rhsNumb;
    int memorySize = 0;
    int address = -1;

    //rhs islemleri
    rhsNumb = findImmediateValue(rhs);
    if(rhsNumb == -1) return "error";


    ///lhs islemleri
    address = findAddress(lhs).first;
    if(address==-1) return "error";
    memorySize = findAddress(lhs).second;
    if(memorySize==0) memorySize=1;


    if(address<usableMemoryAfter||address>=sp){
        return "error";
    }

    if(memorySize==2){
        memory[address].first = rhsNumb%256;
        memory[address+1].first = rhsNumb/256;
        memory[address].second = "";
        memory[address+1].second = "";
    }else{
        memory[address].first = rhsNumb;
        memory[address].second = "";
    }

    return "";
}

//it finds the variable definitions after the int 20h line and
//put thems to memory
string setVariables(list<variable>& variables, int height, int int20h, string lines[]){
    for(int i=int20h+1; i<height-1; i++){
        string temp = lines[i].substr(0,lines[i].length());
        while(temp.at(0)==' '){
            temp.erase(0,1);
        }
        int space = temp.find_first_of(' ');
        if(space==-1){
            return "error";
        }
        string name = temp.substr(0, space);
        for(variable v: variables){
            if(v.name.compare(name)==0){
                return "error";
            }
        }
        temp = temp.substr(space+1);

        while(temp.at(0)==' '){
            temp.erase(0,1);
        }
        space = temp.find_first_of(' ');
        if(space==-1){
            return "error";
        }
        string type = temp.substr(0,space);
        int dataLength = type.compare("dw")==0? 2 : 1;
        if(type.compare("dw")!=0&&type.compare("db")!=0){
            return "error";
        }
        temp = temp.substr(space+1);

        space = temp.find_first_of(' ');
        while(space!=-1){
            temp.erase(space,1);
            space = temp.find_first_of(' ');
        }
        if(temp.length()==0){
            return "error";
        }

        int data;
        char dataType;
        if(temp.at(0)==39){
            data = (int)temp.at(1);
            dataType = 'c';
        }else {
            data = findImmediateValue(temp);
        }
        variable neu;
        neu.name = name;
        neu.dataType = dataType;
        neu.address = memoryPointer;
        if(memoryPointer>65535){
            return "error";
        }else if(dataLength==2&&memoryPointer>65534){
            return "error";
        }
        neu.dataLength = dataLength;
        if(dataLength==1){
            if((int)data>255){
                return "error";
                //we must set OF 1
            }else{
                memory[memoryPointer].first = data;
                memory[memoryPointer].second = neu.name;
            }
        }else{
            if((int)data>65535){
                return "error";
            }else{
                memory[memoryPointer].first = (int)data%256;
                memory[memoryPointer+1].first = (int)data/256;
                memory[memoryPointer].second = neu.name;
                memory[memoryPointer+1].second = neu.name;
            }
        }
        memoryPointer += dataLength;
        variables.push_front(neu);
    }
    return "";

}

//parses input as lines, prepares the raw input to be used
void parseInput(ifstream &inFileSecond, string lines[], int height){
    string now = "";
    for(int i=0; i<height;i++){
        getline(inFileSecond,now);
        lines[i] = now.substr(0,now.size()-1);
    }
}

//finds how many line we have in the input file
int findLineCount(ifstream& inFile)
{
    string now = "";
    int count = 0;
    while(now.compare("code ends")!=0){
        getline(inFile, now);
        count++;
    }
    return  count;
}

//extracts the first inputs of lines and puts them an array called firsts.
//the purpose is easing the usage of the instructions
string takeFirsts(string lines[], string firsts[], int height, string labels[], int& int20h, string ins[]){
    string temp= "";
    for(int i=1; i<height-1; i++){
        temp = lines[i];
        while(temp.at(0)==' '){
            temp.erase(0,1);
        }

        string x;
        istringstream linestream(temp);
        int a = temp.find_first_of(' ');
        if(a==-1){
            x = temp;
        }else{
            getline(linestream, x, ' ');
        }
        int t = temp.compare("int 20h");
        if(t==0){
            int20h = i;
        }
        if(x.at(x.size()-1)==':'){
            if(int20h!=-1 && int20h<i){
                return "error";
            }
            labels[i] = x.substr(0,x.size()-1);
        }else{
            bool isIns = false;
            for(int j = 0; j < 34; j++){

                if(x.compare(ins[j])==0){
                    memoryPointer += 6;
                    isIns = true;
                    break;
                }
            }

            if(int20h<i && isIns && int20h != -1){
                return "error";
            }else if(!isIns && int20h == -1){
                return "error";
            }else{
                firsts[i] = x;
            }
        }
    }
    return "";
}

//takes decimal value as string and converts it to hexa decimal version as again string
string changeToHexa(string str){
    char hexa[100];
    int a;
    istringstream(str) >> a;
    int i = 0;
    while(a!=0){
        int temp = 0;
        temp = a%16;
        if(temp<10){
            hexa[i] = temp+48;
        }else{
            hexa[i] = temp+55;
        }
        i++;
        a = a/16;
    }
    string result = "";
    for(int j=i-1; j>=0; j--){
        result += hexa[j];
    }
    return result;
}

//takes hexa decimal value as string and converts it to decimal version as integer
int changeToDecimal(string str){
    stringstream s;
    s << str;
    int y;
    s >> hex >> y;
//    return to_string(y);
    return y;
}

//finds the memory address and length of given parameter
pair<int,int> findAddress(string str){
    variable* neu;
    pair<int,int> p ;
    p.first = -1;
    p.second = -1;
    int dataLength= 0;
    int address = 0;
    bool isvar = false;
    for(variable v: variables){
        if(str.at(0)=='b'||str.at(0)=='w'){
            if(v.name.compare(str.substr(1))==0){
                neu = &v;
                isvar = true;
                break;
            }
        }
        if(v.name.compare(str)==0){
            neu = &v;
            isvar = true;
            break;
        }
    }
    if(isvar){
        p.first = neu->address;
        p.second = neu->dataLength;
        return p;
    }else{
        if(str.at(0)=='['){
            if(str.at(str.length()-1)==']'){
                str = str.substr(1,str.length()-2);
                if(str.compare("bx")==0){
                    address = bx;
                }else if(str.compare("si")==0){
                    address = si;
                }else if(str.compare("di")==0){
                    address = di;
                }else if(str.compare("bp")==0){
                    address = bp;
                }else{
                    address = findImmediateValue(str);
                }
                dataLength = 0;
                if(address>=65536||address<usableMemoryAfter){
                    p.first = -1;
                    p.second = -1;
                }else{
                    p.first = address;
                    p.second = dataLength;
                }
            }else return p;
        }else if(str.at(1)=='['){

            if(str.at(str.length()-1)==']'){
                if(str.at(0)=='w'){
                    dataLength = 2;
                }else if(str.at(0)=='b'){
                    dataLength = 1;
                }else return p;
                str = str.substr(2, str.length()-3);
            }else return p;

            if(str.at(str.length()-1)=='h'){
                str = str.substr(0,str.length()-1);
                address = changeToDecimal(str);
            }else if(str.at(0)=='0'){
                address = changeToDecimal(str);
            }else{
                if(str.compare("bx")==0){
                    address = bx;
                }else if(str.compare("si")==0){
                    address = si;
                }else if(str.compare("di")==0){
                    address = di;
                }else if(str.compare("bp")==0){
                    address = bp;
                }else{
                    stringstream s;
                    s << str;
                    s >> address;
                }
            }
            if(address>=65536||address<usableMemoryAfter){
                p.first = -1;
                p.second = -1;
            }else{
                p.first = address;
                p.second = dataLength;
            }
        }
    }
    return p;
}

//finds the first argument of an instruction
string findFirst(string line){
    while(line.at(0)==' '){
        line.erase(0,1);
    }
    int space = line.find_first_of(' ');
    if(space==-1){
        return "error";
    }
    line = line.substr(space+1);
    space = line.find_first_of(' ');
    while(space!=-1){
        line.erase(space,1);
        space = line.find_first_of(' ');
    }
    int comma = line.find_first_of(',');
    if(comma!= -1){
        return "error";
    }
    string str = line;
    return str;
}

//takes decimal value and converts it to binary
string changeToBinary(int data, int length){
    string str = "";
    string s = "";
    int n;
    for(int i=0; i<length*8; i++){
        n = data%2;
        data = data/2;
        str =  (n==1? "1":"0")+ str;
    }
    return str;
}

//finds the first and second argument of an instruction
pair<string, string> findFirstSecond(string line){
    pair<string, string> p;
    p.first = "error";
    p.second = "error";

    while(line.at(0)==' '){
        line.erase(0,1);
    }
    int space = line.find_first_of(' ');
    if(space==-1){
        return p;
    }
    line = line.substr(space+1);
    space = line.find_first_of(' ');
    while(space!=-1){
        line.erase(space,1);
        space = line.find_first_of(' ');
    }
    int comma = line.find_first_of(',');
    if(comma == -1){
        return p;
    }
    string lhs = line.substr(0,comma);
    string rhs = line.substr(comma+1);
    if(lhs==""){
        return p;
    }else if(rhs==""){
        return p;
    }
    p.first = lhs;
    p.second = rhs;
    return p;
}

//converts immediate value to integer
int findImmediateValue(string str){
    if(str.find("offset")!= std::string::npos){
        str = str.substr(6,str.length()-1);
        for(variable v: variables){
            if(str.compare(v.name)==0){
                return v.address;
            }
        }
    }
    if(str.at(str.length()-1)=='h'){
        if(str.at(0)>47&&str.at(0)<58){
            return changeToDecimal(str.substr(0,str.length()-1));
        }else return -1;

    }else if(str.at(0)=='0'){
        return changeToDecimal(str);
    }else if(str.at(0)==39){
        if(str.length()==2){
            return 32;
        }else{
            return (int)str.at(1);
        }
    }else if(str.at(str.length()-1)=='d'){
        str = str.substr(0,str.length()-1);
    }
    int result = 0 ;
    stringstream s;
    s << str;
    s >> result;
    return result;
}

//takes binary and converts it to decimal
int fromBinarytoDecimal(int n){
    int power = 1;
    int result = 0;
    while(n!=0){
        result = result + (n%2)*power;
        power = power *2;
        n = n/10;
    }
    return result ;
}