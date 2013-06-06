#ifndef GRAPH_H
#define GRAPH_H

#include "train.h"

struct Edge
{
    const ArriveEvent *e1, *e2;
    int fromId, toId;
    int interval;
    double w, price;
    //double price[9];
};

class Graph
{
public:
    Graph();
    virtual vector<const Edge*> getShortest(int S, int T, int change_limit = oo, int late = 0, int earliest = 0, int latest = 60*24 - 1, double rate = 0)const = 0;
    static Edge genEdge(const ArriveEvent &e1, const ArriveEvent &e2);
    virtual void addEdge(const ArriveEvent &e1, const ArriveEvent &e2) = 0;
protected:
    vector<vector<Edge> > edges;
};

#endif // GRAPH_H
