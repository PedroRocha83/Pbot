#pragma once

#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class Production
{
public:
	Production();
	~Production();

	//Recursos são alterados com a execução.
	void operate(Unit building);
};