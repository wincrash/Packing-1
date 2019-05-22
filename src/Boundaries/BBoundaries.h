#ifndef BBOUNDARIES_H
#define BBOUNDARIES_H


#include "core/Constants.h"
#include "search/ASearch.h"


class BBoundaries
{
public:
   BBoundaries();
    void init(ASearch *search, json duomenys);
    virtual bool check(Point newSphere)=0;

protected:
    virtual void initialization(json duomenys)=0;
    ASearch *search;
    json duomenys;
    REAL radius;
    REAL height;
    Point center;
    Point min;
    Point max;
};

#endif
