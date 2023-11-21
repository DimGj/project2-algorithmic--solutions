#include "graph_search.hpp"

int main(int argc,char** argv)
{
    char* input_file;char* query_file;char* output_file;char* method = new char(4);
    int GraphNearestNeighbors,Extensions,RandomRestarts,NearestNeighbors,TankCandidates;
    vector<vector<byte>> Images;
    GetArgs(argc,argv,&input_file,&query_file,&output_file,&GraphNearestNeighbors,&Extensions,&RandomRestarts,&NearestNeighbors,&TankCandidates,&method);
    OpenFile(input_file,&Images,false);

    delete method;
    return 0;
}




/*Gets all the arguments from the command line (doesnt matter the order they are used)*/
void GetArgs(int argc,char** argv,char** input_file,char** query_file,char** output_file,int* GraphNearestNeighbors,int* Extensions,int* RandomRestarts,int* NearestNeighbors,int* TankCandidates,char** method)
{   
    /*Booleans to mark which value will be passed*/
    bool input_bool = false,query_bool = false,GraphNeighbors_bool = false,
         Extensions_bool = false,output_bool = false,NearestNeighbors_bool = false,RandomRestarts_bool = false,TankCandidates_bool = false,
         method_bool = false;
    
    if(argc < 2) //will need to change to 7 later
    {
        cout<<"Please enter arguments!"<<endl;
        exit(-1);
    }

    *GraphNearestNeighbors = 50,*Extensions = 30,*RandomRestarts = 1,*NearestNeighbors = 1; //Default values are passed
                                 //so even if user doesnt input them,these will be use
    *TankCandidates = -1;

    for(int i=0;i < argc; i++)
    {
        if(input_bool) //If the corresponding bool is true
        {
            *input_file = argv[i]; //Get the appropriate value from the argv array
            input_bool = false; //mark the boolean false so it is excluded next time
        }
        else if(query_bool)
        {
            *query_file = argv[i];
            query_bool = false;
        }
        else if(GraphNeighbors_bool)
        {
            *GraphNearestNeighbors = atoi(argv[i]);
            GraphNeighbors_bool = false;
        }
        else if(Extensions_bool)
        {
            *Extensions = atoi(argv[i]);
            Extensions_bool = false;
        }
        else if(output_bool)
        {
            *output_file = argv[i];
            output_bool = false;
        }
        else if(NearestNeighbors_bool)
        {
            *NearestNeighbors = atoi(argv[i]);
            NearestNeighbors_bool = false;
        }
        else if(RandomRestarts_bool)
        {
            *RandomRestarts = atoi(argv[i]);
            RandomRestarts_bool = false;   
        }
        else if(TankCandidates_bool)
        {
            *TankCandidates= atoi(argv[i]);
            TankCandidates_bool = false;
        }
        else if(method_bool)
        {
            if(atoi(argv[i]) == 1)
                strcpy(*method,"GNNS");
            else if(atoi(argv[i]) == 2)
                strcpy(*method,"MRNG");
            else
            {
                cout<<"Invalid method!"<<endl;
                exit(-1);
            }
            method_bool = false;
        }
        else if(strcmp(argv[i],"-d") == 0) //this checks if the argv arg is the parameter before the actual value we want
            input_bool = true; //if so,mark the appropriate boolean as true so on the next iteration we access the correct value
        else if(strcmp(argv[i],"-q") == 0)
            query_bool = true;
        else if(strcmp(argv[i],"-k") == 0)
            GraphNeighbors_bool = true;
        else if(strcmp(argv[i],"-E") == 0)
            Extensions_bool = true; 
        else if(strcmp(argv[i],"-o") == 0)
            output_bool = true;  
        else if(strcmp(argv[i],"-R") == 0)
            RandomRestarts_bool = true;  
        else if(strcmp(argv[i],"-N") == 0)
            NearestNeighbors_bool = true;
        else if(strcmp(argv[i],"-l") == 0)
            TankCandidates_bool = true;
        else if(strcmp(argv[i],"-m") == 0)
            method_bool = true;
    }
    if(strcmp(*method,"MRNG") == 0 && *TankCandidates == -1)
    {
        cout<<"If MRNG is selected,-l parameter must be passed!"<<endl;
        exit(-1);
    }
}

/*Opens a binary file and returns a 2D vector with all its images*/
void OpenFile(char* filename,vector<vector<byte>>* images,bool test)
{
    Header header; //A header with the file metadata
    ifstream file(filename, ios::binary); //open the file as binary

    file.read(reinterpret_cast<char*>(&header), sizeof(Header)); //get the header bytes

    header.magic_number = htonl(header.magic_number); //Convert the values to 32 bit
    header.num_images = htonl(header.num_images);
    header.num_rows = htonl(header.num_rows);
    header.num_columns = htonl(header.num_columns);

    if(test)
        header.num_images = 10;
    vector<byte> temp(header.num_columns*header.num_rows); //a temp vector to store the pixels

    while (!file.eof()) 
    {
        file.read(reinterpret_cast<char*>(temp.data()), header.num_columns*header.num_rows*sizeof(byte)); //Read an entire image as a vector

        images->push_back(temp); //Store it in the images vector

        if (images->size() == header.num_images) //if we have all the images 
            break; //break the loop
    }
    file.close(); //close the file descriptor
}

/*Basic Pnorm for LSH algorithm*/
double PNorm(vector<byte>& A,vector<byte>& B,int p)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = abs((double)A[i] - (double)B[i]);
        /*if p is 0,we essentially check for no 0 values*/
        if(p == 0 && diff > 1e-6)
            sum += 1.0;
        else /*else find the diff to the power of p and add it to the sum*/
            sum += pow(diff,p);
    }
    /*if p is 0 retun just the sum*/
    if(p == 0)
        return sum;
    /*else return the n-nth root of the sum*/
    return pow(sum,1.0/p);
}

/*Implementation for Cluster file*/
double PNorm(vector<double>& A,vector<byte>& B,int p)
{
    double sum = 0.0,diff = 0.0;

    /*Both vectors must be of the same size*/
    if(A.size() != B.size())
    {
        cout<<"Vector sizes do not match!"<<endl;
        return -1.0;
    }

    /*iterate over both of the vectors*/
    for(int i = 0;i < A.size(); i++)
    {
        /*find the absolute of the difference between the 2 points*/
        diff = abs(A[i] - (double)B[i]);
        /*if p is 0,we essentially check for no 0 values*/
        if(p == 0 && diff > 1e-6)
            sum += 1.0;
        else /*else find the diff to the power of p and add it to the sum*/
            sum += pow(diff,p);
    }
    /*if p is 0 retun just the sum*/
    if(p == 0)
        return sum;
    /*else return the n-nth root of the sum*/
    return pow(sum,1.0/p);
}

