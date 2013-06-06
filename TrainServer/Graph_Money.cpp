//
//  Graph_Money.cpp
//  TrainServer
//
//  Created by 田 博 on 13-5-6.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#include "Graph_Money.h"

Graph_Money::Graph_Money(const vector<const Train*> &trainList)
{
    for(vector<const Train*>::const_iterator i = trainList.begin(); i != trainList.end(); i++)
    {
        for (int j = 0; j + 1 < (int)(*i)->event.size(); j++) {
            addEdge(*((*i)->event[j]), *((*i)->event[j+1]));
        }
    }
}

void Graph_Money::addEdge(const ArriveEvent &e1, const ArriveEvent &e2)
{
    Edge now = genEdge(e1, e2);
    now.w = now.price;
    edges[now.fromId].push_back(now);
}

vector<const Edge*> Graph_Money::getShortest(int S, int T, int change_limit, int late, int earliest, int latest, double rate) const
{
    int N = (int)Station::sta_list.size();
    double best = oo;
    
    vector<const Edge*> path_best;
    
    for(vector<Edge>::const_iterator it_begin = edges[S].begin(); it_begin != edges[S].end(); it_begin++)
        if (it_begin->e1->dept.between(earliest, latest))
        {
            vector<pair<double,Time> > distance(N);
            vector<bool> sure(N);
            vector<const Edge*> path(N);
            assert(distance.size() == N);
            assert(sure.size() == N);
            assert(path.size() == N);
            
            distance[S] = make_pair(0, it_begin->e1->dept);
            for (int j = 0; j < N; j++) {
                if (j != S)
                {
                    distance[j] = make_pair(oo, Time());
                    sure[j] = false;
                }
                path[j] = NULL;
            }
            
            while(!sure[T])
            {
                int near = -1;
                for(int i = 0; i < N;i++)
                {
                    /*cerr << sure[i] << endl;
                     cerr << distance[i].first << endl;
                     if (near != -1)
                     cerr << distance[near].first << endl;*/
                    if (!sure[i] && (near == -1 || distance[i].first < distance[near].first))
                        near = i;
                }
                if (near == -1 || distance[near].first == oo)
                    break;
                
                sure[near] = true;
                for(vector<Edge>::const_iterator it = edges[near].begin(); it != edges[near].end(); it++)
                    if ((near != S || it == it_begin) && !sure[it->toId])
                    {
                        int inter = Time::dis(distance[near].second, it->e1->dept);
                        if (near != S && path[near]->e1->train != it->e1->train && inter < late)
                            continue;
                        double nd = distance[near].first + it->w;
                        
                        /*Time t1 = distance[near].second, t2 = it->e2->arri;
                         int tmp = distance[near].first;
                         Edge tmp2 = *it;*/
                        if (nd < 0)
                        {
                            cerr << nd << endl;
                        }
                        assert(nd >= 0);
                        if (nd < distance[it->toId].first)
                        {
                            distance[it->toId] = make_pair(nd, it->e2->arri);
                            path[it->toId] = &(*it);
                        }
                    }
            }
            if (sure[T] && distance[T].first < best)
            {
                best = distance[T].first;
                path_best = path;
            }
        }
    
    vector<const Edge*> ret;
    
    if (best == oo)
        return ret;
    
    for (int now = T; now != S; now = path_best[now]->e1->station->id) {
        //void *p = path_best[now];
        assert(path_best[now] != NULL);
        ret.push_back(path_best[now]);
    }
    reverse(ret.begin(), ret.end());
    return ret;
}
