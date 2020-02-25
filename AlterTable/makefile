APP         :=Altertable
TARGET      :=Altertable
CONFIG      :=
STRIP_FLAG  := N

OTHER_SRC_DIR := /home/puff/project/yunpiao/server/mysqlUtil
LOCAL_SRC += $(sort $(wildcard $(OTHER_SRC_DIR)/*.cpp *.c))
INCLUDE   += -I$(OTHER_SRC_DIR)
LIB       += -lmysqlclient 
#-----------------------------------------------------------------------
include /home/puff/project/yunpiao/server/comm/comm.mk
include /home/puff/project/yunpiao/server/dbop/dbop.mk
include /home/puff/project/yunpiao/server/3rd/redox/redox.mk
include /home/puff/project/yunpiao/server/3rd/rapidjson/rapidjson.mk
include /home/puff/project/yunpiao/server/protocol/protocol.mk
include /usr/local/tars/cpp/makefile/makefile.tars
