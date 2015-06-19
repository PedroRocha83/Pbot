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
}

void rlMain::onEnd()
{
	learn(StepsTaken);
	saveQtable();
	saveStates();
}