#Compiler Directives
CXX=g++
BOOST_DIR  = /home/michael/Development/boost_1_57_0 
THRIFT_DIR = /usr/local/include/thrift
CPPFLAGS   = -I$(BOOST_DIR)\
		     -I$(THRIFT_DIR)\
			 -Ibackend 
LDFLAGS    = -L/usr/local/lib\
			 -L/usr/lib
LIBS       = -lthrift \
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
USR_SRC= CppServer.cpp \

GEN_LNG_SRC = $(shell if [ -d $(GEN_CPP_DIR) ]; then ls $(GEN_CPP_DIR)/*.cpp | grep -v '.skeleton.cpp'; fi)
GEN_SRC = $(notdir $(GEN_LNG_SRC))
SRC    = $(USR_SRC) $(GEN_SRC)
_IDL=tutorial.thrift \
	shared.thrift
IDL=$(patsubst %,$(IDL_DIR)/%,$(_IDL))
IDL_SEED=$(patsubst %,$(IDL_DIR)/%,tutorial.thrift)
_OBJ=$(SRC:.cpp=.o)
#Generated objs
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))
_DEPS= 
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
	$(Q)thrift -r --gen cpp $(IDL_SEED)
	$(Q)#This hack allows me to restart make, and recompute globals on first gen
	$(Q)if [ ! -d $(GEN_CPP_DIR) ]; then mv gen-cpp backend && make; else rm -rf gen-cpp;	fi
	$(Q)thrift -r --gen js  $(IDL_SEED)
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
