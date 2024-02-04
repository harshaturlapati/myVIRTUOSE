#include	<stdio.h>
#include	"VirtuoseAPI.h"
//#include	<unistd.h>   
#include <windows.h>
#include <iostream>



int main()
{
	//printf("hello world\n");
	VirtContext VC;
	VC = virtOpen("127.0.0.1#53210");
	if (VC == NULL)
	{
		fprintf(stderr, "Erreur dans virtOpen: %s\n", virtGetErrorMessage(virtGetErrorCode(NULL)));
		return -1;
	}
	printf("virtOpen worked\n");

	float identity[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
	virtSetIndexingMode(VC, INDEXING_ALL);
	virtSetForceFactor(VC, 1.0f);
	virtSetSpeedFactor(VC, 1.0f);
	virtSetTimeStep(VC, 0.003f);
	virtSetBaseFrame(VC, identity);
	virtSetObservationFrame(VC, identity);
	virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE);
	virtSetPowerOn(VC, 1);

	int j = 0;

	float des_X[7];
	float K[3];
	K[0] = 100;
	K[1] = 100;
	K[2] = 100;

	// Define a goal
	while (j < 100)
	{
		float myPOS[7];
		virtGetPosition(VC, myPOS);
		for (int i = 0; i <= 6; i++) {
			des_X[i] = myPOS[i];
		}
		Sleep(1); // Very important or the virtGetPosition does not seem to work.
		j = j + 1;
	}

	// Impedance control
	int i = 0;
	while (i < 2000)
	{
		float position[7], speed[6], force[6];

		virtGetPosition(VC, position); //std::cout << "q1 = " << position[0] << "q2 = " << position[1] << "q3 = " << position[2] << "q4 = " << position[3] << "q5 = " << position[4] << "q6 = " << position[5] <<  "q7 = " << position[6] << std::endl;

		for (int idx = 0; idx <= 2; idx++) {
			force[idx] = K[idx] * (des_X[idx] - position[idx]);
		}

		force[3] = 0;
		force[4] = 0;
		force[5] = 0;

		//std::cout << "goal1 = " << des_X[0] << "goal2 = " << des_X[1] << "goal3 = " << des_X[2] << std::endl; //std::cout << "f1 = " << force[0] << "f2 = " << force[1] << "f3 = " << force[2] << "f4 = " << force[3] << "f5 = " << force[4] << "f6 = " << force[5] << std::endl;

		//virtSetForce(VC, force);
		Sleep(1);
		i = i + 1;
	}
	virtSetPowerOn(VC, 0);
	virtClose(VC);

	return 0;
}