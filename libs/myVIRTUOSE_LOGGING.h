
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

class myVIRTUOSE_LOG
{
public:
    int                     data_count      = 0;
    static const int        dim_CART        = 7;
    static const int        dim_f           = 6;
    double                  justnow;
    int64_t                 timestamp;

    int                     DURATION;
    double*                 time_log;
    int64_t*                unix_epoch;
    float**                 UDP_f_log;
    float**                 force_log;
    float**                 Virtuose_POS_log;

    int64_t GetTickUs()
    {
        #if defined(_MSC_VER)
                LARGE_INTEGER start, frequency;

                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start);

                timestamp = duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count();

                return (start.QuadPart * 1000000) / frequency.QuadPart;
        #else
                struct timespec start;
                clock_gettime(CLOCK_MONOTONIC, &start);

                return (start.tv_sec * 1000000LLU) + (start.tv_nsec / 1000);
        #endif
    }


    void write2LOG(int data_count_in, float position[7], float force[6], float UDP_f[6])
    {
        data_count = data_count_in;

        // Data logging starts

        time_log[data_count] = GetTickUs();

        unix_epoch[data_count] = timestamp; // Make sure GetTickUs() is called before timestamp is recorded into unix_epoch.
        
        // Didn't work - figure out the pointers                                   
        //std::cout << access_my_nD(UDP_f_log, 0, 0) << std::endl;

        for (int i = 0; i < dim_CART; i++)
        {
        	Virtuose_POS_log[data_count][i] = position[i];
        }
        
        for (int i = 0; i < dim_f; i++)
        {
        	force_log[data_count][i] = force[i];
            UDP_f_log[data_count][i] = UDP_f[i];
        }
    }


    // Some how the following tricks didn't work in initialising the nD arrays using functions

    void define_nD_array(float** pointer, int arr_length, int my_dim) {
        pointer = new float* [arr_length];
        for (int i = 0; i < arr_length; i++)
            pointer[i] = new float[my_dim];

    }

    float access_my_nD(float** pointer, int i, int j) {
        return pointer[i][j];
    }
    
    myVIRTUOSE_LOG(int duration_in) {
        justnow = GetTickUs();
        DURATION = duration_in;
        int arr_length = DURATION * 1000;
        time_log = new double[arr_length];
        unix_epoch = new int64_t[arr_length];

        // defining the multi dimensional arrays
        //define_nD_array(UDP_f_log,arr_length, dim_f);
        UDP_f_log = new float* [arr_length];
        for (int i = 0; i < arr_length; i++)
            UDP_f_log[i] = new float[dim_f];

        force_log = new float* [arr_length];
        for (int i = 0; i < arr_length; i++)
            force_log[i] = new float[dim_f];

        Virtuose_POS_log = new float* [arr_length];
        for (int i = 0; i < arr_length; i++)
            Virtuose_POS_log[i] = new float[dim_CART];
    }
};

class myWRITE_VIRT_LOG{

public:
    int duration;
    std::string exp_folder;
    char EXEPATH[MAX_PATH];

    std::string GetTimestamp(time_t now) {
        tm* ltm = localtime(&now);
        std::string year = std::to_string(1900 + ltm->tm_year);
        std::string month = std::to_string(1 + ltm->tm_mon);
        std::string day = std::to_string(ltm->tm_mday);
        std::string hour = std::to_string(ltm->tm_hour);
        std::string min = std::to_string(ltm->tm_min);
        std::string sec = std::to_string(ltm->tm_sec);
        std::string timestamp = year + "_" + month + "_" + day + "_" + hour + min + sec + ".csv";
        return timestamp;
    }

    TCHAR* GetEXEpath()
    {
        char buffer[MAX_PATH];
        GetModuleFileName(NULL, buffer, MAX_PATH);
        std::cout << "Executable path: " << buffer << std::endl;
        return buffer;
    }

    // Custom string functions
    void replace_all(
        std::string& s,
        std::string const& toReplace,
        std::string const& replaceWith
    ) {
        std::string buf;
        std::size_t pos = 0;
        std::size_t prevPos;

        // Reserves rough estimate of final size of string.
        buf.reserve(s.size());

        while (true) {
            prevPos = pos;
            pos = s.find(toReplace, pos);
            if (pos == std::string::npos)
                break;
            buf.append(s, prevPos, pos - prevPos);
            buf += replaceWith;
            pos += toReplace.size();
        }

        buf.append(s, prevPos, s.size() - prevPos);
        s.swap(buf);
    }

    void get_exp_folder_v2()
    {
        GetModuleFileName(NULL, EXEPATH, MAX_PATH);
        //std::cout << EXEPATH << std::endl;
        DWORD length = GetModuleFileName(NULL, EXEPATH, MAX_PATH);

        wchar_t wtext[MAX_PATH];
        mbstowcs(wtext, EXEPATH, strlen(EXEPATH) + 1);//Plus null
        LPWSTR ptr = wtext;

        std::string s1(EXEPATH);
        //cout << s1.substr(0, s1.find_last_of("\\/")) << endl;

        std::string s2 = s1.substr(0, s1.find_last_of("\\/"));

        //std::cout << s2 << std::endl;

        exp_folder = s2;

        std::cout << "Executable path: " << exp_folder << std::endl;

        replace_all(exp_folder, "\\", "/");
        std::cout << "Executable path: " << exp_folder << std::endl;

        std::string exp_settings = exp_folder;
        exp_settings.append("/exp_settings.txt");
        std::cout << "exp settings file is at: " << exp_settings << std::endl;

        std::string read_exp_line;
        std::ifstream MyReadFile(exp_settings);
        std::getline(MyReadFile, read_exp_line);
        replace_all(read_exp_line, "\\", "/");
        read_exp_line.erase(0, 1);
        exp_folder.append("/");
        exp_folder.append(read_exp_line);
        exp_folder.append("/");
        std::cout << exp_folder << std::endl;

    }

    void write2FILE(myVIRTUOSE_LOG DATA)
       {
        time_t now_t = time(0);
        std::string file_name = "virtuose_log_file_";

       std::cout << "Writing to file..." << std::endl;
       //
       get_exp_folder_v2();
       std::string base_path = exp_folder;
       //
       std::string timestamp = GetTimestamp(now_t);
       //
       std::ofstream log_file(base_path + file_name + timestamp);
       //
       	if (log_file.is_open())
       	{
       		log_file << "Time(ms),Time(s),Unix_epoch(ns),"
       			<< "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
       			<< "X,Y,Z,qx,qy,qz,qw" << ","
       			<< "f1,f2,f3,f4,f5,f6" << ","
       			<< "Index\n";
       
       		for (int i = 0; i < duration * 1000 && i < DATA.data_count - 1; ++i)
       		{
       			log_file << (DATA.time_log[i] - DATA.time_log[0]) / 1000 << ",";
       			log_file << (DATA.time_log[i] - DATA.time_log[0]) / 1000000 << ",";
       			log_file << DATA.unix_epoch[i] << ",";
       
       			// "UDP_f1,UDP_f2,UDP_f3,UDP_f4,UDP_f5,UDP_f6" << ","
       			for (int j = 0; j < DATA.dim_f; ++j)
       			{
       				log_file << DATA.UDP_f_log[i][j] << ",";
       			}
       
       			for (int j = 0; j < DATA.dim_CART; ++j)
       			{
       				log_file << DATA.Virtuose_POS_log[i][j] << ",";
       			}
       
       			for (int j = 0; j < DATA.dim_f; ++j)
       			{
       				log_file << DATA.force_log[i][j] << ",";
       			}
       
       			log_file << i << "\n";
       		}
       	}
       	std::cout << "Writing to file completed!" << std::endl;
       }

    myWRITE_VIRT_LOG(int duration_in) {
        duration = duration_in;
    }
   
    
};
