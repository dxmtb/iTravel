//
//  Graph_Efficiency.cpp
//  TrainServer
//
//  Created by 田 博 on 13-5-6.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#include "Graph_Efficiency.h"
#include <queue>
using std::queue;

Graph_Efficiency::Graph_Efficiency(const vector<const Train*> &trainList)
{
    for(vector<const Train*>::const_iterator i = trainList.begin(); i != trainList.end(); i++)
    {
        for (int j = 0; j + 1 < (int)(*i)->event.size(); j++) {
            addEdge(*((*i)->event[j]), *((*i)->event[j+1]));
            //assert((*i)->event[j]->low_price <= (*i)->event[j+1]->low_price);
        }
    }
    
    for (int i = 0; i < (int)edges.size(); i++) {
        random_shuffle(edges[i].begin(), edges[i].end());
    }
}

void Graph_Efficiency::addEdge(const ArriveEvent &e1, const ArriveEvent &e2)
{
    Edge now = genEdge(e1, e2);
    now.w = now.price;
    edges[now.fromId].push_back(now);
}

static bool insert_one(map<int,double> &dis, double C, Time T, int D, int K, map<int, pair<const Edge*, int> > &path, const Edge *e, int event_t)
{
    T.day = D;
    int t = T.toInt();
    double w = C - t * K;
    map<int,double>::iterator it = dis.upper_bound(t);
    if (it != dis.begin())
    {
        map<int,double>::iterator it2 = it;
        it2--;
        if (it2 != dis.end() && it2->first <= t && it2->second <= w)
            return false;
    }
    while (it != dis.end() && it->second >= w) {
        it = dis.erase(it);
    }
    dis[t] = w;
    path[t] = make_pair(e, event_t);
    return true;
}

static bool insert_dis(map<int,double> &dis, double C, Time T, int K, map<int, pair<const Edge*, int> > &path, const Edge *e, int event_t)
{
    return insert_one(dis, C, T, 0, K, path, e, event_t) ||
    insert_one(dis, C, T, 1, K, path, e, event_t);
}

vector<const Edge*> Graph_Efficiency::getShortest(int S, int T, int change_limit, int late, int earliest, int latest, double rate) const
{
    assert(rate >= 0);
    int N = (int)Station::sta_list.size();
    double best = oo;
    
    vector<const Edge*> path_best;
    
    
    time_t t_begin = time(0);
    
    for(vector<Edge>::const_iterator it_begin = edges[S].begin(); it_begin != edges[S].end(); it_begin++)
        if (it_begin->e1->dept.between(earliest, latest))
        {
            vector<map<int,double> > distance(N);
            vector<map<int, pair<const Edge*, int> > > path(N);
            assert(distance.size() == N);
            assert(path.size() == N);
            
            for (int j = 0; j < N; j++) {
                assert(distance[j].size() == 0);
                assert(path[j].size() == 0);
            }
            
            insert_dis(distance[S], 0, it_begin->e1->dept, rate, path[S], NULL, 0);
            queue<int> q;
            vector<bool> in(N, false);
            q.push(S);
            
            double nbest = oo;
            
            while (q.size()) {
                //cerr << q.size() << endl;
                int u = q.front();q.pop();
                //cerr << "Edges: " << edges[u].size() << endl;
                if (distance[u].size() == 0)
                    continue;
                
                for (map<int,double>::iterator it = distance[u].begin(); it != distance[u].end();) {
                    double nd = it->second + it->first * rate;
                    if (nd > nbest || nd > best)
                        it = distance[u].erase(it);
                    else
                        it++;
                }
                
                for(vector<Edge>::const_iterator it = edges[u].begin(); it != edges[u].end(); it++)
                    if (u != S || it == it_begin)
                    {
                        for (int day = 0; day <= 1; day++) {
                            Time dept = it->e1->dept;
                            dept.day = day;
                            map<int,double>::iterator event = distance[u].upper_bound(dept.toInt()-1); //FIXME Not consider late
                            
                            if (event == distance[u].begin())
                                continue;
                            
                            event--;
                            
                            //cerr << event->first << ' ' << event->second << endl;
                            //cerr << (event == distance[u].begin()) << endl;
                            
                            int inter = dept.toInt() - event->first;
                            
                            double nd = event->second + event->first * rate + (inter + it->interval) * rate / 60 + it->w;
                            assert(nd >= 0);
                            
                            if (nd < nbest && nd < best)
                            {
                                if (it->toId == T)
                                    nbest = nd;
                                if (insert_dis(distance[it->toId], nd, it->e2->arri, rate, path[it->toId], &(*it), event->first))
                                {
                                    if (!in[it->toId])
                                    {
                                        q.push(it->toId);
                                        in[it->toId] = true;
                                    }
                                }
                                assert(it->e1->station->id == u);
                            }
                        }
                    }
            }
            
            if (distance[T].size())
            {
                cerr << "Get better: " << nbest << endl;
                int best_T;
                nbest = oo;
                for (map<int,double>::iterator it = distance[T].begin(); it != distance[T].end(); it++) {
                    double tmp = it->second + it->first * rate;
                    if (tmp < nbest)
                    {
                        nbest = tmp;
                        best_T = it->first;
                    }
                }
                if (nbest < best)
                {
                    best = nbest;
                    path_best.clear();
                    for (int now = T; now != S;) {
                        const Edge *e = path[now][best_T].first;
                        path_best.push_back(e);
                        
                        best_T = path[now][best_T].second;
                        now = e->e1->station->id;
                    }
                }
            }
            
            cerr << "t_begin: " << t_begin << " time: " << time(0) << endl;
            if (time(0) - t_begin >= 10000)
                break;
        }
    
    if (best == oo)
        return path_best;
    
    reverse(path_best.begin(), path_best.end());
    return path_best;
}
