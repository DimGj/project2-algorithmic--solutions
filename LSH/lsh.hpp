#include "misc.hpp"

class LSH{
    private:
        vector<vector<vector<GraphPoint*>>> hashTables;
        vector<HashFunction> hashFunctions;
        vector<vector<int>> HashFunctionVariables;
        vector<GraphPoint*> Candidates;
        int Range;
        int NearestNeighbors;
        int TableSize;
        int GraphNearestNeighbors;

    public:
         /*Constructor*/
        LSH(int HashTables,int HashFunctions,int Range,int NearestNeighbors,int ImageCount,int graphNearestNeighbors);
        /*Standard hash used by LSH when called from Lsh_main*/
        double Hash(GraphPoint& Point,bool AssignToParent);
        int FindHashValue(vector<int> HashCodes,vector<int> RandomValues);
        double GenerateT(unsigned int w);
        int GenerateR();
        void GenerateRandomVectors(int HashFunctions,int dimension,vector<vector<double>>* RandomVectors);
        int WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,double BruteForce_duration,double LSH_duration);
};

/*Used when std::sort is called*/
bool ComparisonFunctionByDistance(const GraphPoint* A,const GraphPoint* B);
bool ComparisonFunctionByCluster(const Point& A,const Point& B);