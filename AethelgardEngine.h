#pragma once

#include "Q2.h"

// Helper functions
int stringLength(const char* str);
int compareStrings(const char* s1, const char* s2);
void copyStringSafe(char* dest, const char* src, int maxLen);
int parseIntFromString(const char* str);

class AethelgardEngine {
public:
    AethelgardEngine();

    ~AethelgardEngine();

    // Initialization
    void initialize(const char* scenarioFile);
    void loadData(const char* filename);
    void linkEntitiesToKingdoms();

    // Main simulation
    void runSimulation();

private:
    // Realms & Relations
    Kingdom* kingdoms[10];
    int relations[10][10];
    int threatMax;
    int currentDay;
    int totalDays;

    // Entity Arrays 
    LandedLord** landedLords;
    int landedLordCount;
    LandlessLord** landlessLords;
    int landlessLordCount;
    WarriorLord** warriorLords;
    int warriorLordCount;

    // Military (Engine OWNS)
    FootSoldier** footSoldiers;
    int footSoldierCount;
    Knight** knights;
    int knightCount;
    Cavalry** cavalries;
    int cavalryCount;
    BatteringRam** batteringRams;
    int batteringRamCount;
    Catapult** catapults;
    int catapultCount;
    WarShip** warShips;
    int warShipCount;

    // Support (Engine OWNS)
    Sage** sages;
    int sageCount;
    Assassin** assassins;
    int assassinCount;

    // Rebel factions
    Kingdom** rebelFactions;
    int rebelCount;

    // Phase methods
    void runDay();
    void phase1_Economy();
    void phase2_Aging();
    void phase3_ShadowTriggers();
    void phase4_DiplomaticDecay();
    void phase5_OpenWarfare();
    void phase6_VoidRift();
    void phase7_CheckEndgame();

    // Helper methods
    void resolveSuccession(int realmIdx);
    void calculateMilitaryPower(int realmIdx);
    void handleCivilWar(int realmIdx);
    void handleInternalCoup(int landlessLordIdx);
    void handleForeignAssassination(int assassinIdx);
    void processCombat(int attackerIdx, int defenderIdx);
    void createRebelFaction(int parentRealmIdx);

    // Output methods
    void printDailyDashboard();
    void printHeader();
    void printRealmStatus();
    void printDiplomaticRelations();
    void printCourtiers();
    void printThreatLevel();

    // Utility
    int getLowestRelation(int realmIdx);
    int getHighestAmbitionLord(int realmIdx);
    bool hasActivePlotInRealm(int realmIdx);
};