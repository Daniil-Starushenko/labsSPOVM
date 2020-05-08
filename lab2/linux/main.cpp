#include <ncurses.h>
#include <csignal>
#include <cstring>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include<stdio.h>
#include <cstdio>
#include <list>
#include<fstream>
#include <stdio.h>
#include <sys/select.h>
#include <termios.h>
//#include <stropts.h>
const int PROC_DELAY = 1;

using namespace std;
int main(int argc, char** argv) {
    int reciver = 0;
    char userChoise = '\0';
    pid_t pid;
    list<pid_t> pidList;
    istream &putback(char c);

    initscr();
    cbreak();
    noecho();
    //char s = 's';
    sigset_t waitSet; // настройка сигналов
    sigemptyset(&waitSet); //инициализация набора сигналов
    sigaddset(&waitSet, SIGUSR2);// добавление сигнала
    sigprocmask(SIG_BLOCK, &waitSet, NULL);
                      // настройка сигнала
    while (true) {
            userChoise = getch();
        switch (userChoise) {
            case '+': // если "+", то создаем дочерний процесс, запуская другую программу
                pid = fork();
                switch (pid) {
                    case -1:
                        cout << "Erorr while creating child process! (fork)" << endl << endl; // если не создалсчя дочерний процесс
                        exit(EXIT_FAILURE);
                    case 0:
                        execv("/Users/danik/Desktop/Spovm2/child", argv); // запуск другого приложения
                        cout << "Error while loading child process (excec)!" << endl << endl;
                        exit(127);
                    default:
                        pidList.push_back(pid); // добавляем id процесса в список
                        sleep(PROC_DELAY);
                        break;
                }
                break;

            case '-':
                if (pidList.empty()) {
                    cout << "There are no chidren to delete!" << endl; // если больше нет процессов, которые можно удалить
                } else {
                    kill(pidList.back(), SIGKILL); // посылаеме сигнал для завершения процесса последнего
                    pidList.pop_back(); // убираем id удаленного процесса
                }
                break;

            case 'q':
                if (!pidList.empty()) {
                    for (auto &childPid : pidList) { // каждому процессу посылаем сигнал о завершении процессов, идя циклом по списку id
                        kill(childPid, SIGKILL);
                    }
                    pidList.clear(); // очистка списка
                }
                return 0;
                //default:
                //continue;
            }
        
        //std::cin>>s;
            for (auto &childPid: pidList) { // по очереди посылаем сигнал дочерним процессам
            sleep(1);
            kill(childPid, SIGUSR1); // посылаем сигнал дочернему процессу с определенным id
            sigwait(&waitSet, &reciver); //ожидаем сигнала от дочернего процесса, отправляемый после напечатания строки
        
        }
    }
}
