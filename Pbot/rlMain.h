#pragma once

#include <BWAPI.h>
#include <vector>
#include <random>
#include <ctime>
#include <set>

#define actionAvailable 5

using namespace std;
using namespace BWAPI;

enum ACTIONS { gateway, forge, cyberCore, stargate, roboFac };

struct State{
	vector<int> BuildingCount;
	State()
	{
		BuildingCount.resize(5);

		BuildingCount[gateway] = 0;
		BuildingCount[forge] = 0;
		BuildingCount[cyberCore] = 0;
		BuildingCount[stargate] = 0;
		BuildingCount[roboFac] = 0;
	}
	State(State s, int i)
	{		
		BuildingCount = s.BuildingCount;
		BuildingCount[i]++;
	}
	State(int g, int f, int cy, int s, int r)
	{
		BuildingCount.resize(5);

		BuildingCount[gateway] = g;
		BuildingCount[forge] = f;
		BuildingCount[cyberCore] = cy;
		BuildingCount[stargate] = s;
		BuildingCount[roboFac] = r;
	}

	bool compareTo(State s){
		bool r = true;
		for (int i = 0; i < actionAvailable; i++)
		{
			if (BuildingCount[i] != s.BuildingCount[i])
			{
				r = false;
				break;
			};
		};
		return r;
	};
};

struct StateNode
{
	State currentState;
	vector<int> children;

	StateNode(){}
	StateNode(State s)
	{
		currentState = s;
		
		children.resize(5);
		children[gateway] = -1;
		children[forge] = -1;
		children[cyberCore] = -1;
		children[stargate] = -1;
		children[roboFac] = -1;
	}
};

struct LearningTuple
{
	int curState, action, newState;
	float reward;
	LearningTuple()
	{
		curState = 0;
		action = 0;
		newState = 0;
		reward = 0.0f;
	}
	LearningTuple(int a, int b, int c, float d)
	{
		curState = a;
		action = b;
		newState = c;
		reward = d;
	}
};

class rlMain
{
public:

	rlMain();
	~rlMain();

	void onStart();	
	
	//Chooses an action in the current state.
	UnitType chooseAction();

	void onEnd(bool isWinner);

	bool shouldNotChooseAction(UnitType ut);

private:

	int currentState = 0,
		totalEpisodes = 250,
		currentEpisode = 0; // Salvo na tabela Q.

	vector<int> MaxUnitType;
	vector<UnitType> ActionVector;
	vector<StateNode> StateVector;
	vector<vector<float>> Qtable;
	vector<LearningTuple> StepsTaken;
	
	float epsilon = 90.0f; //e-greedy valor entre 0 e 100.
	// alpha e gamma são alterados pelo metodo preparations...
	float alpha = 0.2f;   //learning rate
	float gamma = 0.9f;   //discount factor

	float GetTransitionReward(int nextState);
	float GetEpisodeReward(bool isWinner);
		
	//	Le o arquivo de configuração e carrega a variante a ser testada.
	void preparations();


	set<int> returnPossibleActions(int state);
	int returnBestAction(int state);
	int returnAlternativeAction(int state, int avoidThis, set<int> pickFromThose);

	void QlearningUpdate(int s, int a, int sprime, float r);
	void SaveLearningRecord(int s, int a, int sprime, float r);
	
	//Learns from stored tuples.
	void learn();

	void saveQtable();
	void loadQtable();
	
	void setStateVector();
	void addToStateVector(int& destino, int origem, int mudanca);
	int returnIndexOfMatch(State s);
};