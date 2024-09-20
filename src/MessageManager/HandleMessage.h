#pragma once
/*
 * 单例，作为一个消息处理线程存在
 * 在一个循环中不停遍历msgData_IN和msgData_Out中是否有对应的数据
 * 如果有数据就进行处理
 */
#include <iostream>
#include <mutex>
#include <vector>
#include <deque>
#include <unistd.h>
#include "MessageManager.h"
#include "../MyMysql/MyMysql.h"
#include "../tcp/MyTcpSocket.h"

class HandleMessage
{
public:
    static HandleMessage* GetInstance();

    static void deleteInstance();

    void Print();
    bool Init();

private:
    HandleMessage();
    ~HandleMessage();

    HandleMessage(const HandleMessage& handler);
    const HandleMessage& operator=(const HandleMessage& handler);

private:
    static HandleMessage* m_HandleMessage;
    static mutex m_Mutex;
};