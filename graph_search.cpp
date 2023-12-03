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
    Graph* graph;
    if(strcmp(method,"GNNS") == 0)
        graph = new Graph(GraphNearestNeighbors,NearestNeighbors,Images);
    else //initialize graph for MRNG
        graph = new Graph(Images);
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
                time.push_back(GNNS(ExpansionPoints,&QueryPoint,graph,Expansions,RandomRestarts,NearestNeighbors,10));
                BruteForceTime.push_back(BruteForce(&TrueDistances,Images,*QueryPoint.Vector,NearestNeighbors));
                WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,TrueDistances,&QueryPoint);
                ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances);
            }
            if(GetNewFiles(&query_file,&output_file) == -1)
                return 0;
        }
        else if(strcmp(method,"MRNG") == 0)
        {
            if(NearestNeighbors > TankCandidates)
            for(int i = 0;i < Queries.size(); i++)
            {
                QueryPoint.PointID = i;
                QueryPoint.Vector = &Queries[i];
                time.push_back(SearchOnGraph(&QueryPoint,graph,NearestNeighbors,TankCandidates));
                BruteForceTime.push_back(BruteForce(&TrueDistances,Images,*QueryPoint.Vector,NearestNeighbors));
                WriteToFile(MyFile,time,BruteForceTime,method,ExpansionPoints,TrueDistances,&QueryPoint);
                ClearVectors(time,BruteForceTime,ExpansionPoints,TrueDistances);
            }
            if(GetNewFiles(&query_file,&output_file) == -1)
                return 0;
        }
        else if(strcmp(method,"MRNG") == 0)
        {
            for(int i = 0;i < Queries.size(); i++)
            {
                QueryPoint.PointID = i;
                QueryPoint.Vector = &Queries[i];
                time.push_back(SearchOnGraph(&QueryPoint,graph,NearestNeighbors,TankCandidates));
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

void MRNG(Graph* graph)
{   
    int GraphSize = graph->GetGraphSize();
    vector<GraphPoint>& graphVector = graph->GetGraphVector();
    bool flag = true;

    for (int i = 0; i < GraphSize; i++) 
    {
        GraphPoint& p = graphVector[i];
        vector<GraphPoint*> SortedByCurrentPointDist = graph->GetSortedPointsByDistance(p);

        if (!SortedByCurrentPointDist.empty()) {
            p.Neighbors.push_back(SortedByCurrentPointDist.front());    //Lp
            // Remove the first element from SortedByCurrentPointDist
            SortedByCurrentPointDist.erase(SortedByCurrentPointDist.begin());  //Rp
        }

        vector<GraphPoint*> NeighborsCandidates = SortedByCurrentPointDist;     //Rp-Lp
        for (const GraphPoint* neighbor : p.Neighbors)
        {
            auto it = find(NeighborsCandidates.begin(), NeighborsCandidates.end(), neighbor);
            if (it != NeighborsCandidates.end())
                NeighborsCandidates.erase(it);
        }

        for (int k = 0; k < NeighborsCandidates.size(); k++) 
        {   
            flag = true;
            const GraphPoint& r = *(NeighborsCandidates[k]);
            for (int j = 0; j < p.Neighbors.size(); j++) 
            {
                const GraphPoint& t = *(p.Neighbors[j]);
                double pr = PNorm(r.Vector, p.Vector,2);
                double pt = PNorm(t.Vector, p.Vector,2);
                double rt = PNorm(r.Vector, t.Vector,2);

                if (pr > pt && pr > rt) {
                    flag = false;
                    break;
                }
            }
            if(flag)
                p.Neighbors.push_back(NeighborsCandidates[k]);  //add to Lp (as Neighbor)
        }
    }
}

double SearchOnGraph(GraphPoint* QueryPoint,Graph* graph,int NearestNeighbors,int TankCandidates)
{
    auto start_time = chrono::high_resolution_clock().now();
    MRNG(graph);
    int StartPoint = rand() % graph->GetGraphSize(); //get a uniformly distributed point in the graph
    vector<GraphPoint>& graphVector = graph->GetGraphVector();
    vector<GraphPoint*> PoolOfCandidates; // Candidate set
    set<int> checkedNodes; // To keep track of checked nodes
    PoolOfCandidates.push_back(&graphVector[StartPoint]); // Initialize PoolOfCandidates with the starting point
    int i=0; // i is the index for the first unchecked node in PoolOfCandidates

    while (i < TankCandidates)
    {
        // Find the first unchecked node in PoolOfCandidates
        for (int j = 0; j < PoolOfCandidates.size(); ++j)
        {
            if (checkedNodes.find(PoolOfCandidates[j]->PointID) == checkedNodes.end())
            {
                i = j;
                break;
            }
        }
        if (i == PoolOfCandidates.size())
            break; // No unchecked nodes remaining

        const GraphPoint* p = PoolOfCandidates[i];
        checkedNodes.insert(p->PointID);    // Mark p as checked

         // Explore neighbors of p
        for (GraphPoint* neighbor : graph->GetNeighbors(p->PointID))
        {
            if (checkedNodes.find(neighbor->PointID) == checkedNodes.end())
                PoolOfCandidates.push_back(neighbor);
        }

        // Sort PoolOfCandidates in ascending order of distance to the query point
        sort(PoolOfCandidates.begin(), PoolOfCandidates.end(), [&QueryPoint](const GraphPoint* a, const GraphPoint* b) {
            return PNorm(a->Vector, QueryPoint->Vector, 2) < PNorm(b->Vector, QueryPoint->Vector, 2);
        });

        // Resize PoolOfCandidates to NearestNeighbors, if needed
        if (PoolOfCandidates.size() > TankCandidates)
            PoolOfCandidates.resize(TankCandidates);
    }
    //PoolOfCandidates contains the NearestNeighbors candidates sorted by distance to the query point

    //return vector<GraphPoint*>(PoolOfCandidates.begin(), PoolOfCandidates.begin() + NearestNeighbors);
    auto end_time = chrono::high_resolution_clock().now();
    chrono::duration<double> duration = end_time - start_time;

    return duration.count() * 1000;
}
