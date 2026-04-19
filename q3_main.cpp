#include <iostream>
#include <iomanip>
#include "q3.h"
#include "Interpreter.h"

using namespace std;

int main() {
    // Retro Boot Sequence
    cout << "Lazarus Machine" << endl;
    cout << "ONYX - 16 / 16-Bit Virtual Turing Architecture" << endl;
    cout << "=== BOOTING SILICON PROTOCOL ===" << endl << endl;

    // Boot Menu
    cout << "[1] Turing Complete Human Urdu Calculator" << endl;
    cout << "[2] \"HELLO WORLD\" Urdu Printer" << endl;
    cout << "[3] Hardware Authentication Firewall" << endl;
    
    int selection;
    cout << "Selection (1-3): ";
    cin >> selection;
    
    // consume leftover newline before any cin.getline later
    cin.ignore();

    char verbose;
    cout << "Enable Cycle-by-Cycle Verbose Debug Logging? (Y/N): ";
    cin >> verbose;
    
    // consume leftover newline
    cin.ignore();

    MemoryModule* ram = new MemoryModule();
    RegisterBank* regs = new RegisterBank();
    ALU* alu = new ALU();
    Keyboard* kb = new Keyboard();
    Display* gpu = new Display();
    Processor* cpu = new Processor();
    Mainboard* mb = new Mainboard();

    // Wiring the components
    mb->connect(ram, kb, gpu, cpu);
    cpu->connect(regs, alu, mb);
    
    // pass the user setting to cpu
    if (verbose == 'Y' || verbose == 'y') {
        cpu->setDebug(true);
    } else {
        cpu->setDebug(false);
    }

    // Use the provided hardware interpreter to flash the OS
    Interpreter interp;
    
    if (selection == 1) {
        interp.loadProgramAndFlash("calculator.txt", *ram);
    } else if (selection == 2) {
        interp.loadProgramAndFlash("hello.txt", *ram);
    } else if (selection == 3) {
        interp.loadProgramAndFlash("auth.txt", *ram);
    } else {
        cout << "[BOOTLOADER] Invalid selection. Defaulting to Hello World." << endl;
        interp.loadProgramAndFlash("hello.txt", *ram);
    }

    int cycleCount = 0;
    while (cpu->isHalted() == false) {
        mb->pulseClock();
        cycleCount = cycleCount + 1;
        
        if (verbose == 'Y' || verbose == 'y') {
            if (cycleCount % 100 == 0) {
                cout << "cycles processed: " << cycleCount << endl;
            }
        }
        
        // Safety break for local testing
        if (cycleCount > 50000) {
            if (verbose == 'Y' || verbose == 'y') {
                cout << "[EMULATOR] Emergency shutdown: Cycle limit reached." << endl;
            }
            break;
        }
    }

    // State Dump
    cout << endl << "=== PROCESSOR STATE DUMP ===" << endl;
    cout << "Temperature: " << cpu->getTemp() << " C" << endl;
    cout << hex << uppercase << setfill('0');
    for (int id = 0; id < 8; id += 2) {
        cout << "R" << id << ": 0x" << setw(4) << regs->readReg(id) << "  ";
        cout << "R" << (id + 1) << ": 0x" << setw(4) << regs->readReg(id + 1) << endl;
    }
    cout << endl;
    cout << "PC:    0x" << setw(4) << regs->readReg(8) << endl;
    cout << "IR:    0x" << setw(4) << regs->readReg(9) << endl;
    cout << "FLAGS: 0x" << setw(2) << (regs->readReg(10) & 0xFF) << endl;
    cout << dec << nouppercase << setfill(' ');

    // Final Display Render
    cout << endl;
    gpu->render();

    if (ram != nullptr) delete ram;
    if (regs != nullptr) delete regs;
    if (alu != nullptr) delete alu;
    if (kb != nullptr) delete kb;
    if (gpu != nullptr) delete gpu;
    if (cpu != nullptr) delete cpu;
    if (mb != nullptr) delete mb;

    return 0;
}
