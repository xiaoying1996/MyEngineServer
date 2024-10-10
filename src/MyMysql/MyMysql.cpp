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

}

MyMysql::~MyMysql()
{
    cout << "MyMysql Destroyed..." << endl;
}

string MyMysql::Init()
{
    mysql_init(&mysql);
    if (mysql_real_connect(&mysql, "111.229.232.5", "root", "qwer12345678Cy", "hcl", 3306, NULL, 0) == NULL)
    {
        std::cout<<"Mysql err:"<<mysql_error(&mysql)<<endl;
        return mysql_error(&mysql);
    }
    return "success";
}

bool MyMysql::InsertData(string tablename,vector<string> keys, vector<string> values)
{
    string sqlStr = "insert into " + tablename + "(";
    for (int i = 0; i < keys.size() - 1; i++)
    {
        sqlStr += keys[i];
        sqlStr += ",";
    }
    sqlStr += keys[keys.size() - 1];
    sqlStr += ")";
    sqlStr += " values ";
    sqlStr += "(";
    for (int j = 0; j < values.size() - 1; j++)
    {
        sqlStr += "'";
        sqlStr += values[j];
        sqlStr += "'";
        sqlStr += ",";
    }
    sqlStr += "'";
    sqlStr += values[values.size() - 1];
    sqlStr += "'";
    sqlStr += ");";
    cout << sqlStr << endl;
    mysql_query(&mysql, "set names gbk");
    mysql_query(&mysql,sqlStr.c_str());
    string err = mysql_error(&mysql);
    return true;
}

bool MyMysql::SelectData(string tablename, vector<string> keys, map<string, vector<string>>& res)
{
    if (!keys.size())
    {
        return false;
    }
    string strSelect = "select ";
    for (int i = 0; i < keys.size()-1; i++)
    {
        strSelect += keys[i];
        strSelect += ",";
    }
    strSelect += keys[keys.size() - 1];
    strSelect += " from ";
    strSelect += tablename;
    strSelect += ";";

    mysql_query(&mysql, "set names gbk");
    int iQueryRet = mysql_query(&mysql, strSelect.c_str());
    cout << "iRet = " << iQueryRet << endl;
    string err = mysql_error(&mysql);
    cout << err << endl;
    MYSQL_RES* pRes = mysql_store_result(&mysql);
    if (nullptr != pRes)
    {
        int iRowCount = mysql_num_rows(pRes);
        cout << "row count = " << iRowCount << endl;

        int iColCount = mysql_num_fields(pRes);
        cout << "col count = " << iColCount << endl;

        MYSQL_FIELD* pField = nullptr;
        vector<string> vctFields;
        while (pField = mysql_fetch_field(pRes))
        {
            cout << pField->name << "\t";
            vctFields.push_back(pField->name);
        }

        MYSQL_ROW pQueryRes;
        int iCurRow = 0;
        while (pQueryRes = mysql_fetch_row(pRes))
        {
            cout << "\nrow " << ++iCurRow << " :" << endl;
            for (int i = 0; i < iColCount; ++i)
            {
                if (nullptr == pQueryRes[i])
                {
                    cout << vctFields[i] << " = " << "null" << endl;
                    res[vctFields[i]].push_back("");
                }
                else
                {
                    cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                    res[vctFields[i]].push_back(pQueryRes[i]);
                }
            }
        }

        mysql_free_result(pRes);
    }
    return true;
}

bool MyMysql::SelectDataBySort(string tablename, vector<string> keys, map<string, vector<string>>& res, string sortKey, bool t)
{
    if (!keys.size())
    {
        return false;
    }
    string strSelect = "select ";
    for (int i = 0; i < keys.size() - 1; i++)
    {
        strSelect += keys[i];
        strSelect += ",";
    }
    strSelect += keys[keys.size() - 1];
    strSelect += " from ";
    strSelect += tablename;
    strSelect += " order by ";
    strSelect += sortKey;
    if (!t)
    {
        strSelect += " desc";
    }
    strSelect += ";";

    mysql_query(&mysql, "set names gbk");
    int iQueryRet = mysql_query(&mysql, strSelect.c_str());
    cout << "iRet = " << iQueryRet << endl;
    string err = mysql_error(&mysql);
    cout << err << endl;
    MYSQL_RES* pRes = mysql_store_result(&mysql);
    if (nullptr != pRes)
    {
        int iRowCount = mysql_num_rows(pRes);
        cout << "row count = " << iRowCount << endl;

        int iColCount = mysql_num_fields(pRes);
        cout << "col count = " << iColCount << endl;

        MYSQL_FIELD* pField = nullptr;
        vector<string> vctFields;
        while (pField = mysql_fetch_field(pRes))
        {
            cout << pField->name << "\t";
            vctFields.push_back(pField->name);
        }

        MYSQL_ROW pQueryRes;
        int iCurRow = 0;
        while (pQueryRes = mysql_fetch_row(pRes))
        {
            cout << "\nrow " << ++iCurRow << " :" << endl;
            for (int i = 0; i < iColCount; ++i)
            {
                if (nullptr == pQueryRes[i])
                {
                    cout << vctFields[i] << " = " << "null" << endl;
                    res[vctFields[i]].push_back("");
                }
                else
                {
                    cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                    res[vctFields[i]].push_back(pQueryRes[i]);
                }
            }
        }

        mysql_free_result(pRes);
    }
    return true;
}

bool MyMysql::SelectDataBySortAndOther(string tablename, vector<string> keys, map<string, vector<string>>& res, string sortKey, bool t, string other)
{
    if (!keys.size())
    {
        return false;
    }
    string strSelect = "select ";
    for (int i = 0; i < keys.size() - 1; i++)
    {
        strSelect += keys[i];
        strSelect += ",";
    }
    strSelect += keys[keys.size() - 1];
    strSelect += " from ";
    strSelect += tablename;
    strSelect += other;
    strSelect += " order by ";
    strSelect += sortKey;
    if (!t)
    {
        strSelect += " desc ";
    }
    strSelect += ";";


    mysql_query(&mysql, "set names gbk");
    int iQueryRet = mysql_query(&mysql, strSelect.c_str());
    cout << "iRet = " << iQueryRet << endl;
    string err = mysql_error(&mysql);
    cout << err << endl;
    MYSQL_RES* pRes = mysql_store_result(&mysql);
    if (nullptr != pRes)
    {
        int iRowCount = mysql_num_rows(pRes);
        cout << "row count = " << iRowCount << endl;

        int iColCount = mysql_num_fields(pRes);
        cout << "col count = " << iColCount << endl;

        MYSQL_FIELD* pField = nullptr;
        vector<string> vctFields;
        while (pField = mysql_fetch_field(pRes))
        {
            cout << pField->name << "\t";
            vctFields.push_back(pField->name);
        }

        MYSQL_ROW pQueryRes;
        int iCurRow = 0;
        while (pQueryRes = mysql_fetch_row(pRes))
        {
            cout << "\nrow " << ++iCurRow << " :" << endl;
            for (int i = 0; i < iColCount; ++i)
            {
                if (nullptr == pQueryRes[i])
                {
                    cout << vctFields[i] << " = " << "null" << endl;
                    res[vctFields[i]].push_back("");
                }
                else
                {
                    cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                    res[vctFields[i]].push_back(pQueryRes[i]);
                }
            }
        }

        mysql_free_result(pRes);
    }
    return true;
}

bool MyMysql::SelectDataWithWhere(string tablename, vector<string> keys, map<string, vector<string>>& res, string whereKey, string whereValue)
{
    if (!keys.size())
    {
        return false;
    }
    string strSelect = "select ";
    for (int i = 0; i < keys.size() - 1; i++)
    {
        strSelect += keys[i];
        strSelect += ",";
    }
    strSelect += keys[keys.size() - 1];
    strSelect += " from ";
    strSelect += tablename;
    strSelect += " where ";
    strSelect += whereKey;
    strSelect += " = '";
    strSelect += whereValue;
    strSelect += "'";
    strSelect += ";";

    mysql_query(&mysql, "set names gbk");
    int iQueryRet = mysql_query(&mysql, strSelect.c_str());
    cout << "iRet = " << iQueryRet << endl;
    string err = mysql_error(&mysql);
    cout << err << endl;
    MYSQL_RES* pRes = mysql_store_result(&mysql);
    if (nullptr != pRes)
    {
        int iRowCount = mysql_num_rows(pRes);
        cout << "row count = " << iRowCount << endl;

        int iColCount = mysql_num_fields(pRes);
        cout << "col count = " << iColCount << endl;

        MYSQL_FIELD* pField = nullptr;
        vector<string> vctFields;
        while (pField = mysql_fetch_field(pRes))
        {
            cout << pField->name << "\t";
            vctFields.push_back(pField->name);
        }

        MYSQL_ROW pQueryRes;
        int iCurRow = 0;
        while (pQueryRes = mysql_fetch_row(pRes))
        {
            cout << "\nrow " << ++iCurRow << " :" << endl;
            for (int i = 0; i < iColCount; ++i)
            {
                if (nullptr == pQueryRes[i])
                {
                    cout << vctFields[i] << " = " << "null" << endl;
                }
                else
                {
                    cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                    res[vctFields[i]].push_back(pQueryRes[i]);
                }
            }
        }

        mysql_free_result(pRes);
    }
    return true;
}

bool MyMysql::SelectDataForSingleList(string tablename, vector<string>& res, string strSQL)
{
    mysql_query(&mysql, "set names gbk");
    int iQueryRet = mysql_query(&mysql, strSQL.c_str());
    cout << "iRet = " << iQueryRet << endl;
    string err = mysql_error(&mysql);
    cout << err << endl;
    MYSQL_RES* pRes = mysql_store_result(&mysql);
    if (nullptr != pRes)
    {
        int iRowCount = mysql_num_rows(pRes);
        cout << "row count = " << iRowCount << endl;

        int iColCount = mysql_num_fields(pRes);
        cout << "col count = " << iColCount << endl;

        MYSQL_FIELD* pField = nullptr;
        vector<string> vctFields;
        while (pField = mysql_fetch_field(pRes))
        {
            cout << pField->name << "\t";
            vctFields.push_back(pField->name);
        }

        MYSQL_ROW pQueryRes;
        int iCurRow = 0;
        while (pQueryRes = mysql_fetch_row(pRes))
        {
            cout << "\nrow " << ++iCurRow << " :" << endl;
            for (int i = 0; i < iColCount; ++i)
            {
                if (nullptr == pQueryRes[i])
                {
                    cout << vctFields[i] << " = " << "null" << endl;
                    res.push_back("");
                }
                else
                {
                    cout << vctFields[i] << " = " << pQueryRes[i] << endl;
                    res.push_back(pQueryRes[i]);
                }
            }
        }

        mysql_free_result(pRes);
    }
    return true;
}

bool MyMysql::UpdateData(string tablename, map<string, string> vals, map<string, string> wheres)
{
    if (!vals.size())
    {
        return false;
    }
    string strUpdate = "update ";
    strUpdate += tablename;
    strUpdate += " set ";
    if (vals.size() > 1)
    {
        for (auto iter = vals.begin(); iter != (vals.end()--); iter++)
        {
            strUpdate += iter->first;
            strUpdate += " = '";
            strUpdate += "'";
            strUpdate += iter->second;
            strUpdate += "', ";
        }
    }
    
    auto iter2 = (--vals.end());
    strUpdate += iter2->first;
    strUpdate += " = '";
    strUpdate += iter2->second;
    strUpdate += "'";

    strUpdate += " where ";
    for (auto iter3 = wheres.begin(); iter3 != (--wheres.end()); iter3++)
    {
        strUpdate += iter3->first;
        strUpdate += " = ";
        strUpdate += "'";
        strUpdate += iter3->second;
        strUpdate += "' ";
        strUpdate += "and ";
    }
    auto iter4 = (--wheres.end());
    strUpdate += iter4->first;
    strUpdate += " = ";
    strUpdate += "'";
    strUpdate += iter4->second;
    strUpdate += "'";
    strUpdate += ";";
    mysql_query(&mysql, "set names gbk");
    mysql_query(&mysql, strUpdate.c_str());
    string err = mysql_error(&mysql);
    return true;
}