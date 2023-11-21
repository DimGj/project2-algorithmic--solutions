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

using namespace std;
typedef struct GraphPoint{
    vector<byte>* Data;
    vector<GraphPoint*> Neighbors;
}GraphPoint;

class Graph{
    private:
        vector<GraphPoint*> GraphVector;
    public:
        Graph();
        vector<GraphPoint*> GetNeighbors(GraphPoint& CurrentPoint);
};