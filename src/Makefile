ZOOKEEPER_DIR = /usr/local
LIB_ZKCPP_NAME = libzkcpp.a

CXX = g++
RM = rm -f
CXXFLAGS += -Wall -g 
AR = ar

LIBS += ./$(LIB_ZKCPP_NAME) \
        $(ZOOKEEPER_DIR)/lib/libzookeeper_st.a \
        -lrt -lz

INCS = -I. \
       -I$(ZOOKEEPER_DIR)/include

all: $(LIB_ZKCPP_NAME)

$(LIB_ZKCPP_NAME): zk_cpp.o
		$(AR) curs $@ $^ 
    
example: example.o
		$(CXX) $^ $(LIBS) -o $@

%.o: %.cpp
		$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@
%.o: %.c
		$(CXX) -c $(CXXFLAGS) $(INCS) $< -o $@

clean:
		$(RM) *.o $(LIB_ZKCPP_NAME) example

