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
        if(!(*GraphVector)[i].IsAssigned)
        {
            cout<<"Failed to assign all Points in Graph!"<<endl;
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
