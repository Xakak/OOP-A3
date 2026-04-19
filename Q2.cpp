#include "Q2.h"

void copyString(char* s1, char* s2) {
    int i = 0;
    for (i = 0; s1[i] != '\0'; i++) {
        s2[i] = s1[i];
    }
    s2[i] = '\0';
}


Entity::Entity() {
    id = 0;
    name[0] = '\0';
}


Person::Person() : Entity() {
    age = 0;
    isAlive = true;
}

Noble::Noble() : Person() {
    realmIdx = 0;
    ambitionStat = 0;
    diplomacyStat = 0;
    healthMeter = 0.0f;
    fatherId = -1;
    childCount = 0;
    for (int i = 0; i < 20; i++) {
        childrenIds[i] = -1;
    }
}

LandedLord::LandedLord() : Noble() {
    strategyStat = 0;
}

LandlessLord::LandlessLord() : Noble() {
    assassinationPlotProgress = 0.0f;
    strategyStat = 0;
}

Sage::Sage() : Person() {
    healingPower = 0;
    wisdom = 0;
}

Assassin::Assassin() : Person() {
    stealthStat = 0;
    clientIdx = -1;
}


Asset::Asset() : Entity() {
    hp = 0;
    maxHp = 0;
    armor = 0;
    maintenanceCost = 0;
}

MilitaryUnit::MilitaryUnit() : Asset() {
    realmIdx = 0;
    attackPower = 0;
}

MeleeUnit::MeleeUnit() : MilitaryUnit() {
    servesFrontline = false;
}

FootSoldier::FootSoldier() : MeleeUnit() {
    servesFrontline = true;
}

Knight::Knight() : MeleeUnit() {
    servesFrontline = true;
    shieldBlock = 0;
}

MountedUnit::MountedUnit() : MilitaryUnit() {
    chargeMultiplier = 1.0f;
    terrainPenalty = 0;
}

Cavalry::Cavalry() : MountedUnit() {
}

SiegeEngine::SiegeEngine() : Asset() {
    realmIdx = 0;
    siegeDamage = 0;
    speed = 0;
}

BatteringRam::BatteringRam() : SiegeEngine() {
}

Catapult::Catapult() : SiegeEngine() {
    aoeDamage = 0;
    aoeTargets = 0;
}

Transport::Transport() : Asset() {
    troopCapacity = 0;
    knotsSpeed = 0;
}

// HYBRID CLASSES (Multiple Inheritance)

WarriorLord::WarriorLord() : Noble() {
    shieldBlock = 0;
    attackPower = 15;
    servesFrontline = true;
    strategyStat = 0;
}

WarShip::WarShip() : Transport() {
    realmIdx = 0;
    siegeDamage = 0;
    speed = 0;
    aoeDamage = 0;
    aoeTargets = 0;
}

void WarShip::takeHullDamage(int damage) {
    hp -= damage;
}

void WarShip::takeWeaponDamage(int damage) {
    siegeDamage = (siegeDamage > damage) ? siegeDamage - damage : 0;
}

// KINGDOM BRANCH

Kingdom::Kingdom() {
    wealth = 0;
    defenseStat = 0;
    realmIdx = 0;
    realmName[0] = '\0';
    currentLord = nullptr;
    heirCount = 0;
    for (int i = 0; i < 20; i++) {
        heirIds[i] = -1;
    }
    courtiers = nullptr;
    courtierCount = 0;
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
    warriorLords = nullptr;
    warriorLordCount = 0;
    sages = nullptr;
    sageCount = 0;
}

FrostPeaksKingdom::FrostPeaksKingdom() : Kingdom() {
    coldAttritionModifier = 5;
}

VerdantKingdom::VerdantKingdom() : Kingdom() {
    foodSurplusBonus = 0;
}

CoastalKingdom::CoastalKingdom() : Kingdom() {
    navalTradeBonus = 0;
    shipCount = 0;
}

TheVoidRift::TheVoidRift() : Kingdom() {
    threatLevel = 0;
}