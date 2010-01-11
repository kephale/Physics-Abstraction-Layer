#include "palSolver.h"
/*
	Abstract:
		PAL - Physics Abstraction Layer. 
		Implementation File (solver)

	Author: 
		Adrian Boeing
	Revision History:
		Version 0.1   : 05/07/08 - Original
	TODO:
*/

palSolver::palSolver()
: m_fSolverAccuracy(10.0f)
{
}

float palSolver::GetSolverAccuracy() {
	return m_fSolverAccuracy;
}

void palSolver::SetSolverAccuracy(Float fAccuracy) {
	if (fAccuracy >= 0.0f) {
		m_fSolverAccuracy = fAccuracy;
	} else {
		m_fSolverAccuracy = 0.0f;
	}
}
