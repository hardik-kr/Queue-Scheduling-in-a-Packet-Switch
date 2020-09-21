# the compiler to use
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall
  
# the name to use for both the target source file, and the output file:
TARGET = Q1
EXEC = routing
  
all: $(TARGET)
  
$(TARGET): $(TARGET).cpp
	$(CC) -o $(EXEC) $(TARGET).cpp
	./$(EXEC) 8 4 0.5 INQ 4 Output.txt 10000
