#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <vector>
#include <map>
#include <mysql/mysql.h>
#include<stdlib.h>
using namespace std;

class MyMysql
{
public:
    static MyMysql* GetInstance();
    static void deleteInstance();
    string Init();
    bool InsertData(string tablename,vector<string> keys,vector<string> values);
    bool SelectData(string tablename, vector<string> keys,map<string,vector<string>> &res);
    bool SelectDataBySort(string tablename, vector<string> keys, map<string, vector<string>>& res,string sortKey,bool t);//trueΪ����falseΪ����
    bool SelectDataBySortAndOther(string tablename, vector<string> keys, map<string, vector<string>>& res, string sortKey, bool t,string other);//trueΪ����falseΪ����
    bool SelectDataWithWhere(string tablename, vector<string> keys, map<string, vector<string>>& res,string whereKey,string whereValue);
    bool SelectDataForSingleList(string tablename,vector<string> &res,string strSQL);
    bool UpdateData(string tablename, map<string,string> vals, map<string,string> wheres);
private:
    MyMysql();
    ~MyMysql();

    MyMysql(const MyMysql& manager);
    const MyMysql& operator=(const MyMysql& manager);

private:
    static MyMysql* m_MyMysql;
    static mutex m_Mutex;
    static mutex m_sqlOperator_Mutex;

    MYSQL mysql;    //һ�����ݿ�ṹ��  
    MYSQL_RES* res; //һ��������ṹ��
    MYSQL_ROW row; //char** ��ά���ݣ����һ������¼
};

