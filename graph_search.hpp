#include <iostream>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <cstdint>
#include <random>
#include <arpa/inet.h>
#include <cmath>
#include <bits/stdc++.h>
#include <chrono>
#include "graph.hpp"

using namespace std;

void GNNS(vector<byte>* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps);