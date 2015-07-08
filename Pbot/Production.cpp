#include "Production.h"


Production::Production()
{
}


Production::~Production()
{
}

void Production::onFrame()
{
	resources = checkAvailableResources();
	UnitType ut;
	vector<int> requiredResources;
	for (auto &predio : buildings)
	{
		ut = makeBestChoice(predio);
		requiredResources = cost(ut);
		resources = deductCost(resources, requiredResources);
		predio->train(ut);
	}
}

vector<int> Production::cost(UnitType ut)
{
	vector<int> r;
	r[0] = ut.mineralPrice();
	r[1] = ut.gasPrice();
	r[2] = ut.supplyRequired();
	return r;
}
vector<int> Production::deductCost(vector<int> a, vector<int> b)
{
	vector<int> r;
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
	return r;
}
