class CMD {

private:
    

public:
    float k, b;
    float K[7], B[7];

    float X_d[7];
    float X[7], Xdot[7];
    float F_e[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f};

    float dX[7];

    float f[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };

    void set_F_d(float F_e_input[6]) {
        for (int i = 0; i < 6; i++) {
            F_e[i] = F_e_input[i];
        }
    }

    void set_X_d(float X_d_input[7]) {
        for (int i = 0; i < 7; i++) {
            X_d[i] = X_d_input[i];
        }
    }

    void set_X(float X_input[7]) {
        for (int i = 0; i < 7; i++) {
            X[i] = X_input[i];
        }
    }
    void set_Xdot(float Xdot_input[7]) {
        for (int i = 0; i < 7; i++) {
            Xdot[i] = Xdot_input[i];
        }
    }
    void set_ext_F(float ext_F_input[6]) {
        for (int i = 0; i < 6; i++) {
            F_e[i] = ext_F_input[i];
        }
    }

    float* P_trn(float X_d_input[7], float X_input[7])
    {
        set_X_d(X_d_input);
        set_X(X_input);
        for (int i = 0; i < 3; i++)
        {
            dX[i] = X_d[i] - X[i];
            f[i] = K[i] * dX[i] + F_e[i];
        }
        return f;
    }

    float* PD_trn(float X_d_input[7], float X_input[7])
    {
        set_X_d(X_d_input);
        set_X(X_input);
        for (int i = 0; i < 3; i++)
        {
            dX[i] = X_d[i] - X[i];
            f[i] = K[i] * dX[i] - B[i] * Xdot[i] + F_e[i]; // While pushing the human towards the X_d, also give an opposite force along the current velocity
        }
        return f;
    }

    CMD(float k_input, float b_input) {
        k = k_input;
        b = b_input;
        for (int i = 0; i < 7; i++)
        {
            K[i] = k;
            B[i] = b;
        }
    }

    CMD(float K_input[7], float B_input[7]) {
        for (int i = 0; i < 7; i++)
        {
            K[i] = K_input[i];
            B[i] = B_input[i];
        }
    }
};

class MyVIRTUOSE {                          // The class
    private:
        
    public:                                 // Access specifier
        VirtContext VC;
        float X[7];
        float f[6] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f };
        const char* PORT;                                       // myAttributes
        float forcefactor, speedfactor, dt;                     // force factor, speed factor, and sampling rate - VERY IMPORTANT
        float identity[7] = { 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f };

        void set_dt(float dt_input) {
            dt = dt_input;
        }

        void set_forcefactor(float forcefactor_input) {
            forcefactor = forcefactor_input;
        }

        void set_speedfactor(float speedfactor_input) {
            speedfactor = speedfactor_input;
        }

        void queryPOS() {
            virtGetPosition(VC, X);
        }

        float* getPOS() {
            queryPOS();
            return X;
        }

        void set_f(float f_input[7]) {
            for (int i = 0; i < 6; i++) {
                f[i] = f_input[i];
            }
        }

        void sendCMD_f(float f_input[7]) {
            set_f(f_input);
            virtSetForce(VC, f);
            std::cout << f[0] << f[1] << f[2] << f[3] << f[4] << f[5] << std::endl;
            // reset force to 0 - after every issued command - VERY VERY IMPORTANT if UDP drops out... - discuss with dc.
            for (int i = 0; i < 6; i++) {
                f[i] = 0;
            }
        }

        void debug_getPOS() {
            printf("Virtuose API debugging initiated\n");
            for (int i = 0; i < 10; i++) {
                queryPOS();
                std::cout << "X1 = " << X[0] << "|X2 = " << X[1] << "|X3 = " << X[2] << "|X4 = " << X[3] << "|X5 = " << X[4] << "|X6 = " << X[5] << "|X7 = " << X[6] << std::endl;
            }
            printf("Debugging done, hopefully you're seeing valid data - don't proceed if you see 0's or 10e8 values.\n");
        }

        //virtSetForce(VC, force);

        void setDEFAULTPARAMS() {
            virtSetIndexingMode(VC, INDEXING_ALL);
        	    virtSetForceFactor(VC, forcefactor);
        	    virtSetSpeedFactor(VC, speedfactor);
        	    virtSetTimeStep(VC, dt);
        	    virtSetBaseFrame(VC, identity);
        	    virtSetObservationFrame(VC, identity);
        	    virtSetCommandType(VC, COMMAND_TYPE_IMPEDANCE);
        }

        void engage() {
            virtSetPowerOn(VC, 1);
        }

        void disengage() {
            virtSetPowerOn(VC, 0);
        }

        void connect() {  // Method/function defined inside the class
            std::cout << "Hello World!";
            VC = virtOpen(PORT);
        	    if (VC == NULL)
        	    {
        		    fprintf(stderr, "Erreur dans virtOpen: %s\n", virtGetErrorMessage(virtGetErrorCode(NULL)));
        	    }
            printf("virtOpen worked - nothing else on Virtuose setup has been done\n");
        }

        void disconnect(){
            virtClose(VC);
        }

        void testrun() {
            printf("Doing a test run automatically\n");
            connect();  // Connect to Virtuose
            setDEFAULTPARAMS(); // Impedance mode parameters - as prescribed by the Documentation.
            engage(); // Now the Virtuose will stream data if you hold the handle.
            disengage(); // Now the Virtuose will stop streaming data even if you hold the handle.
            disconnect();
        }

        void quick_start() {
            printf("quick start entails - (i) connect, (ii) setDEFAULTPARAMS and (iii) engage.\n");
            connect();  // Connect to Virtuose
            setDEFAULTPARAMS(); // Impedance mode parameters - as prescribed by the Documentation.
            engage(); // Now the Virtuose will stream data if you hold the handle.
        }
        void quick_stop() {
            printf("quick stop entails - (i) disengage and (ii) disconnect.\n");
            disengage(); // Now the Virtuose will stop streaming data even if you hold the handle.
            disconnect();
        }

        MyVIRTUOSE(const char* port_input, float forcefactor_input, float speedfactor_input, float dt_input) 
        {   // Constructor 1 declaration
            PORT = port_input;
            dt = dt_input;
            forcefactor = forcefactor_input;
            speedfactor = speedfactor_input;
        }

        MyVIRTUOSE(const char* port_input)
        {   // Constructor 2  declaration
            PORT = port_input;
        }
};

class ARM : public MyVIRTUOSE // single colon for inheritance
{
    using MyVIRTUOSE::MyVIRTUOSE; // "using" for inheriting the constructors written for MyVIRTUOSE
public:
    std::string name;

    // can define new methods that allow to perform debugging operations on a single arm at a higher level - while having access to the lower level methods
};
