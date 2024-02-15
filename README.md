# Election Simulator
## Description

I developed this at the start of my 2nd year into my degree.

The election simulator is used to simulate an election over a number of campaign days with 3 parties. Over the days it will randomly trigger events and issues which will affect the parties and their leaders providing either a negative, positive or neutral result. This then can effect the party leaders "stats" which can impact them later in the simulation or even impact the electorates standing with each party which will effect the number of votes they get in that electorate.

The election simulator reads in 3 different parties and their candidates from their respective candidates.txt files. It then loads their leaders and their "stats" from the leaders.txt. Electorates are then loaded from their respective files.

A more low level description of how the simulation is run is found in the .docx

### COMPILATION/EXECUTION INSTRUCTIONS:

Pre-requisite:
- MinGW-w64

Compilation:
- Open Command Prompt window
- Navigate to ElectionSimulator directory
- Enter the following command line to compile "g++.exe  -o bin\Debug\ElectionSimulator.exe obj\Debug\ElectionImplementation.o obj\Debug\ElectionSimulator.o"

Execution:
- Enter the command line "bin\Debug\ElectionSimulator.exe <n> <m>" (n being the number of electorates, m being the number of campaigning days)


Alternatively you could import this into VS and be able to run it through there by setting launch arguments.

