TARGET=main
CC=g++
LDFLAGS= -pthread -I /usr/local/boost_1_46_1/ -std=gnu++0x -O0
LIBS= -L /usr/local/boost_1_46_1/stage/lib/ -lboost_system -lboost_thread -lboost_date_time

all : clean $(TARGET) x

$(TARGET) : argsparser.o client.o resultsAggregator.o server.o session.o configuration_data.o ReadAggregatorFunctor.o Response.o 
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $@.cpp $^ $(INCLDIR)

argsparser.o : argsparser.cpp
	$(CC) $(LDFLAGS) -c $<

client.o : client.cpp
	$(CC) $(LDFLAGS) -c $<

resultsAggregator.o : resultsAggregator.cpp
	$(CC) $(LDFLAGS) -c $<

server.o : server.cpp
	$(CC) $(LDFLAGS) -c $<

session.o :  session.cpp
	$(CC) $(LDFLAGS) -c $<

configuration_data.o :  configuration_data.cpp
	$(CC) $(LDFLAGS) -c $<

ReadAggregatorFunctor.o :  ReadAggregatorFunctor.cpp
	$(CC) $(LDFLAGS) -c $<

Response.o :  Response.cpp
	$(CC) $(LDFLAGS) -c $<

x : $(TARGET)
	./$(TARGET) 81 50 1

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.gch

