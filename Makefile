CC = gcc
CXX = g++

CXXFLAGS = -c -g -Wall -Wextra -pedantic
CCFLAGS = -c
LDFLAGS = 

CPP_SRC = \
	main.cpp \
	dcdhelper.cpp \
	pdbatom.cpp

CPP_SRC_OBJS = $(CPP_SRC:.cpp=.o)

CC_SRC = dcd.c
CC_SRC_OBJS = $(CC_SRC:.c=.o)

EXECUTABLE = main
# INSTALL_PATH = oec

$(EXECUTABLE): $(CPP_SRC_OBJS) $(CC_SRC_OBJS)
	$(CXX) $(CPP_SRC_OBJS) $(CC_SRC_OBJS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

.c.o:
	$(CC) $(CCFLAGS) $< -o $@

all: $(SOURCES) $(EXECUTABLE)

# put:
# 	mkdir -p $(INSTALL_PATH)
# 	cp $(EXECUTABLE) $(INSTALL_PATH)/
	
clean:
	rm -rf $(CPP_SRC_OBJS) $(CC_SRC_OBJS) $(EXECUTABLE)

# install: all put