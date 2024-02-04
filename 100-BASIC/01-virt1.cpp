#include	<stdio.h>
#include	"VirtuoseAPI.h"
//#include	<unistd.h>   
#include <windows.h>
#include <iostream>

int main()
{
	//printf("hello world\n");
	VirtContext VC;
	VC = virtOpen ("127.0.0.1#53210"); 
		if (VC == NULL)
		{
			fprintf(stderr, "Erreur dans virtOpen: %s\n", virtGetErrorMessage(virtGetErrorCode(NULL)));
			return -1;
		}
		printf("virtOpen worked\n");
	virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE); 
	virtSetPowerOn(VC, 1);
	int i = 0;
	while(i<2000)
	{
			float position[7], speed[6], force[6]; 
			//std::cout << position[0] << std::endl;
			virtGetPosition(VC, position); 
			std::cout << "q1 = " << position[0] << "q2 = " << position[1] << "q3 = " << position[2] << "q4 = " << position[3] << "q5 = " << position[4] << "q6 = " << position[5] << std::endl;
	//		virtGetSpeed(VC, speed); 
	//		//update_process(position, speed, force); 
	//		virtSetForce(VC, force);
		Sleep(1);
		i = i + 1;
	}
	virtSetPowerOn(VC, 0);
	virtClose(VC);

return 0;
}