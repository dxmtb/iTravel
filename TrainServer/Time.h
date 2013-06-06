//
//  Time.h
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#ifndef __TrainServer__Time__
#define __TrainServer__Time__

#include "TrainServer.h"

class Time
{
public:
    int day, hour, minute;
    void getDay(const string &str);
    void getTime(const string &str);
    void getInterval(const string &str);
    int toInt() const;
    bool between(int t1, int t2) const;
    static int dis(const Time &t1, const Time &t2);
};


#endif /* defined(__TrainServer__Time__) */
