#include "q3.h"
#include <iostream>

using namespace std;

// constructor allocates segments and initialises them
MemoryModule::MemoryModule() {
    this->codeSeg = new unsigned char[2048];
    this->dataSeg = new unsigned char[1792];

    // initialize all memory to 0
    for (int i = 0; i < 2048; i++) {
        codeSeg[i] = 0;
    }
    for (int j = 0; j < 1792; j++) {
        dataSeg[j] = 0;
    }
}

// destructor cleans up dynamic memory
MemoryModule::~MemoryModule() {
    if (codeSeg != nullptr) {
        delete[] codeSeg;
        codeSeg = nullptr;
    }
    if (dataSeg != nullptr) {
        delete[] dataSeg;
        dataSeg = nullptr;
    }
}

unsigned char MemoryModule::read(unsigned short addr) {
    if (addr <= 0x07FF) {
        return codeSeg[addr];
    }
    if (addr >= 0x0800 && addr <= 0x0EFF) {
        // translate the address for the data segment so that it starts form 0
        unsigned short val = addr - 0x0800; 
        return dataSeg[val];
    }

    // pdf says 0xFF on bad read
    cout << "[HARDWARE FAULT] Segmentation Fault: Read out of bounds" << endl;
    return 0xFF;
}

void MemoryModule::write(unsigned short addr, unsigned char val) {
    
    if (addr <= 0x07FF) {
        codeSeg[addr] = val;
        return;
    }
    if (addr >= 0x0800 && addr <= 0x0EFF) {
        dataSeg[addr - 0x0800] = val;//same logic as read
        return;
    }

    // pdf says discard bad write
    cout << "[HARDWARE FAULT] Segmentation Fault: Write out of bounds" << endl;
}

void MemoryModule::loadRawBinary(unsigned short addr, unsigned short word) {
    
    // i think we shift by 8 for the high byte
    unsigned char b1 = (unsigned char)((word >> 8) & 0xFF);//high byte which is left half
    unsigned char b2 = (unsigned char)(word & 0xFF);//low byte is right half
    
    // write bytes sequentially
    write(addr, b1);
    write(addr + 1, b2);
}

// register bank stuff
RegisterBank::RegisterBank() {
    for (int i = 0; i < 8; i++) {
        regs[i] = 0;
    }
    this->PC = 0;
    this->IR = 0;
    FLAGS = 0; 
    // initialized everything to zero as per the pdf
}

unsigned short RegisterBank::readReg(int id) {
    if (id >= 0 && id <= 7) {
        return regs[id];
    }
    if (id == 8) {
        return PC;
    }
    if (id == 9) {
        return IR;
    }
    if (id == 10) {
        // flags is only 8 bits so cast it
        return (unsigned short)FLAGS;
    }
    
    // pdf says return 0 on bad id
    return 0;
}

void RegisterBank::writeReg(int id, unsigned short val) {
    if (id >= 0 && id <= 7) {
        regs[id] = val;
    } else if (id == 8) {
        PC = val;
    } else if (id == 9) {
        IR = val;
    } else if (id == 10) {
        // cast down for the 8-bit latch
        FLAGS = (unsigned char)val;
    } else {
        // do nothing
    }
}


unsigned short ALU::doArith(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags) {
    unsigned short result = 0;
    if (op == 0x01) {
        result = a + b;
    } else if (op == 0x02) {
        result = a - b;
    } else if (op == 0x03) {
        result = a * b;
    } else if (op == 0x04) {
        if (b == 0) {
            result = 0;
        } else {
            result = a / b;
        }
    }
    
    if (result == 0) {
        flags = flags | 0x01;
        flags = flags & ~0x02;
        flags = flags & ~0x04;
    } else {
        flags = flags & ~0x01;
    }
    return result;
}

unsigned short ALU::doBitwise(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags) {
    unsigned short result = 0;
    if (op == 0x05) {
        result = a & b;
    } else if (op == 0x06) {
        result = a | b;
    } else if (op == 0x07) {
        result = a ^ b;
    } else if (op == 0x08) {
        result = ~a;
    }
    
    if (result == 0) {
        flags = flags | 0x01;
        flags = flags & ~0x02;
        flags = flags & ~0x04;
    } else {
        flags = flags & ~0x01;
    }
    return result;
}

unsigned short ALU::doUnaryAndCmp(unsigned char op, unsigned short a, unsigned short b, unsigned char& flags) {
    unsigned short result = 0;
    if (op == 0x09) {
        result = a + 1;
        if (result == 0) {
            flags = flags | 0x01;
            flags = flags & ~0x02;
            flags = flags & ~0x04;
        } else {
            flags = flags & ~0x01;
        }
    } else if (op == 0x0B) {
        result = a - 1;
        if (result == 0) {
            flags = flags | 0x01;
            flags = flags & ~0x02;
            flags = flags & ~0x04;
        } else {
            flags = flags & ~0x01;
        }
    } else if (op == 0x0A) {
        // not sure if i should clear ZF when doing CMP so I will just clear all other flags explicitly
        if (a == b) {
            flags = flags | 0x01;
            flags = flags & ~0x02;
            flags = flags & ~0x04;
        } else if (a < b) {
            flags = flags | 0x02;
            flags = flags & ~0x01;
            flags = flags & ~0x04;
        } else if (a > b) {
            flags = flags | 0x04;
            flags = flags & ~0x01;
            flags = flags & ~0x02;
        }
    }
    return result;
}

unsigned short ALU::execute(unsigned char opcode, unsigned short a, unsigned short b, unsigned char& flags) {
    unsigned short result = 0;
    if (opcode >= 0x01 && opcode <= 0x04) {
        result = doArith(opcode, a, b, flags);
    } else if (opcode >= 0x05 && opcode <= 0x08) {
        result = doBitwise(opcode, a, b, flags);
    } else if (opcode >= 0x09 && opcode <= 0x0B) {
        result = doUnaryAndCmp(opcode, a, b, flags);
    }
    return result;
}

Keyboard::Keyboard() {
    bufLen = 0;
}

void Keyboard::push(unsigned char val) {
    if (bufLen < 64) {
        buf[bufLen] = val;
        bufLen = bufLen + 1;
    }
}

unsigned char Keyboard::pop() {
    if (bufLen == 0) {
        return 0;
    }
    
    unsigned char val = buf[0];
    for (int i = 0; i < bufLen - 1; i++) {
        buf[i] = buf[i + 1];
    }
    bufLen = bufLen - 1;
    return val;
}

bool Keyboard::isEmpty() {
    if (bufLen == 0) {
        return true;
    }
    return false;
}

Display::Display() {
    curRow = 0;
    curCol = 0;
    for (int r = 0; r < 16; r++) {
        for (int c = 0; c < 32; c++) {
            screen[r][c] = ' ';
        }
    }
}

void Display::printChar(char c) {
    if (curRow < 16) {
        screen[curRow][curCol] = c;
        curCol = curCol + 1;
        
        if (curCol == 32) {
            curCol = 0;
            curRow = curRow + 1;
        }
    }
}

void Display::render() {
    // print top border in green
    cout << "\033[32m";
    cout << "+";
    for (int b = 0; b < 32; b++) { cout << "-"; }
    cout << "+" << endl;
    for (int r = 0; r < 16; r++) {
        cout << "|";
        for (int c = 0; c < 32; c++) {
            cout << screen[r][c];
        }
        cout << "|" << endl;
    }
    cout << "+";
    for (int b = 0; b < 32; b++) { cout << "-"; }
    cout << "+" << endl;
    cout << "\033[0m";
}

Mainboard::Mainboard() {
    ram = nullptr;
    kb = nullptr;
    gpu = nullptr;
    cpu = nullptr;
    ramPower = 0.1;
    gpuPower = 2.0;
    cpuPower = 0.0;
    
    ramActivePower = 0.5;
    gpuActivePower = 15.0;
    
    powered = true;
}

void Mainboard::connect(MemoryModule* m, Keyboard* k, Display* d, Processor* p) {
    this->ram = m;
    this->kb = k;
    this->gpu = d;
    this->cpu = p;
}

unsigned char Mainboard::handleMMIORead(unsigned short addr) {
    if (addr == 0x0FF0) {
        if (kb->isEmpty() != false) {
            cout << endl << "[Hardware Interrupt] Awaiting Keyboard Input: ";
            char inputBuf[64];
            cin.getline(inputBuf, 64);
            for (int i = 0; inputBuf[i] != '\0'; i++) {
                kb->push((unsigned char)inputBuf[i]);
            }
        }
        return kb->pop();
    }
    
    if (addr == 0x0FF3) {
        if (kb->isEmpty() != false) {
            cout << endl << "[Hardware Interrupt] Awaiting Keyboard Input: ";
            char inputBuf[64];
            cin.getline(inputBuf, 64);
            for (int i = 0; inputBuf[i] != '\0'; i++) {
                kb->push((unsigned char)inputBuf[i]);
            }
            kb->push((unsigned char)' ');
        }
        // parse integer from buffer manually
        unsigned char result2 = 0;
        unsigned char c = kb->pop();
        while (c >= '0' && c <= '9') {
            result2 = (unsigned char)(result2 * 10 + (c - '0'));
            c = kb->pop();
        }
        return result2;
    }
    return 0;
}
void Mainboard::handleMMIOWrite(unsigned short addr, unsigned char val) {
    if (addr == 0x0FF1) {
        gpu->printChar((char)val);
    }
    
    if (addr == 0x0FF2) {
        // convert byte to decimal string and send each digit
        unsigned char num = val;
        if (num == 0) {
            gpu->printChar('0');
        } else {
            // find digits 
            unsigned char digits[3];
            int dcount = 0;
            unsigned char tmp = num;
            while (tmp > 0) {
                digits[dcount] = tmp % 10;
                dcount = dcount + 1;
                tmp = tmp / 10;
            }
            // print in reverse order
            for (int i = dcount - 1; i >= 0; i--) {
                gpu->printChar((char)('0' + digits[i]));
            }
        }
    }
}

unsigned char Mainboard::readBus(unsigned short addr) {
    
    if (addr >= 0x0F00 && addr <= 0x0FEF) {
        cout << "[MOTHERBOARD FAULT] Invalid Read Address" << endl;
        return 0x00;
    }

    if (addr >= 0x0FF0 && addr <= 0x0FF3) {
        return handleMMIORead(addr);
    }
    if (ram != nullptr) {
        return ram->read(addr);
    }
    return 0;
}

void Mainboard::writeBus(unsigned short addr, unsigned char val) {
  
    if (addr >= 0x0F00 && addr <= 0x0FEF) {
        cout << "[MOTHERBOARD FAULT] Invalid Write Address" << endl;
        return;
    }

    if (addr >= 0x0FF0 && addr <= 0x0FF3) {
        handleMMIOWrite(addr, val);
        return;
    }
    if (ram != nullptr) {
        ram->write(addr, val);
    }
}

void Mainboard::pulseClock() {
    if (powered == false) {
        return;
    }

    float total = 0;
    total = total + ramPower;
    total = total + gpuPower;
    total = total + cpuPower;
    
    // not sure if i should check halt before or after fetch,
    // checking before for now
    
    if (cpu != nullptr && powered != false) {
        cpu->step();
    }
}

Processor::Processor() {
    regs = nullptr;
    alu = nullptr;
    bus = nullptr;
    temp = 25.0; // spec says 25.0C
    halted = false;
    debugMode = false;
    decodeTable = new Instruction[256];
    
    cacheBase = 0;
    cacheValid = 0;
    for (int i = 0; i < 16; i++) { cacheBlock[i] = 0; }
    
    initDecodeTable();
}

Processor::~Processor() {
    if (decodeTable != nullptr) {
        delete[] decodeTable;
        decodeTable = nullptr;
    }
}

void Processor::connect(RegisterBank* r, ALU* a, Mainboard* m) {
    this->regs = r;
    this->alu = a;
    this->bus = m;
}

bool Processor::isHalted() {
    return halted;
}

void Processor::setDebug(bool d) {
    debugMode = d;
}

float Processor::getTemp() {
    return temp;
}

void Processor::initDecodeTable() {
    for (int i = 0; i < 256; i++) {
        decodeTable[i].isValid = false;
        decodeTable[i].usesALU = false;
        decodeTable[i].aluOp = 0;
        decodeTable[i].writesReg = false;
        decodeTable[i].isMemRead = false;
        decodeTable[i].isMemWrite = false;
        decodeTable[i].isBranch = false;
        decodeTable[i].branchOnZero = false;
        decodeTable[i].branchOnNotZero = false;
        decodeTable[i].is4BitImm = false;
        decodeTable[i].is16BitImm = false;
    }
    
    decodeTable[0x00].isValid = true; // NOP
    
    decodeTable[0x01].isValid = true; // ADD
    decodeTable[0x01].usesALU = true;
    decodeTable[0x01].aluOp = 0x01;
    decodeTable[0x01].writesReg = true;
    
    decodeTable[0x02].isValid = true; // SUB
    decodeTable[0x02].usesALU = true;
    decodeTable[0x02].aluOp = 0x02;
    decodeTable[0x02].writesReg = true;
    
    decodeTable[0x03].isValid = true; // MUL
    decodeTable[0x03].usesALU = true;
    decodeTable[0x03].aluOp = 0x03;
    decodeTable[0x03].writesReg = true;
    
    decodeTable[0x04].isValid = true; // DIV
    decodeTable[0x04].usesALU = true;
    decodeTable[0x04].aluOp = 0x04;
    decodeTable[0x04].writesReg = true;
    
    decodeTable[0x0A].isValid = true; // CMP
    decodeTable[0x0A].usesALU = true;
    decodeTable[0x0A].aluOp = 0x0A;
    decodeTable[0x0A].writesReg = false;
    
    decodeTable[0x10].isValid = true; // JMP
    decodeTable[0x10].isBranch = true;
    
    decodeTable[0x11].isValid = true; // JZ
    decodeTable[0x11].isBranch = true;
    decodeTable[0x11].branchOnZero = true;
    
    decodeTable[0x12].isValid = true; // JNZ
    decodeTable[0x12].isBranch = true;
    decodeTable[0x12].branchOnNotZero = true;
    
    decodeTable[0x1A].isValid = true; // LDR_IMM4
    decodeTable[0x1A].writesReg = true;
    decodeTable[0x1A].is4BitImm = true;
    
    decodeTable[0x1B].isValid = true; // LDR_IMM16
    decodeTable[0x1B].writesReg = true;
    decodeTable[0x1B].is16BitImm = true;
    
    decodeTable[0x20].isValid = true; // LDR
    decodeTable[0x20].isMemRead = true;
    decodeTable[0x20].writesReg = true;
    
    decodeTable[0x21].isValid = true; // STR
    decodeTable[0x21].isMemWrite = true;
}

void Processor::checkHeat() {
    float rise = 0.05;
    temp = temp + rise;
    if (temp > 90.0) {
        cout << "[CRITICAL ERROR] Thermal threshold exceeded" << endl;
        unsigned char f = (unsigned char)regs->readReg(10);
        f = (unsigned char)(f | 0x80);
        regs->writeReg(10, (unsigned short)f);
        halted = true;
    }
}

unsigned char Processor::cachedRead(unsigned short addr) {
    if (cacheValid == 1) {
        if (addr >= cacheBase) {
            if (addr < cacheBase + 16) {
                return cacheBlock[addr - cacheBase];
            }
        }
    }
    // cache miss
    unsigned short alignedBase = (unsigned short)(addr - (addr % 16));
    for (int i = 0; i < 16; i++) {
        unsigned short fetchAddr = (unsigned short)(alignedBase + i);
        cacheBlock[i] = bus->readBus(fetchAddr);
    }
    cacheBase = alignedBase;
    cacheValid = 1;
    return cacheBlock[addr - alignedBase];
}

unsigned short Processor::fetchWord() {
    unsigned short pc = regs->readReg(8);
    unsigned char high = bus->readBus(pc);
    unsigned char low = bus->readBus((unsigned short)(pc + 1));
    
    unsigned short fetched = (unsigned short)((high << 8) | low);
    regs->writeReg(9, fetched);
    regs->writeReg(8, (unsigned short)(pc + 2));
    return fetched;
}

void Processor::decodeStage() {
    unsigned short irVal = regs->readReg(9);
    curOp = (unsigned char)(irVal >> 8);
    curDest = (unsigned char)((irVal >> 4) & 0x0F);
    curSrc = (unsigned char)(irVal & 0x0F);
    
    if (decodeTable[curOp].is16BitImm != false) {
        unsigned short pc = regs->readReg(8);
        unsigned char h = cachedRead(pc);
        unsigned char l = cachedRead((unsigned short)(pc + 1));
        curImm16 = (unsigned short)((h << 8) | l);
        regs->writeReg(8, (unsigned short)(pc + 2));
    } else if (decodeTable[curOp].is4BitImm != false) {
        curImm16 = curSrc;
    }
}

void Processor::executeStage() {
    
    if (decodeTable[curOp].isValid == false) {
        cout << "[PROCESSOR FAULT] Invalid Instruction 0x" << hex << (int)curOp << dec << endl;
        return;
    }
    
    unsigned char flagsReg = (unsigned char)regs->readReg(10);
    if ((flagsReg & 0x80) != 0) {
        halted = true;
        return;
    }
    
    unsigned short result = 0;
    unsigned short aVal = regs->readReg(curDest);
    unsigned short bVal = regs->readReg(curSrc);
    
    if (decodeTable[curOp].usesALU != false) {
        unsigned char f = (unsigned char)regs->readReg(10);
        result = alu->execute(curOp, aVal, bVal, f);
        regs->writeReg(10, (unsigned short)f);
    }
    
    if (decodeTable[curOp].isMemRead != false) {
        result = (unsigned short)bus->readBus(bVal);
    }
    
    if (decodeTable[curOp].isMemWrite != false) {
        bus->writeBus(bVal, (unsigned char)aVal);
        
        if (cacheValid == 1) {
            if (bVal >= cacheBase) {
               if (bVal < cacheBase + 16) {
                    cacheValid = 0;
            }
            }
        }
    }
    
    if (decodeTable[curOp].isBranch != false) {
        unsigned char f = (unsigned char)regs->readReg(10);
        unsigned char zf = (unsigned char)(f & 0x01);
        bool doJump = false;
        if (decodeTable[curOp].branchOnZero != false && zf != 0) {
            doJump = true;
        }
        if (decodeTable[curOp].branchOnNotZero != false && zf == 0) {
            doJump = true;
        }
        if (decodeTable[curOp].branchOnZero == false && decodeTable[curOp].branchOnNotZero == false) {
            doJump = true;
    }
        if (doJump != false) {
            regs->writeReg(8, regs->readReg(curSrc));
        }
    }
    
    if (decodeTable[curOp].is4BitImm != false || decodeTable[curOp].is16BitImm != false) {
        result = curImm16;
    }
    
    if (decodeTable[curOp].writesReg != false) {
        if (decodeTable[curOp].isMemWrite == false && decodeTable[curOp].isBranch == false) {
            if (decodeTable[curOp].usesALU != false && curOp == 0x0A) {
                // skip writeback for CMP
            } else {
                regs->writeReg(curDest, result);
            }
        }
    }
    
    checkHeat();
}
void Processor::step() {
    if (halted != false) {
        return;
    }
    unsigned char hfCheck = (unsigned char)regs->readReg(10);
    if ((hfCheck & 0x80) != 0) {
        halted = true;
        return;
    }

    unsigned short ir = fetchWord();
    
    if (ir == 0x0000) {
        cout << "[PROCESSOR] End-of-File (0x0000) reached. Halting." << endl;
        halted = true;
        return;
    }

    decodeStage();
    
    if (debugMode != false) {
        unsigned char dbgOp = (unsigned char)(ir >> 8);
        unsigned char dbgDest = (unsigned char)((ir >> 4) & 0x0F);
        unsigned char dbgSrc = (unsigned char)(ir & 0x0F);
        cout << "  [TRACE] PC=0x" << hex << (regs->readReg(8) - 2);
        cout << " IR=0x" << ir;
        cout << " OP=0x" << (int)dbgOp;
        cout << " DEST=R" << dec << (int)dbgDest;
        cout << " SRC=R" << (int)dbgSrc << endl;
    }
    executeStage();
}
