#include <iostream>
#include "servant/Communicator.h"
#include "servant/Application.h"
#include <fstream>
#include <vector>
#include <string>
#include "dcache_op.h"
#include "comm_def.h"
#include "reader_etc.pb.h"
#include "auth.pb.h"
#include "auth.tars.h"

using namespace std;
using namespace tars;

DATAOP _invoice_op;
DATAOP _invoice_item_op;
DATAOP _rel_tax_invoice_op; 
DATAOP _rel_user_invoice_op;
DATAOP _open_id_mysql_op;
DATAOP _card_id_mysql_op;
DATAOP _travel_apply_op;
DATAOP _travel_item_op;
protocol::UserInterfacePrx _userInterface_prx;

void DeleteAllApply(const std::string &source_id);
bool QueryInvoice(const std::string code,const std::string no,const std::string &field,std::map<std::string,std::string> &invoice_info);
bool QueryTaxInvoice(const std::string &code,const std::string &no,const std::string &field,vector<map<string, string>> &field_value);
bool QueryInvoiceOp(const string& order_sn, const string& invoice_id,const std::string &field,std::vector<map<string,string>> &field_value);
bool QueryEtcApplyBySourceId(const std::string &b_user_idstd,std::vector<std::map<std::string,std::string>> &rsp);
bool QueryTransByApplyId(const std::string &apply_id,std::vector<std::map<std::string,std::string>> &rsp);
bool DeltelInvoice(const std::map<std::string,std::string> &map_info);
bool DeltelTrans(const std::map<std::string,std::string> &map_info);
bool DeltelApply(const std::map<std::string,std::string> &map_info);
bool WhetherAuth(const std::string &source_id,std::string &user_id,std::string &auth_time);
bool DeltelInvoiceItem(const std::map<std::string,std::string> &map_info);
bool DeltelInvoiceRel(const std::map<std::string,std::string> &map_info);
bool DeltelUserRel(const std::map<std::string,std::string> &map_info,const std::string &user_id);
bool QueryAllUserRel(std::string &user_id,std::vector<std::map<std::string,std::string>> &rsp);


bool DeltelInvoiceItem(const std::map<std::string,std::string> &map_info)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {        
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "order_sn";
        cond.op = DCache::EQ;
        cond.value = map_info.at("order_sn");
        v_cond.push_back(cond);
        cond.fieldName = "invoice_id";
        cond.op = DCache::EQ;
        cond.value = map_info.at("invoice_id");
        v_cond.push_back(cond);  
        cond.fieldName = "line_no";
        cond.op = DCache::EQ;
        cond.value = 1;
        v_cond.push_back(cond);            

        if(!_invoice_item_op->Delete(v_cond))            
        {                
            LOG_ERROR("_invoice_item_op Delete Failed");                              
            break;
        }   

        b_ret = true;             
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelInvoiceItem error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}
bool DeltelInvoiceRel(const std::map<std::string,std::string> &map_info)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {        
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "ticket_sn";
        cond.op = DCache::EQ;
        cond.value = map_info.at("ticket_sn");
        v_cond.push_back(cond);
        cond.fieldName = "ticket_code";
        cond.op = DCache::EQ;
        cond.value = map_info.at("ticket_code");
        v_cond.push_back(cond);              

        if(!_rel_tax_invoice_op->Delete(v_cond))            
        {                
            LOG_ERROR("_rel_tax_invoice_op Delete Failed");                              
            break;
        }   

        b_ret = true;             
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelInvoiceRel error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}
bool DeltelUserRel(const std::map<std::string,std::string> &map_info,const std::string &user_id)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {                
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "order_sn";
        cond.op = DCache::EQ;
        cond.value = map_info.at("order_sn");
        v_cond.push_back(cond);
        cond.fieldName = "user_id";
        cond.op = DCache::EQ;
        cond.value = user_id;
        v_cond.push_back(cond);        
        LOG_DEBUG("user:" << user_id << " order_sn:" << map_info.at("order_sn"));      

        if(!_rel_user_invoice_op->Delete(v_cond))            
        {                
            LOG_ERROR("DeltelUserRel Delete Failed");                              
            break;
        }   

        b_ret = true;             
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelUserRel error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}

bool QueryAllUserRel(std::string &user_id,std::vector<std::map<std::string,std::string>> &rsp)
{
    bool b_ret  = false;
    std::string field = "*";
    __BEGIN_PROC__
        
    try
    {                
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "user_id";
        cond.op = DCache::EQ;
        cond.value = user_id;
        v_cond.push_back(cond);                 

        if(!_rel_user_invoice_op->Query(field, v_cond,rsp))            
        {                
            LOG_ERROR("_travel_apply_op getMKV Failed");                              
            break;
        }

        LOG_DEBUG("Query rsp size :" << rsp.size());
        b_ret = true;        
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelUserRel error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}


bool DeltelInvoice(const std::map<std::string,std::string> &map_info)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {        
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "order_sn";
        cond.op = DCache::EQ;
        cond.value = map_info.at("order_sn");
        v_cond.push_back(cond);
        cond.fieldName = "invoice_id";
        cond.op = DCache::EQ;
        cond.value = map_info.at("invoice_id");
        v_cond.push_back(cond);              

        if(!_invoice_op->Delete(v_cond))            
        {                
            LOG_ERROR("_travel_apply_op Delete Failed");                              
            break;
        }   

        b_ret = true;             
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelInvoice error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}

bool DeltelTrans(const std::map<std::string,std::string> &map_info)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {        
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "apply_id";
        cond.op = DCache::EQ;
        cond.value = map_info.at("apply_id");
        v_cond.push_back(cond);
        cond.fieldName = "source";
        cond.op = DCache::EQ;
        cond.value = map_info.at("source");
        v_cond.push_back(cond);
        cond.fieldName = "line_no";
        cond.op = DCache::EQ;
        cond.value = map_info.at("line_no");
        v_cond.push_back(cond);           

        if(!_travel_item_op->Delete(v_cond))            
        {                
            LOG_ERROR("_travel_apply_op Delete Failed");                              
            break;
        }

        b_ret = true;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelTrans error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}

bool DeltelApply(const std::map<std::string,std::string> &map_info)
{
    bool b_ret  = false;
    __BEGIN_PROC__
        
    try
    {        
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "b_user_id";
        cond.op = DCache::EQ;
        cond.value = map_info.at("b_user_id");
        v_cond.push_back(cond);
        cond.fieldName = "has_red";
        cond.op = DCache::EQ;
        cond.value = map_info.at("has_red");
        v_cond.push_back(cond);
        cond.fieldName = "is_red";
        cond.op = DCache::EQ;
        cond.value = map_info.at("is_red");
        v_cond.push_back(cond);
        cond.fieldName = "source";
        cond.op = DCache::EQ;
        cond.value = map_info.at("source");
        v_cond.push_back(cond);        

        if(!_travel_apply_op->Delete(v_cond))
        {                
            LOG_ERROR("_travel_apply_op Delete Failed");                              
            break;
        } 

        b_ret = true;           
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeltelApply error" << e.what());
        break;                           
    }    
    
    __END_PROC__
    return b_ret;
}

void DeleteAllApply(std::string &source_id)
{
    std::vector<std::map<std::string,std::string>> apply_vec;
    std::string invoice_field = "order_sn,invoice_id,user_id,ticket_sn,ticket_code,ticket_date,created_at";
    std::map<std::string,std::string> invoice_info;
    __BEGIN_PROC__

    try
    {
        //先批量查找所有有效申请
        if(!QueryEtcApplyBySourceId(source_id,apply_vec))
        {
            break;
        }      

        for(auto &item : apply_vec)
        {
            InvoiceCenter::EtcInvoicelist invoice_list;
            LOG_DEBUG("str_invoice: " << item.at("invoices"));
            if(item.at("invoices").empty())
            {
                LOG_ERROR("str_invoice 为空");
                //continue;
            }
                
            if(!CommFunc::JsonStrToMsg(item.at("invoices"),invoice_list))
            {
                LOG_ERROR("DeleteAllApply 转换发票失败");
                //continue;
            }

            int len = invoice_list.invoices_size();
            LOG_DEBUG("len:" << len);
            bool b_break = false;
            for(int i = 0;i < len;i++)
            {
                invoice_info.clear();
                InvoiceCenter::InvoicesEtcMinInfo invoice_item = invoice_list.invoices(i);
                if(!QueryInvoice(invoice_item.code(),invoice_item.no(),invoice_field,invoice_info))
                {
                    LOG_ERROR("QueryInvoice error !");   
                    b_break = true;                 
                    continue;
                }
                
                LOG_DEBUG("invoice_user_id:" << invoice_info.at("user_id"));
                LOG_DEBUG("order_sn:" << invoice_info.at("order_sn") << " invoice_id:" << invoice_info.at("invoice_id")
                << " ticket_sn:" << invoice_info.at("ticket_sn")<< " ticket_code:" << invoice_info.at("ticket_code"));

                DeltelInvoiceItem(invoice_info);
                DeltelInvoiceRel(invoice_info);
                
                std::string user_id;
                std::string auth_time;
                if(WhetherAuth(source_id,user_id,auth_time))
                {
                    LOG_DEBUG("user_id:" << user_id);
                    DeltelUserRel(invoice_info,user_id);
                }

                LOG_DEBUG("user_id:" << user_id);
                DeltelInvoice(invoice_info);  

            }

            // if(b_break)
            //     break;

            std::vector<std::map<std::string,std::string>> trans_map;
            QueryTransByApplyId(item.at("apply_id"),trans_map);
            for(auto &trans_item : trans_map)
            {
               if(!DeltelTrans(trans_item))
               {
                   b_break = true;                 
                    //continue;
               }
            }

            DeltelApply(item);            
        }        
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("DeleteAllApply error :" << e.what());
        break;
    }

    __END_PROC__
}

bool QueryEtcApplyBySourceId(const std::string &b_user_id,std::vector<std::map<std::string,std::string>> &rsp)
{
    bool b_ret = false;
    std::string field = "*";
    __BEGIN_PROC__
        
    try
    {
        LOG_DEBUG("b_user_id:" << b_user_id);
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "b_user_id";
        cond.op = DCache::EQ;
        cond.value = b_user_id;
        v_cond.push_back(cond);            
        cond.fieldName = "source";
        cond.op = DCache::EQ;
        cond.value = "ETCZHUSHOU";
        v_cond.push_back(cond);        

        if(!_travel_apply_op->Query(field, v_cond,rsp))            
        {                
            LOG_ERROR("_travel_apply_op getMKV Failed");                              
            break;
        }

        LOG_DEBUG("Query rsp size :" << rsp.size());
        b_ret = true;         
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("QueryEtcApplyBySourceId error" << e.what());
        break;                           
    }    
    
    __END_PROC__    
    return b_ret;
}

bool QueryTransByApplyId(const std::string &apply_id,std::vector<std::map<std::string,std::string>> &rsp)
{
    bool b_ret = false;
    std::string field = "*";
    __BEGIN_PROC__
        
    try
    {
        LOG_DEBUG("apply_id:" << apply_id);
        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "apply_id";
        cond.op = DCache::EQ;
        cond.value = apply_id;
        v_cond.push_back(cond);       
        cond.fieldName = "source";
        cond.op = DCache::EQ;
        cond.value = "ETCZHUSHOU";
        v_cond.push_back(cond);        

        if(!_travel_item_op->Query(field, v_cond,rsp))            
        {                
            LOG_ERROR("_travel_apply_op getMKV Failed");                              
            break;
        }

        LOG_DEBUG("Query rsp size :" << rsp.size());
        b_ret = true;         
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("QueryEtcApplyBySourceId error" << e.what());
        break;                           
    }    
    
    __END_PROC__    
    return b_ret;
}

bool QueryInvoice(const std::string code,const std::string no,const std::string &field,std::map<std::string,std::string> &invoice_info)
{
    bool b_ret = false;
    vector<map<string,string>> tax_field_value;
    vector<map<string,string>> invoice_field_value;
    string tax_field = "order_sn, invoice_id";

    __BEGIN_PROC__

    try
    {
        
        if(!QueryTaxInvoice(code,no,tax_field,tax_field_value))
        {
            break;
        }

        if(!QueryInvoiceOp(tax_field_value[0].at("order_sn"),tax_field_value[0].at("invoice_id"),field,invoice_field_value))
        {
            break;
        }
        
        if(invoice_field_value.empty())
        {
            LOG_DEBUG("QueryInvoice query empty!");
            break;
        }

        invoice_info = invoice_field_value[0];
        b_ret = true;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("QueryInvoice error:" << e.what());
        break;
    }
    
    __END_PROC__
    return b_ret;
}

bool QueryTaxInvoice(const std::string &code,const std::string &no,const std::string &field,vector<map<string, string>> &field_value)
{
    bool b_ret = false;
    __BEGIN_PROC__

    try
    {
        LOG_DEBUG("code:" << code << " no:" << no);
        LOG_DEBUG("field:" << field);

        if(code.empty() || no.empty() || field.empty())
        {
            LOG_ERROR("QueryTaxInvoice is fail ");
            break;
        }

        vector<Condition> v_cond;
        Condition cond;
        cond.fieldName = "ticket_sn";
        cond.op = EQ;
        cond.value = no;
        v_cond.push_back(cond);

        Condition cond2;
        cond2.fieldName = "ticket_code";
        cond2.op = EQ;
        cond2.value = code;
        v_cond.push_back(cond2);

        if(!_rel_tax_invoice_op->Query(field, v_cond, field_value))
        {
            LOG_ERROR("服务器内部错误: _rel_tax_invoice_op 表查询失败 ticket_sn: " << no << " ticket_code: " << code);            
            break;
        } 

        if(field_value.size() == 0)
        {           
            LOG_ERROR("ticket_sn:" << no  << " ticket_code:" << code << "not exit!" );            
            break; 
        }

        b_ret = true;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("QueryTaxInvoice error:" << e.what());
        break;
    }
    
    __END_PROC__
    return b_ret;
}

bool QueryInvoiceOp(const string& order_sn, const string& invoice_id,const std::string &field,std::vector<map<string,string>> &field_value)
{
    bool b_ret = false;
    __BEGIN_PROC__

    try
    {
        LOG_DEBUG("order_sn:" << order_sn << " invoice_id:" << invoice_id);
        LOG_DEBUG("field:" << field);

        if(order_sn.empty() || field.empty())
        {
            LOG_ERROR("QueryInvoiceOp is fail ");
            break;
        }

        vector<Condition> v_cond;
        Condition cond;    
        cond.fieldName = "order_sn";
        cond.op = DCache::EQ;
        cond.value = order_sn;
        v_cond.push_back(cond);

        if(!invoice_id.empty())
        {
            cond.fieldName = "invoice_id";
            cond.op = DCache::EQ;
            cond.value = invoice_id;
            v_cond.push_back(cond);
        }

        if(!_invoice_op->Query(field, v_cond, field_value))
        {
            LOG_ERROR("_invoice Query Failed,order_sn:" << order_sn << " invoice_id: " << invoice_id);
            break;
        }
        
        if(0 == field_value.size())
        {
            LOG_ERROR("_invoice Query size is 0,order_sn:" << order_sn << " invoice_id: " << invoice_id);
            break;
        }        

        b_ret = true;
    }
    catch(const std::exception& e)
    {
		LOG_ERROR("QueryInvoiceOp error:" << e.what());
        break;
    }
    
    __END_PROC__
    return b_ret;    
}

void init(void)
{
    LOG_DEBUG("------------- init ");
    string conf_file = "conf.xml";
	TC_Config config;
	config.parseFile(conf_file);
    
    _invoice_op = new DCacheOp("invoice");
	if(!_invoice_op->Init(config))
	{
		LOG_ERROR("invoice 数据操作接口初始化失败");
		exit(0);
	}
    
    _invoice_item_op = new DCacheOp("invoice_item");
	if(!_invoice_item_op->Init(config))
	{
		LOG_ERROR("invoice 数据操作接口初始化失败");
		exit(0);
	}

    _rel_tax_invoice_op = new DCacheOp("rel_tax_invoice");
    if(!_rel_tax_invoice_op->Init(config))
	{
		LOG_ERROR("_rel_tax_invoice_op 数据操作接口初始化失败");
		exit(0);
	}

    _rel_user_invoice_op = new DCacheOp("u_rel_user_inovice");
    if(!_rel_user_invoice_op->Init(config))
	{
		LOG_ERROR("_rel_user_invoice_op 数据操作接口初始化失败");
		exit(0);
	}

    _travel_apply_op = new DCacheOp("travel_apply");
    if(!_travel_apply_op->Init(config))
	{
		LOG_ERROR("_travel_apply_op 数据操作接口初始化失败");
		exit(0);
	}

    _travel_item_op = new DCacheOp("travel_item");
    if(!_travel_item_op->Init(config))
	{
		LOG_ERROR("_travel_item_op 数据操作接口初始化失败");
		exit(0);
	}

    Communicator comm;
    //comm.stringToProxy("userCenter.userAuthSvr.userauthObj@tcp -h 10.154.157.157 -p 21005",_userInterface_prx);
    //_userInterface_prx = CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy<protocol::UserInterfacePrx>("userCenter.userAuthSvr.userauthObj@tcp -h 10.154.157.157 -p 21005");
    _userInterface_prx = CommunicatorFactory::getInstance()->getCommunicator()->stringToProxy<protocol::UserInterfacePrx>("userCenter.userAuthSvr.userauthObj@tcp -h 10.22.32.9 -p 10004");
    
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

int main(int argc,char * argv[])
{
    Communicator comm;

    std::string source_id = argv[1];
    cout << "arg count:" << argc << " argv1:" << argv[1] << endl;
    init();
    DeleteAllApply(source_id);

    if(3 ==  argc)
    {
        std::string argv2 = argv[2];

        if("delinvoice" == argv2)
        {
            std::string user_id;
            std::string auth_time;
            if(WhetherAuth(source_id,user_id,auth_time))
            {
                std::vector<std::map<std::string,std::string>> rsp;
                LOG_DEBUG("user_id:" << user_id);
                if(QueryAllUserRel(user_id,rsp))
                {
                    for(auto &item : rsp)
                    {
                        DeltelUserRel(item,user_id);
                    }
                }
            }
        }
    }
    
    
    return 0;
}

//通过ETC的source_id判断是否授权 如果授权,返回小程序的user_id
bool WhetherAuth(const std::string &source_id,std::string &user_id,std::string &auth_time)
{
    bool b_ret = false;
    protocol::AuthInfoReq req;
    protocol::AuthInfoRsp rsp;
    
    __BEGIN_PROC__
    try
    {
        req.set_source("ETCZHUSHOU");
        req.set_source_id(source_id);

        LOG_DEBUG("source_id:" << source_id);
        rsp = _userInterface_prx->GetAuthInfo(req);
        LOG_DEBUG("GetAuthInfo end:");
        if(0 != rsp.code())
        {
            LOG_ERROR("WhetherAuth fail:");
            break;
        }
        
        auto item = rsp.data();
        LOG_DEBUG("is_auth:" << item.is_auth() << " auth_time:" << item.auth_time() << " uid:" << item.uid());
        if(0 == item.is_auth())
            break;
        
        user_id = item.uid();
        auth_time = item.auth_time();
        b_ret = true;
    }
    catch(const std::exception& e)
    {
        LOG_ERROR("WhetherAuth error:"  << e.what());
        break;        
    }

    __END_PROC__
    return b_ret;
}