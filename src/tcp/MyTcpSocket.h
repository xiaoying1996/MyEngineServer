#ifndef _MYTCPSOCKET_H
#define _MYTCPSOCKET_H

#include <iostream>
#include <mutex>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include "../MessageManager/MessageManager.h"

using namespace std;

class MyTcpSocket
{
public:
    static MyTcpSocket* GetInstance();

    static void deleteInstance();

    void Print();
    bool Init();
    void SocketRunning();

private:
    MyTcpSocket();
    ~MyTcpSocket();

    MyTcpSocket(const MyTcpSocket& manager);
    const MyTcpSocket& operator=(const MyTcpSocket& manager);

private:
    static MyTcpSocket* m_MyTcpSocket;
    static mutex m_Mutex;
    int lfd;
    int ret;
    int epfd;
    struct epoll_event evs[1024];
    int size;
    struct epoll_event ev;
};

#endif