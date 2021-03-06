#define _CRT_SECURE_NO_WARNINGS

#define STRING_SIZE 18
#define MAX_PROCESS_QTY 33
#define CREATE_NEW_PROCESS '+'
#define DESTROY_LAST_ADDED_PROCESS '-' 
#define QUIT 'q'

#include <windows.h>
#include <conio.h>
#include <ctime>
#include <iostream>
#include <vector>

using namespace std;

const char* alphabetChars[] = { 
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
	"2", "3", "3", "4", "5", "6", "7", "8"
}; // массив из которого формируется уникальная строка для кааждого нового порожденного процесса

//--------------------------FUNCTIONS--------------------------------------------
string getUniqString();
PROCESS_INFORMATION createNewProcess(string appName, string commandLine);
void printUniqString(char* uniqString);
void processManager(string appName);
char* stringToChars(string str);
//-------------------------------------------------------------------------------


int main(int argc, char* argv[]) {
	setlocale(0, "RUS");
	if (argc == 2) {
		printUniqString(argv[1]);
	} else {
		processManager(argv[0]);
	}
	return 1;
}
         
string getUniqString() { // формирование уникальной строки для каждого процесса
	string uniqString = "";

	srand(time(nullptr));
	for (size_t i = 0; i < STRING_SIZE; i++) {
		uniqString += alphabetChars[rand() % (sizeof(alphabetChars) / sizeof(char*))];
	}

	return uniqString;
}
           
PROCESS_INFORMATION createNewProcess(string appName, string commandLine) { //функция для создания процесса, принимающая название запускаемого приложения и уникальную строку, как некий идентификаатор нового процесса
	STARTUPINFO startupinfo;
	ZeroMemory(&startupinfo, sizeof(startupinfo));
	PROCESS_INFORMATION processInformation;
	ZeroMemory(&processInformation, sizeof(processInformation));
	string str = appName + " " + commandLine;
	CreateProcess(nullptr, stringToChars(str), nullptr, nullptr, TRUE,
		NULL, nullptr, nullptr, &startupinfo, &processInformation);
	return processInformation;
}

void printUniqString(char* uniqString) {  // функция, печатающая уникальную строку данного процесса
	HANDLE canWriteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "canWrite");  // открытие доступа
	HANDLE cannotWriteEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, uniqString);//                 к событиям (одно общее для всех процессов, другое уникальное для данного процесса)
	while (true) {
		if (WaitForSingleObject(canWriteEvent, INFINITE) == WAIT_OBJECT_0) {  // ожидает установки события(canWrite) в сигнальное состояние
			if (WaitForSingleObject(cannotWriteEvent, 50) == WAIT_OBJECT_0) { // проверяет состояние события (cannotWrite), уникального для данного процесса
				SetEvent(canWriteEvent); // ксли всё же этот процесс не может печатать, то мы снова устанавливаем общее событие (canWrite) в сигнальное состояние и печает другой процесс
				return;
			}

			for (size_t i = 0; i < STRING_SIZE; i++) { // посимвольный вывод уникальной строки
				cout << uniqString[i];
				Sleep(100);
			}

			cout << endl;

			SetEvent(canWriteEvent); // после напечатанной строки установка общего события в сигнальное состояние - следующий процесс может печатать
		}
	}
	return;
}

void processManager(string appName) { // обработка нажатий пользователя
	int activeProcessQty = 0;
	char userChoice = NULL;

	HANDLE canWriteEvent = CreateEvent(nullptr, FALSE, TRUE, "canWrite"); // создание общего для всех процессов события

	PROCESS_INFORMATION processInformationArray[MAX_PROCESS_QTY]; // массив  id каждого процесса
	HANDLE cannotWriteEventArray[MAX_PROCESS_QTY]; // массив событий для каждого процесса

	while (userChoice = _getch()) {
		if (tolower(userChoice) == CREATE_NEW_PROCESS && activeProcessQty <= MAX_PROCESS_QTY) { // если нажимаем "+" происходит создание нового процесса
			char* uniqString = stringToChars(getUniqString());
			cannotWriteEventArray[activeProcessQty] = CreateEvent(nullptr, FALSE, FALSE, uniqString); // создание уникального события "cannotWrite"
			processInformationArray[activeProcessQty] = createNewProcess(appName, uniqString); //создание процесса
			activeProcessQty++; // наращивание количества процессов, которые могут писать
		} else if (userChoice == DESTROY_LAST_ADDED_PROCESS) { // обработка нажатия на "-"
			if (--activeProcessQty >= 0) {				
				WaitForSingleObject(canWriteEvent, INFINITE); // ждем, пока процесс допишет свою строчку
				SetEvent(cannotWriteEventArray[activeProcessQty]); // установка для него в сигнальное состояние уникального события (cannotWrite), чтобы он не начал писать свою строку, запрещаем ему
				TerminateProcess((processInformationArray[activeProcessQty]).hProcess, 9); // удаляем процесс
				SetEvent(canWriteEvent); // устанавлием общее событие в сигнальное состояние - теперь вследующий процесс пишет свою строку
			}
		} else if (tolower(userChoice) == QUIT) { // выход
			break;
		}
	}
	
	while (activeProcessQty >= 0) { // удаление процессов 
		WaitForSingleObject(canWriteEvent, INFINITE);
		SetEvent(cannotWriteEventArray[activeProcessQty]);
		TerminateProcess((processInformationArray[activeProcessQty]).hProcess, 9);
		activeProcessQty--;
		SetEvent(canWriteEvent);
	}
	
	system("pause");
}
                                                    // функция перевода типа string в char
char* stringToChars(string str) {
	size_t strSize = str.length();
	char* chars = (char*)malloc((strSize) * sizeof(char));
	for (size_t i = 0; i < strSize; i++) {
		chars[i] = str[i];
	}
	return chars;
}
