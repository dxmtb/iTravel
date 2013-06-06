#include "train.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

static int getDis(const string &str)
{
    if (str == "0")
        return 0;
    const string gongli("公里");
    assert(str.length() > gongli.length());
    string str1 = str.substr(0, str.length() - gongli.length());
    string str2 = str.substr(str.length() - gongli.length());
    //cerr << str2 << endl;
    //assert(false);
    assert(str2 == gongli);
    return atoi(str1.c_str());
}

static double getPrice(const string &str)
{
    int state = 0;
    for (int i = 0; i < (int)str.length(); i++) {
        if (isdigit(str[i]))
        {
            
        }
        else if (str[i] == '.')
        {
            if (state == 0)
                state = 1;
            else
                assert(false);
        }
        else
            assert(false);
    }
    double ret = atof(str.c_str());
    //cerr << str << ' ' << ret << endl;
    return ret;
}

Train::Train(const string &str)
{
    using namespace boost::property_tree;
    
    std::stringstream ss(str);
    ptree pt;
    read_json(ss, pt);
    
    this->name = pt.get_child("ticketInfo").begin()->first;
    
    ptree station_array = pt.get_child("trainScheduleBody");
    
    Station *before = NULL;

    for (ptree::iterator it = station_array.begin(); it != station_array.end(); it++) {
        ptree info = it->second.get_child("content");
        ArriveEvent *e = new ArriveEvent();
        e->train = this;
        ptree::iterator it2 = info.begin();
        
        e->station = Station::getStation((++it2)->second.data());
        e->before = before;
        
        e->arri.getDay((++it2)->second.data());
        
        e->arri.getTime((++it2)->second.data());
        
        e->dept.getTime((++it2)->second.data());
        
        e->stay.getInterval((++it2)->second.data());
        
        e->distance = getDis((++it2)->second.data());
        
        e->price[0] = getPrice((++it2)->second.data());
        
        e->price[1] = getPrice((++it2)->second.data());
        
        e->price[2] = getPrice((++it2)->second.data());
        
        e->price[3] = getPrice((++it2)->second.data());
        
        for (int i = 0; i <= 4; i++) {
            //assert(i != 4);
            if (e->price[i] != 0)
            {
                e->low_price = e->price[i];
                break;
            }
        }
        
        event.push_back(e);
        
        before = e->station;
    }
    
    for(int i = 0; i+1 < (int)event.size(); i++)
    {
        if (event[i]->low_price > event[i+1]->low_price)
            event[i+1]->low_price = event[i]->low_price + 1;
    }
}
