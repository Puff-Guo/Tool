#include <iostream>
#include "servant/Communicator.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include "servant/Application.h"
#include "comm_def.h"
#include "ret_code.h"
#include "user_base.h"
#include "CreateTickertDateIndex.h"

using namespace tars;
std::string g_data_base_obj = "userCenter.userBaseSvr.userBaseObj@tcp -h ";
std::string g_create_index_obj = "invoiceCenter.CreateIndexSvr.CreateTickertDateIndexObj@tcp -h ";

Communicator comm_create_index;
Communicator comm_data_base;

invoiceCenter::CreateTickertDateIndexPrx create_index_prx;
userCenter::userBaseServicePrx user_base_prx;
std::string g_mini_pro_appid;
int g_max_user_id;

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
    string max_id = config.get("/param/<max_uid>","10000000");
    
    g_max_user_id = std::stoi(max_id);
    std::cout << "max_uid:" << g_max_user_id;
    
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

    std::string database_obj = g_data_base_obj + data_base_ip + " -p " + data_base_port;
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

    std::string createindex_obj = g_create_index_obj + create_index_ip + " -p " + create_index_port;
    std::cout << "createindex_obj:" << createindex_obj << std::endl;

    comm_create_index.stringToProxy(createindex_obj,create_index_prx);

    const string app = "tools";
	const string server = "CreateIndex";
	const string log_path = "./log";
    TarsRollLogger::getInstance()->setLogInfo(app, server, log_path, 1024*1024*50, 5);	
    TarsRollLogger::getInstance()->sync(true);
	TarsTimeLogger::getInstance()->setLogInfo(NULL, "", app, server, log_path);
    return true;
}

int IsUser(int user_id)
{
    //判断是否是小程序的用户ID
    userCenter::AuthQueryReq req;
    userCenter::AuthQueryResp rsp;
    req.type = 1;
    req.uid = user_id;
    req.appid = g_mini_pro_appid;    

    try
    {
        std::cout << "begin IsUser user_id:" << user_id << std::endl;
        user_base_prx->findAuthByKey(req,rsp);
        std::cout << "end IsUser user_id:" << user_id << std::endl;
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
        std::cout << "begin CreateIndex user_id:" << user_id << std::endl;
        create_index_prx->CreateTickertTimeIndex(req,rsp);
        std::cout << "end CreateIndex user_id:" << user_id << std::endl;
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

int main(int argc,char ** argv)
{    
    if(!Init())
    {
        std::cout << "init fail!" << std::endl;
        exit(0);
    }

    for(int user_id = 0;user_id <= g_max_user_id;user_id++)
    {
        std::cout << "user_id:" << user_id << std::endl;
        int ret = IsUser(user_id);
        std::cout << "ret:" << ret << std::endl;
        if(RET_OK == ret)
        {
            //存在用户
            if(CreateIndex(user_id))
            {
                //todo 写成功日志
                FDLOG("fail") << user_id << endl; 
            }
            else
            {
                FDLOG("succ") << user_id << endl; 
            }
        }
        else if(RET_ERR_AUTH_QUERY_BYUIDAPPID == ret || RET_SYS_ERR == ret)
        {
            //查选错误/超时 to do 写查选失败日志
            FDLOG("fail") << user_id << endl; 
        } 
        else if(8273 == ret)
        {
            FDLOG("nodata") << user_id << endl; 
        }       
    }   

    return 0;
}