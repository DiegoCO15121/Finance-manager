CC=g++
CFLAGS=-c -O2 -Wall -std=c++17 -MMD
INCLUDES= -I./include -I./include/libpqxx/include -I"C:\Program Files\PostgreSQL\16\include"
LDFLAGS=-L./include/libpqxx/lib -L"C:/Program Files/PostgreSQL/16/lib" -lpqxx -lpq -lws2_32

# Especifica la carpeta de salida para el ejecutable
BINDIR=bin

all: $(BINDIR) Finances

# Crea la carpeta de salida si no existe
$(BINDIR):
	mkdir $(BINDIR)

Finances: main.o db.o utils.o interfaces.o
	$(CC) main.o db.o utils.o interfaces.o $(LDFLAGS) -o "$(BINDIR)\Finances.exe"

main.o: src/main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) src/main.cpp -o main.o 

db.o: src/db.cpp
	$(CC) $(CFLAGS) $(INCLUDES) src/db.cpp -o db.o

utils.o: src/utils.cpp
	$(CC) $(CFLAGS) $(INCLUDES) src/utils.cpp -o utils.o

interfaces.o: src/interfaces.cpp
	$(CC) $(CFLAGS) $(INCLUDES) src/interfaces.cpp -o interfaces.o

-include *.d

clean:
	del /Q *.o *.d "$(BINDIR)\Finances.exe"
