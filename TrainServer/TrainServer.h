//
//  TrainServer.h
//  TrainServer
//
//  Created by 田 博 on 13-5-2.
//  Copyright (c) 2013年 Tsinghua. All rights reserved.
//

#ifndef TrainServer_TrainServer_h
#define TrainServer_TrainServer_h


#include <string>
#include <vector>
#include <map>
#include <set>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <climits>

using std::string;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::cout;
using std::endl;
using std::pair;
using namespace std;

const int oo = INT_MAX;

void log(const string &str);

#define USE_SMP

#endif
