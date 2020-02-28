#include <iostream>
#include <vector>
#include <string>
#include "servant/Application.h"
#include "comm_def.h"
#include "sqlOp.h"

using namespace std;
using namespace tars;

SqlOp _mysql_op;

void init(void)
{
    LOG_DEBUG("------------- init ");
    string conf_file = "conf.xml";
	TC_Config config;
	config.parseFile(conf_file);
    if(0 != _mysql_op.Connect(config))
	{
		 LOG_ERROR("MYSQL 初始化失败");
		 exit(0);
	}    
}

int main(int argc,char * argv[])
{
    std::string dbname;
    std::string tablename;
    int z = 0;
    int ret = 0;

    init();
    for(int i = 0;i < 32;i++)    
    {
        for(int j = 0;j < 32;j++)
        {
            //tablename = "relationship_" + std::to_string(i) + "." + "travel_apply_" + std::to_string(z);      
            tablename = "relationship_" + std::to_string(i) + "." + "u_rel_user_inovice_" + std::to_string(z);          	
						
            // std::string delete_sql = "delete from " + tablename + " where invoices = '';";
            // std::cout << "delete_sql:" << delete_sql << endl;
            // int ret = _mysql_op.Execute(delete_sql);
            // if(0 != ret)
            // {
            //     std::cout << "ret:" << ret << endl;
            //     std::cout << " Execute "<< delete_sql << "fail" << endl;
            //     exit(0);
            // }

            //std::string alter_sql = "ALTER table " + tablename + " modify invoices json not null;";
            std::string alter_sql = "ALTER table " + tablename + " add `ticket_date` int(11) NOT NULL DEFAULT '0' COMMENT '开票日期';";
            std::cout << "alter_sql:"<< alter_sql << endl;
            ret = 0;
            ret = _mysql_op.Execute(alter_sql);
            
            if(0 != ret)
            {
                std::cout << "ret:" << ret << endl;
                std::cout << " Execute "<< alter_sql << "fail" << endl;
                exit(0);
            }

            z++;
        }
    }
}