#pragma once

#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class Production
{
public:
	Production();
	~Production();

	//Recursos s�o alterados com a execu��o.
	void operate(Unit building);
};