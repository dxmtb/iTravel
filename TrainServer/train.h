#ifndef TRAIN_H
#define TRAIN_H

#include "TrainServer.h"
#include "Station.h"
#include "Time.h"

class Train;

struct ArriveEvent
{
    Train *train;
    Station *station, *before;
    Time arri, dept, stay;
    int distance;
    double price[9], low_price;// 0-8:"硬座", "软座", "二等软座", "一等软座", "硬卧上/中/下", "软卧上/下"
};

struct Edge;

class Train
{
public:
    string name;
    vector<ArriveEvent*> event;
    Train(const string &str);
    vector<Edge> generate_Edge();
    static Station* getStation(string str);
    Train();
};

#endif // TRAIN_H
