#include "ClothSimulationApp.h"

int main()
{
	try
	{
		ClothSimulationApp clothSim;
		clothSim.Init();
		clothSim.Run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << "\n";
	}
}