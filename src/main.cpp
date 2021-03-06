#include <cstdlib>
#include <fstream>
#include <chrono>
#include <numeric>
#include <math.h>

#include "src/NewtonMethod.hpp"
#include "src/GasDetonationSystem.hpp"
#include "src/SystemToPlotHugoniotIsentropic.hpp"
#include "src/SystemToPlotShockIsentropic.hpp"
#include "src/ZeldovichSystem.hpp"
#include "src/ImplicitEulerMethod.hpp"

using namespace std;

/*
 * Home tasks of the P.Utkin course
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
		NewtonMethod method;
		double* solution = new double[hugoniot.getSize()];
		NewtonMethod newtonMethod;
		newtonMethod.solve(hugoniot, solution);
		const double p = solution[0] * p_dimensionless;
		file << p << "\t";
		newtonMethod.solve(shock, solution);
		const double p2 = solution[0] * p_dimensionless;
		file << p2 << "\t" << solution[1] << "\t" << solution[2]*T_dimensionless << "\n";

		delete [] solution;
	}
	
	file.close();
}

/** Task 1 - Newton method etc  */
int task1(int argc, char** argv) {
	
	GasDetonationSystem gasDetonationSystem;
	double* solution = new double[gasDetonationSystem.getSize()];
	NewtonMethod newtonMethod;
	newtonMethod.logging = true;
	newtonMethod.solve(gasDetonationSystem, solution);
	
	gasDetonationSystem.printCompleteSolution(solution);
	const double eta = solution[1];
	
	delete [] solution;
	
#if PLOT_ISENTROPICS
	printFileForIsentropicsPlot(eta);
#endif	
	return 0;
}

/** Task 2 - ODE etc */
int task2(int argc, char** argv) {
	ZeldovichSystem zeldovichSystem;
	ImplicitEulerMethod implicitEulerMethod(&zeldovichSystem, 1e-7, 1.0);
	implicitEulerMethod.calculate();
	return 0;
}

#define MEASURES_NUMBER 20
int main(int argc, char** argv) {
	try {
		std::vector<double> durations;
		for (int i = 0; i < MEASURES_NUMBER; i++) {
			std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
			task2(argc, argv);
			std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			double duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			durations.push_back(duration);
		}
		double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
		double mean = sum / durations.size();
		double sq_sum = std::inner_product(durations.begin(), durations.end(), durations.begin(), 0.0);
		double stdev = sqrt(sq_sum / durations.size() - mean * mean);
		std::cout << "Time " << mean / 1e+6 << " +/- " << stdev / 1e+6 << std::endl;
		
	} catch (const char* e) {
		std::cerr << "Exception was thrown: " << e << std::endl;
		exit(-1);
	}
	return 0;
}
