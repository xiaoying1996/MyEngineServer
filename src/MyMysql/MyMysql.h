#pragma once

#include <iostream>
#include <mutex>
#include <mysql/mysql.h>
#include <vector>
#include <map>
using namespace std;

class MyMysql
{
public:
    static MyMysql* GetInstance();

    static void deleteInstance();

    void Print();
    bool Init();
    void selectData(int type,std::string tablename,std::vector<std::string> keyname,std::vector<std::string> &resultStr);
    void selectData(int type,std::string tablename,std::vector<std::string> keyname,std::string whereKey,std::string whereVal,std::vector<std::string> &resultStr);
    int insertData(std::string tablename,map<std::string,std::string> mapData);
private:
    MyMysql();
    ~MyMysql();

    MyMysql(const MyMysql& manager);
    const MyMysql& operator=(const MyMysql& manager);

    MYSQL *conn_prt;     //创造一个MYSQL句柄

private:
    static MyMysql* m_MyMysql;
    static mutex m_Mutex;
    static mutex m_sqlOperator_Mutex;
};

