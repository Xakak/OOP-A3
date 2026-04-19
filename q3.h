#ifndef Q3_H
#define Q3_H
class Processor;
class Mainboard;

class MemoryModule {
private:
    unsigned char* codeSeg;   // 2048 bytes, addresses 0x0000-0x07FF
    unsigned char* dataSeg;   // 1792 bytes, addresses 0x0800-0x0EFF

public:
    MemoryModule();
    ~MemoryModule();

    unsigned char read(unsigned short addr);
    void write(unsigned short addr, unsigned char val);

    // used by interpreter to flash words directly
    void loadRawBinary(unsigned short addr, unsigned short word);
};

class RegisterBank {
private:
    unsigned short regs[8];
    unsigned short PC;
    unsigned short IR;
    unsigned char FLAGS;

public:
    RegisterBank();
    unsigned short readReg(int id);
    void writeReg(int id, unsigned short val);
};

class ALU {
private:
    unsigned short doArith(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags);
    unsigned short doBitwise(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags);
    unsigned short doUnaryAndCmp(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags);

public:
    unsigned short execute(unsigned char opcode, unsigned short a, unsigned short b, unsigned char& flags);
};

class Keyboard {
private:
    unsigned char buf[64];
    int bufLen;

public:
    Keyboard();
    void push(unsigned char val);
    unsigned char pop();
    bool isEmpty();
};

class Display {
private:
    char screen[16][32];
    int curRow;
    int curCol;

public:
    Display();
    void printChar(char c);
    void render();
};

class Processor {
private:
    struct Instruction {
        bool isValid;
        bool usesALU;
        unsigned char aluOp;
        bool writesReg;
        bool isMemRead;
        bool isMemWrite;
        bool isBranch;
        bool branchOnZero;
        bool branchOnNotZero;
        bool is4BitImm;
        bool is16BitImm;
    };

    RegisterBank* regs;
    ALU* alu;
    Mainboard* bus;
    Instruction* decodeTable;
    float temp;

    bool halted;
    bool debugMode;
    
    unsigned char curOp;
    unsigned char curDest;
    unsigned char curSrc;
    unsigned short curImm16;
    
    unsigned char cacheBlock[16];
    unsigned short cacheBase;
    int cacheValid;

    void initDecodeTable();
    void checkHeat();
    
    unsigned short fetchWord();
    void decodeStage();
    void executeStage();
    unsigned char cachedRead(unsigned short addr);

public:
    Processor();
    ~Processor();
    void connect(RegisterBank* r, ALU* a, Mainboard* m);
    void step();

    bool isHalted();
    void setDebug(bool d);
    float getTemp();
};

class Mainboard {
private:
    MemoryModule* ram;
    Keyboard* kb;
    Display* gpu;
    Processor* cpu;

    float ramPower;
    float gpuPower;
    float cpuPower;
    
    float ramActivePower;
    float gpuActivePower;
    
    bool powered;

    unsigned char handleMMIORead(unsigned short addr);
    void handleMMIOWrite(unsigned short addr, unsigned char val);

public:
    Mainboard();
    void connect(MemoryModule* m, Keyboard* k, Display* d, Processor* p);
    unsigned char readBus(unsigned short addr);
    void writeBus(unsigned short addr, unsigned char val);
    void pulseClock();
};

#endif
