//
//  Time.cpp
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#include "Time.h"

void Time::getDay(const string &str)
{
    static const map<string, int> str_to_day = {
        {"-", 0},
        {"第一天", 0},
        {"第二天", 1},
        {"第三天", 2},
        {"第四天", 3}
    };
    map<string,int>::const_iterator it = str_to_day.find(str);
    
    if (it == str_to_day.end())
        cerr << str << endl;
    else
    {
        day = it->second;
        //cerr << str << ' ' << it->second << endl;
    }
}

void Time::getTime(const string &str)
{
    if (str == "起点站")
    {
        hour = minute = -10000000;
    }
    else if (str == "终到站")
    {
        hour = minute = -10000000;
    }
    else
    {
        assert(isdigit(str[0]));
        assert(isdigit(str[1]));
        assert(str[2] == ':');
        assert(isdigit(str[3]));
        assert(isdigit(str[4]));
        assert(str.length() == 5);

        hour = (str[0]-'0') * 10 + (str[1]-'0');
        minute = (str[3]-'0') * 10 + (str[4]-'0');
    }
    //cerr << str << endl;
}

void Time::getInterval(const string &str)
{
    if (str == "-")
        day = hour = minute = 0;
    else
    {
        string nstr = str.substr(0, str.length() - 3);
        /*static set<string> set_str;
        if (!set_str.count(nstr))
        {
            set_str.insert(nstr);
            cerr << nstr << endl;
        }*/
        day = hour = 0;
        minute = atoi(nstr.c_str());
    }
}

int Time::toInt() const
{
    return day * 60 * 24 + hour * 60 + minute;
}

int Time::dis(const Time &t1, const Time &t2)
{
    int ret = (t2.hour-t1.hour)*60 + (t2.minute-t1.minute);
    if (ret < 0)
        ret += 24 * 60;
    assert(ret >= 0);
    return ret;
}

bool Time::between(int t1, int t2)const
{
    int t = toInt() % (60*24);
    if (t1 <= t2)
        return (t1 <= t && t <= t2);
    return (t >= t1 || t <= t2);
}