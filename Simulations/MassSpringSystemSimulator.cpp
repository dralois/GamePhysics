#include "MassSpringSystemSimulator.h"

#pragma region Properties

// Get verf�gbare Testcases
const char * MassSpringSystemSimulator::getTestCasesStr()
{
	return "Demo 2,Demo 3,Demo 4";
}

// Set Masse des Systems
void MassSpringSystemSimulator::setMass(float mass)
{
	m_fMass = mass;
	for (auto masspoint = m_MassPoints.begin(); masspoint != m_MassPoints.end(); masspoint++)
	{
		masspoint->Mass = m_fMass;
	}
}

// Set Federkraft der Federn
void MassSpringSystemSimulator::setStiffness(float stiffness)
{
	m_fStiffness = stiffness;
	for (auto spring = m_Springs.begin(); spring != m_Springs.end(); spring++)
	{
		spring->Stiffness = m_fStiffness;
	}
}

// Set D�mpfung
void MassSpringSystemSimulator::setDampingFactor(float damping)
{
	m_fDamping = damping;
	for (auto masspoint = m_MassPoints.begin(); masspoint != m_MassPoints.end(); masspoint++)
	{
		masspoint->Damping = m_fDamping;
	}
}

// Get Anzahl Massepunkte
int MassSpringSystemSimulator::getNumberOfMassPoints()
{
	return m_MassPoints.size();
}

// Get Anzahl Federn
int MassSpringSystemSimulator::getNumberOfSprings()
{
	return m_Springs.size();
}

// Get Position eines Massepunktes
Vec3 MassSpringSystemSimulator::getPositionOfMassPoint(int index)
{
	return m_MassPoints[index].Position;
}

// Get Geschwindigkeit eines Massepunktes
Vec3 MassSpringSystemSimulator::getVelocityOfMassPoint(int index)
{
	return m_MassPoints[index].Velocity;
}

#pragma endregion

#pragma region Events

// Speichere Klickposition
void MassSpringSystemSimulator::onClick(int x, int y)
{
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

// Speichere Mausbewegung
void MassSpringSystemSimulator::onMouse(int x, int y)
{
	m_oldtrackmouse.x = x;
	m_oldtrackmouse.y = y;
	m_trackmouse.x = x;
	m_trackmouse.y = y;
}

#pragma endregion

#pragma region Functions

#pragma region Internal

// Berechnet Federkraft
Vec3 MassSpringSystemSimulator::X_CalcSpringForce(Spring &spring, const Vec3 &point1, const Vec3 &point2)
{
	// Aktualisiere L�nge
	spring.CurrentLenght = sqrt((float)point1.squaredDistanceTo(point2));
	// F_ij = -k * (l - L) * (x_i - x_j) / l
	return spring.Stiffness * (spring.CurrentLenght - spring.InitLenght) *
		((point1 - point2) / spring.CurrentLenght);
}

// Demo Szenen Setup f�r Demo 1-3
void MassSpringSystemSimulator::X_SetupDefaultDemo()
{
	setMass(10.0f);
	setDampingFactor(0.0f);
	setStiffness(40.0f);
	m_fSphereSize = .05f;
	applyExternalForce(Vec3(0, 0, 0));
	int p0 = addMassPoint(Vec3(0.0, 0.0f, 0), Vec3(-1.0, 0.0f, 0), false);
	int p1 = addMassPoint(Vec3(0.0, 2.0f, 0), Vec3(1.0, 0.0f, 0), false);
	addSpring(p0, p1, 1.0);
}

#pragma endregion

// TODO
// Initialisiere HUD je nach Demo
void MassSpringSystemSimulator::initUI(DrawingUtilitiesClass * DUC)
{
	this->DUC = DUC;
	switch (m_iTestCase)
	{
	case 0:
	case 1:
	case 2:
		TwAddVarRW(DUC->g_pTweakBar, "Sphere Size", TW_TYPE_FLOAT, &m_fSphereSize, "min=0.01 step=0.01");
		break;
	default:
		break;
	}
}

// Setzte Simulation zur�ck
void MassSpringSystemSimulator::reset()
{
	m_oldtrackmouse.x = m_oldtrackmouse.y = 0;
	m_trackmouse.x = m_trackmouse.y = 0;
	m_fDamping = m_fMass = m_fSphereSize = m_fStiffness = 0;
	m_externalForce = Vec3(0.0f);
	m_MassPoints.clear();
	m_Springs.clear();
}

// Rendere Simulation
void MassSpringSystemSimulator::drawFrame(ID3D11DeviceContext * pd3dImmediateContext)
{
	switch (m_iTestCase)
	{
	case 0:
	case 1:
	case 2:
		DUC->setUpLighting(Vec3(), 0.4*Vec3(1, 1, 1), 100, 0.6*Vec3(0.97, 0.86, 1));
		// Zeichne Lines f�r Federn
		for (auto spring = m_Springs.begin(); spring != m_Springs.end(); spring++)
		{
			DUC->beginLine();
			// Bestimme Differenz (Max. 1.0)
			float diff = min(abs((spring->CurrentLenght - spring->InitLenght) / spring->InitLenght), 1.0f);
			// Linie wird roter je nach dem wie hoch die Abweichung von der InitLength ist
			DUC->drawLine(
				spring->Point1.Position.toDirectXVector(),
				Vec3(diff, 1 - diff, 0),
				spring->Point2.Position.toDirectXVector(),
				Vec3(diff, 1 - diff, 0));
			DUC->endLine();
		}
		// Zeichne Kugeln f�r Massepunkte
		for (auto masspoint = m_MassPoints.begin(); masspoint != m_MassPoints.end(); masspoint++)
		{
			DUC->drawSphere(masspoint->Position.toDirectXVector(), Vec3(m_fSphereSize));
		}
		break;
	default:
		break;
	}
}

// TODO
// Aktiviere verschiedene Demos
void MassSpringSystemSimulator::notifyCaseChanged(int testCase)
{
	m_iTestCase = testCase;
	// Setzte Simulation zur�ck
	reset();
	// Je nach Case
	switch (m_iTestCase)
	{
	case 0:
	{
		cout << "Demo 2!" << endl;
		setIntegrator(EULER);
		X_SetupDefaultDemo();
		break;
	}
	case 1:
	{
		cout << "Demo 3!" << endl;
		setIntegrator(MIDPOINT);
		X_SetupDefaultDemo();
		break;
	}
	case 2:
	{
		cout << "Demo 4!" << endl;
		break;
	}
	default:
		cout << "Empty Demo!" << endl;
		break;
	}
}

// TODO
// Gravitation etc. berechnen
void MassSpringSystemSimulator::externalForcesCalculations(float timeElapsed)
{

}

// TODO: Midpoint, Damping, External forces, ggf. Leapfrog
// Simuliere einen Schritt
void MassSpringSystemSimulator::simulateTimestep(float timeStep)
{
	// Je nach Integrationsverfahren
	switch (m_iIntegrator)
	{
	case EULER:
	{
		// Aktualisiere L�ngen und Kr�fte
		for (auto spring = m_Springs.begin(); spring != m_Springs.end(); spring++)
		{
			// Akkumuliere Federkraft
			spring->Point1.Force += X_CalcSpringForce(*spring, spring->Point1.Position, spring->Point2.Position);
			// Analog f�r Punkt 2
			spring->Point2.Force += -1.0f * spring->Point1.Force;
		}
		// Aktualisiere Geschwindigkeit/Position
		for (auto masspoint = m_MassPoints.begin(); masspoint != m_MassPoints.end(); masspoint++)
		{
			// Nur f�r bewegliche Punkte
			if (!masspoint->Fixed)
			{
				// Aktualisiere zuerst Position
				masspoint->Position += timeStep * masspoint->Velocity;
				// Dann Geschwindigkeit
				masspoint->Velocity = masspoint->Velocity + (((-1.0f * masspoint->Force) / masspoint->Mass) * timeStep);
			}
			// Kraft zur�cksetzen
			masspoint->Force = Vec3(0.0f);
		}
		break;
	}
	case MIDPOINT:
	{
		break;
	}
	case LEAPFROG:
	{
		break;
	}
	default:
		break;
	}
}

// F�gt Massepunkt hinzu, gibt dessen Position im Array zur�ck
int MassSpringSystemSimulator::addMassPoint(Vec3 position, Vec3 Velocity, bool isFixed)
{
	// Erstelle neuen Massepunkt
	Masspoint newMass;
	newMass.Force = m_externalForce;
	newMass.Damping = m_fDamping;
	newMass.Position = position;
	newMass.Velocity = Velocity;
	newMass.Fixed = isFixed;
	newMass.Mass = m_fMass;
	// Speichere im Array und gebe Position zur�ck
	m_MassPoints.push_back(newMass);
	return m_MassPoints.size() - 1;
}

// F�gt neue Feder zw. Massepunkten hinzu
void MassSpringSystemSimulator::addSpring(int masspoint1, int masspoint2, float initialLength)
{
	// Erstelle Feder
	Spring newSpring(m_MassPoints[masspoint1], m_MassPoints[masspoint2]);
	newSpring.InitLenght = newSpring.CurrentLenght = initialLength;
	newSpring.Stiffness = m_fStiffness;
	// Speichere im Array
	m_Springs.push_back(newSpring);
}

// TODO
// F�ge Kraft hinzu (bswp. durch Mausinteraktion)
void MassSpringSystemSimulator::applyExternalForce(Vec3 force)
{

}

#pragma endregion

#pragma region Initialisation

MassSpringSystemSimulator::MassSpringSystemSimulator() :
	m_externalForce(Vec3(0.0f)),
	m_fSphereSize(0.0f),
	m_fStiffness(0.0f),
	m_oldtrackmouse{},
	m_fDamping(0.0f),
	m_iIntegrator(0),
	m_trackmouse{},
	m_MassPoints{},
	m_fMass(0.0f),
	m_Springs{}
{
	m_iTestCase = 0;
}

#pragma endregion
