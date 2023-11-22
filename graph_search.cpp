#include "graph_search.hpp"

int main(int argc,char** argv)
{
    srand((unsigned int)(time(nullptr)));
    char* input_file;char* query_file;char* output_file;char* method = new char(4);
    int GraphNearestNeighbors,Expansions,RandomRestarts,NearestNeighbors,TankCandidates;
    vector<vector<byte>> Images,Queries;
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&GraphNearestNeighbors,&Expansions,&RandomRestarts,&NearestNeighbors,&TankCandidates,&method);
    OpenFile(input_file,&Images,false);
    OpenFile(query_file,&Queries,true);
    Graph graph(GraphNearestNeighbors,NearestNeighbors,Images);
    //Example run of getNeighbors
    vector<GraphPoint*> neighbors = graph.GetNeighbors(0);
    if(strcmp(method,"GNNS") == 0)
    {
        for(int i = 0;i < Queries.size(); i++)
            GNNS(&Queries[i],&graph,Expansions,RandomRestarts,NearestNeighbors,5);
    }
    delete method;
    return 0;
}

void GNNS(vector<byte>* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps)
{
    int Y = 0;
    vector<tuple<GraphPoint*, double>> ExpansionPoints;
    for(int i = 0;i < RandomRestarts; i++) //For a number of random restarts
    {
        Y = rand() % graph->GetGraphSize(); //get a uniformly distributed point in the graph
        for(int t = 0;t < GreedySteps; t++) //for a number of greedy steps
        {
            Y = graph->GetClosestNeighbors(Y,Expansions,ExpansionPoints,QueryPoint); //find the closest neighbor of Y to Query Point
            if(Y == -1) //if no neighbor was found ,restart with a new point
                break;
        }

    }
    /*Dev,should never be true*/
    if(ExpansionPoints.size() <= 0)
    {
        cout<<"No points were found!"<<endl;
        exit(0);
    }
    sort(ExpansionPoints.begin(),ExpansionPoints.end(),NeighborsComparisonFunction); //Sort all the explored nodes
    /*Dev,print them(Should write to file instead)*/
    for(int i = 0;i < NearestNeighbors; i++)
    {
        cout<<"Image ID: "<<get<0>(ExpansionPoints[i])->PointID<<endl
            <<"Distance: "<<get<1>(ExpansionPoints[i])<<endl;
    }
    /*Set IsExpanded to 0 for the next query point,since we are using pointers to the values of the GraphVector and not copied values*/
    for(int i = 0; i < ExpansionPoints.size(); i++)
        get<0>(ExpansionPoints[i])->IsExpanded = false;
}
