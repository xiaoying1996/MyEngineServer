#include "MyMysql.h"

MyMysql* MyMysql::m_MyMysql = nullptr;
mutex MyMysql::m_Mutex;
mutex MyMysql::m_sqlOperator_Mutex;

MyMysql* MyMysql::GetInstance()
{
    if (m_MyMysql == nullptr)
    {
        unique_lock<mutex> lock(m_Mutex);
        if (m_MyMysql == nullptr)
        {
            m_MyMysql = new (nothrow) MyMysql();
        }
    }
    return m_MyMysql;
}

void MyMysql::deleteInstance()
{
    unique_lock<mutex> lock(m_Mutex);
    if (m_MyMysql)
    {
        delete m_MyMysql;
        m_MyMysql = nullptr;
    }
}

MyMysql::MyMysql()
{
    Init();
}

MyMysql::~MyMysql()
{
    cout << "MyMysql Destroyed..." << endl;
}

void MyMysql::Print()
{
    cout << "address:" << this << endl;
}

bool MyMysql::Init() {
    cout << "MyMysql Initiating..." << endl;
    conn_prt = mysql_init(NULL);  //初始化MYSQL句柄

    if(!conn_prt)   //若初始化句柄失败
    {
        printf("init mysql failed\n");
        return false;
    }else{
        printf("init mysql success\n");
    }

    std::string host = "127.0.0.1";
    std::string user = "root";
    std::string pwd = "qwer12345678Cy";
    std::string db = "2048Data";
    if (!mysql_real_connect(conn_prt, host.c_str(), user.c_str(), pwd.c_str(), db.c_str(), 0, NULL, CLIENT_MULTI_STATEMENTS)) {
        std::cout << "mysql_real_connect err" << std::endl;
        return 0;
    }else{
        printf("connect mysql success\n");
    }
    return true;
}

/*
 * 据库对象对数据库进行查询
 * type 1.登录 2.注册 3.待定
 * tablename:待查询的数据库名称
 * keyname：需要查询的字段
 * resultStr：查询到的结果
 */
void MyMysql::selectData(int type,std::string tablename,std::vector<std::string> keyname,std::vector<std::string> &resultStr)
{
    if(!keyname.size())
    {
        resultStr.push_back("no key input");
        return;
    }
    std::string queryStr = "";
    queryStr += "select ";
    for(int i = 0;i<keyname.size()-1;i++)
    {
        queryStr += keyname[i] ;
        queryStr += ",";
    }
    queryStr +=keyname[keyname.size()-1];
    queryStr += " from ";
    queryStr += tablename;
    queryStr += ";";
    std::cout<<"sql Do Select:" << queryStr<<endl;
    m_sqlOperator_Mutex.lock();

    m_sqlOperator_Mutex.unlock();
}

/*
 * 据库对象对数据库进行查询
 * tablename:待查询的数据库名称
 * keyname：需要查询的字段
 * whereVal:where限制条件
 * resultStr：查询到的结果
 */
void MyMysql::selectData(int type,std::string tablename,std::vector<std::string> keyname,std::string whereKey,std::string whereVal,std::vector<std::string> &resultStr)
{
    std::cout<<"enter do select with where"<<endl;
    if(!keyname.size())
    {
        resultStr.push_back("no key input");
        return;
    }
    std::string queryStr = "";
    queryStr += "select ";
    for(int i = 0;i<keyname.size()-1;i++)
    {
        queryStr += keyname[i] ;
        queryStr += ",";
    }
    queryStr +=keyname[keyname.size()-1];
    queryStr += " from ";
    queryStr += tablename;
    queryStr += " where " + whereKey + " = '" + whereVal + "'";

    queryStr += ";";
    std::cout<<"sql Do Select with where:" << queryStr<<endl;
    m_sqlOperator_Mutex.lock();
    int iQueryRet = mysql_query(conn_prt, queryStr.c_str());
    cout << "iRet = " << iQueryRet << endl;
    if(iQueryRet ==0)
    {
        MYSQL_RES *pRes = mysql_store_result(conn_prt);
        if(nullptr != pRes)
        {
            switch (type) {
                case 1:
                {
                    int iRowCount = mysql_num_rows(pRes);
                    cout << "row count = " << iRowCount << endl;

                    int iColCount = mysql_num_fields(pRes);
                    cout << "col count = " << iColCount << endl;

                    MYSQL_FIELD *pField = nullptr;
                    vector<string> vctFields;
                    while (pField = mysql_fetch_field(pRes) )
                    {
                        cout << pField->name << "\t";
                        vctFields.push_back(pField->name);
                    }

                    MYSQL_ROW pQueryRes;
                    int iCurRow = 0;
                    while (pQueryRes = mysql_fetch_row(pRes) )
                    {
                        cout << "\nrow " << ++iCurRow << " :" << endl;
                        for(int i = 0; i < iColCount; ++i)
                        {
                            if(nullptr == pQueryRes[i])
                            {
                                cout << vctFields[i] << " = " << "null" << endl;
                            }
                            else
                            {
                                cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                                resultStr.push_back(pQueryRes[i]);
                            }
                        }
                    }
                    mysql_free_result(pRes);
                    break;
                }
                case 2:
                    int iRowCount = mysql_num_rows(pRes);
                    cout << "row count = " << iRowCount << endl;
                    mysql_free_result(pRes);
                    if(iRowCount >=1)
                    {
                        //该用户名已注册
                        resultStr.push_back("NAMEHASREGISTERED");
                    }else{
                        //未注册，进行注册
                        cout << "进行注册"<< endl;
                        resultStr.push_back("REGISTERSUCCESS");
                    }
                    break;
            }
        }
    }else{
        cout << "MYSQL RUN ERROR" << endl;
    }
    m_sqlOperator_Mutex.unlock();
}

/*
 * 对数据库进行插入操作
 * tablename 插入哪个表
 * mapData 插入的数据
 * 返回值 0.插入成功   1.插入的数据为空  9.数据写入数据库失败
 */
int MyMysql::insertData(std::string tablename,map<std::string,std::string> mapData)
{
    //INSERT INTO t_student (name, age) VALUE ("xiaoming", 16);
    if(!mapData.size())
    {
        return 1;
    }
    std::string queryStr = "";
    queryStr += "INSERT INTO ";
    queryStr += tablename;
    queryStr += "(";
    int size = 1;
    for(auto iter = mapData.begin();iter != mapData.end();iter++)
    {
        queryStr += iter->first;
        if(size != mapData.size())
        {
            queryStr += ",";
            size++;
        }
    }
    queryStr += ") VALUE (";
    size = 1;
    for(auto iter = mapData.begin();iter != mapData.end();iter++)
    {
        queryStr += "'";
        queryStr += iter->second;
        queryStr += "'";
        if(size != mapData.size())
        {
            queryStr += ",";
            size++;
        }
    }
    queryStr += ");";
    std::cout<<"sql do insert: "<< queryStr<<endl;
    int iQueryRet = mysql_query(conn_prt, queryStr.c_str());
    cout << "iRet = " << iQueryRet << endl;
    if(iQueryRet ==0)
        return 0;
    else
        return 9;
}