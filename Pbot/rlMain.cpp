#include "rlMain.h"

rlMain::rlMain()
{
	ActionVector.resize(5);
	ActionVector[gateway] = UnitTypes::Protoss_Gateway;
	ActionVector[forge] = UnitTypes::Protoss_Forge;
	ActionVector[cyberCore] = UnitTypes::Protoss_Cybernetics_Core;
	ActionVector[stargate] = UnitTypes::Protoss_Stargate;
	ActionVector[roboFac] = UnitTypes::Protoss_Robotics_Facility;

	MaxUnitType.resize(5);
	MaxUnitType[gateway] = 8;
	MaxUnitType[forge] = 2;
	MaxUnitType[cyberCore] = 2;
	MaxUnitType[stargate] = 4;
	MaxUnitType[roboFac] = 2;
}

rlMain::~rlMain()
{
}

// Interface com a BWAPI e Economy

void rlMain::onStart()
{
	//Inicializa valores diferentes para cada partida.
	time_t t = time(0);
	srand(t);

	setStateVector();

	Qtable.resize(StateVector.size());
	for (auto& qsa : Qtable)
	{
		qsa.resize(actionAvailable);
	}
}
 
void rlMain::onEnd(bool isWinner)
{
	//Altera a recompensa do último estado para a recompensa do episódio.
	StepsTaken.back().reward = GetEpisodeReward(isWinner);
	learn();
	saveQtable();
}

// Parte principal

UnitType rlMain::chooseAction()
{
	UnitType r; // O que será retornado;

	int bestAction, alternativeAction, nextState;
	float transitionReward;

	set<int> possibleActions = returnPossibleActions(currentState);

	if (possibleActions.size() != 0)
		bestAction = returnBestAction(currentState);
	
	//só verifica o egreedy caso haja alternativas.
	if ((rand() % 100 > epsilon) && (possibleActions.size() > 1))
	{
		alternativeAction = returnAlternativeAction(currentState, bestAction, possibleActions);
		bestAction = alternativeAction;
	}

	nextState = StateVector[currentState].children[bestAction];
	transitionReward = GetTransitionReward(nextState);

	SaveLearningRecord(currentState, bestAction, nextState, transitionReward);

	QlearningUpdate(currentState, bestAction, nextState, transitionReward);

	currentState = nextState;
	r = ActionVector[bestAction];
	return r;
}

void rlMain::QlearningUpdate(int s, int a, int sprime, float r)
{
	Qtable[s][a] = ((1 - alpha)*Qtable[s][a]) + (alpha*(r + gamma*Qtable[sprime][returnBestAction(sprime)]));
}

// Escolha de ações

set<int> rlMain::returnPossibleActions(int state)
{
	set<int> r;

	if (StateVector[state].currentState.BuildingCount[gateway] < MaxUnitType[gateway])
		r.insert(gateway);

	if (StateVector[state].currentState.BuildingCount[forge] < MaxUnitType[forge])
		r.insert(forge);

	if ((StateVector[state].currentState.BuildingCount[gateway] != 0)
		&& (StateVector[state].currentState.BuildingCount[cyberCore] < MaxUnitType[cyberCore]))
	{
		r.insert(cyberCore);
	}

	if (StateVector[state].currentState.BuildingCount[cyberCore] != 0)
	{
		if (StateVector[state].currentState.BuildingCount[stargate] < MaxUnitType[stargate])
			r.insert(stargate);

		if (StateVector[state].currentState.BuildingCount[roboFac] < MaxUnitType[roboFac])
			r.insert(roboFac);
	}

	return r;
}

int rlMain::returnBestAction(int state)
{
	int r = 0;
	float qsa = 0;
	for (int i = 0; i < actionAvailable; i++)
	{
		if (qsa < Qtable[state][i])
		{
			r = i;
			qsa = Qtable[state][i];
		}
	}
	return r;
}

int rlMain::returnAlternativeAction(int state, int avoidThis, set<int> pickFromThose)
{
	int r = (rand() % actionAvailable);

	while ((r = avoidThis) || (pickFromThose.count(r) == 0))
	{
		r = (rand() % actionAvailable);
	}

	return r;
}

// Recompensa

float rlMain::GetTransitionReward(int nextState)
{
	float r = 0;
	State s = StateVector[nextState].currentState;

	if (s.BuildingCount[gateway] > s.BuildingCount[stargate] + s.BuildingCount[roboFac])
		r += 10;

	if ((s.BuildingCount[forge] * 3) > s.BuildingCount[gateway])
		r += 10;

	if ((s.BuildingCount[cyberCore] < s.BuildingCount[stargate]) && (s.BuildingCount[cyberCore] > 1))
		r += 10;

	if (s.BuildingCount[stargate] * 2 < s.BuildingCount[gateway])
		r += 10;

	if (s.BuildingCount[roboFac] * 2 < s.BuildingCount[gateway])
		r += 10;

	return r;
}

	
// A recompensa não pode ser negativa.
float rlMain::GetEpisodeReward(bool isWinner)
{
	if (!isWinner)
		return 0;

	float r = 0,
		temp = 0;
	int frames = Broodwar->getFrameCount();
	
	temp = (86400 - (float)frames / (float)86400) * 1000.0f;

	if (temp > 0)
		r = temp;

	return r;
}

// Route

void rlMain::learn()
{
	StepsTaken.shrink_to_fit();
	LearningTuple lt;
	for (int i = StepsTaken.size() - 1; i > -1; i--)
	{
		lt = StepsTaken[i];
		QlearningUpdate(lt.curState,lt.action,lt.newState,lt.reward);
	}
}

void rlMain::SaveLearningRecord(int s, int a, int sprime, float r)
{
	LearningTuple lt(s, a, sprime, r);
	StepsTaken.push_back(lt);
}

// Aux

void rlMain::setStateVector()
{
	int count = 0;
	int current = 0;
	bool keepGoing = true;

	// É um exagero mesmo!
	StateVector.resize((MaxUnitType[gateway] + 1) * (MaxUnitType[forge] + 1) * (MaxUnitType[cyberCore] + 1) * (MaxUnitType[stargate] + 1) * (MaxUnitType[roboFac] + 1));

	State s(0, 0, 0, 0, 0); // Estado inicial sem prédios.
	StateNode sn(s);
	StateVector[current] = sn;

	set<int> possibleActions;

	while (keepGoing)
	{
		possibleActions = returnPossibleActions(current);
		if (possibleActions.count(gateway) != 0)
		{
			addToStateVector(count, current, gateway);
		}
		if (possibleActions.count(forge) != 0)
		{
			addToStateVector(count, current, forge);
		}
		if (possibleActions.count(cyberCore) != 0)
		{
			addToStateVector(count, current, cyberCore);
		}
		if (possibleActions.count(stargate) != 0)
		{
			addToStateVector(count, current, stargate);
		}
		if (possibleActions.count(roboFac) != 0)
		{
			addToStateVector(count, current, roboFac);
		}
		keepGoing = (current != count);
		current++;
	}
	StateVector.shrink_to_fit();
}

void rlMain::addToStateVector(int& destino,int origem, int mudanca)
{
	State x(StateVector[origem].currentState, mudanca);
	int temp = returnIndexOfMatch(x);

	if (temp == -1){
		temp = ++destino;
		StateNode xn(x);
		StateVector[temp] = xn;
	}
	StateVector[origem].children[mudanca] = temp;
}

int rlMain::returnIndexOfMatch(State s)
{
	int r = -1;
	int current = 0;

	while ((r == -1) && (current < StateVector.size()))
	{
		if (s.compareTo(StateVector[current].currentState))
			r = current;
		else
			current++;
	}
	return r;
}

void rlMain::loadQtable()
{

}

void rlMain::saveQtable()
{

}

void rlMain::preparations()
{

}