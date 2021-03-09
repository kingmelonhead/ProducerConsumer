CC=gcc
CFLAGS=-g -lm
LIBS=-l_mon
LIB_DIRS=-L.
all : monitor consumer producer 

monitor: monitor.o lib_mon.a
	$(CC) $(LIB_DIRS) -o $@ monitor.o $(LIBS) $(CFLAGS)

consumer: consumer.o lib_mon.a
	$(CC) $(LIB_DIRS) -o $@ consumer.o $(LIBS) $(CFLAGS)

producer: producer.o lib_mon.a
	$(CC) $(LIB_DIRS) -o $@ producer.o $(LIBS) $(CFLAGS)

lib_mon.o: lib_mon.c 
	$(CC) -c -O $^

consumer.o: consumer.c 
	$(CC) -c -O $^

producer.o: producer.c 
	$(CC) -c -O $^

monitor.o: monitor.c 
	$(CC) -c -O $^ 

lib_mon.a: lib_mon.o
	ar rcs $@ $^

clean:
	rm monitor consumer producer *.txt *.o *.a *.log
