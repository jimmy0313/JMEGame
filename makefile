# This is a general makefile template.

                               BIN_DIR = ./
                                TARGET = $(BIN_DIR)/libJMEGame.a
                              C_CFLAGS = 
                                MACROS = -D_REENTRANT -D__USE_POSIX -D__linux__ -DLINUX
                                    CC = gcc
                            CPP_CFLAGS = -static
                                  MAKE = make
                                LINKER = g++
			 INCLUDE_DIRS = -I../third_party -I../third_party/google -I../third_party/mongo \
					-I../JMEngine/include -I../JMEngine/include/net -I../JMEngine/include/db -I../JMEngine/include/json \
                                        -I../JMEngine/include/log -I../JMEngine/include/memory -I../JMEngine/include/rpc -I../MEngine/include/server -I../JMEngine/include/util \
                                        -Iinclude/game -Iinclude/db \
					-I/usr/include -I/usr/include/mysql -I/boost_1_49_0 

                                  #LIBS = -lJMEngine -lboost_filesystem -lboost_system -lboost_thread -lboost_date_time -lprotobuf -lpthread -lmysqlclient_r -lz 

                            OPTIM_FLAG = 
                                   CPP = g++ -std=c++11
                                LFLAGS = 
                              LIB_DIRS = -L/usr/local/lib/ -L/usr/lib -L../JMEngine  
                                 VPATH = ./source ./source/db ./source/game
                            OBJECT_DIR = ./.obj/
                              CPPFILES = \
					 JME_MongoConnection.cpp \
					 JME_MysqlConnection.cpp \
					 JME_RPCManager.cpp \
					 JME_ServerInterface.cpp 

                                CFILES =  

# To use 'make debug=0' build release edition.
ifdef debug
	ifeq ("$(origin debug)", "command line")
		IS_DEBUG = $(debug)
	endif
else
	IS_DEBUG = 1
endif
ifndef IS_DEBUG
	IS_DEBUG = 1
endif
ifeq ($(IS_DEBUG), 1)
	OPTIM_FLAG += -g3
else
	MACROS += -DNDEBUG
endif

# To use 'make quiet=1' all the build command will be hidden.
# To use 'make quiet=0' all the build command will be displayed.
ifdef quiet
	ifeq ("$(origin quiet)", "command line")
		QUIET = $(quiet)
	endif
endif
ifeq ($(QUIET), 1)
	Q = @
else
	Q =
endif

OBJECTS := $(addprefix $(OBJECT_DIR), $(notdir $(CPPFILES:%.cpp=%.o)))
OBJECTS += $(addprefix $(OBJECT_DIR), $(notdir $(CFILES:%.c=%.o)))

CALL_CFLAGS := $(C_CFLAGS) $(INCLUDE_DIRS) $(MACROS) $(OPTIM_FLAG)
CPPALL_CFLAGS := $(CPP_CFLAGS) $(INCLUDE_DIRS) $(MACROS) $(OPTIM_FLAG)
LFLAGS += $(OPTIM_FLAG) $(LIB_DIRS) $(LIBS)

all: checkdir $(TARGET)

$(TARGET): $(OBJECTS)
	$(Q)ar -cr $@ $(OBJECTS)

$(OBJECT_DIR)%.o:%.cpp
	$(Q)$(CPP) $(strip $(CPPALL_CFLAGS)) -c $< -o $@

$(OBJECT_DIR)%.o:%.c
	$(Q)$(CC) $(strip $(CALL_CFLAGS)) -c $< -o $@

checkdir:
	@if ! [ -d "$(BIN_DIR)" ]; then \
		mkdir $(BIN_DIR) ; \
		fi
	@if ! [ -d "$(OBJECT_DIR)" ]; then \
		mkdir $(OBJECT_DIR); \
		fi
clean:
	$(Q)rm -f $(OBJECTS)
cleanall: clean
	$(Q)rm -f $(TARGET)

.PHONY: all clean cleanall checkdir
