APP         :=Altertable
TARGET      :=Altertable
CONFIG      :=
STRIP_FLAG  := N

OTHER_SRC_DIR := /home/puff/project/yunpiao/util/mysqlUtil
LOCAL_SRC += $(sort $(wildcard $(OTHER_SRC_DIR)/*.cpp *.c))
INCLUDE   += -I$(OTHER_SRC_DIR)
LIB       += -lmysqlclient 
#-----------------------------------------------------------------------
include /home/puff/project/yunpiao/util/comm/comm.mk
include /home/puff/project/yunpiao/util/dbop/dbop.mk
include /home/puff/project/yunpiao/3rd/redox/redox.mk
include /home/puff/project/yunpiao/3rd/rapidjson/rapidjson.mk
include /home/puff/project/yunpiao/protocol/protocol.mk
include /usr/local/tars/cpp/makefile/makefile.tars
