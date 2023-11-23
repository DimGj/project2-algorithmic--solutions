#include "graph_search.hpp"

int main(int argc,char** argv)
{
    srand((unsigned int)(time(nullptr)));

    //Variable Declarations
    char* input_file;char* query_file;char* output_file;char* method = new char(4);
    int GraphNearestNeighbors,Expansions,RandomRestarts,NearestNeighbors,TankCandidates;
    vector<vector<byte>> Images,Queries;
    vector<tuple<GraphPoint*, double>> ExpansionPoints;
    vector<double> TrueDistances,time,BruteForceTime;
    GraphPoint QueryPoint;

    //Get arguments from command line
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&GraphNearestNeighbors,&Expansions,&RandomRestarts,&NearestNeighbors,&TankCandidates,&method);
    //Open files and check if they exist
    if(CheckFileExistance(input_file,false,Images) == -1 || CheckFileExistance(query_file,true,Queries) == -1)
        return 0;
    //initialize graph for GNNS
    Graph graph(GraphNearestNeighbors,NearestNeighbors,Images);
    ofstream MyFile(output_file);

    while(1)
    {
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
            if(GetNewFiles(&query_file,&output_file) == -1)
                return 0;
        }
        Queries.clear();
        if(CheckFileExistance(query_file,true,Queries) == -1)
            return 0;
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

    if(NearestNeighbors < ExpansionPoints.size()) //if the neighbors we found are more than the nearest neighbors we want
        ExpansionPoints.erase(ExpansionPoints.begin() + NearestNeighbors,ExpansionPoints.end()); //Delete the rest of them(vector is sorted by Distance to query point,so we delete after NearestNeighbors)

    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}
