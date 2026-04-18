#include <iostream>
#include "q1.h"

using namespace std;

int main() {
    // Sophomore level setup
    int width = 25;
    int height = 15;
    int maxPop = 30;
    
    // Rule 2: Manual allocation
    World* myWorld = new World(width, height, maxPop);

    // Initial dummy organisms for Q1
    // Using simple coordinates
    myWorld->addOrganism(new FractalSprawler(2, 2, 2, 2));
    myWorld->addOrganism(new FractalSprawler(10, 5, 2, 2));
    myWorld->addOrganism(new KineticHunter(18, 10, 3, 3));

    int totalIterations = 50;
    cout << "Press Enter to start simulation and to advance each turn..." << endl;
    cin.get();

    for (int i = 0; i < totalIterations; i++) {
        cout << "--- Iteration " << i + 1 << " ---" << endl;
        
        myWorld->runIteration();
        myWorld->draw();
        
        cout << "Press Enter for next turn...";
        cin.get(); // Turn-based progression requirement
    }

    // Cleaning up all memory
    delete myWorld;

    return 0;
}
