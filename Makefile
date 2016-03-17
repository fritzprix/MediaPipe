AR := ar

REL_CXXFLAGS :=	-O2 -g -Wall -fmessage-length=0
DBG_CXXFLAGS :=  -O0 -g3 -Wall -fmessage-length=0
DYN_FLAGS := -shared -fPIC 

OBJS := FLVTag MediaStream 
UTEST_TARGET :=  mpipe_test

DBG_OBJS = $(OBJS:%=%.do)
REL_OBJS = $(OBJS:%=%.o)


LIBS := 

DBG_STATIC_TARGET :=	libmpiped.a
DBG_DYNAMIC_TARGET :=   libmpiped.so

REL_STATIC_TARGET := 	libmpipe.a
REL_DYNAMIC_TARGET:=	libmpipe.so


.PHONY = all clean utest 

all : debug

debug : $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(UTEST_TARGET)

$(DBG_STATIC_TARGET): $(DBG_OBJS)
	$(AR) rcs -o $@ $(DBG_OBJS)
	
$(DBG_DYNAMIC_TARGET): $(DBG_OBJS)
	$(CXX) -o $@ $(DBG_CXXFLAGS) $(DYN_FLAGS) $(DBG_OBJS)
	
$(UTEST_TARGET) : $(UTEST_TARGET:%=%.do)
	$(CXX) -c -o $@ $(DBG_CXXFLAGS) $(DBG_OBJS)
	
	

%.do:%.cpp
	$(CXX) -c -o $@ $(DYN_FLAGS) $(DBG_CXXFLAGS) $<
	
%.o:%.cpp
	$(CXX) -c -o $@ $(DYN_FLAGS) $(REL_CXXFLAGS) $<

clean:
	rm -f $(DBG_STATIC_TARGET) $(DBG_DYNAMIC_TARGET) $(DBG_OBJS)\
		  $(REL_STATIC_TARGET) $(REL_DYNAMIC_TARGET) $(REL_OBJS) $(UTEST_TARGET)
