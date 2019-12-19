#ifndef POINT_H
#define POINT_H

#include "entity.h"

class Point : public Entity
{
private:
    int id;

public:
    Point();
    Point(float x, float y, float z, int id);

    void step();

    int getID();

    const bool isPoint();
};
#endif
