#include "graph.hpp"

Graph::Graph(int GraphNearestNeighbors,int NearestNeighbors,vector<vector<byte>>& Images)
{
    int HashTables = 4,HashFunctions = 5;
    LSH Lsh(HashTables,HashFunctions,10000,NearestNeighbors,Images.size(),GraphNearestNeighbors);
    GraphPoint NewPoint;
    GraphPoint* Point;
    GraphVector = new vector<GraphPoint>;
    GraphVector->resize(Images.size());
    for(int i = 0;i < Images.size(); i++)
    {
        /*Assign Values to a GraphPoint variable*/
        NewPoint.PointID = i; //This is the primary key of the image
        NewPoint.Vector = &Images[i]; //Vector of the image

        (*GraphVector)[i] = NewPoint;

        Lsh.Hash((*GraphVector)[i],false); //Hash the Pointer
    }
    for(int i = 0;i < GraphVector->size(); i++)
        Lsh.Hash((*GraphVector)[i],true);
    
    /*Dev,should delete after(checks if all points exist in graph)*/
    for(int i = 0;i < GraphVector->size(); i++)
    {
        if(this->GetNeighborsCount(i) <= 0)
        {
            cout<<"Failed to initialize Graph Properly!"<<endl;
            exit(-1);
        }
    }
}

Graph::~Graph()
{
    delete GraphVector;
}

vector<GraphPoint*>Graph::GetNeighbors(int PointID)
{
    if(PointID > GraphVector->size() || PointID < 0)
    {
        cout<<"Invalid PointID!"<<endl;
        exit(-1);
    }
    return (*GraphVector)[PointID].Neighbors;
}

int Graph::GetClosestNeighbors(int PointID,int Expansions,vector<tuple<GraphPoint*, double>>& ExpansionPoints,GraphPoint* QueryPoint)
{
    vector<GraphPoint*> Neighbors = this->GetNeighbors(PointID);
    if(Neighbors.size() == 0) //If Datapoint has no neighbors,don't iterate the remaining greedy points(0 neighbors over and over)
        return -1;

   // vector<tuple<GraphPoint*, double>> CurrentExpansions;
    int count = 0,IterationsCount = 0;

    if(Neighbors.size() > Expansions) //Find the min of both
        IterationsCount = Expansions;
    else
        IterationsCount = Neighbors.size();

    double MinDistance = numeric_limits<double>::max();
    int MinDistancePointID = -1;
    double L2Norm = 0.0;
    for(int i = 0;i < IterationsCount; i++)
    {
        if(!Neighbors[i]->IsExpanded) //If Datapoint hasn't been explored
        {
            //Add it to the vector
            L2Norm = PNorm(Neighbors[i]->Vector, QueryPoint->Vector,2);
            ExpansionPoints.push_back(tuple(Neighbors[i],L2Norm));
            //CurrentExpansions.push_back(tuple(Neighbors[i],L2Norm));
            //Find the min L2 Distance from the neighbors
            if(L2Norm < MinDistance)
            {
                //store it as the new min distance
                MinDistance = L2Norm;
                //store the PointID of the graph
                MinDistancePointID = Neighbors[i]->PointID;
            }
            Neighbors[i]->IsExpanded = true; //mark it as explored
        }
        else
            count++; //else check the number of explored neighbors this point has
    }
    if(count == IterationsCount) //If the number of explored neighbors is the number of expansions or the number of neighbors(no new datapoints will appear on the rest of the greedy iterations)
        return -1;  //break from the loop
    //sort(CurrentExpansions.begin(),CurrentExpansions.end(),NeighborsComparisonFunction); //else sort the current expansion array which has the neighbors for the current Point
    return MinDistancePointID;   //return the Min Distance Point ID
}

int Graph::GetGraphSize(){ return GraphVector->size();}

int Graph::GetNeighborsCount(int PointID)
{   
    if(PointID > GraphVector->size() || PointID < 0)
    {
        cout<<"Invalid PointID!"<<endl;
        exit(-1);
    }
    return (*GraphVector)[PointID].Neighbors.size();
}

bool NeighborsComparisonFunction(tuple<GraphPoint*,double>& A,tuple<GraphPoint*,double>& B) { return get<1>(A) < get<1>(B) ;}
