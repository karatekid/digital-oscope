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
			 
LDFLAGS    = -L/usr/local/lib\
			 -L$(BOOST_LIB)
LIBS       = -lthrift \
			 -ldwf \
			 -lboost_system \
			 -lboost_thread
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
SRC_W_HEAD= Device.cpp \
			AnalogInput.cpp \
			DigitalInput.cpp
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

#User Files
$(ODIR)/%.o: $(LDIR)/%.cpp $(DEPS)
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CPPFLAGS) -I$(IDIR)
#Generated Files TODO:(don't like how copying the same as userfile)
$(ODIR)/%.o: $(GEN_CPP_DIR)/%.cpp $(DEPS)
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CPPFLAGS) -I$(IDIR)

$(BINFILE):	$(OBJ) $(DEPS)
	$(E)Linking $@
	$(Q)$(CXX) -o $@ $^ $(CPPFLAGS) $(LIBS) $(LDFLAGS)

generate: $(IDL)
	$(E)Generating C++ and JS files from IDL
	$(Q)thrift -r --gen cpp $(IDL)
	$(Q)#This hack allows me to restart make, and recompute globals on first gen
	$(Q)if [ ! -d $(GEN_CPP_DIR) ]; then mv gen-cpp backend && make; else rm -rf gen-cpp;	fi
	$(Q)#This doesn't update the generated code on a .thrift change
	$(Q)thrift -r --gen js  $(IDL)
	$(Q)if [ ! -d $(GEN_JS_DIR) ]; then mv gen-js frontend; else rm -rf gen-js;	fi 

test: all
	$(Q)chromium-browser frontend/tutorial.html
	$(Q)./$(BINFILE)

clean:
	$(E)Removing Files
	$(Q)rm -f $(ODIR)/*.o $(BINFILE)
	$(Q)if [ -d $(ODIR) ]; then rmdir $(ODIR); fi

nuke: clean
	$(E)Removing generated code
	$(Q)rm -rf $(GEN_DIRS)
