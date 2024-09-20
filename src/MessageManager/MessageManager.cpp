#include "MessageManager.h"

MessageManager* MessageManager::m_MessageManager = nullptr;
mutex MessageManager::m_Mutex;
mutex MessageManager::m_msgData_IN_Mutex;
mutex MessageManager::m_msgData_Out_Mutex;

MessageManager* MessageManager::GetInstance()
{
    if (m_MessageManager == nullptr)
    {
        unique_lock<mutex> lock(m_Mutex);
        if (m_MessageManager == nullptr)
        {
            m_MessageManager = new (nothrow) MessageManager();
        }
    }
    return m_MessageManager;
}

void MessageManager::deleteInstance()
{
    unique_lock<mutex> lock(m_Mutex);
    if (m_MessageManager)
    {
        delete m_MessageManager;
        m_MessageManager = nullptr;
    }
}

MessageManager::MessageManager()
{
    Init();
}

MessageManager::~MessageManager()
{
    cout << "MessageManager Destroyed..." << endl;
}

void MessageManager::Print()
{
    cout << "address:" << this << endl;
}

bool MessageManager::Init() {
    cout << "MessageManager Initiating..." << endl;
    return true;
}

/*
 * 判断数据容器中是否有数据
 */
bool MessageManager::msgData_IN_NotEmpty()
{
    m_msgData_IN_Mutex.lock();
    bool isEmpty = msgData_IN.size();
    m_msgData_IN_Mutex.unlock();
    return isEmpty;
}
bool MessageManager::msgData_Out_NotEmpty()
{
    m_msgData_Out_Mutex.lock();
    bool isEmpty = msgData_Out.size();
    m_msgData_Out_Mutex.unlock();
    cout<<"---------------------------------------------"<<isEmpty<<msgData_Out.size()<<endl;
    return isEmpty;
}

/*
 * 当服务器socket接收到数据时，推送到msgData_IN中
 */
void MessageManager::Push_msgData_IN(int clientFd,std::string msg)
{
    MessageData data;
    data.clientFd = clientFd;
    data.datas.push_back(msg);
    m_msgData_IN_Mutex.lock();
    msgData_IN.push_back(data);
    m_msgData_IN_Mutex.unlock();
}

void MessageManager::Push_msgData_Out(int clientFd,std::string msg)
{
}

//获取头部第一个数据，并将其抛弃
MessageData MessageManager::Pop_msgData_IN()
{
    m_msgData_IN_Mutex.lock();
    MessageData data = msgData_IN[0];
    msgData_IN.pop_front();
    m_msgData_IN_Mutex.unlock();
    return data;
}
MessageData MessageManager::Pop_msgData_OUT()
{
    m_msgData_Out_Mutex.lock();
    MessageData data = msgData_Out[0];
    msgData_Out.pop_front();
    m_msgData_Out_Mutex.unlock();
    cout<<"pop a data out"<<endl;
    return data;
}