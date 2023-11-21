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

struct Header {
    int32_t magic_number;
    int32_t num_images;
    int32_t num_rows;
    int32_t num_columns;
};

void OpenFile(char* filename,vector<vector<byte>>* images,bool test);
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* GraphNearestNeighbors,int* Extensions,int* RandomRestarts,int* NearestNeighbors,int* TankCandidates,char** method);
double PNorm(vector<byte>& A,vector<byte>& B,int p);
double PNorm(vector<double>& A,vector<byte>& B,int p);

