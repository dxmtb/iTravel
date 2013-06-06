//
//  main.cpp
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#include "TrainServer.h"
#include "train.h"
#include "Graph_Time.h"
#include "Graph_Money.h"
#include "Graph_Efficiency.h"
#include "include/json/json.h"

const string TMP_DIR("/tmp/trainserver/");

void log(const string &str)
{
    time_t lt;
    lt =time(0);
    cerr << ctime(&lt) << ' ' << str << endl;
}

void init_train(vector<const Train*> &train_list, const char *input)
{
    stringstream buffer;
    ifstream file(input);
    buffer << file.rdbuf();
    int count = 0;
    while (true) {
        string line("");
        std::getline(buffer, line);
        if (line.length() < 10)
        {
            cerr << "get to line whose length is " << line.length() << endl;
            break;
        }
        Train *now = new Train(line);
        train_list.push_back(now);
        cerr << "Finish " << count++ << endl;
        //if (count == 1000)break;
    }
}

void output(const Json::Value &pt)
{
    Json::FastWriter writer;
    std::string str = writer.write(pt);
    cerr << str << endl;
    cout << str << endl;
}

void error_output(const string &e)
{
    Json::Value pt;
    pt["status"] = "error";
    pt["error"] = e.c_str();
    output(pt);
}

void output_event_list(const vector<const Edge*> &result)
{
    if (result.size() == 0)
    {
        error_output("没有符合要求的方案 T_T");
        return ;
    }
    
    vector<pair<Train*,pair<string,string> > > ret;
    assert(result.size() >= 1);
    Train* train_name = NULL;
    string from, to;
    double money_cost = 0;
    int time_cost = 0;
    Time before_time;
    before_time.day = -1;
    for (int i = 0; i < (int)result.size(); i++) {
        assert(result[i]->e1->train == result[i]->e2->train);
        if (result[i]->e1->train != train_name)
        {
            if (train_name != NULL)
            {
                ret.push_back(make_pair(train_name, make_pair(from, to)));
            }
            train_name = result[i]->e1->train;
            from = result[i]->e1->station->name;
        }
        to = result[i]->e2->station->name;
        money_cost += result[i]->price;
        
        if (before_time.day != -1)
        {
            Time now = result[i]->e1->dept;
            now.day = 0;
            time_cost += Time::dis(before_time, now);
        }
        time_cost += result[i]->interval;
        before_time = result[i]->e2->arri;
        before_time.day = 0;
    }
    assert(train_name != NULL);
    ret.push_back(make_pair(train_name, make_pair(from, to)));
    
    assert(time_cost > 0);
    assert(money_cost > 0);
    
    Json::Value pt;
    pt["status"] = "succeed";
    pt["money_cost"] = money_cost;
    pt["time_cost"] = time_cost;
    Json::Value plan_t;
    for (int i = 0; i < (int)ret.size(); i++) {
        Json::Value array;
        array["name"] = ret[i].first->name.c_str();
        array["from"] = ret[i].second.first.c_str();
        array["to"] = ret[i].second.second.c_str();
        plan_t.append(array);
    }

    pt["plan"] = plan_t;
    Json::Value trains_t;
    for (int i = 0; i < (int)ret.size(); i++) {
        Json::Value array;
        train_name = ret[i].first;
        for (int j = 0; j < train_name->event.size(); j++) {
            Json::Value event;
            ArriveEvent *e = train_name->event[j];
            event["station"] = e->station->name.c_str();
            event["arri"] = e->arri.toInt();
            event["dept"] = e->dept.toInt();
            array.append(event);
        }
        trains_t[train_name->name.c_str()] = array;
    }
    pt["trains"] = trains_t;
    output(pt);
}

bool exist(const string &fname)
{
    fstream _file;
    _file.open(fname,ios::in);
    if(!_file)
        return false;
    return true;
}

void init_interaction()
{
    ofstream f(TMP_DIR + "count.txt");
    f << '0';
    f.close();
}

string int_to_str(int n)
{
    stringstream ss;
    ss << n;
    return ss.str();
}

int main(int argc, const char * argv[])
{
    const char *input = argv[1];
    //const char *input = "/Users/tb/train2.txt";

    
    vector<const Train*> train_list;
    
    init_train(train_list, input);
    
    Graph_Time gt(train_list);
    Graph_Money gm(train_list);
    Graph_Efficiency ge(train_list);
    
    cerr << "Train: " << train_list.size() << endl;
    cerr << "Station: " << Station::sta_list.size() << endl;
    
    Station::output();
    
    int count = 1;
    
    init_interaction();
    
    while (true) {
        string count_s_f = int_to_str(count++), count_s_s;
        
        count_s_s = count_s_f;

        string fname = TMP_DIR + count_s_f + ".query";
        
        while (!exist(fname)) {
            log("Waiting for query " + count_s_f);
            sleep(1);
        }
        
        while (exist(fname + ".lock")) {
            log("Waiting for query lock" + count_s_f);
            sleep(1);
        }
        
        log("Get query " + count_s_f);
#undef USE_SMP
        
#ifdef USE_SMP
        int fpid = fork();
        
        if (fpid == 0){
            //son for calculation
            cerr << "Child " << fpid << " enter" << endl;
#endif
            
            string from, to;
            int change_limit, late, need, rate, earliest, latest;
            vector<const Edge*> result;
            
            ifstream fin(fname);
            fin >> from >> to >> change_limit >> late >> need >> earliest >> latest;
            if (need == 2)
                fin >> rate;
            fin.close();
            
            if (from.length() == 0)
                break;
            
            fname = TMP_DIR + count_s_s + ".result";
            freopen(fname.c_str(), "w", stdout);
            
            bool flag = true;
            if (Station::str_to_ptr.count(from) == 0)
            {
                error_output("未找到起始车站");
                flag = false;
            }
            
            if (Station::str_to_ptr.count(to) == 0)
            {
                error_output("未找到终点车站");
                flag = false;
            }
            
            if (earliest < 0 || earliest >= 24*60)
                earliest = 0;
            
            if (latest < 0 || latest >= 24*60)
                latest = 24*60 - 1;
            
#ifdef USE_SMP
            cerr << "Child " << fpid << " enter2" << endl;
#endif

            if (flag)
            {
                switch (need) {
                    case 0:
                        //min time
                        result = gt.getShortest(Station::str_to_ptr[from]->id, Station::str_to_ptr[to]->id, change_limit, late, earliest, latest);
                        break;
                    case 1:
                        //min cost
                        result = gm.getShortest(Station::str_to_ptr[from]->id, Station::str_to_ptr[to]->id, change_limit, late, earliest, latest);
                        break;
                    case 2:
                        //min cost + time * K
                        result = ge.getShortest(Station::str_to_ptr[from]->id, Station::str_to_ptr[to]->id, change_limit, late, earliest, latest, rate);
                        break;
                    default:
                        break;
                }
                
                output_event_list(result);
            }
            
            ofstream fin2(TMP_DIR + count_s_s + ".finish");
            fin2 << count_s_s << endl;
            fin2.close();
#ifdef USE_SMP    
            cerr << "Child " << fpid << " quit" << endl;
            return 0;
        }
        else
            cerr << "father to continue: " << count << endl;
#endif
    }
    
    return 0;
}

