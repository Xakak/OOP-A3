#pragma once

void copyString(char* s1, char* s2);

class Entity {
public:
    int id;
    char name[50];

    Entity();
};


class Person : public Entity {
public:
    int age;
    bool isAlive;

    Person();
};

// Noble - inherits from Person
class Noble : public Person {
public:
    int realmIdx;
    int ambitionStat;
    int diplomacyStat;
    float healthMeter;
    int fatherId;
    int childrenIds[20];
    int childCount;

    Noble();
};

// LandedLord 
class LandedLord : public Noble {
public:
    int strategyStat;

    LandedLord();
};

// LandlessLord 
class LandlessLord : public Noble {
public:
    float assassinationPlotProgress;
    int strategyStat;

    LandlessLord();
};

class Sage : public Person {
public:
    int realmIdx;
    int healingPower;
    int wisdom;

    Sage();
};

class Assassin : public Person {
public:
    int stealthStat;
    int clientIdx;

    Assassin();
};

class Asset : public Entity {
public:
    int hp;
    int maxHp;
    int armor;
    int maintenanceCost;

    Asset();
};

class MilitaryUnit : public Asset {
public:
    int realmIdx;
    int attackPower;

    MilitaryUnit();
};

class MeleeUnit : public MilitaryUnit {
public:
    bool servesFrontline;

    MeleeUnit();
};

class FootSoldier : public MeleeUnit {
public:
    FootSoldier();
};

class Knight : public MeleeUnit {
public:
    int shieldBlock;

    Knight();
};

class MountedUnit : public MilitaryUnit {
public:
    float chargeMultiplier;
    int terrainPenalty;

    MountedUnit();
};

class Cavalry : public MountedUnit {
public:
    Cavalry();
};

class SiegeEngine : public Asset {
public:
    int realmIdx;
    int siegeDamage;
    int speed;

    SiegeEngine();
};

class BatteringRam : public SiegeEngine {
public:
    BatteringRam();
};

class Catapult : public SiegeEngine {
public:
    int aoeDamage;
    int aoeTargets;

    Catapult();
};

class Transport : public Asset {
public:
    int troopCapacity;
    int knotsSpeed;

    Transport();
};


// WarriorLord - multiple inheritance resolved
class WarriorLord : public Noble {
public:
    int shieldBlock;
    int attackPower;
    bool servesFrontline;
    int strategyStat;

    WarriorLord();
};

// WarShip -multiple inheritance 
class WarShip : public Transport {
public:
    int realmIdx;
    int siegeDamage;
    int speed;
    int aoeDamage;
    int aoeTargets;

    WarShip();

    void takeHullDamage(int damage);
    void takeWeaponDamage(int damage);
};


class Kingdom {
public:
    int wealth;
    int defenseStat;
    int realmIdx;
    char realmName[50];

    LandedLord* currentLord;
    int heirIds[20];
    int heirCount;
    LandlessLord** courtiers;
    int courtierCount;

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
    WarriorLord** warriorLords;
    int warriorLordCount;

    Sage** sages;
    int sageCount;

    Kingdom();
};

class FrostPeaksKingdom : public Kingdom {
public:
    int coldAttritionModifier;

    FrostPeaksKingdom();
};

class VerdantKingdom : public Kingdom {
public:
    int foodSurplusBonus;

    VerdantKingdom();
};

class CoastalKingdom : public Kingdom {
public:
    int navalTradeBonus;
    int shipCount;

    CoastalKingdom();
};

class TheVoidRift : public Kingdom {
public:
    int threatLevel;

    TheVoidRift();
};