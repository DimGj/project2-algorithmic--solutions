/*Given a File,a query point and the N-nearest points found by brute force algo,writes items to file*/
int LSH::WriteToFile(ofstream& MyFile,Point point,vector<double>& Distances,double BruteForce_duration,double LSH_duration)
{
    int max = 0;
    /*If this is not checked,seg fault will happen*/
    if(Candidates.size() == 0)
    {
        cout<<"No candidates were found!"<<endl;
        return -1;
    }
    /*Need to implement to find the N nearest neighbors in the R range*/
    sort(Candidates.begin(),Candidates.end(),ComparisonFunctionByDistance);

    /*Implement Brute force search here to find the true distances between points*/


    MyFile<<"Query: "<<point.PointID<<endl;
    /*If the number of neighbors we want is more than the potential candidates*/
    /*We must check only up to the size of the candidates*/
    if(NearestNeighbors > Candidates.size())
        max = Candidates.size();
    else/*else if the candidates are more than the neighbors we want*/
        max = NearestNeighbors;/*we must check up until the number of neighbors*/
         
    for(int i = 0;i < max; i++)
    {
        /*Check if they are within range*/
        if(Candidates[i]->Distance <= Range)
        {
            MyFile<<"Nearest Neighbor "<<i<<": "<<Candidates[i].PointID<<endl
                  <<"distanceLSH: "<<Candidates[i].Distance<<endl
                  <<"distanceTrue: "<<Distances[i]<<endl
                  <<"tLSH: "<<LSH_duration<<"ms"<<endl
                  <<"tTrue: "<<BruteForce_duration<<"ms"<<endl;
        }
    }
    MyFile<<Range<<"-near Neighbors: "<<endl;
    for(int i = 0;i < Candidates.size(); i++)
        MyFile<<Candidates[i].PointID<<endl;

    return 0;
}

/*Basic brute force algorithm that compares all points to the query point and returns the N closest ones*/
double BruteForce(vector<double>* Distances,vector<vector<byte>> Points,vector<byte> QueryPoint,int NearestNeighbors)
{   
    /*Count ms it takes to complete*/
    auto start_time = chrono::high_resolution_clock().now();
    /*For all points in the dataset*/
    for(int i = 0;i < Points.size(); i++)
        Distances->push_back(PNorm(Points[i],QueryPoint,2));
    /*Sort the points*/
    sort(Distances->begin(),Distances->end());

    Distances->erase(Distances->begin() + NearestNeighbors,Distances->end());
        
    auto end_time = chrono::high_resolution_clock().now();

    chrono::duration<double> duration = end_time - start_time;
    /*Return the duration of the brute force search*/
    return duration.count() * 1000;
}