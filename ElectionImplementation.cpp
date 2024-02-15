//include statements for various functions
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <ostream>
#include <random>
#include <vector>
#include <map>
#include <algorithm>
#include "ElectionLibrary.h"

using namespace std;

/*  Start of Election functions */

//Election constructor which takes numOfElectorates and numOfDays
Election::Election(int electorates, int days){
    numOfElectorates = electorates;
    numOfDays = days;
}

// function used to generate/load all data used within the election.
void Election::generateElection(){

    loadIssues(); // loads all issues
    loadEvents(); // loads all events
    loadElectorates(); // loads all electorates
    generateElectorateClusters(); // generates clusters for electorates
    calculateElectoratePop(); // calculates the total pop of each electorate
    generateParties(); // generates parties

}

//function used to run all functions used from running the campaign simulation
// to printing the post campaign report.
void Election::runElection(){
    runCampaign(); // runs the campaign simulation
    calculatePopularity(); // calculates popularity after campaign
    reportElection(); // reports on the election
}

// function used to run all functions which wrap up the election
void Election::finishElection(){
    tallyVotes(); // calculates and tally's the votes for each electorate
    determineWinner(); // determines winner and prints results
}

// function used to run functions for loading leaders and the stance ranges
// parties are then generated and then candidates.
// also ensures each party leader has pointer assigned to their own party
void Election::generateParties(){

    loadLeadersAndStanceRange(); // loads party leaders, generates parties, loads stance ranges
    generateCandidates(); // generates candidates for each party.

    // sets the leaders related party to the party that they're leading
    parties[0].getLeader().setRelatedParty(&parties[0]);
    parties[1].getLeader().setRelatedParty(&parties[1]);
    parties[2].getLeader().setRelatedParty(&parties[2]);

}

// function used to load the individual issues and add them to the issues vector
void Election::loadIssues(){
    issues.push_back(Issue("COVID-19 Financial Situation", "The financial situation caused by COVID-19 to Australia's population which is having a huge impact to businesses and families.", IssueType::ECONOMIC));
    issues.push_back(Issue("Sauce Debate","The ongoing debate of whether tomato sauce belongs in the fridge or cupboard which has the nation divided.", IssueType::SOCIAL));
    issues.push_back(Issue("Toilet Paper Shortage","The national toilet paper shortage causing unrest with the nation.", IssueType::LOGISTICS));
    issues.push_back(Issue("Global Warming","The ongoing issue of Global warming and how to best handle ongoing affairs for it.", IssueType::ENVIRONMENTAL));
    issues.push_back(Issue("Mandatory Vaccines","COVID-19 mandatory vaccines.", IssueType::HEALTH));
}

// function used to load the individual events and add them to the events vector
// the statements for events are dynamic and candidate names will be entered upon printing the statements.
void Election::loadEvents(){
    events.push_back(Event(EventType::DEBATE, "%s & %s have decided to have a debate\n", 6, Characteristic::DEBATING));
    events.push_back(Event(EventType::CANDIDATE_RELATED, "Oh no! %s has been involved in a scandal!\n", 10, Characteristic::POPULARITY));
    events.push_back(Event(EventType::CANDIDATE_RELATED, "%s has played a prank on another candidate\n", 5, Characteristic::POPULARITY));
    events.push_back(Event(EventType::LEADER_RELATED, "The Party Leaders %s & %s have decided to have a friendly boxing match in %s \n", 10, Characteristic::POPULARITY));
    events.push_back(Event(EventType::LEADER_RELATED, "The Party Leaders %s & %s have decided to have a have a debate in %s today!\n", 10, Characteristic::POPULARITY));
    events.push_back(Event(EventType::ISSUE_RELATED, "%s has observed how other countries are handling the %s issue.\n", 7, Characteristic::POPULARITY));
    events.push_back(Event(EventType::ISSUE_RELATED, "Some new information has been released on the %s issue by %s\n", 5, Characteristic::POPULARITY));
}

// function used to load electorates from Electorates.txt and add each electorate to the
// electorates vector.
void Election::loadElectorates(){

    ifstream electorateFile;
    string line;

    electorateFile.open("Electorates.txt");

    if(electorateFile.bad() || !electorateFile.is_open()){
        cerr<<"Electorate file was unable to be opened"<<endl;
        exit(1);
    }

    // uses numOfElectorates to determine how many to load from the file.
    for(int x = 0; x < numOfElectorates; x++){
        getline(electorateFile, line);

        stringstream lineStream(line);
        string electorateName, pop;
        int totalPopulation;

        getline(lineStream, electorateName, ',');
        getline(lineStream, pop);

        totalPopulation = stoi(pop);


        electorates.push_back(Electorate(issues, electorateName, totalPopulation));
    }

    electorateFile.close();

}

// function used to generate clusters for each electorate.
void Election::generateElectorateClusters(){

    // for loop to run through each electorate.
    for(Electorate &currentElectorate: electorates){

        RandomGenerator random;
        int minPop, maxPop, pop;

        // gets the totalpop from the current electorate which is based off
        // total pop in the electorates.txt file
        pop = currentElectorate.getPopulation();

        // maxPop holds 1/4 of the total pop
        maxPop = pop/4;
        // min pop holds 1/8 of the total pop.
        minPop = maxPop/2;

        // for loop runs 4 times to generate 4 clusters
        for(int x = 0; x < 4; x++){
            // creates Electorate cluster by using range inbetween minpop and maxpop
            // this is provided by using the randomIntRange function.
            // This range ensures that total pop of 4 clusters does not go above the original totalpop
            currentElectorate.addCluster(ElectorateCluster(random.randomIntRange(minPop, maxPop)));
        }

        // Variables used to for the min max range of the approach and significance of stances
        // these variables can be changed for different results.
        int sigMin, sigMax, appMin, appMax;
        sigMin = 1;
        sigMax = 9;
        appMin = 0;
        appMax = 100;

        // After clusters are generated and added to the electorate
        // we use this for loop to run through each electorate and generate
        // individual stances for each issue for each cluster.
        for(ElectorateCluster &currentCluster : currentElectorate.getClusters()){
            for(Issue &currentIssue : issues){

                Issue newIssue(currentIssue);
                Stance newStance(newIssue, random.randomIntRange(sigMin, sigMax), random.randomIntRange(appMin, appMax));

                currentCluster.addStance(newStance);

            }
        }

    }
}

// After all electorates have been generated and their clusters have been generated
// we use this function to calculate the totalPopulation of the electorate
// based on the population of each cluster.
void Election::calculateElectoratePop(){

    for(Electorate &currentElectorate : electorates){
        unsigned int pop = 0;
        for(ElectorateCluster &currentCluster : currentElectorate.getClusters()){
            pop += currentCluster.getPopulation();
        }
        currentElectorate.setPopulation(pop);

    }


}

// function used to load the leaders from the Leaders.txt
// this also loads the stance ranges of each party which is
// also held in the Leaders.txt file.
// The parties are then generated and added to the party vector
void Election::loadLeadersAndStanceRange(){

    ifstream leaderFile;
    string line;


    leaderFile.open("Leaders.txt");

    if(leaderFile.bad() || !leaderFile.is_open()){
        cerr<<"Leaders file unable to be opened"<<endl;
        exit(1);
    }

    while(getline(leaderFile, line)){

        stringstream lineStream(line);
        string leaderName, partyName, nextNum;
        int num;
        int ranges[5][4];

        getline(lineStream, partyName, ',');
        getline(lineStream, leaderName, ',');

        for(int x = 0; x < 5; x++){
            for(int y = 0; y < 4; y++){
                getline(lineStream, nextNum, ',');
                num = stoi(nextNum);
                ranges[x][y] = num;
            }
        }

        // creates the leader from the information collected
        Candidate newLeader(leaderName, ranges, issues);

        // creates a party using the new leader and ranges collected from the file
        Party newParty(partyName, newLeader, ranges);

        // adds the party to the parties vector
        parties.push_back(newParty);
    }

    leaderFile.close();
}

// function used to generate candidates for each party using their respective candidate .txt file
void Election::generateCandidates(){

    ifstream candidateFile;
    string fileName;
    string candidateName;

    // for loop used to run through each party
    for(Party& party : parties){

        // if statements used to determine which file to load the candidates from based on the party name
        if(party.getName() == "Labor Party") fileName = "LaborPartyCandidates.txt";
        else if(party.getName() == "Liberal Party") fileName = "LiberalPartyCandidates.txt";
        else if(party.getName() == "Foam Party") fileName = "FoamPartyCandidates.txt";

        candidateFile.open(fileName);

        if(candidateFile.bad() || !candidateFile.is_open()){
            cerr<<fileName<<" was unable to be opened"<<endl;
            exit(1);
        }

        // loop used to run through each electorate and generate a candidate for each electorate
        // uses the electorate name in the candidate constructor.
        // then loads the candidate into the current party.
        for(Electorate e : electorates){
            getline(candidateFile, candidateName);

            Candidate newCandidate(e.getName(), candidateName, party.stanceRanges, issues);
            party.loadCandidate(newCandidate);

        }

        candidateFile.close();
    }


}

// function used to print all information about the election.
// This is all the information you see before the campaign.
void Election::printElection(){

    cout<<"=======================Election Simulator=======================\n"<<endl;

    cout<<"                         ~~~~ISSUES~~~\n\n"<<endl;
    cout<<"The 5 issues that Party's and Candidates will be campaigning on and discussing:"<<endl;
    cout<<endl;

    int count = 1;

    // prints out all the individual issues
    for(Issue i : issues){
        cout<<"ISSUE #"<<count<<" - ";
        i.printIssue();
        count++;
        cout<<endl;
    }
    cout<<endl;

    /* Start of print for Party information */
    cout<<"============================================================================"<<endl;
    cout<<"                           ~~~~PARTIES~~~\n\n"<<endl;
    cout<<"Party 1: Labor Party"<<endl;
    cout<<endl;
    cout<<"The Labor party wants to provide equal opportunities for everyone in the nation, and attempt to make decisions on what's best for the people."<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[0].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[0].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[0].printParty();
    cout<<"-----------------------------------------------------------------------"<<endl;

    cout<<"Party 2: Liberal Party"<<endl;
    cout<<endl;
    cout<<"The Liberal party make decisions on what they think is best for the nation, even if this may effect the people of that nation"<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[1].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[1].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[1].printParty();
    cout<<"-----------------------------------------------------------------------"<<endl;

    cout<<"Party 3: Foam Party"<<endl;
    cout<<endl;
    cout<<"The Foam Party just wants everyone to have a good time, they also struggle with making choices on much bigger issues."<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[2].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[2].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[2].printParty();
    cout<<"============================================================================"<<endl;
    /* End of print for Party information*/

    /* Start of print for Electorate information*/
    cout<<"                              ~~~~Electorates~~~\n\n"<<endl;

    cout<<"There are "<<electorates.size()<<" electorates"<<endl;
    cout<<endl;

    for(Electorate& currentElectorate : electorates){
        currentElectorate.printElectorate();

        cout<<"----------------------------------------------------------------------"<<endl;
        cout<<"Candidates: "<<endl;
        for(Party& currentParty : parties){
            auto it = currentParty.getCandidates().find(currentElectorate.getName());
            cout<<it->second.getName()<<" (";
            it->second.printCharacteristics();
            cout<<")"<<endl;

        }

        cout<<"\n"<<endl;

    }
    /* End of print for Electorate information*/
}

// function used to print all relevant information after the campaign
// this is all the information you see after campaigning has ended.
void Election::reportElection(){


    cout<<"-----------=======================POST CAMPAIGN REPORT===============================-----------"<<endl;
    /* Start of print for Party information */
    cout<<"                                    ~~~~PARTIES~~~\n"<<endl;
    cout<<"Party 1: Labor Party"<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[0].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[0].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[0].printParty();
    cout<<"-----------------------------------------------------------------------"<<endl;

    cout<<"Party 2: Liberal Party"<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[1].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[1].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[1].printParty();
    cout<<"-----------------------------------------------------------------------"<<endl;

    cout<<"Party 3: Foam Party"<<endl;
    cout<<endl;
    cout<<"Leader: "<<parties[2].getLeader().getName()<<endl;
    cout<<"Leaders Characteristics: ";
    parties[2].getLeader().printCharacteristics();
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    parties[2].printParty();
    cout<<"============================================================================"<<endl;
    /* End of print for Party information */

    /* Start of print for Electorate information */
    cout<<"                              ~~~~Electorates~~~\n\n"<<endl;

    for(Electorate& currentElectorate : electorates){
        currentElectorate.printElectorate();

        cout<<"----------------------------------------------------------------------"<<endl;
        cout<<"Candidates: "<<endl;
        for(Party& currentParty : parties){
            auto it = currentParty.getCandidates().find(currentElectorate.getName());
            cout<<it->second.getName()<<" (";
            it->second.printCharacteristics();
            cout<<")"<<endl;

        }

        cout<<"\n"<<endl;

    }
    /* End of print for Electorate information */
}


// This function is for running the campaigning simulation
// It will run from 1-30 days depending on the how many days were entered during execution
// for each day, each electorate will have a 50% chance to run an event.
// this is done by generating a random number between 1-2, if it's 2 then an event will be triggered
// otherwise nothing will happen on that day for that electorate
void Election::runCampaign(){

    bool leaderEvent = false; // used to determin if a leader event has happened
    bool eventFound = false; // used to determine when an event is found.
    RandomGenerator random; // used for generating random number/shuffling array
    int eventRoll; // used to hold a 1 or 2 to determine if an event goes ahead
    int eventNumber; // used to hold the eventNumber i.e the ID

    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~CAMPAIGNING HAS STARTED~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    // for loop to run from 1 - x amount of days
    // for loop counts down as so we can print the remaining days/loops
    for(numOfDays; numOfDays>0; --numOfDays){
        cout<<"\n\n----------===== "<< numOfDays<<" Day(s) until Election =====----------"<<endl;

        leaderEvent = false; // sets the leaderEvent to false at the start of each day

        // For loop is run each day, to determine if an event occurs in each electorate.
        for(Electorate &currentElectorate : electorates){
            cout<<"-------------------------------------------------"<<endl;
            cout<<"Daily report for "<< currentElectorate.getName()<<": "<<endl;
            cout<<"================================================="<<endl;

            // used to generate a random number 1 or 2
            // this provides a 50/50 chance of either
            eventRoll = random.randomIntRange(1,2);

            // if eventRoll == 2 then an event is ran
            if(eventRoll == 2){
                random.shuffleArray(); // shuffles the array which is used to get random candidates/leaders
                eventFound = false;

                // This do while loop gets an event number then
                // the logic in the do while loop first checks to see if it's leader related.
                // If it's a leader related event it will then check to see if a leader event
                // has already been ran for this election day.
                // If a leader event has already been run, then it continues to loop until it's not a leader event.
                do{
                    random.shuffleArray();
                    eventNumber = getEvent();
                    if(eventNumber == 3 || eventNumber == 4){
                        if(!leaderEvent){
                            leaderEvent = true;
                            eventFound = true;
                        } else continue;
                    }
                    else{
                        eventFound = true;
                    }
                } while(!eventFound);

                //Handle event will run once an event has been found in the above loop
                handleEvent(eventNumber, &currentElectorate);

                cout<<endl;

            }
            else{
                cout<<"Nothing happened in "<<currentElectorate.getName()<<" today"<<endl;
            }
        }
    }
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~CAMPAIGNING HAS FINISHED~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cout<<endl;

}

// function used to obtain random event by first randomising a number between 1-10.
// depending on the number it then returns the event code to a specific event
// the event code will then be used to grab the event from the event vector.
// the variables here can be changed to alter the probability
int Election::getEvent(){
    RandomGenerator random;

    int rollEvent = random.randomIntRange(1, 20);

    if(rollEvent <= 9) return 0; // Candidate Debate sitting at 40% chance
    else if(rollEvent <= 10) return 1; // Candidate related, Scandal 5% chance
    else if(rollEvent <= 12) return 2; // Candidate related, prank 10% chance
    else if(rollEvent <= 13) return 3; // Leader related, boxing 5% chance
    else if(rollEvent <= 14) return 4; // Leader related, debating 5% chance
    else if(rollEvent <= 16) return 5; // Issue related, international influence 10% chance
    else if(rollEvent <= 20) return 6; // Issue related, new issue information 20% chance

    return 0;
}



// This function is to first identify the event and print out any statements regarding the event
// it also collects the correct parameters needed to then pass to the processEvent function which will
// hold the logic for each event.
// The way that random candidates and leaders are chosen for events and ensuring that the picks are unique
// is that there is a static array RandomGenerator::shuffledArray which holds 3 numbers {0,1,2}
// these numbers are shuffled prior to an event running, then the candidates/leaders are picked by using the
// first 2 elements from the shuffled array. This shuffle happens in the multiple sections before
// handleEvent is called.
void Election::handleEvent(int event, Electorate* elec){

    vector<Candidate*> candidatesInElect; // holds pointers to each candidate in the current electorate
    vector<Candidate*> leaders; // holds pointers to the party leaders

    RandomGenerator random; // used for shuffling array and generating random number between range.

    //for loop which collects the memory addresses of the related candidates for the current electorate
    //adds them to a vector holding pointers to the candidates.
    for(Party& currentParty : parties){
        Candidate& partyCandidate = currentParty.getCandidates().find(elec->getName())->second;
        candidatesInElect.push_back(&partyCandidate);
    }

    //for loop which collects the memory addresses of the leaders
    //adds them to a vector holding pointers to the leaders.
    for(Party& currentParty : parties){
        Candidate& partyLeader = currentParty.getLeader();
        leaders.push_back(&partyLeader);
    }

    random.shuffleArray(); // shuffles static int array

    //Switch case which determines which event to run based on the event number
    switch(event){
        case 0: // Candidate debate event
        {
            Candidate *cand1 = candidatesInElect[RandomGenerator::shuffledArray[0]];
            Candidate *cand2 = candidatesInElect[RandomGenerator::shuffledArray[1]];
            printf(events[event].getStatement().c_str(), cand1->getName().c_str(), cand2->getName().c_str());
            processEvent(event, cand1, cand2, elec);
        break;
        }


        case 1: // Candidate Related event 1 - Scandal
        {
            Candidate *cand1 = candidatesInElect[RandomGenerator::shuffledArray[0]];
            printf(events[event].getStatement().c_str(), cand1->getName().c_str());
            processEvent(event, cand1, nullptr, elec);
        }
        break;

        case 2: // Candidate Related event 2 - Prank
        {
            Candidate *cand = candidatesInElect[RandomGenerator::shuffledArray[0]];
            printf(events[event].getStatement().c_str(), cand->getName().c_str());
            processEvent(event, cand, nullptr, elec);
        break;
        }


        case 3: // Leader Related event 1 - Boxing match
        {
            Candidate *lead1 = leaders[RandomGenerator::shuffledArray[0]];
            Candidate *lead2 = leaders[RandomGenerator::shuffledArray[1]];
            printf(events[event].getStatement().c_str(), lead1->getName().c_str(), lead2->getName().c_str(), elec->getName().c_str());
            processEvent(event, lead1, lead2, elec);
        break;
        }

        case 4: // Leader Related event 2 - Debate
        {
            Candidate *lead1 = leaders[RandomGenerator::shuffledArray[0]];
            Candidate *lead2 = leaders[RandomGenerator::shuffledArray[1]];
            printf(events[event].getStatement().c_str(), lead1->getName().c_str(), lead2->getName().c_str(), elec->getName().c_str());
            processEvent(event, lead1, lead2, elec);
        break;
        }

        case 5: // Issue Related event 1 - international influence
        {
            IssueType eventIssueType = static_cast<IssueType>(random.randomIntRange(0, 4));
            events[event].setRelatedIssue(eventIssueType);
            int issueType = static_cast<underlying_type<IssueType>::type>(eventIssueType);
            printf(events[event].getStatement().c_str(), elec->getName().c_str(), issues[issueType].getIssueCode().c_str());
            processEvent(event, nullptr, nullptr, elec);
        break;
        }

        case 6: // Issue Related event 1 - information released by candidate
        {
            Candidate *cand = candidatesInElect[RandomGenerator::shuffledArray[0]]; // pointer to random candidate in electorate
            IssueType eventIssueType = static_cast<IssueType>(random.randomIntRange(0, 4)); // gets random issue type
            events[event].setRelatedIssue(eventIssueType); // sets the current events issue type to the random one collected
            int issueType = static_cast<underlying_type<IssueType>::type>(eventIssueType); // collects the number of the issueType to identify in the issues vector

            // prints the event statement and fills in the statement with candidate name and related issue
            printf(events[event].getStatement().c_str(), issues[issueType].getIssueCode().c_str(), cand->getName().c_str());
            processEvent(event, cand, nullptr, elec); // sends info to process event, nullptr as there is no 2nd candidate.
        break;
        }



    }
}

// Function used for processing events, in cases where only one person is affected or no electorate is impacted
// a null ptr will be sent as person2 and electorate argument
void Election::processEvent(int event, Candidate* person1, Candidate* person2, Electorate* elec){

    RandomGenerator random;
    Event currentEvent = events[event];
    Candidate* winner;
    Candidate* loser;

    switch(event){
        // case for debate event
        // gets the charisma of both candidates and divides it by 2
        // rolls for both candidates based on the impacted char + half their charisma
        // uses a standard deviation of 3 to and then returns the roll and compares them
        // winner then has their debating and popularity characteristic updated.
        // The electorate is then influenced based on the winners stances.
        case 0:
        {
            // variables to hold info used to calculate event outcome
            int cand1Roll, cand2Roll;
            int cand1Charisma, cand2Charisma;
            bool positiveImpact = true;
            cand1Charisma = person1->getCharValue(Characteristic::CHARISMA)/2; // get charisma values
            cand2Charisma = person2->getCharValue(Characteristic::CHARISMA)/2;

            cand1Roll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + cand1Charisma, 3); // gets std deviation roll
            cand2Roll = random.standardDeviation(person2->getCharValue(currentEvent.getImpactedChar()) + cand2Charisma, 3);
            //cout<<"person 1 roll: "<<cand1Roll<<" Person 2 roll: "<<cand2Roll<<endl;
            if(cand1Roll > cand2Roll){
                cout<<person1->getName()<<" has won the debate for "<<person1->getRelatedParty()->getName()<<"!"<<endl;
                winner = person1;
            }
            else if(cand2Roll > cand1Roll){
                cout<<person2->getName()<<" has won the debate for "<<person2->getRelatedParty()->getName()<<"!"<<endl;
                winner = person2;
            }
            else{
                cout<<"There was no clear winner of the debate!"<<endl;
                break;
            }

            // updates winners characteristic which using the impact range of the event
            // it then influences the electorate which updates their stance approach to be more like the candidates
            winner->updateChar(currentEvent.getImpactedChar(), currentEvent.getImpactRange());
            winner->updateChar(Characteristic::POPULARITY, currentEvent.getImpactRange());
            elec->influenceElectorate(winner->getStances(), positiveImpact);
            cout<<"The electorates stances on current issues has been influenced by the points made by "<<winner->getName();
            cout<<" during the debate and has increased their popularity"<<endl;
        break;
        }

        // case for Candidate 1 - Scandal event
        // gets the charisma of targeted candidate and divides it by 2
        // rolls for the candidate based on the impacted char + half their charisma
        // uses a standard deviation of 5 and then returns the roll
        // and compares it to the eventPassRoll
        case 1:
        {
            // variables to hold info used to calculate event outcome
            int candRoll, candCharisma, mngTeamEventHandle;
            int const eventPassRoll = 30; // this can be changed to impact the pass chance value

            mngTeamEventHandle = person1->getRelatedParty()->getManagerialTeam().getCharValue(Characteristic::EVENTHANDLE); // gets mng team eventhandle
            candCharisma = person1->getCharValue(Characteristic::CHARISMA)/2; // gets charisma for candidate
            candRoll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + candCharisma, 5) + mngTeamEventHandle;

            // if event is successful, their popularity is still impacted however
            // only by half and they get a boost in charisma.
            if(candRoll >= eventPassRoll){
                cout<<person1->getName()<<" was somehow able to talk themselves out of the scandal!"<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), ((currentEvent.getImpactRange() - mngTeamEventHandle) * -1));
                person1->updateChar(Characteristic::CHARISMA, currentEvent.getImpactRange());
            }
            // if event is unsuccessful candidate gets an impact to their popularity
            else{
                cout<<person1->getName()<<" has not been able to explain themselves"<<endl;
                cout<<elec->getName()<<" are not happy with how "<<person1->getName()<<" has handled this situation"<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), (currentEvent.getImpactRange() * -1));
            }
        break;
        }


        // case for Candidate 2 - Prank event
        // gets the charisma of targeted candidate and divides it by 2
        // rolls for the candidate based on the impacted char + half their charisma
        // uses a standard deviation of 5 and then returns the roll
        // and compares it to the eventPassRoll
        case 2:
        {
            // variables to hold info used to calculate event outcome
            int candRoll, candCharisma;
            int const eventPassRoll = 20;

            candCharisma = person1->getCharValue(Characteristic::CHARISMA)/2; // gets charisma for candidate
            candRoll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + candCharisma, 5);

            // if event is successful then candidate gets boost to their charisma and impacted char (popularity)
            if(candRoll >= eventPassRoll){
                cout<<elec->getName()<<" found the prank that "<<person1->getName()<<" pulled was hilarious!"<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), currentEvent.getImpactRange());
                person1->updateChar(Characteristic::CHARISMA, currentEvent.getImpactRange());
            }
            // if event is unsuccessful, candidate gets negative impact to popularity
            else{
                cout<<elec->getName()<<" was not impressed with the prank that "<<person1->getName()<<" pulled."<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), (currentEvent.getImpactRange() * -1));
            }
        break;
        }

        // Leader 1 - boxing event
        // collects mng team event handling, charisma and popularity and
        // rolls using ((charisma/2 + eventHandling) + popularity) with std deviation of 3.
        case 3:
        {
            // variables to hold info used to calculate event outcome
            int lead1Roll, lead2Roll;
            int lead1Modifier, lead2Modifier;
            int lead1MngEventHandling, lead2MngEventHandling;

            // gets both candidates mngTeam event handle value
            // collects charisma with added event handle and rolls using the modifier and popularity
            lead1MngEventHandling = person1->getRelatedParty()->getManagerialTeam().getCharValue(Characteristic::EVENTHANDLE);
            lead2MngEventHandling = person2->getRelatedParty()->getManagerialTeam().getCharValue(Characteristic::EVENTHANDLE);
            lead1Modifier = (person1->getCharValue(Characteristic::CHARISMA)/2) + lead1MngEventHandling;
            lead2Modifier = (person2->getCharValue(Characteristic::CHARISMA)/2) + lead2MngEventHandling;
            lead1Roll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + lead1Modifier, 3);
            lead2Roll = random.standardDeviation(person2->getCharValue(currentEvent.getImpactedChar()) + lead2Modifier, 3);

            // if/else statements determine the winner/loser.
            // if there is no winner or loser, both leaders get rewarded equally
            // if there is a winner/loser then the winner is rewarded and loser get rewarded only half
            // of the impacted stat
            if(lead1Roll > lead2Roll){
                winner = person1;
                loser = person2;
            }
            else if(lead2Roll > lead1Roll){
                winner = person2;
                loser = person1;
            }
            else{
                cout<<"There was no clear winner of the bout!"<<endl;
                cout<<"The nation is impressed with both leaders!"<<endl;
                break;
            }

            cout<<winner->getName()<<" has won the bout!"<<endl;

            // updates winners/losers characteristic  using the impact range of the event
            // the loser gets the impactRange divided by 2.
            winner->updateChar(currentEvent.getImpactedChar(), currentEvent.getImpactRange());
            loser->updateChar(currentEvent.getImpactedChar(), (currentEvent.getImpactRange()/2));
            cout<<"The nation is impressed with how "<<winner->getName();
            cout<<" handled the fight."<<endl;
        break;
        }

        // Leader 2 - Debate event
        // collects mng team event handling, charisma and popularity and
        // rolls using ((charisma/2 + eventHandling) + popularity) with std deviation of 3
        // The winner influences EVERY electorate based on their stances
        case 4:
        {
            // variables to hold info used to calculate event outcome
            int lead1Roll, lead2Roll;
            int lead1Modifier, lead2Modifier;
            int lead1MngEventHandling, lead2MngEventHandling;
            bool positiveImpact = true;

            // gets both candidates mngTeam event handle value
            // collects charisma with added event handle and rolls using the modifier and popularity
            // with std deviation of 3
            lead1MngEventHandling = person1->getRelatedParty()->getManagerialTeam().getCharValue(Characteristic::EVENTHANDLE);
            lead2MngEventHandling = person2->getRelatedParty()->getManagerialTeam().getCharValue(Characteristic::EVENTHANDLE);
            lead1Modifier = (person1->getCharValue(Characteristic::CHARISMA)/2) + lead1MngEventHandling;
            lead2Modifier = (person2->getCharValue(Characteristic::CHARISMA)/2) + lead2MngEventHandling;
            lead1Roll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + lead1Modifier, 3);
            lead2Roll = random.standardDeviation(person2->getCharValue(currentEvent.getImpactedChar()) + lead2Modifier, 3);

            // checks if lead1 is the winner
            if(lead1Roll > lead2Roll){
                winner = person1;
            }
            // checks if lead2 is the winner
            else if(lead2Roll > lead1Roll){
                winner = person2;
            }
            // else if there is a draw
            else{
                cout<<"There was no clear winner of the debate!"<<endl;
                break;
            }

            cout<<winner->getName()<<" has won the debate for "<<winner->getRelatedParty()->getName()<<"!"<<endl;

            // updates winners characteristic which using the impact range of the event
            // it then influences the electorate which updates their stance approach to be more like the candidates
            winner->updateChar(currentEvent.getImpactedChar(), currentEvent.getImpactRange());
            winner->updateChar(Characteristic::POPULARITY, currentEvent.getImpactRange());

            // for loop to influence every electorate
            for(Electorate& currentElectorate : electorates){
                currentElectorate.influenceElectorate(winner->getStances(), positiveImpact);
            }

            cout<<"The nations stances on current issues has been influenced by the points made by "<<winner->getName();
            cout<<" during the debate and has increased their popularity"<<endl;
        break;
        }

        // Issue 1 - International influence
        // rolls 50/50 to determine if the electorate
        // is going to be impacted by international influence
        // creates stance using random range for the issue
        // for the international influence
        case 5:
        {
            int checkImpact = random.randomIntRange(1,2);

            // used to get the int of the issue to identify the issue to generate a random influence range
            int issueType = static_cast<underlying_type<IssueType>::type>(events[event].getIssueType());

            // if checkImpact is 2 it will influence the electorate
            if(checkImpact == 2){
                // creates a stance for the issue with random ranges for the international influence
                Stance internationalStance(issues.at(issueType), random.randomIntRange(1,9), random.randomIntRange(1,100));
                cout<<elec->getName()<<" are more in agreence with other countries views, their stances have been impacted by International influence"<<endl;
                elec->influenceStance(internationalStance, true); // influences electorate using the created stance.
            }
            else cout<<"Other countries have similar stances to "<<elec->getName()<<" so they are happy with their views."<<endl;
        break;
        }

        // Issue 2 - new information event
        // rolls against candidate mngTeam event handling, charisma and popularity.
        // if successful, influences electorate and increases popularity and influences
        // electorate positively.
        // else it decreases popularity and influence electorates negatively.
        case 6:
        {
            // variables to hold info used to calculate event outcome
            int candRoll, candCharisma;
            int const eventPassRoll = 15; // event pass roll for event
            bool positiveImpact;

            // used to get the int of the issue to identify the issue to impact influence
            int issueType = static_cast<underlying_type<IssueType>::type>(events[event].getIssueType());

            candCharisma = person1->getCharValue(Characteristic::CHARISMA)/2;
            candRoll = random.standardDeviation(person1->getCharValue(currentEvent.getImpactedChar()) + candCharisma, 5);

            // checks if candidates roll is successful against eventPassRoll
            // This is successful outcome
            if(candRoll >= eventPassRoll){
                cout<<person1->getName()<<" was able to confirm the new information was credible. "<<endl;
                cout<<elec->getName()<<" are happy that "<<person1->getName()<<" was able to confirm this new information"<<endl;
                cout<<elec->getName()<<" stances are more aligned with "<<person1->getName()<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), currentEvent.getImpactRange());
                positiveImpact = true;
            }
            // if cand roll is not equal orr higher to pass roll
            // runs unsuccessful event outcome
            else{
                cout<<person1->getName()<<" was unable to confirm the new information was credible. "<<endl;
                cout<<elec->getName()<<" are not happy that "<<person1->getName()<<" would share this fake information"<<endl;
                cout<<elec->getName()<<" stances are less aligned with "<<person1->getName()<<endl;
                person1->updateChar(currentEvent.getImpactedChar(), (currentEvent.getImpactRange() * -1));
                positiveImpact = false;
            }

            // influences electorate
            elec->influenceStance(person1->getStances().at(issueType), positiveImpact);

        break;
        }


    }
}

// function used to tally the votes for each electorate.
// it compares each candidates stances with eacg electorate clusters stances
// then calculates who has a closer stance, and calculates votes based on
// the stances won for that cluster.
// prints out all voting information for each cluster/candidate
void Election::tallyVotes(){

    cout<<"~~~~~~~~~~~~~~~~~~~~~================VOTING HAS STARTED================~~~~~~~~~~~~~~~~~~~~~\n"<<endl;
    // for loop runs through each electorate
    for(Electorate& currentElectorate : electorates){

        vector<Candidate*> candidatesInElect; // vector of pointers to hold candidates for current elec

        // for loop to collect candidates for the current electorate
        for(Party& currentParty : parties){
            Candidate& partyCandidate = currentParty.getCandidates().find(currentElectorate.getName())->second;
            candidatesInElect.push_back(&partyCandidate);
        }


        // how to tally the votes.
        // collect each candidate for an electorate
        // iterate through each individual stance
        // create formula which compares the candidates stance
        // with the electorate clusters stance.
        // compare the ranges for each candidate for that particular stance
        // one with the lowest range gets 1 point for that stance
        // candidate with most points for each stance for the cluster
        // wins the votes for that cluster
        // candidate with the most votes after all clusters are checked will win
        // the electorate and add to the electorate won tally of the party

        cout<<currentElectorate.getName()<<" (Population: "<<currentElectorate.getPopulation()<<") Vote Distribution: "<<endl;
        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
        int count = 1; // count to keep track of current cluster

        // for loop to run through each cluster in an electorate
        for(ElectorateCluster& currentCluster : currentElectorate.getClusters()){
            cout<<currentElectorate.getName()<<" Cluster #"<<count<<" (Population: "<<currentCluster.getPopulation()<<")"<<endl;

            // calls to calculate cluster votes and provides the cluster and candidates in elec
            calculateClusterVotes(&candidatesInElect, &currentCluster);
            cout<<"-----------------------------------------------------------"<<endl;
            // runs through the candidates to print the total votes for a cluster
            for(Candidate* candidate : candidatesInElect){
                cout<<candidate->getName()<<" votes: "<<candidate->getClusterVotes()<<endl;
            }
            cout<<"\n-----------------------------------------------------------"<<endl;


            count++;
        }

        cout<<currentElectorate.getName()<<" total vote tally: "<<endl;
        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
        int winnerVoteCount = 0; // used to keep track of the current winner vote count
        Candidate* winner; // pointer to hold the winner

        // runs through each candidate
        for(Candidate* candidate : candidatesInElect){
            cout<<candidate->getName()<<" total votes: "<<candidate->getTotalVotes()<<endl;

            // if a candidates votes is more than the current winner vote count
            // it assigns their total votes to the winner vote count
            // then assigns them as the winner
            if(candidate->getTotalVotes() > winnerVoteCount){
                winnerVoteCount = candidate->getTotalVotes();
                winner = candidate;
            }
        }

        cout<<"\n"<<winner->getName()<<" has won the election in "<<currentElectorate.getName()<<" for the "
        <<winner->getRelatedParty()->getName()<<" with a total of "<<winner->getTotalVotes()<<" votes!\n"<<endl;

        // increments the electoratesWon by 1 for the party of the winner of the electorate
        winner->getRelatedParty()->updateElectoratesWon();
    }
    cout<<"~~~~~~~~~~~~~~~~~~~~~================VOTING HAS FINISHED================~~~~~~~~~~~~~~~~~~~~~\n"<<endl;



}

// calculate cluster function which determines the votes per candidate per cluster
// based on which candidate has majority closer stances to the cluster
// using the formula described below
void Election::calculateClusterVotes(vector<Candidate*>* candidatesInElec, ElectorateCluster* cluster){

    Candidate* stanceWinner; // pointer to hold winner
    RandomGenerator random;
    int totalClusterPop = cluster->getPopulation(); // gets cluster total pop

    // divides by the amount of stances
    // which will be used to calculate the total votes for the cluster per stance won
    int stanceVotes = totalClusterPop/5;
    int stanceIndex = 0;

    // for loop to run through the stances for the cluster and compare them with each candidate
    for(Stance& currentStance : cluster->getStances()){
        int previousRange = 999999; // variable to hold the "range" between candidate and electorate cluster stance

        // for loop to run through each candidate
        // collects the candidates App/Sig for the currentStance
        // uses getRange to determine the stance range of the app/Sig of the candidate and electorate cluster
        // adds both ranges together and then also uses 1/4 of the candidates popularity to reduce the range
        // if the totalRange is lower than the previous Range
        // it will assign previousRange to totalRange
        // and assign the candidate as stanceWinner
        for(Candidate* candidate : *candidatesInElec){
            int candStanceApp = candidate->getStances().at(stanceIndex).getApproach();
            int candStanceSig = candidate->getStances().at(stanceIndex).getSignificance();

            int appRange = getRange(candStanceApp, currentStance.getApproach());
            int sigRange = getRange(candStanceSig, currentStance.getSignificance());

            int totalRange = appRange + sigRange;
            totalRange -= (candidate->getCharValue(Characteristic::POPULARITY)/4);

            if(totalRange < previousRange){
                previousRange = totalRange;
                stanceWinner = candidate;
            }
        }

        stanceWinner->updateStancesWon(); // increments stancesWon by 1 for stanceWinner

        stanceIndex++; // goes to next stance
    }

    // runs through each candidate and calculates the votes for the cluster using the stanceVotes
    // as collected earlier which multiplies the stanceVotes by the amount of stances won by each candidate
    // with a standard deviation of 3
    // this provides a better chance that we dont get any exact amount of votes
    // which means that the total votes may not be equal to the population of the cluster
    // however, we can write this off as "Donkey Voters".
    for(Candidate* candidate : *candidatesInElec){
        candidate->calculateClusterVotes(random.standardDeviation(stanceVotes, 3));
    }


}

// function uses the popularity of the party leaders which impacts their candidates
// popularity before the election to make some last minute balances where possible.
// This impact will only happen to candidates where their popularity is less than their leaders.
// They will only receive 1/4 of their leaders popularity value.
void Election::calculatePopularity(){

    for(Party& currentParty : parties){

        map<string, Candidate>::iterator it;
        map<string, Candidate>& candidates = currentParty.getCandidates();
        int leaderPop = currentParty.getLeader().getCharValue(Characteristic::POPULARITY);
        for(it = candidates.begin(); it != candidates.end(); it++){
            int candidatePop = it->second.getCharValue(Characteristic::POPULARITY);
            if(candidatePop < leaderPop){
                int popModifier = leaderPop/4;
                it->second.updateChar(Characteristic::POPULARITY, popModifier);
            }
        }
    }
}


// function used to determine the winner after tallying the votes
// runs through each party, gets the electoratesWon
// if electoratesWon is higher than currentWinnerElecCount then the
// currentParty will be assigned as winner and currentWinnerElecCount will be assigned
// currentParty's electoratesWon.
// if currentParty's electoratesWon are not grater than currentWinnerElecCount
// it checks if it is higher than the runnerUpElecCount and if it is higher
// it assigns their electoratesWon as the runnerUpElecCount.
// These 2 variables are then compared to see if they are equal (to determine hung parliment)
void Election::determineWinner(){

    cout<<"The votes are in and the Election is coming to an end.\n"<<endl;
    cout<<"===================================RESULTS===================================\n"<<endl;
    Party* winner; // holds pointer to winner Party

    // below variables holds winner and runner up elec counts
    // these 2 variables are used to determine a hung parliment
    int currentWinnerElecCount = -1;
    int runnerUpElecCount = -1;
    bool hungParliment = false;

    // for loop to run through each party and check electorates won
    // and prints out how many electorates won for each party
    for(Party &currentParty : parties){
        cout<<currentParty.getName()<<" has "<<currentParty.getElectoratesWon()<<" Candidate's who have been elected in their respective electorate."<<endl;

        // if currentParty's electorates won is higher than current winnerElec count
        // assigns currentParty as winner and sets their elec count to winner elecCount
        if(currentParty.getElectoratesWon() > currentWinnerElecCount){
            currentWinnerElecCount = currentParty.getElectoratesWon();
            winner = &currentParty;
        }
        // if not higher than winner count but higher than runner up, sets their elec count as the runner up count
        else if(currentParty.getElectoratesWon() > runnerUpElecCount){
            runnerUpElecCount = currentParty.getElectoratesWon();
        }
    }

    // checks for hung parliment
    if(currentWinnerElecCount == runnerUpElecCount) hungParliment = true;

    // if a hung parliment, then prints out the result
    if(hungParliment){
        cout<<"\n\nOh no! No party has enough seats to secure parliament!"<<endl;
        cout<<"\n\nTHIS HAS RESULTED IN A HUNG PARLIAMENT, NO ONE HAS BEEN ELECTED PRIME MINISTER\n\n"<<endl;
    }
    // if not hung parliment, prints the winner/prime minister
    else{
        cout<<endl;
        cout<<winner->getName()<<" has won the election!\n"<<endl;
        cout<<winner->getLeader().getName()<<" has been elected as Prime Minister!\n\n"<<endl;
    }


}



/*  Start of Issue functions */
// constructor for Issue
Issue::Issue(string iCode, string issueStatement, IssueType typeIssue){
    statement = issueStatement;
    type = typeIssue;
    code = iCode;
}

//copy constructor for Issue
Issue::Issue(const Issue &issueCopy){
    type = issueCopy.type;
    statement = issueCopy.statement;
    code = issueCopy.code;
}

// print function for Issue
void Issue::printIssue(){
    cout<<code<<":"<<endl;
    cout<<statement<<endl;
}

// Below are getters/setters needed
string Issue::getStatement(){
    return this->statement;
}

IssueType& Issue::getIssueType(){
    return this->type;
}

string Issue::getIssueCode(){
    return this->code;
}

/*  Start of Electorate functions */
//Electorate constructor
Electorate::Electorate(vector<Issue> issues, string eName, int totalPop){
    name = eName;
    totalPopulation = totalPop;
}

// function used to add a cluster to clusters vector
void Electorate::addCluster(ElectorateCluster cluster){
    clusters.push_back(cluster);
}

// function used to print electorate information
void Electorate::printElectorate(){

    cout<<name<<" (Population: "<<totalPopulation<<")"<<endl;
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cout<<"Stances (Sig/App)                ";
    cout<<"COVID-19 Financial";
    cout<<setw(15)<<"Sauce Debate";
    cout<<setw(25)<<"Toilet Paper Shortage";
    cout<<setw(17)<<"Global Warming";
    cout<<setw(20)<<"Mandatory Vaccines";
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"------------------------------------------------------------------"<<endl;
    printClusters();

}

// function used to print all clusters for an electorate
void Electorate::printClusters(){

        int count = 1;
        for(ElectorateCluster& currentCluster : clusters){
            cout<<"Cluster #"<<count<<" ("<<currentCluster.getPopulation()<<" people)";
            currentCluster.printStances();
            count++;
        }
}

// below functions are getters/setters for Electorate
string Electorate::getName(){
    return this->name;
}

int Electorate::getPopulation(){
    return this->totalPopulation;
}

void Electorate::setPopulation(unsigned int pop){
    this->totalPopulation = pop;
}

vector<ElectorateCluster>& Electorate::getClusters(){
    return this->clusters;
}

//function used to influence each stance of an electorate
void Electorate::influenceElectorate(vector<Stance> candidateStances, bool positiveImpact){

    RandomGenerator random; // randomGenerator used to provide random stance range to update
                            // between 1-3

    // if statement runs if it is a positive impact
    // i.e. updates stances to be more aligned with the candidates.
    // runs through each cluster and each stance and compares the stance with the stances from the candidate
    // updates it accordingly.
    if(positiveImpact){
        for(ElectorateCluster &cluster : clusters){
            for(Stance &clusterStance : cluster.getStances()){
                for(Stance &candidateStance : candidateStances){
                    if(clusterStance.getIssue().getIssueType() == candidateStance.getIssue().getIssueType()){
                        if(clusterStance.getApproach() > candidateStance.getApproach()){
                            cluster.updateStance(clusterStance.getIssue(), (random.randomIntRange(1,3) * -1));
                        }
                        else cluster.updateStance(clusterStance.getIssue(), random.randomIntRange(1,3));

                    }
                }
            }
        }
    }
    // if not a positiveImpact
    else{
        for(ElectorateCluster &cluster : clusters){
            for(Stance &clusterStance : cluster.getStances()){
                for(Stance &candidateStance : candidateStances){
                    if(clusterStance.getIssue().getIssueType() == candidateStance.getIssue().getIssueType()){
                        if(clusterStance.getApproach() > candidateStance.getApproach()){
                            cluster.updateStance(clusterStance.getIssue(), random.randomIntRange(1,3));
                        }
                        else cluster.updateStance(clusterStance.getIssue(), (random.randomIntRange(1,3) * -1));

                    }
                }
            }
        }
    }

}

// function used to influence/update an individual stance of an electorate
void Electorate::influenceStance(Stance stance, bool positiveImpact){

    RandomGenerator random; // randomGenerator used to provide random stance range to update
                            // between 1-3

    // if statement runs if it is a positive impact
    // i.e. updates stance to be more aligned with the candidates.
    // runs through each cluster and compares the stance with the stance provided
    // updates it accordingly.
    if(positiveImpact){
        for(ElectorateCluster &cluster : clusters){
            for(Stance &clusterStance : cluster.getStances()){
                if(clusterStance.getIssue().getIssueCode() == stance.getIssue().getIssueCode()){
                    if(clusterStance.getApproach() > stance.getApproach()){
                        cluster.updateStance(clusterStance.getIssue(), (random.randomIntRange(1,3) * -1));
                    }
                    else cluster.updateStance(clusterStance.getIssue(), random.randomIntRange(1,3));
                }
            }
        }
    }
    // else runs if it's not a positiveImpact
    // i.e. updates stance to be less aligned with the candidates.
    else{
        for(ElectorateCluster &cluster : clusters){
            for(Stance &clusterStance : cluster.getStances()){
                if(clusterStance.getIssue().getIssueCode() == stance.getIssue().getIssueCode()){
                    if(clusterStance.getApproach() > stance.getApproach()){
                        cluster.updateStance(clusterStance.getIssue(), random.randomIntRange(1,3));
                    }
                    else cluster.updateStance(clusterStance.getIssue(), (random.randomIntRange(1,3) * -1));
                }
            }
        }
    }

}

/*  Start of ElectorateCluster functions */
//ElectorateCluster constructor
ElectorateCluster::ElectorateCluster(int pop){
    population = pop;
}

// used to add a stance to ElectorateClusters stances vector
void ElectorateCluster::addStance(Stance newStance){
    stances.push_back(newStance);
}

// used to update the stances of clusters
void ElectorateCluster::updateStance(Issue issue, int stanceApp){

    for(Stance &currentStance : stances){
        int newStanceApp = currentStance.getApproach() + stanceApp;
        if(currentStance.getIssue().getIssueType() == issue.getIssueType()){
            currentStance.setApproach(newStanceApp);
        }
    }

}

// used to print stances of clusters
void ElectorateCluster::printStances(){

    cout<<setw(16);
    for(Stance& stance : stances){
        if(stance.getIssue().getIssueType() == IssueType::ECONOMIC) cout<<setw(16);
        else if(stance.getIssue().getIssueType() == IssueType::ENVIRONMENTAL) cout<<setw(20);
        else cout<<setw(16);
        cout<<stance.getSignificance()<<"/"<<stance.getApproach();
    }
    cout<<endl;
}

// used to get population of cluster
int ElectorateCluster::getPopulation(){
    return this->population;
}

//used to get stances of cluster
vector<Stance>& ElectorateCluster::getStances(){
    return this->stances;
}
/*  End of ElectorateCluster functions */



/*  Start of RandomGenerator functions */

// static engine used by RandomGenerator functions
default_random_engine RandomGenerator::randomEngine = default_random_engine(time(0));

//function used to generate random int between range
int RandomGenerator::randomIntRange(int min, int max){
    uniform_int_distribution<int> distribution(min,max);

    return distribution(randomEngine);
}

// static array which holds 3 ints 0,1,2 which are used to randomise candidates/leaders
// and ensure we dont select duplicate candidates/leaders.
int RandomGenerator::shuffledArray[3] = {1, 2, 0};

//function used to shuffle the static array.
void RandomGenerator::shuffleArray(){
    shuffle(shuffledArray, shuffledArray + 3, randomEngine);
}

// function used for standardDeviation
int RandomGenerator::standardDeviation(int charvalue, int deviation){

    int result;
    normal_distribution<double> dist(charvalue, deviation);
    result = round(dist(randomEngine));

    return result;
}

/*  End of RandomGenerator functions */


/*  Start of Stance functions */
//Stance constructor
Stance::Stance(Issue newIssue, int sig, int app) : issue(newIssue){
    significance = sig;
    approach = app;
}

// function used to get approach
int Stance::getApproach(){
    return this->approach;
}

// function used to set approach
// ensures that it never goes under 0 or above 100
void Stance::setApproach(int newStanceApp){
    if(newStanceApp > 100) approach = 100;
    else if(newStanceApp < 0) approach = 0;
    else approach = newStanceApp;
}

// function used to get significance
int Stance::getSignificance(){
    return this->significance;
}

// function used to get Issue
Issue Stance::getIssue(){
    return this->issue;
}


/*  Start of Person functions */
// Person constructor
Person::Person(string pName){
    name = pName;
}

// Person default contructor
Person::Person(){
    name = "";
}

// function used to get value of a Characteristic of a person
unsigned int Person::getCharValue(Characteristic chr){
    return characteristics.find(chr)->second;
}

// function used to upddate a Characteristic value
void Person::updateChar(Characteristic chr, int value){

    int updatedValue = characteristics.at(chr) + value;

    // ensures characteristic doesn't go higher than 100 or lower than 0
    if(updatedValue > 100) characteristics.at(chr) = 100;
    else if(updatedValue < 0)  characteristics.at(chr) = 0;
    else characteristics.at(chr) += value;


}

// function used to print characteristics of a person
void Person::printCharacteristics(){

    map<Characteristic, unsigned int>::iterator it;

    for(it = characteristics.begin(); it != characteristics.end(); it++){
        cout<<it->first<<": "<<it->second;
        auto nextIt = it;
        if(++nextIt != characteristics.end()){
            cout<<", ";
        }
    }

}

// below are getters/setters for Person
void Person::setName(string newName){
    name = newName;
}

string Person::getName(){
    return this->name;
}

/*  Start of Candidate functions */

// Constructor specifically used to create leader candidate
// Leaders characteristics will be random but at a higher range than other candidates
Candidate::Candidate(string lName, int sRanges[5][4], vector<Issue> sIssues) : Person(lName){
    // creates random obj to return random num
    RandomGenerator random;

    // initialises votes/electorate and characteristics of leaders
    totalVotes = 0;
    electorateRepresented = "";
    characteristics.insert(std::pair<Characteristic, int>(Characteristic::POPULARITY, random.randomIntRange(25, 30)));
    characteristics.insert(std::pair<Characteristic, int>(Characteristic::CHARISMA, random.randomIntRange(25, 30)));

    int count = 0;

    // adds stances to leader
    for(Issue& copyIssue : sIssues){
        stances.push_back(Stance(Issue(copyIssue), random.randomIntRange(sRanges[count][0], sRanges[count][1]), random.randomIntRange(sRanges[count][2], sRanges[count][3])));
        count++;
    }
}

//Constructor for all other candidates
Candidate::Candidate(string electorateName, string cName, int sRanges[5][4], vector<Issue> sIssues) : Person(cName){
    RandomGenerator random;

    stancesWon = 0;
    totalVotes = 0;
    electorateRepresented = electorateName;

    characteristics.insert(std::pair<Characteristic, int>(Characteristic::POPULARITY, random.randomIntRange(10, 15)));
    characteristics.insert(std::pair<Characteristic, int>(Characteristic::CHARISMA, random.randomIntRange(10, 15)));
    characteristics.insert(std::pair<Characteristic, int>(Characteristic::DEBATING, random.randomIntRange(10, 15)));

    int count = 0;
    for(Issue& copyIssue : sIssues){
        stances.push_back(Stance(Issue(copyIssue), random.randomIntRange(sRanges[count][0], sRanges[count][1]), random.randomIntRange(sRanges[count][2], sRanges[count][3])));
        count++;
    }
}

//Candidate default constructor
Candidate::Candidate(){
    totalVotes = 0;
    stancesWon = 0;
}

//function used to print the stances of a candidate
void Candidate::printStances(){

    int width = name.length();
    cout<<setw(24-width);
    for(Stance& stance : stances){
        if(stance.getIssue().getIssueType() == IssueType::ECONOMIC) cout<<setw(24-width);
        else if(stance.getIssue().getIssueType() == IssueType::ENVIRONMENTAL) cout<<setw(20);
        else cout<<setw(16);
        cout<<stance.getSignificance()<<"/"<<stance.getApproach();
    }
    cout<<endl;
}

// function used to increment stancesWon
void Candidate::updateStancesWon(){
    stancesWon++;
}

// function used to calculate cluster votes based on stances won
// stanceVotes based on 1/4 of population of current cluster
void Candidate::calculateClusterVotes(int stanceVotes){
    clusterVotes = stanceVotes * stancesWon;

    updateTotalVotes(clusterVotes); // adds cluster votes to total votes
    stancesWon = 0; // sets stancesWon to 0 after calculations have been completed
}

// function used to update the total votes of the candidate
void Candidate::updateTotalVotes(int votes){
    totalVotes += votes;
}


// Below functions are setters/getters for candidate
int Candidate::getTotalVotes(){
    return this->totalVotes;
}

int Candidate::getClusterVotes(){
    return this->clusterVotes;
}


string Candidate::getElectorateName(){
    return this->electorateRepresented;
}

vector<Stance>& Candidate::getStances(){
    return this->stances;
}

void Candidate::setRelatedParty(Party* party){
    relatedParty = party;
}

Party* Candidate::getRelatedParty(){
    return this->relatedParty;
}

/*  End of Candidate functions */

/*  Start of Party functions */
// Party constructor
Party::Party(string pName, Candidate lCandidate, int sRanges[5][4]){
    name = pName;
    leader = lCandidate;
    electoratesWon = 0;

    for(int x = 0; x < 5; x++){
        for(int y = 0; y < 4; y++){
            stanceRanges[x][y] = sRanges[x][y];
        }
    }
}

// function used to add candidate to candidates Map
// sets the new candidates related party to the current party its being added to
void Party::loadCandidate(Candidate newCandidate){

    newCandidate.setRelatedParty(this);
    candidates.insert(pair<string, Candidate>(newCandidate.getElectorateName(), newCandidate));
}

// function used to print party information
void Party::printParty(){
    cout<<"Stances (Sig/App)   ";
    cout<<"COVID-19 Financial";
    cout<<setw(15)<<"Sauce Debate";
    cout<<setw(25)<<"Toilet Paper Shortage";
    cout<<setw(17)<<"Global Warming";
    cout<<setw(20)<<"Mandatory Vaccines";
    cout<<endl;
    cout<<"----------------------------------------------------------------------";
    cout<<"-------------------------------------------"<<endl;
    cout<<"Party(Ranges): ";
    cout<<setw(9)<<stanceRanges[0][0]<<"-"<<stanceRanges[0][1]<<"/"<<stanceRanges[0][2]<<"-"<<stanceRanges[0][3];
    cout<<setw(10)<<stanceRanges[1][0]<<"-"<<stanceRanges[1][1]<<"/"<<stanceRanges[1][2]<<"-"<<stanceRanges[1][3];
    cout<<setw(11)<<stanceRanges[2][0]<<"-"<<stanceRanges[2][1]<<"/"<<stanceRanges[2][2]<<"-"<<stanceRanges[2][3];
    cout<<setw(15)<<stanceRanges[3][0]<<"-"<<stanceRanges[3][1]<<"/"<<stanceRanges[3][2]<<"-"<<stanceRanges[3][3];
    cout<<setw(9)<<stanceRanges[4][0]<<"-"<<stanceRanges[4][1]<<"/"<<stanceRanges[4][2]<<"-"<<stanceRanges[4][3];
    cout<<endl;

    cout<<"Candidates "<<endl;
    map<string, Candidate>::iterator it;

    for(it = candidates.begin(); it != candidates.end(); it++){
        cout<<it->second.getName()<<": ";
        it->second.printStances();
    }

}

// below functions are setters/getters for Party
Candidate& Party::getLeader(){
    return this->leader;
}

string Party::getName(){
    return this->name;
}

map<string, Candidate>& Party::getCandidates(){
    return this->candidates;
}

ManagerialTeam& Party::getManagerialTeam(){
    return this->mngTeam;
}

// used to increment electoratesWon
void Party::updateElectoratesWon(){
    electoratesWon++;
}

int Party::getElectoratesWon(){
    return this->electoratesWon;
}

/*  End of Party functions */


/*  Start of ManagerialTeam functions */
// constructer for ManagerialTeam
ManagerialTeam::ManagerialTeam(string mname){

    RandomGenerator random;

    name = mname + " Managerial Team";
    characteristics.insert(std::pair<Characteristic, int>(Characteristic::EVENTHANDLE, random.randomIntRange(1,5)));
}
// default constructor for ManagerialTeam
ManagerialTeam::ManagerialTeam(){

     RandomGenerator random;

     characteristics.insert(std::pair<Characteristic, int>(Characteristic::EVENTHANDLE, random.randomIntRange(1,5)));
}

/*  End of ManagerialTeam functions */


/*  Start of Event functions */
// static int to keep track of total number of events, sets the currrent numOfEvents to eventCode identifier in constructor
int Event::numOfEvents = 0;

// constructor for Event
Event::Event(EventType event, string statement, int range, Characteristic chr){
    type = event;
    eventStatement = statement;
    impactRange = range;
    impactChar = chr;
    eventCode = numOfEvents;
    numOfEvents++;
}

// below getters/setters for Event
string Event::getStatement() {
    return this->eventStatement;
};

Characteristic Event::getImpactedChar(){
    return this->impactChar;
}

int Event::getImpactRange(){
    return this->impactRange;
}

void Event::setRelatedIssue(IssueType issue){
    relatedIssue = issue;
}

IssueType Event::getIssueType(){
    return relatedIssue;
}

/*  End of Event functions */


/*  Start of General functions */

// overloading << operator for Charactistic class objects
ostream& operator << (ostream& os, const Characteristic& obj){

    string characteristicString;
    int chrType = static_cast<underlying_type<Characteristic>::type>(obj);

    if(chrType == 0){
        characteristicString = "Popularity";
    }
    else if(chrType == 1){
        characteristicString = "Charisma";
    }
    else if(chrType == 2){
        characteristicString = "Event Handling";
    }
    else if(chrType == 3){
        characteristicString = "Debating";
    }

    os<<characteristicString;
    return os;
}

// generic function used to return the range of 2 ints.
int getRange(int one, int two){

    if(one > two) return (one-two);
    else if (two > one) return (two-one);
    else return 0;

}

/*  End of General functions */
