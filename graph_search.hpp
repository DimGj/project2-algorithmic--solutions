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
#include "./Graph/graph.hpp"

using namespace std;

void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances);
void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,GraphPoint* QueryPoint);
double GNNS(vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps);