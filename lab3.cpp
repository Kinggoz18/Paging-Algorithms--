/*======================================================================
| Lab3
|
| Name: Lab3.cpp
|
| Written by: Chigozie Muonagolu - March 2023
|
| Purpose: To simulate two different page replacement policies on a fictional architecture.
|
| usage: ./labs
|
| Description of properties:
|
|------------------------------------------------------------------
*/

#include <iostream>
#include <time.h>
#include <random>
#include <algorithm>
#include <limits.h>
#include  <list>
#include <fstream>
#include <string>

using namespace std;

//Jobs class
class Job{
    public:
        int jobNumb = -1;    //The Job refrence number
        int PageNumb = -999; //The Page refrence number
        int TimeStamp = INT_MAX;  //The pages time stamp
};

//function declarations
bool PageRequestPipeline(ifstream& currentFile, string currLine);  //Function to read in a sequence page requests
bool SearchPhysicalMemory(int PageNumb);   //Helper function to search if a Page is in the PhysicalMemory array
bool SearchSwapSpace(int PageNumb);   //Helper function to search if a Page is in the SwapSpace array

void RemoveJob(int jobNumb);             //Helper function to terminate job from the PhysicalMemory array
void MoveToSwapSpace(int indexToMove);            //Function to move a page to swap space
void printPhysicalMemoory();            //Prints the layout of the physical memory
void printSwapSpace();            //Prints the layout of the swap space
void ResetGlobalVariables();        //Resets the global variables after each run
void MoveFromSwapSpace(int pageNumb, string algo);      //Moves a page from the swap space to physical memory
//Call the algorithms
void RunLeastRecentlyUsed(string basePath);
void RunRandom(string basePath);

int FindAvailableIndex();               //Function to find a free index in the PhysicalMemory array, returns -1 if none
int GetRandomIndex();                   //Gets a random index
int GetOldestPage();                    //Gets the oldest accessed page


//Algorithms
void LeastRecentlyUsed(ifstream& currentFile, string currLine);   //Least Recently Used
void Random(ifstream& currentFile, string currLine);      //Random

//Global variables
string fileNames [6] = {"job_data_1.csv", "job_data_2.csv", 
"job_data_3.csv", "job_data_4.csv", "job_data_5.csv", "job_data_6.csv"};    //array of data file name
const int MAX_SPACE = 35;

Job SwapSpace[15];     //variable to represent swap space
Job PhysicalMemory [10];     //List representing the main memory 
Job currentJob;    //Current job/Line being read
int firstPageLoad = 0;   //variable to keep count of the first time a page loads
int pageFaults = 0;     //variable to keep count of references made to a page already in physical memory
int pageHits = 0;    //variable to keep count pages moved from physical memory to swap spac
int CurrentTime = 100;  //Current time
int usedSpace = 0;      //To keep track of used spaces

//Program entry point
int main(){
    string basePath = "./data/";    //Base file path
    int option = -1;
    while(option != 0){
        cout<<"Enter a corresponding number to run an algorithm or terminate program.\n"<<
        "0. Terminate program\n"<<
        "1. Least Recently Used\n"<<
        "2. Random\n";
        cin>>option;
        switch (option)
        {
        case 0:
            cout<<"Terminating program...\n";
            break;
        case 1:
            RunLeastRecentlyUsed(basePath);
            break;
        case 2:
            RunRandom(basePath);
            break;
        default:
            cout<<"Invalid option selected\n";
            break;
        }
    }
    
    return 0;
}
//Function to run LeastRecentlyUsed
void RunLeastRecentlyUsed(string basePath){
cout<<"Outputs for Least Recently Used.\n";
    for(int i =0; i < 6; i++){
        string fileName = basePath + fileNames[i];  //name of the file being read
        ifstream currentFile (fileName);    //The current file
        string currLine;    //The current line being read

        LeastRecentlyUsed(currentFile, currLine);
        printf("\nFile %s\nPage hits: %d -- Page faults: %d -- First Page Load: %d\n", fileName.c_str(),  pageHits, pageFaults, firstPageLoad);
        printPhysicalMemoory();
        printSwapSpace();
        currentFile.close();     // Close the file
        ResetGlobalVariables(); //Reset the global variables
    }
}
//Functions to run Random
void RunRandom(string basePath){
cout<<"Outputs for Random.\n";
    for(int i =0; i < 6; i++){
        string fileName = basePath + fileNames[i];  //name of the file being read
        ifstream currentFile (fileName);    //The current file
        string currLine;    //The current line being read

        Random(currentFile, currLine);
        printf("\nFile %s\nPage hits: %d -- Page faults: %d -- First Page Load: %d\n", fileName.c_str(),  pageHits, pageFaults, firstPageLoad);
        printPhysicalMemoory();
        printSwapSpace();
        currentFile.close();     // Close the file
        ResetGlobalVariables(); //Reset the global variables
    }
}
//Functiont Definition for ResetGlobalVariables
void ResetGlobalVariables(){
    // Reset the elements of the SwapSpace array
    for (int i = 0; i < 15; i++) {
        SwapSpace[i] = Job();
    }
    // Reset the elements of the PhysicalMemory array
    for (int i = 0; i < 10; i++) {
        PhysicalMemory[i] = Job();
    }
    // Set the other variables to their default values
    currentJob = Job();
    firstPageLoad = 0;
    pageFaults = 0;
    pageHits = 0;
    CurrentTime = 100;
    usedSpace = 0;

}
//Functiont Definition for PageRequestPipeline
bool PageRequestPipeline(ifstream& currentFile, string currLine){
    if(getline( currentFile, currLine)){
        string jobNo = currLine.substr(0, 1);  //extract Job number
        string pageNo = currLine.substr(2, (currLine.length() - 3));  //extract Page number
        
        currentJob.jobNumb = stoi(jobNo);   //Set the currentJob jobNumb
        currentJob.PageNumb = stoi(pageNo);    //Set the currentJob page number
        currentJob.TimeStamp = CurrentTime;  //Set the Timestap
        CurrentTime+=1;    //Increament the current time
        return true;
    }
    else{
        return false;
    }
}

//Functiont Definition for printPhysicalMemoory
void printPhysicalMemoory(){
    int count = 0;
    printf("===================== Physical Memory layout ======================\n");
    for(int i = 0; i < 10; i++){
       if(PhysicalMemory[i].jobNumb!=-1){
         printf("Job number: %d - Page Number: %d\n", PhysicalMemory[i].jobNumb, PhysicalMemory[i].PageNumb);
         count+=1;
       }
    }
    cout<<"Pages in use: "<<count<<"\n";
}

//Functiont Definition for printSwapSpace
void printSwapSpace(){
    int count = 0;
    printf("===================== Swap space layout ======================\n");
    for(int i = 0; i < 15; i++){
       if(SwapSpace[i].jobNumb!=-1){
         printf("Job number: %d - Page Number: %d\n", SwapSpace[i].jobNumb, SwapSpace[i].PageNumb);
         count+=1;
       }
    }
    cout<<"Pages in use: "<<count<<"\n";
}

//Functiont Definition for SearchPhysicalMemory
bool SearchPhysicalMemory(int PageNumb){

    for(int i =0; i< 10; i++){
        //If the jobNumb to search for has been discorvered
        if(PhysicalMemory[i].PageNumb!=-999 && PhysicalMemory[i].PageNumb==PageNumb){
            PhysicalMemory[i].TimeStamp = CurrentTime;  //Update its last time accessed
            CurrentTime+=1;    //Update current time
            return true;
        }
    }
    return false;
}

//Functiont Definition for SearchSwapSpace
bool SearchSwapSpace(int PageNumb){
    for(int i =0; i< 15; i++){
        //If the jobNumb to search for has been discorvered
        if(SwapSpace[i].PageNumb!=-999 && SwapSpace[i].PageNumb==PageNumb){
            SwapSpace[i].TimeStamp = CurrentTime;  //Update its last time accessed
            CurrentTime+=1;    //Update current time
            return true;
        }
    }
    return false;
}
//Functiont Definition for RemoveJob
void RemoveJob(int jobNumb){
    //Remove the job from PhysicalMemory
    for(int i =0; i< 10; i++){
        Job curr = PhysicalMemory[i];
        //Set all the jobs in PhysicalMemory matching the jobNumb to the default job object
        if(curr.jobNumb!=-1 && curr.jobNumb==jobNumb){
            PhysicalMemory[i] = Job();
            usedSpace-=1;   //Updated used space
        }
    }
    //Remove the job from SwapSpace
    for(int i =0; i< 15; i++){
        Job curr = SwapSpace[i];
        //Set all the jobs in PhysicalMemory matching the jobNumb to the default job object
        if(curr.jobNumb!=-1 && curr.jobNumb==jobNumb){
            SwapSpace[i] = Job();
            usedSpace-=1;   //Updated used space
        }
    }
}

//Functiont Definition for IsSpaceAvailable
int FindAvailableIndex(){
    for(int i = 0; i < 10; i++){
        if(PhysicalMemory[i].jobNumb == -1){
            return i;
        }
    }
    return -1;
}

//Functiont Definition for GetOldestPage
int GetOldestPage(){
    int oldestTime = PhysicalMemory[0].TimeStamp;
    int index = 0;
    for(int i =1; i<10; i++){
        if(PhysicalMemory[i].TimeStamp < oldestTime){
            oldestTime = PhysicalMemory[i].TimeStamp;
            index = i;
        }
    }
    return index;
}

//Functiont Definition for MoveToSwapSpace
void MoveToSwapSpace(int indexToMove){
    Job temp = PhysicalMemory[indexToMove];
    for(int i = 0; i< 15; i++){
        if(SwapSpace[i].jobNumb == -1){
            SwapSpace[i] = temp;
            PhysicalMemory[indexToMove] = currentJob;
            break;
        }
    }
}

//Functiont Definition for GetRandomIndex
int GetRandomIndex(){
    srand(time(NULL));
    return rand() % 11;
}
//Functiont Definition for MoveToSwapSpace
void MoveFromSwapSpace(int pageNumb, string algo){
    //Get the page index
    int index = 0;
    for(int i = 0; i< 15; i++){
        if(SwapSpace[i].PageNumb == pageNumb){
            index = i;
            break;
        }
    }
    //Peforme swap operation
    int indexToSwap;
    if(algo == "LRU"){  //Determine the algorithm being used
        indexToSwap = GetOldestPage();
    }else if( algo == "Random"){
        indexToSwap = GetRandomIndex();
    }
    Job temp = SwapSpace[index];
    temp.TimeStamp =  CurrentTime;  //Update its timestamp
    CurrentTime+=1;
    SwapSpace[index] = PhysicalMemory[indexToSwap];
    PhysicalMemory[indexToSwap] = temp;
}

//Functiont Definition for Random
void Random(ifstream& currentFile, string currLine){
    int i = 0;
    while(PageRequestPipeline(currentFile, currLine)){
        //If the current job is to be terminated
        if(currentJob.PageNumb == -999){
            RemoveJob(currentJob.jobNumb);  //Free up space in the physical memory and swap space
        }else{
             //When we exceed our MAX_SPACE
            if(usedSpace >= MAX_SPACE){
                printf("Insufficient memory loading job %d requesting page %d\n", currentJob.jobNumb, currentJob.PageNumb);
                break;
            }
            else{
                //Check if the current page is in already in phsycial memory
            if(SearchPhysicalMemory(currentJob.PageNumb)){
                pageHits+=1;    //Page hit is increamented
            }
            else if( SearchSwapSpace(currentJob.PageNumb)){  //Else if it is in the swap space
                //Move it and update page hits
                MoveFromSwapSpace(currentJob.PageNumb, "Random");
                pageHits+=1;    //Page hit is increamented
            }
             else{
                i = FindAvailableIndex();   //Get the next available index from physical memory
                //If there is free space
                if(i != -1){
                    PhysicalMemory[i] = currentJob;
                }
                else{   //Else if there is not 
                    int IndexToSwap = GetRandomIndex();  //Get a random page
                    pageFaults+=1;
                    MoveToSwapSpace(IndexToSwap);
                }
                firstPageLoad+=1;   //The first page load is increamented 
                usedSpace+=1;   //Increament used space
            }
            }
        }
    }
}

//Functiont Definition for LeastRecentlyUsed
void LeastRecentlyUsed(ifstream& currentFile, string currLine){
    int i = 0;
    //While reading the current file
    while(PageRequestPipeline(currentFile, currLine)){
        //If the current job is to be terminated
        if(currentJob.PageNumb == -999){
            RemoveJob(currentJob.jobNumb);  //Free up space in the physical memory and swap space
        }
        else 
        {
            //When we exceed our MAX_SPACE
            if(usedSpace >= MAX_SPACE){
                printf("Insufficient memory loading job %d requesting page %d\n", currentJob.jobNumb, currentJob.PageNumb);
                break;
            }
            else{
                //Check if the current page is in already in phsycial memory
            if(SearchPhysicalMemory(currentJob.PageNumb)){
                pageHits+=1;    //Page hit is increamented
            }
            else if( SearchSwapSpace(currentJob.PageNumb)){  //Else if it is in the swap space
                //Move it and update page hits
                MoveFromSwapSpace(currentJob.PageNumb, "LRU");
                pageHits+=1;    //Page hit is increamented
            }
            else{
                i = FindAvailableIndex();   //Get the next available index from physical memory
                //If there is free space
                if(i != -1){
                    PhysicalMemory[i] = currentJob;
                }
                else{   //Else if there is not 
                    int IndexToSwap = GetOldestPage();  //Get the last accessed page
                    pageFaults+=1;
                    MoveToSwapSpace(IndexToSwap);
                }
                firstPageLoad+=1;   //The first page load is increamented 
                usedSpace+=1;   //Increament used space
            }
        }
        }
    }
}