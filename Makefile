CC=gcc
CFLAGS=-g

all : monitor consumer producer

monitor: monitor_source.c
	$(CC) -o $@ $^ $(CFLAGS)
consumer: consumer_source.c
	$(CC) -o $@ $^ $(CFLAGS)
producer: producer_source.c
	$(CC) -o $@ $^ $(CFLAGS)
clean:
	rm monitor consumer producer *.txt
