#pragma once

#include <BWAPI.h>
#include <vector>

#include "State.h"

using namespace std;
using namespace BWAPI;

struct LearningTuple{
	int currentState, chosenAction, newState;
	float reward;
};

class rlMain
{
public:

	rlMain();
	~rlMain();

	void onStart();
	void onFrame();
	void onEnd();

private:

	int currentState;
	vector<State> StateVector;
	vector<UnitType> ActionVector;
	vector<vector<float>> Qtable;
	vector<LearningTuple> StepsTaken;

	//Chooses an action in the current state.
	UnitType chooseAction(State s);

	void storeLearningTuple(int currentState,int action,int reward,int newState);
	//Learns from stored tuples.
	void learn(vector<LearningTuple> st);

	float reward(int s, int a);
	float finalReward();

	void saveQtable();
	void loadQtable();

	void saveStates();
	void loadStates();
};

