SRC = ./src
OBJ = ./obj

all: obj_path kernel_stats.o arg_parse.o

kernel_stats.o: $(SRC)/kernel_stats.c $(SRC)/kernel_stats.h
	mpicc -c $(SRC)/kernel_stats.c -o $(OBJ)/kernel_stats.o

arg_parse.o: $(SRC)/arg_parse.c $(SRC)/arg_parse.h
	mpicc -c $(SRC)/arg_parse.c -o $(OBJ)/arg_parse.o

obj_path:
	mkdir $(OBJ)

clean:
	rm -rf $(OBJ)
