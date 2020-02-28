#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "servant/Communicator.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "servant/Application.h"
#include "comm_def.h"
#include "user_base.h"
#include "CreateTickertDateIndex.h"

using namespace std;
using namespace tars;

using namespace std;
using namespace tars;
std::string g_data_base_obj = "userCenter.userBaseSvr.userBaseObj@tcp -h ";
std::string g_create_index_obj = "invoiceCenter.CreateIndexSvr.CreateTickertDateIndexObj@tcp -h ";

Communicator comm_create_index;
Communicator comm_data_base;

invoiceCenter::CreateTickertDateIndex create_index_prx;
userCenter::userBaseService user_base_prx;
std::string g_mini_pro_appid;
int g_max_user_id;
std::string g_tool_gen_fail_file;
std::string g_deal_file;


bool Init();
int IsUser(int user_id);
bool CreateIndex(int user_id);


bool Init()
{
    //初始化指针
    LOG_DEBUG("------------- init ");
    string conf_file = "conf.xml";
	TC_Config config;
	config.parseFile(conf_file);

    g_mini_pro_appid = config.get("/param/<appid>","wx8280850d07d385eb");
    int max_id = config.get("/param/<max_uid>","10000000");
    
    g_max_user_id = std::stoi(max_id);
    std::cout << "max_uid:" << g_max_user_id;

    g_tool_gen_fail_file = config.get("/param/<dealtool_fail_file>","tool_fail_file");
    g_deal_file = config.get("/param/<deal_file_name>","fail");
    
    std::string data_base_ip = config.get("/databaseobj/<ip>");
    std::cout << "data_base_ip:" << data_base_ip << std::endl;
    if(data_base_ip.empty())
    {
        return false;
    }

    std::string data_base_port = config.get("/databaseobj/<port>");
    std::cout << "data_base_port:" << data_base_port << std::endl;
    if(data_base_port.empty())
    {
        return false;
    }

    std::string database_obj = g_data_base_obj + data_base_ip + " " + data_base_port;
    std::cout << "database_obj:" << database_obj << std::endl;

    comm_data_base.stringToProxy(database_obj,user_base_prx);

    std::string create_index_ip = config.get("/createindex/<ip>");
    std::cout << "create_index_ip:" << create_index_ip << std::endl;
    if(create_index_ip.empty())
    {
        return false;
    }

    std::string create_index_port = config.get("/createindex/<port>");
    std::cout << "create_index_port:" << create_index_port << std::endl;
    if(create_index_port.empty())
    {
        return false;
    }

    std:string createindex_obj = g_create_index_obj + create_index_ip + " " + create_index_port;
    std::cout << "createindex_obj:" << createindex_obj << std::endl;

    comm_create_index.stringToProxy(createindex_obj,create_index_prx);
}

int IsUser(int user_id)
{
    //判断是否是小程序的用户ID
    userCenter::AuthQueryReq req;
    userCenter::AuthQueryReq rsp;
    rep.type = 1;
    rep.uid = user_id;
    rqp.appid = g_mini_pro_appid;    

    try
    {
        user_base_prx->findAuthByKey(req.rsp);
        return rsp.auth_req_resp.code;
    }
    catch(const std::exception& e)
    {
        //todo 写超时文件日志
        return RET_SYS_ERR;
    }    
}

bool CreateIndex(int user_id)
{
    invoiceCenter::CreateTickerDateIndexReq req;
    invoiceCenter::CreateTickerDateIndexRsp rsp;

    req.uid = user_id;
    try
    {
        create_index_prx->CreateTickertTimeIndex(req,rsp);
        if(RET_OK == rsp.code)
        {
            return true;
        }
        else
        {
            //todo 写失败文件
            return false;
        }
    }
    catch(const std::exception& e)
    {
        //todo 写超时文件日志
        return false;        
    }    
}

std::vector<std::string> split(const std::string &source,const std::string &delim)
{    
	std::vector<string> vec;
    std::string srcStr = source;

	auto nPos = srcStr.find(delim.c_str());
	while(std::string::npos != nPos)
	{
		string temp = srcStr.substr(0, nPos);
        cout << "substr:" << temp << " srcStr:" << srcStr << " npos:" << endl;
		vec.push_back(temp);
		srcStr = srcStr.substr(nPos+1);
		nPos = srcStr.find(delim.c_str());
	}

	vec.push_back(srcStr);
	return vec;
}

int main(int argc,char ** argv)
{      
    try
    {
        cout << "begin" << endl;
        cout << "deal file name:" << g_deal_file; << endl;
        ifstream read_file_stream(g_deal_file);
        std::string str_user_id;

        while(std::getline(read_file_stream,str_user_id))
        {
            cout << "str_user_id" <<  str_user_id << endl;
            int user_id = std::stoi(str_user_id);

            if(!CreateIndex(user_id))
            {
                FDLOG(g_tool_gen_fail_file) << user_id << std::endl; 
            }
        }
    }
    catch(exception &ex）
    {
        cerr << "ex:" << ex.what() << endl;
    }
    catch(...)
    {
        cerr << "unknown exception." << endl;
    }
   
    return 0;
}