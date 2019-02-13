all:server client
client:client.c client.h 
	gcc $^ -o $@
server:server.c
	gcc $^ -o $@ -lsqlite3 -lpthread
	
.PHONY:clean
clean:
	rm -rf *.o server client *.db
