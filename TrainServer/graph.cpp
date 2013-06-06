#include "graph.h"

Graph::Graph()
{
    edges.resize(Station::sta_list.size());
}

Edge Graph::genEdge(const ArriveEvent &e1, const ArriveEvent &e2)
{
    Edge now;
    now.e1 = &e1;
    now.e2 = &e2;
    assert(e2.before == e1.station);
    assert(e1.train == e2.train);
    now.fromId = e2.before->id;
    now.toId = e2.station->id;
    now.interval = Time::dis(e1.dept, e2.arri);
    now.price = e2.low_price - e1.low_price;
    
    return now;
}


