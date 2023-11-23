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
    vector<tuple<GraphPoint*, double>> ExpansionPoints;
    vector<double> TrueDistances;

    vector<double> time;
    vector<double> BruteForceTime;
    GraphPoint QueryPoint;
    ofstream MyFile(output_file);
    MyFile<<method<<" results"<<endl; //Write method
    if(strcmp(method,"GNNS") == 0)
    {
        for(int i = 0;i < Queries.size(); i++)
        {
            QueryPoint.PointID = i;
            QueryPoint.Vector = &Queries[i];
            time.push_back(GNNS(ExpansionPoints,&QueryPoint,&graph,Expansions,RandomRestarts,NearestNeighbors,5));
            BruteForceTime.push_back(BruteForce(&TrueDistances,Images,*QueryPoint.Vector,NearestNeighbors));
            WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,TrueDistances,&QueryPoint);
            ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances);
        }
    }
    MyFile.close();
    delete method;
    return 0;
}

double GNNS(vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint,Graph* graph,int Expansions,int RandomRestarts,int NearestNeighbors,int GreedySteps)
{
    auto start_time = chrono::high_resolution_clock().now();
    int Y = 0;
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
        
    /*Set IsExpanded to 0 for the next query point,since we are using pointers to the values of the GraphVector and not copied values*/
    for(int i = 0; i < ExpansionPoints.size(); i++)
        get<0>(ExpansionPoints[i])->IsExpanded = false;

    if(NearestNeighbors < ExpansionPoints.size())
        ExpansionPoints.erase(ExpansionPoints.begin() + NearestNeighbors,ExpansionPoints.end());

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}

void WriteToFile(ostream& MyFile,vector<double>& time,vector<double>& BruteForceTime,char* method,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances,GraphPoint* QueryPoint)
{
    MyFile<<"Query: "<<QueryPoint->PointID<<endl; //
    for(int i = 0;i < ExpansionPoints.size(); i++)
    {
        MyFile<<"Nearest Neighbor-"<<i<<": "<<get<0>(ExpansionPoints[i])->PointID<<endl
              <<"distanceApproximate: "<<get<1>(ExpansionPoints[i])<<endl
              <<"distanceTrue: "<<TrueDistances[i]<<endl;
    }
    double AlgorithmAverageTime = 0.0,BruteForceAverageTime = 0.0;
    if(time.size() != BruteForceTime.size())
        cout<<"Incorrect Distances sizes!"<<endl;
    for(int i = 0;i < time.size();i++)
    {
        AlgorithmAverageTime += time[i];
        BruteForceAverageTime += BruteForceTime[i];

    }
    double MAF = get<1>(ExpansionPoints[0]) / TrueDistances[0];
    AlgorithmAverageTime /= time.size();
    BruteForceAverageTime /= BruteForceTime.size();
    MyFile<<"tAverageApproximate: "<<AlgorithmAverageTime<<"ms"<<endl
          <<"tAverageTrue: "<<BruteForceAverageTime<<"ms"<<endl
          <<"MAF: "<<MAF<<endl;
}

void ClearVectors(vector<double>& time,vector<double>& BruteForceTime,vector<tuple<GraphPoint*, double>>& ExpansionPoints,vector<double>& TrueDistances)
{
    ExpansionPoints.clear();
    TrueDistances.clear();
    time.clear();
    BruteForceTime.clear();
}
