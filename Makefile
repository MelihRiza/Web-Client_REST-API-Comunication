client: client.cpp
	g++ -o client client.cpp -Wall

run: client
	./client

clean:
	rm -f *.o client