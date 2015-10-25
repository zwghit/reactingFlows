#include <cstdlib>
#include <fstream>

#include "src/NewthonMethod.hpp"
#include "src/GasDetonationSystem.hpp"
#include "src/SystemToPlotHugoniotIsentropic.hpp"
#include "src/SystemToPlotShockIsentropic.hpp"

using namespace std;

/*
 * The first home task of the P.Utkin course
 * "Numerical simulation of reacting flows"
 * All values in SI system
 */

/**
 * Write file for plot of Hugoniot isentropic
 * @param eta0 eta to plot around
 */
void printFileForIsentropicsPlot(const double& eta0) {
	
	std::fstream file("HugoniotIsentropic.txt", std::ios::out);
	
	std::cout << "\n\nPrinting file for Hugoniot plot...\n\n";
	for(double eta = 0.15; eta < 20; eta += 0.01) {
		file << eta << "\t";
		
		SystemToPlotHugoniotIsentropic hugoniot(eta);
		SystemToPlotShockIsentropic shock(eta);
		NewthonMethod method;
		double* solution = new double[hugoniot.getSize()];
		NewthonMethod newthonMethod;
		newthonMethod.solve(hugoniot, solution);
		const double p = solution[0] * p_dimensionless;
		file << p << "\t";
		newthonMethod.solve(shock, solution);
		const double p2 = solution[0] * p_dimensionless;
		file << p2 << "\t" << solution[1] << "\t" << solution[2]*T_dimensionless << "\n";

		delete [] solution;
	}
	
	file.close();
}



int main(int argc, char** argv) {
	
	GasDetonationSystem gasDetonationSystem;
	double* solution = new double[gasDetonationSystem.getSize()];
	NewthonMethod newthonMethod;
	newthonMethod.logging = true;
	newthonMethod.solve(gasDetonationSystem, solution);
	
	gasDetonationSystem.printCompleteSolution(solution);
	const double eta = solution[1];
	
	delete [] solution;
	
#if PLOT_ISENTROPICS
	printFileForIsentropicsPlot(eta);
#endif
	
	
	return 0;
}

