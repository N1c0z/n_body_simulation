gcc -c  physics.c -o physics.o
gcc main.c physics.o -I include/ -L lib/ -l:libraylib.a -lm -lpthread -ldl -lrt
