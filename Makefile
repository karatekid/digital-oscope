#Compiler Directives
CXX=g++
BOOST_DIR  = /home/michael/Development/boost_1_57_0
BOOST_LIB  = $(BOOST_DIR)/stage/lib
THRIFT_DIR = /usr/local/include/thrift
CPPFLAGS   = -I$(BOOST_DIR)\
		     -I$(THRIFT_DIR)\
			 -Ibackend \
			 -std=c++11 \
			 -Wl,-rpath=$(BOOST_LIB) #Instead of setting LD_LIBRARY_PATH 
TEST_CPPFLAGS   = -I$(THRIFT_DIR)\
			      -Ibackend \
			      -std=c++11
			 
LDFLAGS    = -L/usr/local/lib\
			 -L$(BOOST_LIB)
TEST_LDFLAGS = -L/usr/local/lib
LIBS       = -lthrift \
			 -ldwf \
			 -lboost_system \
			 -lboost_thread
TEST_LIBS  = -lthrift \
			 -ldwf
#Directories
IDIR=backend/include
LDIR=backend/lib
ODIR=backend/obj
IDL_DIR=idl
GEN_CPP_DIR = backend/gen-cpp
GEN_JS_DIR = frontend/gen-js
GEN_DIRS = $(GEN_CPP_DIR)\
		   $(GEN_JS_DIR)
#Files
BINFILE= cppServer
TEST_BINFILE=tester
SRC_W_HEAD= Device.cpp \
			AnalogInput.cpp \
			DigitalInput.cpp \
			Input.cpp
USR_SRC= CppServer.cpp \
		 $(SRC_W_HEAD)

GEN_LNG_SRC = $(shell if [ -d $(GEN_CPP_DIR) ]; then ls $(GEN_CPP_DIR)/*.cpp | grep -v '.skeleton.cpp'; fi)
GEN_SRC = $(notdir $(GEN_LNG_SRC))
SRC    = $(USR_SRC) $(GEN_SRC)
_IDL=oscope.thrift
IDL=$(patsubst %,$(IDL_DIR)/%,$(_IDL))
_OBJ=$(SRC:.cpp=.o)
#Generated objs
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
_DEPS=$(SRC_W_HEAD:.cpp=.h) 
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

TEST_FILE= test.cpp 
TEST_SRC= $(TEST_FILE) \
		  $(GEN_SRC) \
		 $(SRC_W_HEAD)
TEST_OBJ=$(patsubst %,$(ODIR)/%,$(TEST_FILE:.cpp=.o))

#Flag Handling
ifdef DEBUG
CPPFLAGS := $(CPPFLAGS) -g
endif
ifdef VERBOSE
        Q =
        E = @true 
else
        Q = @
        E = @echo 
endif

.PHONY: all help clean
all: generate $(BINFILE)
help:
	$(E)
	$(E)This makefile creates a C++ server and a JS client
	$(E)These are used to communicate with the Digilent
	$(E)Analog Discovery board. This Makefiles commands are:
	$(E)
	$(E)help: 		To Display this printout
	$(E)all:  		To build everything
	$(E)generate:	To generate the code from an updated idl
	$(E)test:		To test the new files -in dev
	$(E)clean:		Removes all *.o files and executables
	$(E)nuke:		Removes all generated code, and cleans
	$(E)

#User Files w/ Header
$(ODIR)/%.o: $(LDIR)/%.cpp $(IDIR)/%.h
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(TEST_CPPFLAGS) -I$(IDIR)
$(ODIR)/%.o: $(LDIR)/%.cpp
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CPPFLAGS) -I$(IDIR)
$(TEST_OBJ): $(LDIR)/$(TEST_FILE)
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(TEST_CPPFLAGS) -I$(IDIR)

#Generated Files TODO:(don't like how copying the same as userfile)
$(ODIR)/%.o: $(GEN_CPP_DIR)/%.cpp $(GEN_CPP_DIR)/%.h
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(TEST_CPPFLAGS) -I$(IDIR)

$(BINFILE):	$(OBJ) $(DEPS)
	$(E)Linking $@
	$(Q)$(CXX) -o $@ $^ $(CPPFLAGS) $(LIBS) $(LDFLAGS)

$(TEST_BINFILE): $(filter-out backend/obj/CppServer.o, $(OBJ)) $(TEST_OBJ) $(DEPS)
	$(E)Linking $@
	$(Q)$(CXX) -o $@ $^ $(TEST_CPPFLAGS) $(TEST_LIBS) $(TEST_LDFLAGS)

generate: $(IDL)
	$(E)Generating C++ and JS files from IDL
	$(Q)thrift -r --gen cpp $(IDL)
	$(Q)#This hack allows me to restart make, and recompute globals on first gen
	$(Q)if [ ! -d $(GEN_CPP_DIR) ]; then mv gen-cpp backend && make; else rm -rf gen-cpp;	fi
	$(Q)#This doesn't update the generated code on a .thrift change
	$(Q)thrift -r --gen js  $(IDL)
	$(Q)if [ ! -d $(GEN_JS_DIR) ]; then mv gen-js frontend; else rm -rf gen-js;	fi 

test: generate $(TEST_BINFILE)
	$(Q)#chromium-browser frontend/tutorial.html
	$(Q)./$(TEST_BINFILE)

clean:
	$(E)Removing Files
	$(Q)rm -f $(ODIR)/*.o $(BINFILE)
	$(Q)if [ -d $(ODIR) ]; then rmdir $(ODIR); fi

rm_gen:
	$(E)Removing generated code
	$(Q)rm -rf $(GEN_DIRS)

nuke: clean rm_gen
