#pragma once

#include <iostream>
#include <mutex>
#include <vector>
#include <deque>
using namespace std;

struct MessageData{
    int clientFd;
    vector<string>  datas;
};

class MessageManager
{
public:
    static MessageManager* GetInstance();

    static void deleteInstance();

    void Print();
    bool Init();
    bool msgData_IN_NotEmpty();
    bool msgData_Out_NotEmpty();
    void Push_msgData_IN(int clientFd,string msg);
    void Push_msgData_Out(int clientFd,string msg);
    MessageData Pop_msgData_IN();
    MessageData Pop_msgData_OUT();

private:
    MessageManager();
    ~MessageManager();

    MessageManager(const MessageManager& manager);
    const MessageManager& operator=(const MessageManager& manager);

private:
    static MessageManager* m_MessageManager;
    static mutex m_Mutex;

    static mutex m_msgData_IN_Mutex;
    static mutex m_msgData_Out_Mutex;
    deque<MessageData> msgData_IN;
    deque<MessageData> msgData_Out;
};

