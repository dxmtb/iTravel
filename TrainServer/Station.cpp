//
//  Station.cpp
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#include "Station.h"

//private:
map<int, Station*> Station::id_to_ptr;
map<string, Station*> Station::str_to_ptr;
vector<Station*> Station::sta_list;
int Station::total_station;

Station* Station::getStation(const string &str)
{
    map<string, Station*>::iterator it = Station::str_to_ptr.find(str);
    Station *sta = NULL;
    if (it == str_to_ptr.end())
    {
        sta = new Station();
        sta->id = total_station;
        sta->name = str;
        total_station++;
        str_to_ptr[str] = sta;
        id_to_ptr[sta->id] = sta;
        sta_list.push_back(sta);
    }
    else
        sta = it->second;
    return sta;
}

void Station::output()
{
    for (vector<Station*>::iterator it = sta_list.begin(); it != sta_list.end(); it++) {
        cerr << (*it)->id << ' ' << (*it)->name << endl;
    }
}