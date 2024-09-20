#include "HandleMessage.h"

HandleMessage* HandleMessage::m_HandleMessage = nullptr;
mutex HandleMessage::m_Mutex;

HandleMessage* HandleMessage::GetInstance()
{
    if (m_HandleMessage == nullptr)
    {
        unique_lock<mutex> lock(m_Mutex);
        if (m_HandleMessage == nullptr)
        {
            m_HandleMessage = new (nothrow) HandleMessage();
        }
    }
    return m_HandleMessage;
}

void HandleMessage::deleteInstance()
{
    unique_lock<mutex> lock(m_Mutex);
    if (m_HandleMessage)
    {
        delete m_HandleMessage;
        m_HandleMessage = nullptr;
    }
}

HandleMessage::HandleMessage()
{
    Init();
}

HandleMessage::~HandleMessage()
{
    cout << "HandleMessage Destroyed..." << endl;
}

void HandleMessage::Print()
{
    cout << "address:" << this << endl;
}

bool HandleMessage::Init() {
    cout << "HandleMessage Initiating..." << endl;
    //在这个循环中，不停遍历待处理的消息容器中是否有数据
    //如果有数据则进行相应的处理
    while(1) {
        std::cout<<"正在处理消息"<<std::endl;
        sleep(1);
    }
    return true;
}