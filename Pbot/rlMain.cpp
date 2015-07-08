#include "rlMain.h"

rlMain::rlMain()
{
}


rlMain::~rlMain()
{
}

void rlMain::onStart()
{
	loadQtable();
	loadStates();

	// Inicializa vari�veis usadas pelo Qlearning;
	epsilon = 0.9f;
	alpha = (float)(0.9 - (currentEpisode * 0.8 / 1000));
	gamma = 0.5f;
}

UnitType rlMain::onFrame()
{
	return chooseAction();
}

void rlMain::onEnd()
{
	learn();
	saveQtable();
	saveStates();
}

UnitType rlMain::chooseAction()
{
	UnitType r; // O que ser� retornado;

	//Random Number Generators!
	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> distFloat;
	uniform_int_distribution<> distInt(0, 10);

	currentState = verifyCurrentState();
	vector<float> actionsCurrentState = Qtable[currentState];
	
	int bestAction = 0;
	int choice;
	float Qvalue = 0; //Qvalue da melhor a��o.
	
	//encontra a melhor a��o atual.
	for (int i = 0; i < 11; i++)
	{
		if (actionsCurrentState[i]>Qvalue)
		{
			bestAction = i;
			Qvalue = actionsCurrentState[i];
		}
	}

	//usa e-greedy para decidir se usa essa a��o.
	//verifica se � poss�vel executar a melhor a��o para evitar escolhas imposs�veis.
	if ((distFloat(gen) < epsilon) && (Broodwar->canMake(ActionVector[bestAction])))
	{
		r = ActionVector[bestAction];
		choice = bestAction;
	}
	else //caso decida n�o usar;
	{
		int alternative = distInt(gen);

		//Caso a alternativa seja igual a melhor a��o ou seja inv�lida, escolha outra.'
		while ((alternative == bestAction) && (!Broodwar->canMake(ActionVector[alternative])))
			alternative = distInt(gen);

		r = ActionVector[alternative];
		choice = alternative;
	}
	//Ao inv�s de aprender no momento, as escolhas s�o salvas e a aprendizagem ocorre no onEnd.
	storeLearningTuple(currentState, choice, 0, StateVector[currentState].children[choice]);
	//Atualiza o estado atual mesmo antes da a��o ser concretizada pois � esperado que o agente seja chamado nesse novo estado.
	currentState = StateVector[currentState].children[choice];
	return r;
}