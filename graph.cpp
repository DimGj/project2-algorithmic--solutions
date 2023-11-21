#include "graph.hpp"

Graph::Graph()
{

}

vector<GraphPoint*>Graph::GetNeighbors(GraphPoint& Point)
{
    return Point.Neighbors;
}
