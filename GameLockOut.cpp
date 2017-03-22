#include <iostream>
#include <fstream>
#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <vector>
#include <codecvt>

bool CheckFileAlreadyExist(std::wifstream& in);
void CreateDefaultFile(std::wifstream& in);
void AddFileToVector(std::vector<std::wstring>& exeVector, std::wifstream& in);
bool CheckProcess(std::vector<std::wstring>& exeVector);
bool CompareProcessName(PROCESSENTRY32& pe32, std::vector<std::wstring>& exeVector);
bool CheckFirstProcess(HANDLE hProcessSnap, PROCESSENTRY32& pe32);
bool ExitProcess(PROCESSENTRY32& pe32);

int main() {
	std::wifstream in;
	std::vector<std::wstring> exeVector;
	CheckFileAlreadyExist(in);	//	checks if there is an existing list or else it makes one
	AddFileToVector(exeVector, in);
	CheckProcess(exeVector);	//	compare process to every vector content
	return 0;
}

//	check if file exists
//	if not then make one
bool CheckFileAlreadyExist(std::wifstream& in) {
	in.open("exelist.txt");
	if (in) {
		in.close();
		return true;	//	already have a file existing
	}
	else {
		CreateDefaultFile(in);
		return false;	//	a default one was created
	}
}

void CreateDefaultFile(std::wifstream& in) {
	std::wofstream out;
	out.open("exelist.txt", std::ofstream::out);
	//	default exe's
	out << "osu!.exe" << std::endl;
	out << "Steam.exe" << std::endl;
	out.close();
}

void AddFileToVector(std::vector<std::wstring>& exeVector, std::wifstream& in) {
	in.open("exelist.txt");
	while (!in.eof()) {
		std::wstring s;
		std::getline(in, s);
		exeVector.push_back(s);
	}
	in.close();
}
bool CheckProcess(std::vector<std::wstring>& exeVector) {
	HANDLE hProcessSnap;	// takes snapshot of all processes
	PROCESSENTRY32 pe32;	// will be used to go through the processes one by one from hProcessSnap

	pe32.dwSize = sizeof(PROCESSENTRY32);	// initialize its size

	bool detectedProgram = false;

	while (true) {
		Sleep(10000);	// prevent from running so many un-needed times
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);	// take snapshot of all current processes
		if (!CheckFirstProcess(hProcessSnap, pe32))	//	if a first process isn't found, then go back up and start another snapshot
			continue;
		//	loop through entire snapshot and check the 
		do {
			// compare pe32.szExeFile to a given exe name	
			//	if the comparison is a match then print out it name and terminate the process
			if (CompareProcessName(pe32, exeVector))
			{
				_tprintf(TEXT("\nPROCESS NAME:  %s"), pe32.szExeFile);
				ExitProcess(pe32);	// Call TerminateProcess 
			}
		} while (Process32Next(hProcessSnap, &pe32));

	}
	return detectedProgram;
}

bool ExitProcess(PROCESSENTRY32& pe32) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);	//	HANDLE to be used to call TerminateProcess
	//	attempt to close process, return true if it worked
	if (!TerminateProcess(hProcess, 0)) {
		_tprintf(TEXT("\nFAILEDTOTERMINATE:  %s"), pe32.szExeFile);
		return false;
	}
	_tprintf(TEXT("\nTERMINATED:  %s"), pe32.szExeFile);
	return true;
}


bool CheckFirstProcess(HANDLE hProcessSnap, PROCESSENTRY32& pe32) {
	if (!Process32First(hProcessSnap, &pe32)) {
		_tprintf(TEXT("No Processes Found!"));
		return false;
	}
	return true;
}

bool CompareProcessName(PROCESSENTRY32& pe32, std::vector<std::wstring>& exeVector) {
	//	traverse through vector comparing contents to pe32.szExeFile
	for (std::vector<std::wstring>::iterator itr = exeVector.begin();
		itr != exeVector.end(); ++itr) {
		if ((*itr).find(pe32.szExeFile) != std::wstring::npos) {
			return true;
		}
	}
	return false;
}