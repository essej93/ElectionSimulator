//include statements for various functions
#include <iostream>
#include <iomanip>
#include "ElectionLibrary.h"

using namespace std;

// Main function which collects the arguments upon execution
int main(int argle, char* argv[]){

    // if statement to ensure the correct amount of arguments have been provided.
    if(argle != 3){
        cerr<<"Please enter valid arguments:\n./<exe> <number of electorates> <number of days for campaign>"<<endl;
        return 1;
    }
    int numOfDays, numOfElectorates;

    // converts entered values into integers.
    numOfElectorates = stoi(argv[1]);
    numOfDays = stoi(argv[2]);

    // if statement to validate correct parameters have been provided
    if((numOfDays > 30 || numOfDays < 1) || (numOfElectorates > 10 || numOfElectorates < 1)){
        cerr<<"Number of electorates must be between 1 - 10 inclusive. Number of campign days must be 1 - 30 inclusive."<<endl;
        return 1;
    }

    //creates Election object and provides the numOfElectorates and days for the campaign
    Election election = Election(numOfElectorates, numOfDays);

    // calls function which runs all functions for generating/loading the election
    election.generateElection();

    // calls function to print all the info of the election.
    election.printElection();

    // calls the function which runs all functions to execute the election simulation
    election.runElection();

    // calls the function which runs all functions which calculates votes and prints the result
    election.finishElection();

    return 0;
}
