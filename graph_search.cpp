#include "graph_search.hpp"

int main(int argc,char** argv)
{
    char* input_file;char* query_file;char* output_file;char* method = new char(4);
    int GraphNearestNeighbors,Extensions,RandomRestarts,NearestNeighbors,TankCandidates;
    vector<vector<byte>> Images;
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&GraphNearestNeighbors,&Extensions,&RandomRestarts,&NearestNeighbors,&TankCandidates,&method);
    OpenFile(input_file,&Images,false);

    Graph graph(GraphNearestNeighbors,NearestNeighbors,Images);
    //Example run of get neighbors
    vector<GraphPoint*> neighbors = graph.GetNeighbors(0);
    delete method;
    return 0;
}
