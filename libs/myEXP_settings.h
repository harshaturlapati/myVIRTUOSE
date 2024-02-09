
// STATEMENTS to include so you get exp_folder from exp_settings.txt
std::string exp_folder;
char EXEPATH[MAX_PATH];

// Windows functions to get executable paths
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


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
// STATEMENTS to include so you get exp_folder from exp_settings.txt