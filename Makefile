
-include .config
-include version.mk

# set build utilities 
CXX := g++
AR := ar
MKDIR := mkdir
PY:=python
PIP:=pip
TOOL_DIR:=./tools
CONFIG_PY:= ./tools/jconfigpy/jconfigpy.py

LIB_DIR=$(LDIR-y:%=-L%)
LIB=$(SLIB-y:%=-l:%) -lpthread

# Dev build library object
DBG_STATIC_TARGET=libmpiped.a
DBG_DYNAMIC_TARGET=libmpiped.so

# Release build library object
REL_STATIC_TARGET=libmpipe.a
REL_DYNAMIC_TARGET=libmpipe.so

CONFIG_ENTRY:=./config.json
CONFIG_TARGET:=./.config
CONFIG_AUTOGEN=./$(AUTOGEN_DIR)/autogen.h

# Unit Testing Target
DBG_UT_TARGET=mpipe_testd_v$(VER_MAJOR).$(VER_MINOR)
REL_UT_TARGET=mpipe_testr_v$(VER_MAJOR).$(VER_MINOR)

# C++ Flags for release build
REL_CXXFLAGS :=	-O2 -g -Wall -fmessage-length=0 -pthread

# C++ Flags for Dev build (Note : __DBG Macro can be used to detect build context)
DBG_CXXFLAGS :=  -O0 -g3 -Wall -fmessage-length=0 -D__DBG -pthread
DYN_FLAGS := -fPIC
 
AUTOGEN_DIR=autogen
DBG_OBJ_CACHE=Debug
REL_OBJ_CACHE=Release

#objects harvested through whole source tree depending on its configuration
DBG_OBJS = $(OBJ-y:%=$(DBG_OBJ_CACHE)/%.do)
REL_OBJS = $(OBJ-y:%=$(REL_OBJ_CACHE)/%.o)

DBG_SH_OBJS=$(OBJ-y:%=$(DBG_OBJ_CACHE)/%.s.do)
REL_SH_OBJS=$(OBJ-y:%=$(REL_OBJ_CACHE)/%.s.o)

DBG_UT_OBJS = $(OBJ-UT:%=$(DBG_OBJ_CACHE)/%.do)
REL_UT_OBJS = $(OBJ-UT:%=$(REL_OBJ_CACHE)/%.o)

.PHONY = $(PHONY)
.SILENT : $(SILENT)

PHONY= all clean debug release utest


SILENT= all  \
	debug\
	release \
	$(OBJ-y:%=Debug/%.do) \
	$(OBJ-y:%=Release/%.o) \
	$(DBG_STATIC_TARGET) \
	$(DBG_DYNAMIC_TARGET) \
	$(REL_STATIC_TARGET) \
	$(REL_DYNAMIC_TARGET) \
	$(DBG_OBJS) \
	$(REL_OBJS)

VPATH=$(SRC-y)
INC=$(INC-y:%=-I%)

all : debug devtest

ifeq ($(DEFCONF),)
config : $(AUTOGEN_DIR) $(CONFIG_PY)
	$(PY) $(CONFIG_PY) -c -i $(CONFIG_ENTRY) -o $(CONFIG_TARGET) -g $(CONFIG_AUTOGEN)
else
config : $(AUTOGEN_DIR) $(CONFIG_PY)
	$(PY) $(CONFIG_PY) -s -i $(CONFIG_DIR)/$(DEFCONF) -t $(CONFIG_ENTRY) -o $(CONFIG_TARGET) -g $(CONFIG_AUTOGEN)
endif


debug : $(DBG_OBJ_CACHE) $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) 

release :$(REL_OBJ_CACHE) $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET)

devtest : $(DBG_OBJ_CACHE) $(DBG_UT_TARGET)

reltest : $(REL_OBJ_CACHE) $(REL_UT_TARGET)

$(DBG_UT_TARGET) : $(DBG_OBJS) $(DBG_UT_OBJS) 
	@echo "Build unit test target...$@"
	$(CXX) -o $@ $(DBG_OBJS) $(DBG_UT_OBJS) $(INC) $(LIB_DIR) $(LIB) 

$(DBG_STATIC_TARGET): $(DBG_OBJS)
	@echo "Build static archive...$@"
	$(AR) rcs -o $@ $(DBG_OBJS)
	
$(DBG_DYNAMIC_TARGET): $(DBG_SH_OBJS)
	@echo "Build so target...$@"
	$(CXX) -o $@  -shared $(DBG_CXXFLAGS) $(DYN_FLAGS) $(DBG_SH_OBJS) 

$(REL_UT_TARGET) : $(REL_OBJS) $(REL_UT_OBJS)
	@echo "Build unit test target...$@"
	$(CXX) -o $@ $(REL_OBJS) $(REL_UT_OBJS) $(INC) $(LIB_DIR) $(LIB)
	
$(REL_STATIC_TARGET): $(REL_OBJS)
	@echo "Build static archive...$@"
	$(AR) rcs -o $@ $(REL_OBJS)
	
$(REL_DYNAMIC_TARGET): $(REL_SH_OBJS)
	@echo "Build so target...$@"
	$(CXX) -o $@ -shared $(REL_CXXFLAGS) $(DYN_FLAGS) $(REL_SH_OBJS) 
	
$(CONFIG_PY):
	@echo "Installing jconfigpy into $(TOOL_DIR)"
	$(PIP) install jconfigpy -t $(TOOL_DIR)
	

$(DBG_OBJ_CACHE)/%.do:%.cpp
	@echo "Compile... $@"
	$(CXX) -c -o $@ $(DBG_CXXFLAGS) $(INC) $< $(LIB_DIR) $(LIB)
	
$(REL_OBJ_CACHE)/%.o:%.cpp
	@echo "Compile... $@"
	$(CXX) -c -o $@ $(REL_CXXFLAGS) $(INC) $< $(LIB_DIR) $(LIB)
	
$(DBG_OBJ_CACHE)/%.s.do:%.cpp
	@echo "Compile... $@"
	$(CXX) -c -o $@ $(DYN_FLAGS) $(DBG_CXXFLAGS) $(INC) $< $(LIB_DIR) $(LIB)
	
$(REL_OBJ_CACHE)/%.s.o:%.cpp
	@echo "Compile... $@"
	$(CXX) -c -o $@ $(DYN_FLAGS) $(REL_CXXFLAGS) $(INC) $< $(LIB_DIR) $(LIB)

$(DBG_OBJ_CACHE) $(REL_OBJ_CACHE) $(AUTOGEN_DIR) :
	@echo "MKDIR... $@"
	$(MKDIR) $@



clean:
	rm -rf $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(DBG_OBJS) $(DBG_UT_TARGET)\
		 $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET) $(REL_OBJS) $(REL_UT_TARGET)\
		$(DBG_OBJ_CACHE) $(REL_OBJ_CACHE)
		
config_clean:
	rm -rf $(CONFIG_TARGET) $(CONFIG_AUTOGEN) $(AUTOGEN_DIR) $(REPO-y) $(LDIR-y) .config 
