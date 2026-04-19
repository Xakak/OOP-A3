#include "Q2.h"
#include "AethelgardEngine.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== Aethelgard Simulator ===" << endl << endl;

    AethelgardEngine engine;

    engine.initialize("scenario_alpha.csv");

    // Link entities to kingdoms
    engine.linkEntitiesToKingdoms();

    // Run simulation
    engine.runSimulation();

    cout << "\n=== Simulation Complete ===" << endl;

    return 0;
}