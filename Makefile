CC=gcc
FLAGS=-O2

TARGET=steady-state_GA
OBJS=steady-state_GA.o

$(TARGET): $(OBJS) 
	$(CC) $(FLAGS) -o $(TARGET) $(OBJS) -lm

%.o: %.c
	$(CC) -c $<

clean:
	rm $(TARGET) $(OBJS)
