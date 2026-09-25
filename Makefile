CFLAGS   = -std=gnu11 -w -DRPC_SVC_FG -Wno-incompatible-pointer-types -Wno-implicit-function-declaration
CXXFLAGS = -std=c++17 -w
CC  = gcc
CXX = g++

TARGETS = server client
SOURCES_SERVER = fileops_server.cpp fileops_svc.c fileops_xdr.c
SOURCES_CLIENT = fileops_client.cpp fileops_clnt.c fileops_xdr.c

RPCGEN_OUTPUTS = fileops.h fileops_xdr.c fileops_clnt.c fileops_svc.c

all: $(RPCGEN_OUTPUTS) $(TARGETS)

$(RPCGEN_OUTPUTS): fileops.x
	rpcgen fileops.x

server: $(SOURCES_SERVER)
	$(CC)  $(CFLAGS)   -c fileops_svc.c       -o fileops_svc.o
	$(CC)  $(CFLAGS)   -c fileops_xdr.c       -o fileops_xdr.o
	$(CXX) $(CXXFLAGS) -c fileops_server.cpp  -o fileops_server.o
	$(CXX) $(CXXFLAGS) fileops_server.o fileops_svc.o fileops_xdr.o -o server

client: $(SOURCES_CLIENT)
	$(CC)  $(CFLAGS)   -c fileops_clnt.c      -o fileops_clnt.o
	$(CC)  $(CFLAGS)   -c fileops_xdr.c       -o fileops_xdr.o
	$(CXX) $(CXXFLAGS) -c fileops_client.cpp  -o fileops_client.o
	$(CXX) $(CXXFLAGS) fileops_client.o fileops_clnt.o fileops_xdr.o -o client

clean:
	rm -f $(TARGETS) $(RPCGEN_OUTPUTS)
	rm -f *.o
	rm -rf server_files/

.PHONY: all clean
