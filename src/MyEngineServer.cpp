//
// Created by chenying on 24-9-20.
//
#include <iostream>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "tcp/MyTcpSocket.h"
#include "MessageManager/HandleMessage.h"
using namespace std;

void *HandleMessage(void *)
{
    HandleMessage::GetInstance()->Init();
    std::cout<<"创建消息处理线程成功"<<std::endl;
    return 0;
}

int main()
{
    //创建一个线程，用来处理接收到的数据
    pthread_t thread_HandleMessage;
    pthread_create(&thread_HandleMessage, NULL, HandleMessage, NULL);

    std::cout<<"hello world"<<std::endl;
    MyTcpSocket::GetInstance();
    std::cout<<"end"<<std::endl;
    int i = 0;
}