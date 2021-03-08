CC=gcc
CFLAGS=-g

all : monitor consumer producer

monitor: monitor.c library_for_monitor.c
	$(CC) -o $@ $^ $(CFLAGS)
consumer: consumer.c library_for_monitor.c
	$(CC) -o $@ $^ $(CFLAGS)
producer: producer.c library_for_monitor.c
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm monitor consumer producer *.txt
