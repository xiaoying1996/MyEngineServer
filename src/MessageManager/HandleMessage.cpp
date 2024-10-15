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
        //std::cout<<"正在处理消息"<<std::endl;
        sleep(1);
        if(MessageManager::GetInstance()->msgData_IN_NotEmpty())
        {
            ProcessProtoData(MessageManager::GetInstance()->Pop_msgData_IN());
        }
    }
    return true;
}

void HandleMessage::ProcessProtoData(MessageData data)
{
    std::cout<<"use mysql start"<<endl;
    if(data.datas.size())
    {
        string str = data.datas.at(0);
        MainMessage protoMsg;
        protoMsg.ParseFromString(str);
        if(protoMsg.type() == MessageType::NAME_REPEAT_CHECK)//检查用户名是否重复
        {
            bool ret;
            Name_Repeat_Check content = protoMsg.content1();
            string name = content.name();
            CheckNameRepeat(name,ret);
            MessageData data_Ret;
            data_Ret.clientFd = data.clientFd;
            MainMessage m;
            Name_Repeat_Repost *message = new Name_Repeat_Repost();
            m.set_type(MessageType::NAME_REPEAT_REPOST);
            message->set_name(name);
            message->set_state(ret);
            m.set_allocated_content2(message);
            std::string msgStr;
            m.SerializeToString(&msgStr);
            m.clear_content2();
            m.Clear();
            data_Ret.datas.push_back(msgStr);
            MessageManager::GetInstance()->Push_msgData_Out(data_Ret);
        }
        else if(protoMsg.type() == MessageType::NUMBER_REPEAT_CHECK)//检查手机号是否重复
        {
            bool ret;
            Number_Repeat_Chexk content = protoMsg.content3();
            string number = content.number();
            CheckNumberRepeat(number,ret);
            MessageData data_Ret;
            data_Ret.clientFd = data.clientFd;
            MainMessage m;
            Number_Repeat_Repost *message = new Number_Repeat_Repost();
            m.set_type(MessageType::NUMBER_REPEAT_REPOST);
            message->set_number(number);
            message->set_state(ret);
            m.set_allocated_content4(message);
            std::string msgStr;
            m.SerializeToString(&msgStr);
            m.clear_content4();
            m.Clear();
            data_Ret.datas.push_back(msgStr);
            MessageManager::GetInstance()->Push_msgData_Out(data_Ret);
        }
        else if(protoMsg.type() == MessageType::EMAIL_REPEAT_CHECK)//检查手机号是否重复
        {
            bool ret;
            Email_Repeat_Chexk content = protoMsg.content5();
            string email = content.email();
            CheckEmailRepeat(email,ret);
            MessageData data_Ret;
            data_Ret.clientFd = data.clientFd;
            MainMessage m;
            Email_Repeat_Repost *message = new Email_Repeat_Repost();
            m.set_type(MessageType::EMAIL_REPEAT_REPOST);
            message->set_email(email);
            message->set_state(ret);
            m.set_allocated_content6(message);
            std::string msgStr;
            m.SerializeToString(&msgStr);
            m.clear_content4();
            m.Clear();
            data_Ret.datas.push_back(msgStr);
            MessageManager::GetInstance()->Push_msgData_Out(data_Ret);
        }
    }
    std::cout<<"use mysql end"<<endl;
}

void HandleMessage::CheckNameRepeat(string name,bool &ret) {
    vector<string> res;
    std::string sqlStr = "select user_name,sleep(1) from user where user_name = '" + name + "';";
    MyMysql::GetInstance()->SelectDataForSingleList("user", res, sqlStr);
    ret = res.size();
}

void HandleMessage::CheckNumberRepeat(string number,bool &ret)
{
    vector<string> res;
    std::string sqlStr = "select phone_number,sleep(1) from user where phone_number = '" + number + "';";
    MyMysql::GetInstance()->SelectDataForSingleList("user", res, sqlStr);
    ret = res.size();
}

void HandleMessage::CheckEmailRepeat(string email,bool &ret)
{
    vector<string> res;
    std::string sqlStr = "select email,sleep(1) from user where email = '" + email + "';";
    MyMysql::GetInstance()->SelectDataForSingleList("user", res, sqlStr);
    ret = res.size();
}