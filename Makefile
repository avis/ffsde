LDFLAGS=-Wl,-rpath,/usr/lib64/nss -Wl,-rpath,/usr/lib64 -Wl,--rpath -Wl,/usr/lib64 -Lini -lnss3 -lsqlite3 -liniparser -L/usr/lib64 -lplds4 -lplc4 -lnspr4 -lpthread -ldl
INCLUDES=-I/usr/include/nss3 -I/usr/include/nspr4 -Iini
all:
	cd ini && make
#	gcc -Iini -I/usr/include/nss3 -I/usr/include/nspr4 -c ff_key3db_dump.c
	gcc -pg $(INCLUDES) -c ffsde.c
	gcc -pg -o ffsde ffsde.o $(LDFLAGS)
	gcc -pg -o ffsde $(INCLUDES) ffsde.c $(LDFLAGS)
#	gcc  $(LDFLAGS) -o ff_key3db_dump ff_key3db_dump.o  -lnspr4 -lnss3 -lsqlite3 -liniparser
#	gcc -o ff_key3db_dump -I/usr/include/nss -I/usr/include/nspr  $(LDFLAGS) ff_key3db_dump.c

