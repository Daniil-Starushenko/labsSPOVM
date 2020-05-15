//lab5Spovm
#include <iostream>
#include <thread>
#include <functional>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <sstream>
#include <Windows.h>

using namespace std;
typedef BOOL(__cdecl Reader)(char[BUFSIZ], char[BUFSIZ]); //прототип функции чтения
typedef BOOL(__cdecl Writer)(char[BUFSIZ], char[BUFSIZ]);// прототип функций записи

mutex locker; //создаем мьютекс
condition_variable readingFinished; //условные переменные
condition_variable writingFinished; //условные переменные

int currentFile = 0;
int filesNum = 3;
char buffer[BUFSIZ]{ 0 }; //буфер с нулями

BOOL readFileAsync(HINSTANCE asyncRW, string file, char dest[BUFSIZ]) {
    Reader* reader = (Reader*)GetProcAddress(asyncRW, "asyncFileRead");
    if (reader == nullptr) {
        return FALSE;
    }
    stringstream stream;
    stream << "ref" << "\\" << file << ".txt";
    if (reader((char*)stream.str().data(), dest) == FALSE) {
        return FALSE;
    } // функция, вызывающая функцию из библиотеки для чтения из файла

    return TRUE;
}

BOOL writeFileAsync(HINSTANCE asyncRW, string file, char dest[BUFSIZ]) {
    Writer* writer = (Writer*)GetProcAddress(asyncRW, "asyncFileWrite");
    if (writer == nullptr) {
        return FALSE;
    }
    stringstream stream;
    stream << "ref" << "\\" << file << ".txt";
    if (writer((char*)stream.str().data(), dest) == FALSE) {
        return FALSE;
    }
    return TRUE;
} // функция, вызывающая функцию из библиотеки, записывающая в результирующий файл,

BOOL appendFileAsync(HINSTANCE AsyncRW, string file, char dest[BUFSIZ]) {
    char source[BUFSIZ]{ 0 };
    if (readFileAsync(AsyncRW, file, source) == FALSE) {
        return FALSE;
    } //функция, которая берет значение из результируещего файла и складывает со значением в буфере
    string result = source;
    result += dest;
    char resultBuffer[BUFSIZ]{ 0 };
    memcpy_s(resultBuffer, BUFSIZ, result.data(), result.size());

    if (writeFileAsync(AsyncRW, file, resultBuffer) == FALSE) {
        return FALSE; // записывает в результирующий
    }
    return TRUE;
}

int main() {

    HINSTANCE AsyncRW = LoadLibrary("aioLib.dll"); // загружаем библиоотеку
    if (AsyncRW == NULL) {
        cout << "Error: cannot load dynamic library" << endl;
        return 0;
    }

    thread readerThread([&]() { //создание потока
        while (true) {
            unique_lock<mutex> uniqueLocker(locker); //конструкция
            writingFinished.wait(uniqueLocker); //ждем окончания записи

            if (readFileAsync(AsyncRW, to_string(++currentFile), buffer) == FALSE) {
                continue;
            } //вызываем функцию

            thread tr([]() {
                Sleep(100);
                readingFinished.notify_one(); // посылаем сигнал, что чтение закончено
                });
            tr.detach(); // запускает поток отдельно от главного

            if (currentFile == filesNum) {
                break;
            }
        }
        });

    thread writerThread([&]() {
        while (true) {
            unique_lock<mutex> uniqueLocker(locker);
            readingFinished.wait(uniqueLocker); //ждем
            if (appendFileAsync(AsyncRW, "concatenated", (char*)buffer) == FALSE) {
                break;
            }
            memset(buffer, 0, BUFSIZ); //зануление буфера

            thread tr([]() {
                Sleep(100);
                writingFinished.notify_one();
                });
            tr.detach();

            if (currentFile == filesNum) {
                break;
            }
        }
        });

    Sleep(100);
    writingFinished.notify_one(); // установка в сигнальное состояние

    readerThread.join();
    writerThread.join();

    FreeLibrary(AsyncRW);
    return 0;
}
