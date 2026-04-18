#include "q1.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

Organism::Organism(int _x, int _y, int _width, int _height) {
    x = _x; 
    y = _y; 
    width = _width; 
    height = _height;
    lifeStatus = 1; 
    
    internalShape = new bool*[height]; 
    for (int r = 0; r < height; r++) { 
        internalShape[r] = new bool[width];
        for (int c = 0; c < width; c++) {
            internalShape[r][c] = true;
        }
    }
}

Organism::~Organism() {
    for (int i = 0; i < height; i++) {
        delete[] internalShape[i];
    }
    delete[] internalShape;
}

void Organism::resizeBody(int newWidth, int newHeight) {
    bool** tmp = new bool*[newHeight];
    for (int i = 0; i < newHeight; i++) {
        tmp[i] = new bool[newWidth];
    }
    
    for (int i = 0; i < height; i++) {
        delete[] internalShape[i];
    }
    delete[] internalShape;

    internalShape = tmp;
    width = newWidth;
    height = newHeight;
    // fill it
}

float Organism::getAverageNutrients(Tile** world) {
    float nutrientSum = 0; 
    int tileCount = 0;    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int localY = y + i;
            int localX = x + j;
            if (world[localY][localX].nutrientLevel >= 0) {
                nutrientSum += world[localY][localX].nutrientLevel;
                tileCount++;
            }
        }
    }
    if (tileCount > 0) {
        return nutrientSum / tileCount;
    } else {
        return 0;
    }
}

void Organism::reduceSize(int shrinkAmount) {
    int newWidth = width - shrinkAmount;
    int newHeight = height - shrinkAmount;

    if (newWidth <= 0 || newHeight <= 0) {
        lifeStatus = 0;
        return;
    }

    bool** newMap = new bool*[newHeight];
    for (int i = 0; i < newHeight; i++) {
        newMap[i] = new bool[newWidth];
        for (int j = 0; j < newWidth; j++) {
            newMap[i][j] = internalShape[i][j];
        }
    }

    for (int i = 0; i < height; i++) {
        delete[] internalShape[i];
    }
    delete[] internalShape;

    internalShape = newMap;
    width = newWidth;
    height = newHeight;
}

World::World(int w, int h, int m) {
    srand(time(0));
    gridWidth = w; 
    gridHeight = h; 
    maxPopulation = m;
    currentPopulation = 0;
    
    grid = new Tile*[gridHeight];
    for (int i = 0; i < gridHeight; i++) {
        grid[i] = new Tile[gridWidth];
        for (int j = 0; j < gridWidth; j++) {
            grid[i][j].nutrientLevel = (float)(rand() % 1001) / 10.0f;
            grid[i][j].toxicity = (float)(rand() % 101) / 10.0f;
            grid[i][j].occupant = nullptr;
        }
    }

    population = new Organism*[maxPopulation];
    for (int i = 0; i < maxPopulation; i++) {
        population[i] = nullptr;
    }
}

World::~World() {
    for (int i = 0; i < gridHeight; i++) {
        delete[] grid[i];
    }
    delete[] grid;
    
    for (int i = 0; i < maxPopulation; i++) {
        if (population[i] != nullptr) {
            delete population[i];
        }
    }
    delete[] population;
}

void World::syncGrid() {
    // tried doing this in a single pass but it kept overwriting my other organisms pointers so I am just clearing the entire board first and then retamp them. It is slower but works.
    for (int row = 0; row < gridHeight; row++) { 
        for (int col = 0; col < gridWidth; col++) {
            grid[row][col].occupant = nullptr;
        }
    }

    for (int i = 0; i < maxPopulation; i++) {
        Organism* o = population[i];
        if (o != nullptr && o->getStatus() != 0) {
            for (int dy = 0; dy < o->getHeight(); dy++) {
                for (int dx = 0; dx < o->getWidth(); dx++) {
                    if (o->getShape()[dy][dx]) {
                        int gy = o->getY() + dy;
                        int gx = o->getX() + dx;
                        if (gx >= 0 && gx < gridWidth && gy >= 0 && gy < gridHeight) {
                            grid[gy][gx].occupant = o;
                        }
                    }
                }
            }
        }
    }
}

void World::runIteration() {
    for (int i = 0; i < maxPopulation; i++) {
        if (population[i] != nullptr && population[i]->getStatus() != 0) {
            population[i]->update(grid, gridWidth, gridHeight);
        }
    }

    for (int i = 0; i < maxPopulation; i++) {
        Organism* o = population[i];
        if (o == nullptr) {
            continue;
        }

        if (o->getStatus() == 0) {
            removeOrganism(i);
        } else if (o->getStatus() == 2) { 
            // 2 means it wants to split, set back to 1 after
            addOrganism(new FractalSprawler((o->getX() + o->getWidth() + 2) % gridWidth, o->getY(), 2, 2));
            o->setStatus(1);
        }
    }

    syncGrid();
}

void World::addOrganism(Organism* o) {
    for (int i = 0; i < maxPopulation; i++) {
        if (population[i] == nullptr) {
            population[i] = o;
            currentPopulation++;
            return;
        }
    }
    delete o;
}

void World::removeOrganism(int idx) {
    if (population[idx] != nullptr) {
        delete population[idx];
        population[idx] = nullptr;
        currentPopulation--;
    }
}

float World::getAverageToxicity() const {
    float total = 0;
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            total += grid[i][j].toxicity;
        }
    }
    return total / (gridWidth * gridHeight);
}

void World::draw() {
    for (int i = 0; i < gridHeight; i++) {
        for (int j = 0; j < gridWidth; j++) {
            Organism* o = grid[i][j].occupant;
            if (o != nullptr) {
                if (o->getType() == 1) {
                    cout << "\033[32mG\033[0m";
                } else {
                    cout << "\033[31mR\033[0m";
                }
            } else {
                cout << ".";
            }
        }
        cout << endl;
    }
    cout << "Population: " << currentPopulation << " | Avg Tox: " << getAverageToxicity() << endl;
}

FractalSprawler::FractalSprawler(int _x, int _y, int _width, int _height) : Organism(_x, _y, _width, _height) {
    type = 1;
}

void FractalSprawler::update(Tile** world, int worldWidth, int worldHeight) {
    int cX = x + width / 2;
    int cY = y + height / 2;
    int range = 2 * width;
    
    float totalNut = 0;
    int count = 0;
    
    for (int dy = -range; dy <= range; dy++) {
        for (int dx = -range; dx <= range; dx++) {
            int targetX = cX + dx;
            int targetY = cY + dy;
            if (targetX >= 0 && targetX < worldWidth && targetY >= 0 && targetY < worldHeight) {
                totalNut += world[targetY][targetX].nutrientLevel;
                count++;
            }
        }
    }

    float score;
    if (count > 0) {
        score = totalNut / count;
    } else {
        score = 0;
    }

    if (score > 70) {
        if (width + 2 <= 10 && height + 2 <= 10 && x + width + 2 <= worldWidth && y + height + 2 <= worldHeight) {
            resizeBody(width + 2, height + 2);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    internalShape[i][j] = true;
                }
            }
        }

        if (width >= 5) {
            // flag for world to handle spawning
            lifeStatus = 2;
            resizeBody(2, 2);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    internalShape[i][j] = true;
                }
            }
        }
    } else if (score < 30) {
        if (width > 2) {
            resizeBody(width - 2, height - 2);
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    internalShape[i][j] = true;
                }
            }
        } else {
            lifeStatus = 0;
        }
    }
}

KineticHunter::KineticHunter(int _x, int _y, int _width, int _height) : Organism(_x, _y, _width, _height) {
    type = 2;
}

void KineticHunter::update(Tile** world, int worldWidth, int worldHeight) {
    int cX = x + width / 2;
    int cY = y + height / 2;
    int range = 4 * width;
    int tx = -1, ty = -1;

    for (int dy = -range; dy <= range; dy++) {
        for (int dx = -range; dx <= range; dx++) {
            int sy = cY + dy, sx = cX + dx;
            if (sx >= 0 && sx < worldWidth && sy >= 0 && sy < worldHeight) {
                Organism* potential = world[sy][sx].occupant;
                if (potential != nullptr && potential->getType() == 1) {
                    tx = potential->getX(); 
                    ty = potential->getY();
                    break;
                }
            }
        }
        if (tx != -1) break; 
    }

    if (tx != -1) {
        if (x < tx && x < worldWidth - width) {
            x++;
        } else if (x > tx && x > 0) {
            x--;
        }
        
        if (y < ty && y < worldHeight - height) {
            y++;
        } else if (y > ty && y > 0) {
            y--;
        }
    } else {
        int move = rand() % 4;
        if (move == 0 && y > 0) {
            y--;
        } else if (move == 1 && y < worldHeight - height) {
            y++;
        } else if (move == 2 && x > 0) {
            x--;
        } else if (move == 3 && x < worldWidth - width) {
            x++;
        }
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (internalShape[i][j]) {
                int py = y + i;
                int px = x + j;
                if (px >= 0 && px < worldWidth && py >= 0 && py < worldHeight) {
                    world[py][px].nutrientLevel -= 10.0f;
                    if (world[py][px].nutrientLevel < 0) {
                        world[py][px].nutrientLevel = 0;
                    }
                    
                    if (world[py][px].occupant != nullptr && world[py][px].occupant != this) {
                        world[py][px].occupant->reduceSize(1);
                    }
                }
            }
        }
    }
}
