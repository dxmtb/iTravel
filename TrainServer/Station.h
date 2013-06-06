//
//  Station.h
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#ifndef __TrainServer__Station__
#define __TrainServer__Station__

#include "TrainServer.h"

class Station
{
public:
    int id;
    string name;
    static Station *getStation(const string &str);
    static void output();
    static map<int, Station*> id_to_ptr;
    static map<string, Station*> str_to_ptr;
    static vector<Station*> sta_list;
    static int total_station;
};

#endif /* defined(__TrainServer__Station__) */
