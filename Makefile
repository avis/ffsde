LDFLAGS=-Wl,-rpath,/usr/lib64/nss -Wl,-rpath,/usr/lib64 -Wl,--rpath -Wl,/usr/lib64 -Lini -lnss3 -lsqlite3 -liniparser -L/usr/lib64 -lplds4 -lplc4 -lnspr4 -lpthread -ldl
INCLUDES=-I/usr/include/nss3 -I/usr/include/nspr4 -Iini
all:
	cd ini && make
	gcc -pg $(INCLUDES) -c ffsde.c
	gcc -pg -o ffsde ffsde.o $(LDFLAGS)
	gcc -pg -o ffsde $(INCLUDES) ffsde.c $(LDFLAGS)

clean:

	rm -rf ffsde
	rm -rf *.o
	rm -rf ini/*.o
	rm -rf ini/*.so
	rm -rf gmon.out