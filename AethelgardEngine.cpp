#include "AethelgardEngine.h"
#include <iostream>
#include <fstream>

using namespace std;

// HELPER FUNCTIONS
int stringLength(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

int compareStrings(const char* s1, const char* s2) {
    int i = 0;
    while (s1[i] != '\0' && s2[i] != '\0') {
        if (s1[i] != s2[i]) {
            return (s1[i] < s2[i]) ? -1 : 1;
        }
        i++;
    }
    if (s1[i] == '\0' && s2[i] == '\0') return 0;
    return (s1[i] == '\0') ? -1 : 1;
}

void copyStringSafe(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (src[i] != '\0' && i < maxLen - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int parseIntFromString(const char* str) {
    int result = 0;
    int i = 0;
    while (str[i] == ' ' || str[i] == '\t') i++;
    bool negative = false;
    if (str[i] == '-') {
        negative = true;
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return negative ? -result : result;
}

// CONSTRUCTOR & DESTRUCTOR
AethelgardEngine::AethelgardEngine() {
    currentDay = 0;
    threatMax = 60;
    totalDays = 0;

    // Initialize kingdoms array
    for (int i = 0; i < 10; i++) {
        kingdoms[i] = nullptr;
    }

    // Initialize relations
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            relations[i][j] = 0;
        }
    }

    // Initialize all entity pointers
    landedLords = nullptr;
    landedLordCount = 0;
    landlessLords = nullptr;
    landlessLordCount = 0;
    warriorLords = nullptr;
    warriorLordCount = 0;
    footSoldiers = nullptr;
    footSoldierCount = 0;
    knights = nullptr;
    knightCount = 0;
    cavalries = nullptr;
    cavalryCount = 0;
    batteringRams = nullptr;
    batteringRamCount = 0;
    catapults = nullptr;
    catapultCount = 0;
    warShips = nullptr;
    warShipCount = 0;
    sages = nullptr;
    sageCount = 0;
    assassins = nullptr;
    assassinCount = 0;
    rebelFactions = nullptr;
    rebelCount = 0;
}

AethelgardEngine::~AethelgardEngine() {
    for (int i = 0; i < landedLordCount; i++) {
        if (landedLords[i] != nullptr) delete landedLords[i];
    }
    if (landedLords != nullptr) delete[] landedLords;

    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr) delete landlessLords[i];
    }
    if (landlessLords != nullptr) delete[] landlessLords;

    for (int i = 0; i < warriorLordCount; i++) {
        if (warriorLords[i] != nullptr) delete warriorLords[i];
    }
    if (warriorLords != nullptr) delete[] warriorLords;

    for (int i = 0; i < footSoldierCount; i++) {
        if (footSoldiers[i] != nullptr) delete footSoldiers[i];
    }
    if (footSoldiers != nullptr) delete[] footSoldiers;

    for (int i = 0; i < knightCount; i++) {
        if (knights[i] != nullptr) delete knights[i];
    }
    if (knights != nullptr) delete[] knights;

    for (int i = 0; i < cavalryCount; i++) {
        if (cavalries[i] != nullptr) delete cavalries[i];
    }
    if (cavalries != nullptr) delete[] cavalries;

    for (int i = 0; i < batteringRamCount; i++) {
        if (batteringRams[i] != nullptr) delete batteringRams[i];
    }
    if (batteringRams != nullptr) delete[] batteringRams;

    for (int i = 0; i < catapultCount; i++) {
        if (catapults[i] != nullptr) delete catapults[i];
    }
    if (catapults != nullptr) delete[] catapults;

    for (int i = 0; i < warShipCount; i++) {
        if (warShips[i] != nullptr) delete warShips[i];
    }
    if (warShips != nullptr) delete[] warShips;

    for (int i = 0; i < sageCount; i++) {
        if (sages[i] != nullptr) delete sages[i];
    }
    if (sages != nullptr) delete[] sages;

    for (int i = 0; i < assassinCount; i++) {
        if (assassins[i] != nullptr) delete assassins[i];
    }
    if (assassins != nullptr) delete[] assassins;

    // Delete kingdoms
    for (int i = 0; i < 10; i++) {
        if (kingdoms[i] != nullptr) delete kingdoms[i];
    }

    // Delete rebel factions
    for (int i = 0; i < rebelCount; i++) {
        if (rebelFactions[i] != nullptr) delete rebelFactions[i];
    }
    if (rebelFactions != nullptr) delete[] rebelFactions;
}

// INITIALIZATION
void AethelgardEngine::initialize(const char* scenarioFile) {
    cout << "[AETHELGARD] Loading scenario: " << scenarioFile << endl;

    // Load all data from CSV
    loadData(scenarioFile);

    // Link entities to kingdoms
    linkEntitiesToKingdoms();

    cout << "[AETHELGARD] Initialization complete with " << threatMax << " days" << endl;
}

// ============================================================
// LOAD DATA FROM CSV
// ============================================================

void AethelgardEngine::loadData(const char* filename) {
    cout << "[LOAD] Starting to load data from: " << filename << endl;

    // First initialize kingdoms array
    for (int i = 0; i < 10; i++) {
        if (kingdoms[i] == nullptr) {
            kingdoms[i] = new Kingdom();
        }
        kingdoms[i]->realmIdx = i;
    }

    // Open file
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "[ERROR] Cannot open file: " << filename << endl;
        return;
    }
    cout << "[LOAD] File opened successfully" << endl;

    char buffer[500];
    int lineNum = 0;
    int currentSection = -1; // 0=Kingdoms, 1=Relations, 2=Lords, 3=Military, 4=Sages, 5=Assassins, 6=Config

    // Temporary storage for parsing
    LandedLord* tempLanded[50];
    LandlessLord* tempLandless[50];
    WarriorLord* tempWarrior[50];
    int tempLandedCount = 0;
    int tempLandlessCount = 0;
    int tempWarriorCount = 0;

    FootSoldier* tempFoot[5000];  // Large enough for all units
    Knight* tempKnight[5000];
    Cavalry* tempCavalry[5000];
    BatteringRam* tempRam[500];
    Catapult* tempCat[500];
    WarShip* tempShip[500];
    int tempFootCount = 0;
    int tempKnightCount = 0;
    int tempCavalryCount = 0;
    int tempRamCount = 0;
    int tempCatCount = 0;
    int tempShipCount = 0;

    Sage* tempSage[50];
    int tempSageCount = 0;

    Assassin* tempAssassin[50];
    int tempAssassinCount = 0;

    while (file.getline(buffer, sizeof(buffer))) {
        lineNum++;

        // Skip empty lines and comments
        int i = 0;
        while (buffer[i] == ' ' || buffer[i] == '\t') i++;
        if (buffer[i] == '\0' || buffer[i] == '#' || buffer[i] == ',') continue;

        // Check for section headers
        if (buffer[0] == '[') {
            // Find section name
            char section[50];
            int j = 1;
            while (buffer[j] != ']' && buffer[j] != '\0' && j < 49) {
                section[j-1] = buffer[j];
                j++;
            }
            section[j-1] = '\0';

            if (compareStrings(section, "KINGDOMS") == 0) currentSection = 0;
            else if (compareStrings(section, "RELATIONS") == 0) currentSection = 1;
            else if (compareStrings(section, "LORDS") == 0) currentSection = 2;
            else if (compareStrings(section, "MILITARY") == 0) currentSection = 3;
            else if (compareStrings(section, "SAGES") == 0) currentSection = 4;
            else if (compareStrings(section, "ASSASSINS") == 0) currentSection = 5;
            else if (compareStrings(section, "CONFIG") == 0) currentSection = 6;
            continue;
        }

        // Skip header rows (first data row after section)
        if (currentSection == 0 && buffer[0] == 'R' && buffer[1] == 'e') continue;
        if (currentSection == 2 && buffer[0] == 'L' && buffer[1] == 'o') continue;
        if (currentSection == 3 && buffer[0] == 'R' && buffer[1] == 'e') continue;
        if (currentSection == 4 && buffer[0] == 'R' && buffer[1] == 'e') continue;
        if (currentSection == 5 && buffer[0] == 'A' && buffer[1] == 's') continue;

        // Parse based on section
        if (currentSection == 0) { // KINGDOMS
            char tokens[10][50];
            int tokenCount = 0;
            int charIdx = 0;
            char current[50];

            // Simple tokenization
            int k = 0;
            int t = 0;
            while (buffer[k] != '\0' && tokenCount < 6) {
                if (buffer[k] == ',') {
                    current[charIdx] = '\0';
                    copyStringSafe(tokens[tokenCount], current, 50);
                    tokenCount++;
                    charIdx = 0;
                } else if (buffer[k] != '\r' && buffer[k] != '\n') {
                    if (charIdx < 49) {
                        current[charIdx] = buffer[k];
                        charIdx++;
                    }
                }
                k++;
            }
            if (charIdx > 0) {
                current[charIdx] = '\0';
                copyStringSafe(tokens[tokenCount], current, 50);
                tokenCount++;
            }

            if (tokenCount >= 6) {
                int realmId = parseIntFromString(tokens[0]);
                char* terrain = tokens[5];

                if (realmId >= 0 && realmId < 10) {
                    Kingdom* kingdom = nullptr;

                    if (compareStrings(terrain, "Frozen") == 0) {
                        if (realmId == 9) {
                            kingdom = new TheVoidRift();
                        } else {
                            kingdom = new FrostPeaksKingdom();
                        }
                    } else if (compareStrings(terrain, "Coastal") == 0) {
                        kingdom = new CoastalKingdom();
                    } else {
                        kingdom = new VerdantKingdom();
                    }

                    kingdom->realmIdx = realmId;
                    copyStringSafe(kingdom->realmName, tokens[1], 50);
                    kingdom->wealth = parseIntFromString(tokens[2]);
                    kingdom->defenseStat = parseIntFromString(tokens[4]);

                    delete kingdoms[realmId];
                    kingdoms[realmId] = kingdom;
                }
            }
        }
        else if (currentSection == 1) { // RELATIONS
            // Skip empty or comment lines
            int check = 0;
            while (buffer[check] == ' ' || buffer[check] == '\t') check++;
            if (buffer[check] == '\0' || buffer[check] == '#') continue;

            // Find which row we're on by counting non-empty lines
            static int relRow = 0;
            if (relRow >= 10) {
                // Skip after 10 rows
                return;
            }

            // Parse 10 space-separated integers
            int col = 0;
            int start = 0;
            while (buffer[start] == ' ' || buffer[start] == '\t') start++;
            int pos = start;

            while (col < 10 && buffer[pos] != '\0') {
                if (buffer[pos] == ' ' || buffer[pos] == '\t') {
                    if (pos > start) {
                        char num[20];
                        int n = 0;
                        for (int x = start; x < pos && n < 19; x++) {
                            num[n++] = buffer[x];
                        }
                        num[n] = '\0';
                        relations[relRow][col] = parseIntFromString(num);
                        col++;
                    }
                    start = pos + 1;
                }
                pos++;
            }
            if (col < 10 && pos > start) {
                char num[20];
                int n = 0;
                for (int x = start; x < pos && n < 19; x++) {
                    num[n++] = buffer[x];
                }
                num[n] = '\0';
                relations[relRow][col] = parseIntFromString(num);
            }

            cout << "[LOAD] Parsed relations row " << relRow << endl;
            relRow++;
        }
        else if (currentSection == 2) { // LORDS
            // Skip header line
            int check = 0;
            while (buffer[check] == ' ' || buffer[check] == '\t') check++;
            if (buffer[check] == 'L' || buffer[check] == 'l') {
                continue;
            }

            char tokens[15][50];
            int tokenCount = 0;
            int charIdx = 0;
            char current[50];

            int k = 0;
            while (buffer[k] != '\0' && tokenCount < 10) {
                if (buffer[k] == ',') {
                    current[charIdx] = '\0';
                    copyStringSafe(tokens[tokenCount], current, 50);
                    tokenCount++;
                    charIdx = 0;
                } else if (buffer[k] != '\r' && buffer[k] != '\n') {
                    if (charIdx < 49) {
                        current[charIdx] = buffer[k];
                        charIdx++;
                    }
                }
                k++;
            }
            if (charIdx > 0) {
                current[charIdx] = '\0';
                copyStringSafe(tokens[tokenCount], current, 50);
                tokenCount++;
            }

            if (tokenCount >= 10) {
                int lordId = parseIntFromString(tokens[0]);
                char* name = tokens[1];
                int age = parseIntFromString(tokens[2]);
                char* classType = tokens[4];
                int realmIdx = parseIntFromString(tokens[5]);
                int strategy = parseIntFromString(tokens[6]);
                int ambition = parseIntFromString(tokens[7]);
                int diplomacy = parseIntFromString(tokens[8]);
                int fatherId = parseIntFromString(tokens[9]);

                if (compareStrings(classType, "LandedLord") == 0) {
                    LandedLord* lord = new LandedLord();
                    lord->Entity::id = lordId;
                    copyStringSafe(lord->Entity::name, name, 50);
                    lord->age = age;
                    lord->isAlive = true;
                    lord->realmIdx = realmIdx;
                    lord->strategyStat = strategy;
                    lord->ambitionStat = ambition;
                    lord->diplomacyStat = diplomacy;
                    lord->fatherId = fatherId;
                    tempLanded[tempLandedCount++] = lord;
                }
                else if (compareStrings(classType, "LandlessLord") == 0) {
                    LandlessLord* lord = new LandlessLord();
                    lord->Entity::id = lordId;
                    copyStringSafe(lord->Entity::name, name, 50);
                    lord->age = age;
                    lord->isAlive = true;
                    lord->realmIdx = realmIdx;
                    lord->strategyStat = strategy;
                    lord->ambitionStat = ambition;
                    lord->diplomacyStat = diplomacy;
                    lord->fatherId = fatherId;
                    tempLandless[tempLandlessCount++] = lord;
                }
                else if (compareStrings(classType, "WarriorLord") == 0) {
                    WarriorLord* lord = new WarriorLord();
                    lord->Entity::id = lordId;
                    copyStringSafe(lord->Entity::name, name, 50);
                    lord->age = age;
                    lord->isAlive = true;
                    lord->realmIdx = realmIdx;
                    lord->strategyStat = strategy;
                    lord->ambitionStat = ambition;
                    lord->diplomacyStat = diplomacy;
                    lord->fatherId = fatherId;
                    tempWarrior[tempWarriorCount++] = lord;
                }
            }
        }
        else if (currentSection == 3) { // MILITARY
            // Skip header line
            int check = 0;
            while (buffer[check] == ' ' || buffer[check] == '\t') check++;
            if (buffer[check] == 'R' || buffer[check] == 'r') {
                cout << "[LOAD] Skipping MILITARY header" << endl;
                continue;
            }

cout << "[LOAD] Parsing MILITARY line: " << buffer << endl;

            // Check if this is actually data (starts with a digit)
            if (buffer[0] < '0' || buffer[0] > '9') {
                cout << "[LOAD] Skipping non-numeric MILITARY line: " << buffer << endl;
                continue;
            }

            cout << "[LOAD] Parsing MILITARY: " << buffer << endl;

            char tokens[10][50];
            int tokenCount = 0;
            int charIdx = 0;
            char current[50];

            int k = 0;
            while (buffer[k] != '\0' && tokenCount < 7) {
                if (buffer[k] == ',') {
                    current[charIdx] = '\0';
                    copyStringSafe(tokens[tokenCount], current, 50);
                    tokenCount++;
                    charIdx = 0;
                } else if (buffer[k] != '\r' && buffer[k] != '\n') {
                    if (charIdx < 49) {
                        current[charIdx] = buffer[k];
                        charIdx++;
                    }
                }
                k++;
            }
            if (charIdx > 0) {
                current[charIdx] = '\0';
                copyStringSafe(tokens[tokenCount], current, 50);
                tokenCount++;
            }

            if (tokenCount >= 3) {
                int realmIdx = parseIntFromString(tokens[0]);
                char* unitType = tokens[1];
                int count = parseIntFromString(tokens[2]);
                int attack = (tokenCount >= 4) ? parseIntFromString(tokens[3]) : 10;
                int hp = (tokenCount >= 5) ? parseIntFromString(tokens[4]) : 100;
                int siegeDmg = (tokenCount >= 6) ? parseIntFromString(tokens[5]) : 0;
                int aoeTargets = (tokenCount >= 7) ? parseIntFromString(tokens[6]) : 0;

                if (compareStrings(unitType, "FootSoldier") == 0) {
                    for (int c = 0; c < count; c++) {
                        FootSoldier* fs = new FootSoldier();
                        fs->Entity::id = tempFootCount;
                        fs->realmIdx = realmIdx;
                        fs->hp = hp;
                        fs->maxHp = hp;
                        fs->attackPower = attack;
                        fs->maintenanceCost = 5;
                        fs->armor = 2;
                        fs->servesFrontline = true;
                        tempFoot[tempFootCount++] = fs;
                    }
                }
if (compareStrings(unitType, "Knight") == 0) {
                    for (int c = 0; c < count; c++) {
                        Knight* k = new Knight();
                        k->Entity::id = tempKnightCount;
                        k->realmIdx = realmIdx;
                        k->hp = hp;
                        k->maxHp = hp;
                        k->attackPower = attack;
                        k->maintenanceCost = 15;
                        k->armor = 8;
                        k->shieldBlock = 10;
                        k->servesFrontline = true;
                        tempKnight[tempKnightCount++] = k;
                    }
                }
                else if (compareStrings(unitType, "Cavalry") == 0) {
                    for (int c = 0; c < count; c++) {
                        Cavalry* cav = new Cavalry();
                        cav->Entity::id = tempCavalryCount;
                        cav->realmIdx = realmIdx;
                        cav->hp = hp;
                        cav->maxHp = hp;
                        cav->attackPower = attack;
                        cav->maintenanceCost = 25;
                        cav->armor = 5;
                        cav->chargeMultiplier = 2.0f;
                        cav->terrainPenalty = 0;
                        tempCavalry[tempCavalryCount++] = cav;
                    }
                }
                else if (compareStrings(unitType, "Cavalry") == 0) {
                    for (int c = 0; c < count; c++) {
                        Cavalry* cav = new Cavalry();
                        cav->Entity::id = tempCavalryCount;
                        cav->realmIdx = realmIdx;
                        cav->hp = hp;
                        cav->maxHp = hp;
                        cav->attackPower = attack;
                        cav->maintenanceCost = 25;
                        cav->armor = 5;
                        cav->chargeMultiplier = 2.0f;
                        cav->terrainPenalty = 0;
                        tempCavalry[tempCavalryCount++] = cav;
                    }
                }
                else if (compareStrings(unitType, "BatteringRam") == 0) {
                    for (int c = 0; c < count; c++) {
                        BatteringRam* br = new BatteringRam();
                        br->Entity::id = tempRamCount;
                        br->realmIdx = realmIdx;
                        br->hp = hp;
                        br->maxHp = hp;
                        br->siegeDamage = siegeDmg;
                        br->speed = 2;
                        br->maintenanceCost = 30;
                        br->armor = 15;
                        tempRam[tempRamCount++] = br;
                    }
                }
                else if (compareStrings(unitType, "Catapult") == 0) {
                    for (int c = 0; c < count; c++) {
                        Catapult* cat = new Catapult();
                        cat->Entity::id = tempCatCount;
                        cat->realmIdx = realmIdx;
                        cat->hp = hp;
                        cat->maxHp = hp;
                        cat->siegeDamage = siegeDmg;
                        cat->aoeDamage = siegeDmg / 2;
                        cat->aoeTargets = aoeTargets;
                        cat->speed = 1;
                        cat->maintenanceCost = 35;
                        cat->armor = 5;
                        tempCat[tempCatCount++] = cat;
                    }
                }
                else if (compareStrings(unitType, "WarShip") == 0) {
                    for (int c = 0; c < count; c++) {
                        WarShip* ws = new WarShip();
                        ws->Entity::id = tempShipCount;
                        ws->realmIdx = realmIdx;
                        ws->hp = hp;
                        ws->maxHp = hp;
                        ws->armor = 10;
                        ws->maintenanceCost = 40;
                        ws->siegeDamage = siegeDmg;
                        ws->speed = 3;
                        ws->troopCapacity = 100;
                        ws->knotsSpeed = 10;
                        ws->aoeDamage = siegeDmg / 2;
                        ws->aoeTargets = aoeTargets;
                        tempShip[tempShipCount++] = ws;
                    }
                }
            }
        }
        else if (currentSection == 4) { // SAGES
            char tokens[10][50];
            int tokenCount = 0;
            int charIdx = 0;
            char current[50];

            int k = 0;
            while (buffer[k] != '\0' && tokenCount < 4) {
                if (buffer[k] == ',') {
                    current[charIdx] = '\0';
                    copyStringSafe(tokens[tokenCount], current, 50);
                    tokenCount++;
                    charIdx = 0;
                } else if (buffer[k] != '\r' && buffer[k] != '\n') {
                    if (charIdx < 49) {
                        current[charIdx] = buffer[k];
                        charIdx++;
                    }
                }
                k++;
            }
            if (charIdx > 0) {
                current[charIdx] = '\0';
                copyStringSafe(tokens[tokenCount], current, 50);
                tokenCount++;
            }

            if (tokenCount >= 4) {
                int realmIdx = parseIntFromString(tokens[0]);
                char* name = tokens[1];
                int healingPower = parseIntFromString(tokens[2]);
                int wisdom = parseIntFromString(tokens[3]);

                Sage* sage = new Sage();
                sage->Entity::id = tempSageCount;
                copyStringSafe(sage->Entity::name, name, 50);
                sage->realmIdx = realmIdx;
                sage->healingPower = healingPower;
                sage->wisdom = wisdom;
                sage->age = 40;
                sage->isAlive = true;
                tempSage[tempSageCount++] = sage;
            }
        }
        else if (currentSection == 5) { // ASSASSINS
            char tokens[10][50];
            int tokenCount = 0;
            int charIdx = 0;
            char current[50];

            int k = 0;
            while (buffer[k] != '\0' && tokenCount < 5) {
                if (buffer[k] == ',') {
                    current[charIdx] = '\0';
                    copyStringSafe(tokens[tokenCount], current, 50);
                    tokenCount++;
                    charIdx = 0;
                } else if (buffer[k] != '\r' && buffer[k] != '\n') {
                    if (charIdx < 49) {
                        current[charIdx] = buffer[k];
                        charIdx++;
                    }
                }
                k++;
            }
            if (charIdx > 0) {
                current[charIdx] = '\0';
                copyStringSafe(tokens[tokenCount], current, 50);
                tokenCount++;
            }

            if (tokenCount >= 5) {
                int id = parseIntFromString(tokens[0]);
                char* name = tokens[1];
                int age = parseIntFromString(tokens[2]);
                int stealthStat = parseIntFromString(tokens[3]);
                int clientRealmIdx = parseIntFromString(tokens[4]);

                Assassin* ass = new Assassin();
                ass->Entity::id = id;
                copyStringSafe(ass->Entity::name, name, 50);
                ass->age = age;
                ass->stealthStat = stealthStat;
                ass->clientIdx = clientRealmIdx;
                ass->isAlive = true;
                tempAssassin[tempAssassinCount++] = ass;
            }
        }
        else if (currentSection == 6) { // CONFIG
            if (buffer[0] == 'T' && buffer[1] == 'h') {
                // ThreatMax=XX
                int eqPos = 0;
                while (buffer[eqPos] != '=' && buffer[eqPos] != '\0') eqPos++;
                if (buffer[eqPos] == '=') {
                    threatMax = parseIntFromString(buffer + eqPos + 1);
                    totalDays = threatMax;
                }
            }
        }
    }

    file.close();

    // Transfer temporary arrays to engine arrays
    landedLords = new LandedLord*[tempLandedCount];
    for (int i = 0; i < tempLandedCount; i++) landedLords[i] = tempLanded[i];
    landedLordCount = tempLandedCount;

    landlessLords = new LandlessLord*[tempLandlessCount];
    for (int i = 0; i < tempLandlessCount; i++) landlessLords[i] = tempLandless[i];
    landlessLordCount = tempLandlessCount;

    warriorLords = new WarriorLord*[tempWarriorCount];
    for (int i = 0; i < tempWarriorCount; i++) warriorLords[i] = tempWarrior[i];
    warriorLordCount = tempWarriorCount;

    footSoldiers = new FootSoldier*[tempFootCount];
    for (int i = 0; i < tempFootCount; i++) footSoldiers[i] = tempFoot[i];
    footSoldierCount = tempFootCount;

    knights = new Knight*[tempKnightCount];
    for (int i = 0; i < tempKnightCount; i++) knights[i] = tempKnight[i];
    knightCount = tempKnightCount;

    cavalries = new Cavalry*[tempCavalryCount];
    for (int i = 0; i < tempCavalryCount; i++) cavalries[i] = tempCavalry[i];
    cavalryCount = tempCavalryCount;

    batteringRams = new BatteringRam*[tempRamCount];
    for (int i = 0; i < tempRamCount; i++) batteringRams[i] = tempRam[i];
    batteringRamCount = tempRamCount;

    catapults = new Catapult*[tempCatCount];
    for (int i = 0; i < tempCatCount; i++) catapults[i] = tempCat[i];
    catapultCount = tempCatCount;

    warShips = new WarShip*[tempShipCount];
    for (int i = 0; i < tempShipCount; i++) warShips[i] = tempShip[i];
    warShipCount = tempShipCount;

    sages = new Sage*[tempSageCount];
    for (int i = 0; i < tempSageCount; i++) sages[i] = tempSage[i];
    sageCount = tempSageCount;

    assassins = new Assassin*[tempAssassinCount];
    for (int i = 0; i < tempAssassinCount; i++) assassins[i] = tempAssassin[i];
    assassinCount = tempAssassinCount;

    cout << "[DATA] Loaded " << tempLandedCount << " LandedLords, " << tempLandlessCount << " LandlessLords, " << tempWarriorCount << " WarriorLords" << endl;
    cout << "[DATA] Loaded " << tempFootCount << " FootSoldiers, " << tempKnightCount << " Knights, " << tempCavalryCount << " Cavalries" << endl;
    cout << "[DATA] Loaded " << tempSageCount << " Sages, " << tempAssassinCount << " Assassins" << endl;
    cout << "[DATA] ThreatMax = " << threatMax << endl;
    cout << "[LOAD] Data loading complete!" << endl;
}

void AethelgardEngine::linkEntitiesToKingdoms() {
    // Link lords to their respective kingdoms
    for (int i = 0; i < landedLordCount; i++) {
        if (landedLords[i] != nullptr && landedLords[i]->realmIdx >= 0 && landedLords[i]->realmIdx < 10) {
            int realmIdx = landedLords[i]->realmIdx;
            if (kingdoms[realmIdx] != nullptr) {
                kingdoms[realmIdx]->currentLord = landedLords[i];
            }
        }
    }

    // Link military units to their kingdoms
    // First, count units per realm
    int realmFootCount[10] = {0};
    int realmKnightCount[10] = {0};
    int realmCavalryCount[10] = {0};
    int realmRamCount[10] = {0};
    int realmCatCount[10] = {0};
    int realmShipCount[10] = {0};

    for (int i = 0; i < footSoldierCount; i++) {
        if (footSoldiers[i] != nullptr && footSoldiers[i]->realmIdx >= 0 && footSoldiers[i]->realmIdx < 10) {
            realmFootCount[footSoldiers[i]->realmIdx]++;
        }
    }
    for (int i = 0; i < knightCount; i++) {
        if (knights[i] != nullptr && knights[i]->realmIdx >= 0 && knights[i]->realmIdx < 10) {
            realmKnightCount[knights[i]->realmIdx]++;
        }
    }
    for (int i = 0; i < cavalryCount; i++) {
        if (cavalries[i] != nullptr && cavalries[i]->realmIdx >= 0 && cavalries[i]->realmIdx < 10) {
            realmCavalryCount[cavalries[i]->realmIdx]++;
        }
    }
    for (int i = 0; i < batteringRamCount; i++) {
        if (batteringRams[i] != nullptr && batteringRams[i]->realmIdx >= 0 && batteringRams[i]->realmIdx < 10) {
            realmRamCount[batteringRams[i]->realmIdx]++;
        }
    }
    for (int i = 0; i < catapultCount; i++) {
        if (catapults[i] != nullptr && catapults[i]->realmIdx >= 0 && catapults[i]->realmIdx < 10) {
            realmCatCount[catapults[i]->realmIdx]++;
        }
    }
    for (int i = 0; i < warShipCount; i++) {
        if (warShips[i] != nullptr && warShips[i]->realmIdx >= 0 && warShips[i]->realmIdx < 10) {
            realmShipCount[warShips[i]->realmIdx]++;
        }
    }

    // Allocate arrays in kingdoms and copy pointers
    for (int r = 0; r < 10; r++) {
        if (kingdoms[r] == nullptr) continue;

        // Foot soldiers
        if (realmFootCount[r] > 0) {
            kingdoms[r]->footSoldiers = new FootSoldier*[realmFootCount[r]];
            kingdoms[r]->footSoldierCount = 0;
            for (int i = 0; i < footSoldierCount; i++) {
                if (footSoldiers[i] != nullptr && footSoldiers[i]->realmIdx == r) {
                    kingdoms[r]->footSoldiers[kingdoms[r]->footSoldierCount++] = footSoldiers[i];
                }
            }
        }

        // Knights
        if (realmKnightCount[r] > 0) {
            kingdoms[r]->knights = new Knight*[realmKnightCount[r]];
            kingdoms[r]->knightCount = 0;
            for (int i = 0; i < knightCount; i++) {
                if (knights[i] != nullptr && knights[i]->realmIdx == r) {
                    kingdoms[r]->knights[kingdoms[r]->knightCount++] = knights[i];
                }
            }
        }

        // Cavalry
        if (realmCavalryCount[r] > 0) {
            kingdoms[r]->cavalries = new Cavalry*[realmCavalryCount[r]];
            kingdoms[r]->cavalryCount = 0;
            for (int i = 0; i < cavalryCount; i++) {
                if (cavalries[i] != nullptr && cavalries[i]->realmIdx == r) {
                    kingdoms[r]->cavalries[kingdoms[r]->cavalryCount++] = cavalries[i];
                }
            }
        }

        // Battering Rams
        if (realmRamCount[r] > 0) {
            kingdoms[r]->batteringRams = new BatteringRam*[realmRamCount[r]];
            kingdoms[r]->batteringRamCount = 0;
            for (int i = 0; i < batteringRamCount; i++) {
                if (batteringRams[i] != nullptr && batteringRams[i]->realmIdx == r) {
                    kingdoms[r]->batteringRams[kingdoms[r]->batteringRamCount++] = batteringRams[i];
                }
            }
        }

        // Catapults
        if (realmCatCount[r] > 0) {
            kingdoms[r]->catapults = new Catapult*[realmCatCount[r]];
            kingdoms[r]->catapultCount = 0;
            for (int i = 0; i < catapultCount; i++) {
                if (catapults[i] != nullptr && catapults[i]->realmIdx == r) {
                    kingdoms[r]->catapults[kingdoms[r]->catapultCount++] = catapults[i];
                }
            }
        }

        // WarShips
        if (realmShipCount[r] > 0) {
            kingdoms[r]->warShips = new WarShip*[realmShipCount[r]];
            kingdoms[r]->warShipCount = 0;
            for (int i = 0; i < warShipCount; i++) {
                if (warShips[i] != nullptr && warShips[i]->realmIdx == r) {
                    kingdoms[r]->warShips[kingdoms[r]->warShipCount++] = warShips[i];
                }
            }
        }
    }

    cout << "[AETHELGARD] Entities linked to kingdoms" << endl;
}

// MAIN SIMULATION LOOP
void AethelgardEngine::runSimulation() {
    cout << "====================================================" << endl;
    cout << "     AETHELGARD SIMULATION BEGINS" << endl;
    cout << "====================================================" << endl;

    printHeader();

    // Run daily loop
    while (currentDay < totalDays) {
        currentDay++;
        cout << "\n===== DAY " << currentDay << " =====" << endl;

        runDay();

        // Check for endgame condition
        if (currentDay >= threatMax) {
            phase6_VoidRift();
            phase7_CheckEndgame();
            break;
        }
    }

    cout << "\n====================================================" << endl;
    cout << "     SIMULATION COMPLETE" << endl;
    cout << "====================================================" << endl;
}

void AethelgardEngine::runDay() {
    phase1_Economy();
    phase2_Aging();
    phase3_ShadowTriggers();
    phase4_DiplomaticDecay();
    phase5_OpenWarfare();

    printDailyDashboard();
}

// PHASE 1: ECONOMY
void AethelgardEngine::phase1_Economy() {
    cout << "[Phase 1] Environment & Economic" << endl;

    for ( int i = 0; i < 10; i++) {
        if (kingdoms[i] == nullptr) continue;

        Kingdom* kingdom = kingdoms[i];
        int taxIncome = 10000; 
        int maintenance = 0;

        // Calculate maintenance costs
        // This would sum up maintenanceCost from all units

        // Apply kingdom-specific bonuses
        // VerdantKingdom: foodSurplusBonus
        // CoastalKingdom: navalTradeBonus

        // Calculate net wealth
        kingdom->wealth = kingdom->wealth + taxIncome - maintenance;

        // Handle bankruptcy
        if (kingdom->wealth < 0) {
            kingdom->wealth = 0;
            // Lose 5% of foot soldiers
            int loss = (kingdom->footSoldierCount > 0) ? (kingdom->footSoldierCount / 20) : 0;
            if (loss > 0 && kingdom->footSoldiers != nullptr) {
                cout << "  [EVENT] " << kingdom->realmName << " suffers bankruptcy! " << loss << " foot soldiers lost." << endl;
                // Would actually remove soldiers here
            }
        }
    }
}

// PHASE 2: AGING & MORTALITY
void AethelgardEngine::phase2_Aging() {
    cout << "[Phase 2] Aging & Mortality" << endl;

    // Age all lords
    for (int i = 0; i < landedLordCount; i++) {
        if (landedLords[i] != nullptr && landedLords[i]->isAlive) {
            landedLords[i]->age++;

            // Geriatric decay for age >= 70
            if (landedLords[i]->age >= 70) {
                landedLords[i]->healthMeter += 30.0f;
            }

            // Death check
            if (landedLords[i]->healthMeter >= 100.0f) {
                landedLords[i]->isAlive = false;
                cout << "[DEATH] " << landedLords[i]->name << " has died of old age!" << endl;
                resolveSuccession(landedLords[i]->realmIdx);
            }
        }
    }

    // Age landless lords too
    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr && landlessLords[i]->isAlive) {
            landlessLords[i]->age++;
        }
    }

    // Age warrior lords
    for (int i = 0; i < warriorLordCount; i++) {
        if (warriorLords[i] != nullptr && warriorLords[i]->isAlive) {
            warriorLords[i]->age++;
        }
    }
}

// PHASE 3: SHADOW TRIGGERS
void AethelgardEngine::phase3_ShadowTriggers() {
    cout << "[Phase 3] Shadow Triggers" << endl;

    // 3a: Civil War Tension
    // Would calculate tension based on:
    // +30 if no currentLord
    // +5 if wealth < 0
    // +ambition/10 for high ambition subjects
    // -3 for high strategy rulers
    // -1 for large armies

    // 3b: Internal Coup (Ambition > 60)
    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr &&
            landlessLords[i]->ambitionStat > 60 &&
            landlessLords[i]->isAlive) {

            // Plot progression
            float progress = (landlessLords[i]->ambitionStat - 60) / 5.0f;
            landlessLords[i]->assassinationPlotProgress += progress;

            // Check if realm has negative diplomacy
            int worstRel = getLowestRelation(landlessLords[i]->realmIdx);
            if (worstRel < 0) {
                landlessLords[i]->assassinationPlotProgress += 2.0f;
            }

            // Check for ruler's strategy (reduces plot)
            if (kingdoms[landlessLords[i]->realmIdx] != nullptr &&
                kingdoms[landlessLords[i]->realmIdx]->currentLord != nullptr) {
                int rulerStrategy = kingdoms[landlessLords[i]->realmIdx]->currentLord->strategyStat;
                if (rulerStrategy > 50) {
                    landlessLords[i]->assassinationPlotProgress -= (rulerStrategy / 25.0f);
                }
            }

            // Floor at 0
            if (landlessLords[i]->assassinationPlotProgress < 0) {
                landlessLords[i]->assassinationPlotProgress = 0;
            }

            // Strike at 100
            if (landlessLords[i]->assassinationPlotProgress >= 100.0f) {
                handleInternalCoup(i);
            }
        }
    }

    // 3c: Foreign Assassination
    for (int i = 0; i < assassinCount; i++) {
        if (assassins[i] != nullptr && assassins[i]->isAlive) {
            int clientRealm = assassins[i]->clientIdx;
            if (hasActivePlotInRealm(clientRealm)) {
                int worstRel = getLowestRelation(clientRealm);
                if (worstRel <= -50) {
                    handleForeignAssassination(i);
                }
            }
        }
    }
}

// PHASE 4: DIPLOMATIC DECAY

void AethelgardEngine::phase4_DiplomaticDecay() {
    cout << "[Phase 4] Geopolitical Decay" << endl;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i != j && relations[i][j] > -100) {
                relations[i][j] -= 1;
            }
        }
    }
}

// PHASE 5: OPEN WARFARE

void AethelgardEngine::phase5_OpenWarfare() {
    cout << "[Phase 5] Open Warfare" << endl;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i != j && relations[i][j] <= -80) {
                // War declared!
                cout << "[WAR] " << kingdoms[i]->realmName << " declares war on " << kingdoms[j]->realmName << "!" << endl;
                processCombat(i, j);
            }
        }
    }
}

// PHASE 6: VOID RIFT
void AethelgardEngine::phase6_VoidRift() {
    cout << "[Phase 6] Void Rift Escalation" << endl;

    int voidRiftThreat = currentDay;
    cout << "[THREAT] Abyssal Swarm threat: " << voidRiftThreat 
         << " / " << threatMax << endl;
}

// PHASE 7: CHECK ENDGAME
void AethelgardEngine::phase7_CheckEndgame() {
    cout << "[Phase 7] Checking Endgame" << endl;

    // Calculate total military power
    int totalPower = 0;
    for (int i = 0; i < 10; i++) {
        if (kingdoms[i] == nullptr || i == 9) continue; // Skip Void Rift

        Kingdom* k = kingdoms[i];
        int realmPower = 0;
        realmPower += k->footSoldierCount * 1;
        realmPower += k->cavalryCount * 5;
        realmPower += k->knightCount * 8;
        realmPower += k->warShipCount * 12;
        realmPower += k->batteringRamCount * 25;
        realmPower += k->warriorLordCount * 10;
        totalPower += realmPower;
    }

    // Calculate swarm power - based on current day (threat level increases by 1 each day)
    int swarmPower = currentDay * 100;

    cout << "Total Military Power: " << totalPower << endl;
    cout << "Abyssal Swarm Power: " << swarmPower << endl;

    if (totalPower >= swarmPower) {
        cout << "[ABYSSAL ECLIPSE] >>> VICTORY <<<" << endl;
        cout << "Aethelgard survives but with 60% casualties!" << endl;
    } else {
        cout << "[ABYSSAL ECLIPSE] >>> DEFEAT <<<" << endl;
        cout << "The Abyssal Swarm has consumed all..." << endl;
    }
}

// HELPER METHODS

void AethelgardEngine::resolveSuccession(int realmIdx) {
    if (realmIdx < 0 || realmIdx >= 10 || kingdoms[realmIdx] == nullptr) return;

    // Find the landless/warrior lord with highest ambition
    int highestAmbition = -1;
    int successorIdx = -1;

    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr &&
            landlessLords[i]->realmIdx == realmIdx &&
            landlessLords[i]->isAlive &&
            landlessLords[i]->ambitionStat > highestAmbition) {
            highestAmbition = landlessLords[i]->ambitionStat;
            successorIdx = i;
        }
    }

    // Also check warrior lords
    for (int i = 0; i < warriorLordCount; i++) {
        if (warriorLords[i] != nullptr &&
            warriorLords[i]->realmIdx == realmIdx &&
            warriorLords[i]->isAlive &&
            warriorLords[i]->ambitionStat > highestAmbition) {
            highestAmbition = warriorLords[i]->ambitionStat;
            successorIdx = i + 1000; // Mark as warrior lord
        }
    }

    if (successorIdx >= 0) {
        // Would promote the successor to currentLord
        cout << "[SUCCESSION] New ruler promoted in realm " << realmIdx << endl;
    }
}

void AethelgardEngine::calculateMilitaryPower(int realmIdx) {
    // Power = (Foot*1) + (Cavalry*5) + (Knight*8) + (Ship*12) + (Ram*25) + (WarriorLord*10)
    int power = 0;

    if (kingdoms[realmIdx] != nullptr) {
        Kingdom* k = kingdoms[realmIdx];
        power += k->footSoldierCount * 1;
        power += k->cavalryCount * 5;
        power += k->knightCount * 8;
        power += k->warShipCount * 12;
        power += k->batteringRamCount * 25;
        power += k->warriorLordCount * 10;
    }
}

void AethelgardEngine::handleCivilWar(int realmIdx) {
    cout << "[EVENT] Civil War in realm " << realmIdx << "!" << endl;
    createRebelFaction(realmIdx);
}

void AethelgardEngine::handleInternalCoup(int landlessLordIdx) {
    if (landlessLordIdx < 0 || landlessLordIdx >= landlessLordCount) return;

    LandlessLord* plotter = landlessLords[landlessLordIdx];
    int realmIdx = plotter->realmIdx;

    if (kingdoms[realmIdx] == nullptr || kingdoms[realmIdx]->currentLord == nullptr) return;

    // Calculate Assassin Power = Plotter.AmbitionStat + (Realm.Wealth / 100)
    int assassinPower = plotter->ambitionStat + (kingdoms[realmIdx]->wealth / 100);

    // Calculate Target Defense = Ruler.StrategyStat + (Realm.DefenseStat / 10) + AgeModifier
    LandedLord* ruler = kingdoms[realmIdx]->currentLord;
    int ageModifier = (ruler->age < 40) ? 5 : -5;
    int targetDefense = ruler->strategyStat + (kingdoms[realmIdx]->defenseStat / 10) + ageModifier;

    if (assassinPower > targetDefense) {
        // Success - kill the ruler
        ruler->isAlive = false;
        cout << "[ASSASSINATION] " << ruler->name << " murdered by " << plotter->name << "!" << endl;
        plotter->assassinationPlotProgress = 0;
        resolveSuccession(realmIdx);
    } else {
        // Failure - kill the plotter
        plotter->isAlive = false;
        cout << "[ASSASSINATION] " << plotter->name << " failed and was executed!" << endl;
        plotter->assassinationPlotProgress = 0;

        // All realms lose 30 relations with this realm
        for (int i = 0; i < 10; i++) {
            if (i != realmIdx) {
                relations[i][realmIdx] -= 30;
            }
        }
    }
}

void AethelgardEngine::handleForeignAssassination(int assassinIdx) {
    if (assassinIdx < 0 || assassinIdx >= assassinCount) return;

    Assassin* ass = assassins[assassinIdx];
    int clientRealm = ass->clientIdx;

    // Find client's worst rival
    int worstRival = -1;
    int worstRel = 100;
    for (int i = 0; i < 10; i++) {
        if (i != clientRealm && relations[clientRealm][i] < worstRel) {
            worstRel = relations[clientRealm][i];
            worstRival = i;
        }
    }

    if (worstRival == -1 || kingdoms[worstRival] == nullptr) return;

    // Get target ruler
    LandedLord* target = kingdoms[worstRival]->currentLord;
    if (target == nullptr) return;

    // Check: Assassin.StealthStat > Target.StrategyStat
    if (ass->stealthStat > target->strategyStat) {
        // Success
        target->isAlive = false;
        cout << "[ASSASSINATION] " << target->name << " assassinated by " << ass->name << "!" << endl;
        resolveSuccession(worstRival);
    } else {
        // Failure
        relations[worstRival][clientRealm] -= 20;
        cout << "[ASSASSINATION] " << ass->name << " failed to kill " << target->name << "!" << endl;
    }

    // Delete the assassin (expended)
    delete ass;
    assassins[assassinIdx] = nullptr;
}

void AethelgardEngine::processCombat(int attackerIdx, int defenderIdx) {
    // Simplified combat - would implement hourly war loop
    cout << "Combat between " << kingdoms[attackerIdx]->realmName
         << " and " << kingdoms[defenderIdx]->realmName << endl;

    // Would implement:
    // - Calculate effective damage = base * (1 + strategy/100)
    // - Order: Cavalry -> Knights -> FootSoldiers
    // - Simultaneous resolution
}

void AethelgardEngine::createRebelFaction(int parentRealmIdx) {
    // Would clone parent realm with halved stats and redistribute half of military
}

// ============================================================
// OUTPUT METHODS
// ============================================================

void AethelgardEngine::printDailyDashboard() {
    printRealmStatus();
    printThreatLevel();
}

void AethelgardEngine::printHeader() {
    cout << "\n====================================================" << endl;
    cout << "         THE SCHOLAR'S DASHBOARD" << endl;
    cout << "====================================================" << endl;
}

void AethelgardEngine::printRealmStatus() {
    cout << "\n--- REALM LEDGER ---" << endl;
    for (int i = 0; i < 10; i++) {
        if (kingdoms[i] != nullptr) {
            cout << "Realm " << i << ": " << kingdoms[i]->realmName
                 << " | Wealth: " << kingdoms[i]->wealth
                 << " | Defense: " << kingdoms[i]->defenseStat;
            if (kingdoms[i]->currentLord != nullptr) {
                cout << " | Lord: " << kingdoms[i]->currentLord->name;
            }
            cout << endl;
        }
    }
}

void AethelgardEngine::printDiplomaticRelations() {
    cout << "\n--- DIPLOMATIC WEB ---" << endl;
    for (int i = 0; i < 10; i++) {
        cout << "Realm " << i << ": ";
        for (int j = 0; j < 10; j++) {
            cout << relations[i][j] << " ";
        }
        cout << endl;
    }
}

void AethelgardEngine::printCourtiers() {
    cout << "\n--- WHISPERING COUNCIL ---" << endl;
    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr &&
            landlessLords[i]->assassinationPlotProgress > 50.0f) {
            cout << landlessLords[i]->name << " (Plot: "
                 << landlessLords[i]->assassinationPlotProgress << "%)" << endl;
        }
    }
}

void AethelgardEngine::printThreatLevel() {
    cout << "\n--- CHRONOMETER ---" << endl;
    cout << "Day: " << currentDay << " / " << threatMax << endl;
    cout << "Invasion Progress: " << currentDay << " / " << threatMax << endl;
}

// ============================================================
// UTILITY METHODS
// ============================================================

int AethelgardEngine::getLowestRelation(int realmIdx) {
    int lowest = 100;
    for (int i = 0; i < 10; i++) {
        if (i != realmIdx && relations[realmIdx][i] < lowest) {
            lowest = relations[realmIdx][i];
        }
    }
    return lowest;
}

int AethelgardEngine::getHighestAmbitionLord(int realmIdx) {
    int highestAmbition = -1;
    int highestIdx = -1;

    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr &&
            landlessLords[i]->realmIdx == realmIdx &&
            landlessLords[i]->isAlive &&
            landlessLords[i]->ambitionStat > highestAmbition) {
            highestAmbition = landlessLords[i]->ambitionStat;
            highestIdx = i;
        }
    }

    return highestIdx;
}

bool AethelgardEngine::hasActivePlotInRealm(int realmIdx) {
    for (int i = 0; i < landlessLordCount; i++) {
        if (landlessLords[i] != nullptr &&
            landlessLords[i]->realmIdx == realmIdx &&
            landlessLords[i]->assassinationPlotProgress >= 100.0f) {
            return true;
        }
    }
    return false;
}