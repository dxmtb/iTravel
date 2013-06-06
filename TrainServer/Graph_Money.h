//
//  Graph_Money.h
//  TrainServer
//
//  Created by 田 博 on 13-5-6.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#ifndef __TrainServer__Graph_Money__
#define __TrainServer__Graph_Money__

#include "graph.h"

class Graph_Money : public Graph
{
public:
    Graph_Money(const vector<const Train*> &trainList);
    vector<const Edge*> getShortest(int S, int T, int change_limit = oo, int late = 0, int earliest = 0, int latest = 60*24 - 1, double rate = 0) const;
private:
    void addEdge(const ArriveEvent &e1, const ArriveEvent &e2);
};

#endif /* defined(__TrainServer__Graph_Money__) */
