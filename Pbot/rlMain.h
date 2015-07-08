#pragma once

#include <BWAPI.h>
#include <vector>
#include <random>

const int SAVectorLength = 11; 

using namespace std;
using namespace BWAPI;

struct LearningTuple{
	int currentState, chosenAction, newState;
	float reward;
};

struct State{
	vector<int> BuildingCount; //SAVectorLength define o tamanho.
	bool compare(State s){
		bool r = true;
		for (int i = 0; i == SAVectorLength - 1; i++){
			if (BuildingCount[i] != s.BuildingCount[i]){
				r = false;
				break;
			};
		};
		return r;
	};
};

struct StateNode{
	State currentState;
	vector<int> parents, children; //SAVectorLength define o tamanho. 
};

class rlMain
{
public:

	rlMain();
	~rlMain();

	void onStart();
	UnitType onFrame();
	void onEnd();

private:

	int currentState;
	vector<StateNode> StateVector;
	vector<UnitType> ActionVector; //SAVectorLength define o tamanho.
	vector<vector<float>> Qtable;
	vector<LearningTuple> StepsTaken;

	int currentEpisode;

	float epsilon; //e-greedy
	float alpha;   //learning rate
	float gamma;   //discount factor

	//Chooses an action in the current state.
	UnitType chooseAction();

	void storeLearningTuple(int currentState,int action,int reward,int newState);
	//Learns from stored tuples.
	void learn();

	float reward(int s, int a);
	float finalReward();

	void saveQtable();
	void loadQtable();

	void saveStates();
	void loadStates();

	int verifyCurrentState();
};

