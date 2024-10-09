#include "MyTcpSocket.h"

MyTcpSocket* MyTcpSocket::m_MyTcpSocket = nullptr;
mutex MyTcpSocket::m_Mutex;

MyTcpSocket* MyTcpSocket::GetInstance()
{
    if (m_MyTcpSocket == nullptr)
    {
        unique_lock<mutex> lock(m_Mutex);
        if (m_MyTcpSocket == nullptr)
        {
            m_MyTcpSocket = new (nothrow) MyTcpSocket();
        }
    }
    return m_MyTcpSocket;
}

void MyTcpSocket::deleteInstance()
{
    unique_lock<mutex> lock(m_Mutex);
    if (m_MyTcpSocket)
    {
        delete m_MyTcpSocket;
        m_MyTcpSocket = nullptr;
    }
}

MyTcpSocket::MyTcpSocket()
{
    Init();
    //创建一个线程，用来处理返回给客户端的数据
}

MyTcpSocket::~MyTcpSocket()
{
    cout << "MyTcpSocket Destroyed..." << endl;
}

void MyTcpSocket::Print()
{
    cout << "address:" << this << endl;
}

bool MyTcpSocket::Init()
{
    cout<<"MyTcpSocket Initiating"<<endl;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(lfd == -1)
    {
        perror("socket error");
        exit(1);
    }
    // 绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(9609);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 本地多有的ＩＰ
    // 127.0.0.1
    // inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr.s_addr);

    // 设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定端口
    int ret = bind(lfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if(ret == -1)
    {
        perror("bind error");
        exit(1);
    }

    // 监听
    ret = listen(lfd, 64);
    if(ret == -1)
    {
        perror("listen error");
        exit(1);
    }

    // 现在只有监听的文件描述符
    // 所有的文件描述符对应读写缓冲区状态都是委托内核进行检测的epoll
    // 创建一个epoll模型
    epfd = epoll_create(100);
    if(epfd == -1)
    {
        perror("epoll_create");
        exit(0);
    }

    // 往epoll实例中添加需要检测的节点, 现在只有监听的文件描述符
    ev;
    ev.events = EPOLLIN;    // 检测lfd读读缓冲区是否有数据
    ev.data.fd = lfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if(ret == -1)
    {
        perror("epoll_ctl");
        exit(0);
    }


    epoll_event evs[1024];
    size = sizeof(evs) / sizeof(struct epoll_event);
    printf("epoll初始化成功\n");
    SocketRunning();
    return true;
}

void MyTcpSocket::SocketRunning()
{
    while(1)
    {
        //遍历MessageManager
        while(1)
        {
            if(MessageManager::GetInstance()->msgData_Out_NotEmpty())
            {
                MessageData data = MessageManager::GetInstance()->Pop_msgData_OUT();
                send(data.clientFd, data.datas[0].c_str(), data.datas[0].length(), 0);
            }
            else
            {
                break;
            }
        }

        cout<<"epoll running....."<<endl;
        // 调用一次, 检测一次
        int num = epoll_wait(epfd, evs, size, -1);
        printf("==== num: %d\n", num);

        for(int i=0; i<num; ++i)
        {
            // 取出当前的文件描述符
            int curfd = evs[i].data.fd;
            // 判断这个文件描述符是不是用于监听的,如果是用于监听的就创建新的连接
            if(curfd == lfd)
            {
                // 建立新的连接
                int cfd = accept(curfd, NULL, NULL);
                // 将文件描述符设置为非阻塞
                // 得到文件描述符的属性
                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);
                // 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
                // 通信的文件描述符检测读缓冲区数据的时候设置为边沿模式
                ev.events = EPOLLIN | EPOLLET;    // 读缓冲区是否有数据
                ev.data.fd = cfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                if(ret == -1)
                {
                    perror("epoll_ctl-accept");
                    exit(0);
                }
            }
            //如果不是用于监听的就读取数据
            else
            {
                // 处理通信的文件描述符
                // 接收数据
                char buf[32];
                char strRecv[33];
                int len;
                string strRes = "";
                memset(buf, 0, sizeof(buf));
                // 循环读数据
                while(1)
                {
                    len = recv(curfd, buf, sizeof(buf), 0);
                    if(len == 0)
                    {
                        // 非阻塞模式下和阻塞模式是一样的 => 判断对方是否断开连接
                        printf("客户端断开了连接...\n");
                        // 将这个文件描述符从epoll模型中删除
                        epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                        close(curfd);
                        break;
                    }
                    else if(len > 0)
                    {
                        // 通信
                        // 接收的数据打印到终端
                        write(STDOUT_FILENO, buf, len);
                        strcpy(strRecv,buf);
                        strRecv[len] = 0;
                        strRes += strRecv;
                        // 发送数据
                        //send(curfd, buf, len, 0);
                    }
                    else
                    {
                        // len == -1
                        if(errno == EAGAIN)
                        {
                            printf("数据读完了...\n");
                            break;
                        }
                        else
                        {
                            perror("recv");
                            exit(0);
                        }
                    }
                }
                cout<<"Result: "<<strRes<<endl;
                MessageManager::GetInstance()->Push_msgData_IN(curfd,strRes);
            }
        }
    }
}