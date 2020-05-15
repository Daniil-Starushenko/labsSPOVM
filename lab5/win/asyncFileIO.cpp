//lab5Lib
#include "pch.h"
#include <utility>
#include <limits.h>
#include "asyncFileIO.h"


VOID CALLBACK FileIOCompletionRoutine( //функция обратного вызова
    __in  DWORD dwErrorCode,
    __in  DWORD dwNumberOfBytesTransfered,
    __in  LPOVERLAPPED lpOverlapped ) {}


bool asyncFileRead(char path[BUFSIZ], char buffer[BUFSIZ]) { //функция чтения, пердаем путь к файлу, и буфер, в который записываем информацию
    HANDLE fileHandler = CreateFileA(path,
        GENERIC_READ,
        0,                                                // do not share
        NULL,                                            // default security
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,    // normal file, asynchronious I/O
        NULL);                                            // no attribute template
    if (fileHandler == INVALID_HANDLE_VALUE) {
        return false;
    }

    OVERLAPPED ol{ 0 }; //инициализвация структуры

    if (FALSE == ReadFileEx(fileHandler, buffer, BUFSIZ - 1, &ol, FileIOCompletionRoutine)) {
        CloseHandle(fileHandler);
        return false;
    } // дескриптор файла, буфер, размер байт, структуру, функцию обратного вызова

    SleepEx(5000, TRUE);
    CloseHandle(fileHandler); //закрываем дескриптор
    return true;
}
//                  аналогично
bool asyncFileWrite(char path[BUFSIZ], char buffer[BUFSIZ]) {
    HANDLE fileHandler = CreateFileA(path,
        GENERIC_WRITE,
        0,                                                // do not share
        NULL,                                            // default security
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,    // normal file, asynchronious I/O
        NULL);                                            // no attribute template
    if (fileHandler == INVALID_HANDLE_VALUE) {
        return false;
    }

    SetEndOfFile(fileHandler); // указатель в конец файла

    OVERLAPPED ol{ 0 };

    if (FALSE == WriteFileEx(fileHandler, buffer, strlen(buffer), &ol, FileIOCompletionRoutine)) {
        CloseHandle(fileHandler);
        return false;
    } // записываем в файл из буфера

    SleepEx(5000, true);
    CloseHandle(fileHandler);
    return true;
}
