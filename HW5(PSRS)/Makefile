CC = mpicc
CFLAGS = -fopenmp
LDFLAGS =

TARGET = psrs

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
