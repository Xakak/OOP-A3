#ifndef Q1_H
#define Q1_H

class Organism;

struct Tile {
    float nutrientLevel;
    float toxicity;
    Organism* occupant;
};

class World {
private:
    Tile** grid;
    Organism** population;
    int gridWidth;
    int gridHeight;
    int maxPopulation;
    int currentPopulation;

public:
    World(int w, int h, int m);
    ~World();

    void runIteration();
    void addOrganism(Organism* o);
    void removeOrganism(int idx);
    void syncGrid(); // Renamed from refreshGridMap
    void draw();

    float getAverageToxicity() const;
    int getWidth() const { return gridWidth; }
    int getHeight() const { return gridHeight; }
    int getPopCount() const { return currentPopulation; }
    Tile** getRawGrid() { return grid; }
};

class Organism {
protected:
    int x, y;
    int width, height;
    bool** internalShape;
    int type;
    int lifeStatus;

    void resizeBody(int newWidth, int newHeight); // Student helper

public:
    Organism(int _x, int _y, int _width, int _height);
    virtual ~Organism();

    virtual void update(Tile** world, int worldWidth, int worldHeight) = 0;

    float getAverageNutrients(Tile** world);
    
    bool** getShape() const { return internalShape; }
    
    int getType() const { return type; }
    int getStatus() const { return lifeStatus; }
    void setStatus(int s) { lifeStatus = s; }

    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void setWidth(int newWidth) { width = newWidth; } // Clean parameters
    void setHeight(int newHeight) { height = newHeight; }
    void reduceSize(int shrinkAmount);
};

class FractalSprawler : public Organism {
public:
    FractalSprawler(int _x, int _y, int _width, int _height);
    void update(Tile** world, int worldWidth, int worldHeight);
};

class KineticHunter : public Organism {
public:
    KineticHunter(int _x, int _y, int _width, int _height);
    void update(Tile** world, int worldWidth, int worldHeight);
};

#endif
