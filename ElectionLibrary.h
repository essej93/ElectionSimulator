#ifndef ELECTIONLIBRARY_H_INCLUDED
#define ELECTIONLIBRARY_H_INCLUDED

//include statements for various functions
#include <map>
#include <vector>
#include <random>

// Enum classes
// Characteristics will be used by Person(Inherited by Managerial Team & Candidates)
// which will be used to identify characteristic trait values
enum class Characteristic{POPULARITY, CHARISMA, EVENTHANDLE, DEBATING};
// IssueType used by Issue and Event to identify specific issue types
enum class IssueType{ECONOMIC, SOCIAL, LOGISTICS, ENVIRONMENTAL, HEALTH};
// Event type which is used by the event class to identify the specific events
enum class EventType{DEBATE, CANDIDATE_RELATED, LEADER_RELATED, ISSUE_RELATED};

//declaring Party up here so Candidate can use it
class Party;

// Random Generator class which includes certain random functionalities
class RandomGenerator{
    private:
    static std::default_random_engine randomEngine;

    public:
    static int shuffledArray[3];
    int randomIntRange(int, int);
    int standardDeviation(int, int);
    void shuffleArray();

};

// Issue class used to hold information about the 5 different issues
// code variable is a small string which holds a small few word summary
// statement is used to hold the full statement of the issue
class Issue{
    private:
    std::string code;
    std::string statement;
    IssueType type;

    public:
    Issue(std::string, std::string, IssueType);
    Issue(const Issue & issueCopy);
    Issue();
    void setStatement(std::string);
    void setIssueType(IssueType);
    void printIssue();
    IssueType& getIssueType();
    std::string getIssueCode();
    std::string getStatement();

};

// Stance class used to hold the approach/significance of each individual issue
// the issue that the stance is held for is identified by the Issue object within Stance.
class Stance{
    private:
    int approach;
    int significance;
    Issue issue;

    public:
    Stance(Issue, int, int);
    Stance(Issue, std::string);
    void setApproach(int);
    void setSignificance(int);
    int getApproach();
    int getSignificance();
    Issue getIssue();

};

// Event class used to hold all the information of the random events that can
// occur throughout the campaign.
// eventStatement holds the full statement of the event
// type holds the type of event debat/candidate etc.
// impactRange is used as a factor when an event occurs.
// impactChar holds the characteristic that is impacted after the event
// eventCode is just a unique identifier for each event that is loaded
// IssueType is a variable only used for event type of ISSUE RELATED and
// is used to hold the type of issue that is being discussed as part of the event
class Event{
    private:
    std::string eventStatement;
    EventType type;
    int impactRange;
    Characteristic impactChar;
    IssueType relatedIssue;
    static int numOfEvents;
    int eventCode;

    public:
    Event(EventType, std::string, int, Characteristic);
    std::string getStatement();
    Characteristic getImpactedChar();
    int getImpactRange();
    void setRelatedIssue(IssueType);
    IssueType getIssueType();
};


// ElectorateCluster is used to hold the population and stances for
// a cluster in an electorate
// Each cluster has 5 stances for each individual issue with unique approaches/significane
// for each stance.
class ElectorateCluster{
    protected:
    int population;
    std::vector<Stance> stances;

    public:
    ElectorateCluster(int);
    void addStance(Stance);
    void printStances();
    void updateStance(Issue, int);
    int getPopulation();
    std::vector<Stance>& getStances();
};

// The Electorate class is used to hold information regarding individual electorates.
// Each electorate has a vector which holds ElectorateClusters
// The totalPopulation of the electorate is based off the total of all clusters.
class Electorate{
    private:
    std::string name;
    std::vector<ElectorateCluster> clusters;
    unsigned int totalPopulation;

    public:
    Electorate(std::vector<Issue>, std::string, int);
    void addCluster(ElectorateCluster);
    std::string getName();
    int getPopulation();
    void influenceElectorate(std::vector<Stance>, bool);
    void influenceStance(Stance, bool);
    void setPopulation(unsigned int);
    void printElectorate();
    void printClusters();
    std::vector<ElectorateCluster>& getClusters();

};

//Person class is a generic class that Candidate/Managerial team inherits from
//It is used to hold information that each inherited class will use
//which includes a map of characteristics to identify values of characterists.
class Person{
    protected:
    std::string name;
    std::map<Characteristic,unsigned int> characteristics;


    public:
    Person(std::string);
    Person();
    void updateChar(Characteristic, int);
    unsigned int getCharValue(Characteristic);
    void printCharacteristics();
    std::string getName();
    void setName(std::string);

};

// Candidate class which inherits from Person
// This class is used to hold information for each individual Candidate.
// Each candidate has a vector which holds their individual stances which are
// generated using the party stanceRange
// it also holds information on the stances won for their electorate, totalVotes, and cluster votes
// this information is all used in the voting system.
// This also holds a pointer to the party that their a part of.
class Candidate: public Person{
    private:
    std::vector<Stance> stances;
    std::string electorateRepresented;
    int totalVotes;
    int clusterVotes;
    int stancesWon;
    Party* relatedParty;

    public:
    Candidate(std::string, int[5][4], std::vector<Issue>);
    Candidate(std::string, std::string, int[5][4], std::vector<Issue>);
    Candidate();
    void printCandidate();
    void printStances();
    std::vector<Stance>& getStances();
    std::string getElectorateName();
    void setRelatedParty(Party*);
    Party* getRelatedParty();
    int getTotalVotes();
    int getClusterVotes();
    void updateTotalVotes(int);
    void calculateClusterVotes(int);
    void updateStancesWon();

};

// ManagerialTeam which inherits from person
//used to identify a managerial team within a party
// uses different Characteristics than regular candidates
class ManagerialTeam: public Person{

    public:
    ManagerialTeam(std::string);
    ManagerialTeam();

};

//Party class which holds all information for an individual party
// Uses a map to hold all candidates that are a part of the party which are identified by
// the electorate they are assigned to.
// has a leader variable to hold the information on the leader of the part
// has a ManagerialTeam which can be used by the leader/Candidates.
// holds information on how many electorates they've won
class Party{
    private:
    std::string name;
    Candidate leader;
    ManagerialTeam mngTeam;
    std::map<std::string, Candidate> candidates;
    int electoratesWon;

    public:
    Party(std::string, Candidate, int[5][4]);
    void loadCandidate(Candidate);
    void printParty();
    void updateElectoratesWon();
    int getElectoratesWon();
    Candidate& getLeader();
    ManagerialTeam& getManagerialTeam();
    std::string getName();
    std::map<std::string, Candidate>& getCandidates();
    int stanceRanges[5][4];


};

// Election class which holds ALL information of the election
// vectors hold all Parties, Electorates, Issues and Events
// numOfElectorates and numOfDays are collected by the main(upon execution)
// and fed into the election class to use.
// The election class is the main class of the system which is used to
// generate/load all the information needed for the program.
// It also holds the functions for processing events, reporting on the election,
// the voteTally system and closing the election as well as a handfull of
// smaller functions used for basic calculations.
class Election{
    private:
    int numOfElectorates, numOfDays;
    std::vector<Party> parties;
    std::vector<Electorate> electorates;
    std::vector<Issue> issues;
    std::vector<Event> events;

    public:
    Election(int, int);
    void generateElection();
    void generateParties();
    void generateElectorate();
    void generateCandidates();
    void generateElectorateClusters();
    void loadLeaders();
    void loadElectorates();
    void loadIssues();
    void loadEvents();
    void loadLeadersAndStanceRange();
    void calculateElectoratePop();
    void printElection();
    void runCampaign();
    void runElection();
    void finishElection();
    void reportElection();
    void tallyVotes();
    void determineWinner();
    void calculateClusterVotes(std::vector<Candidate*>*, ElectorateCluster*);
    void calculatePopularity();
    int getEvent();
    void handleEvent(int, Electorate*);
    void processEvent(int, Candidate*, Candidate*, Electorate*);

};



/* General Functions*/
// << operator overloader for Characteristic enum class
// used to allow easy printing of Characteristics.
std::ostream& operator << (std::ostream&, const Characteristic&);

// simple function to calculate and return the difference of 2 ints
// used when calculating votes and checking stance ranges.
int getRange(int,int);


#endif // ELECTIONLIBRARY_H_INCLUDED
