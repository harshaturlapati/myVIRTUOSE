#include <myINCLUDES.h>         // baked in
#include "VirtuoseAPI.h"
#include <myVIRTUOSE.h>         // baked in
#include <myVIRTUOSE_UDP.h>     // baked in
#include <myVIRTUOSE_LOGGING.h> // baked in

int main() {

    /// <summary>
    /// MyVIRTUOSE methods          : connect, quick_start, set_dt, debug_getPOS, quick_stop, testrun
    /// MyVIRTUOSE constructor1     : MyVIRTUOSE(const char* port_input, float forcefactor_input, float speedfactor_input, float dt_input)
    /// MyVIRTUOSE constructor2     : MyVIRTUOSE(const char* port_input,)
    /// ARM methods                 : 
    /// CMD methods                 : 
    /// </summary>
    /// <returns></returns>

    // UDP class object
    myVIRTUOSE_UDP myUDP(27017, 27018, "127.0.0.1", "127.0.0.1");
    float input_pos[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };
    myUDP.setup_UDP();
    
    // Virtuose object attributes
    const char* myPORT = "127.0.0.1#53210";
    float myFORCEFACTOR = 1.0f, mySPEEDFACTOR = 1.0f, myDT = 0.001f;

    // Virtuose object definition
    ARM RightARM("127.0.0.1#53210", myFORCEFACTOR, mySPEEDFACTOR, myDT);
    RightARM.name = "RightARM";
    RightARM.quick_start();
    RightARM.debug_getPOS();

    // Impedance control parameters
    float k = 0.1f, b = 0.1f;
    
    // Command structure object
    CMD cmd_R(k, b);

    for (int i = 0; i < 7; i++) {
        cmd_R.X_d[i] = RightARM.getPOS()[i];
    }

    int duration = 1000;
    myVIRTUOSE_LOG RightARM_LOG(duration); // duration input

    myWRITE_VIRT_LOG Right_LOG_writer(duration);

    int data_count = 0;
    printf("Press Q to exit loop\n");
    while (!(GetKeyState('Q') & 0x8000))
    {
        myUDP.UDP_send_recv_v3(RightARM.getPOS());

        cmd_R.P_trn(cmd_R.X_d, RightARM.X);
        
        RightARM_LOG.write2LOG(data_count, cmd_R.X, cmd_R.f, myUDP.UDP_f);
        
        // RightARM.sendCMD_f(cmd_R.f); // issues the commanded force and resets the force variable also
        
        data_count = data_count + 1;
    }

    Right_LOG_writer.write2FILE(RightARM_LOG);

    myUDP.cleanup();
    RightARM.quick_stop();
  

    return 0;
}

//////////// debug statements
// myObj.debug_getPOS(); // you can observe that the first few iterations, the API does NOT pick up the right positions from the 