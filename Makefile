main : main.o
	cc -o main main.o

main.o : main.c
	cc -c main.c -O3 

generate_data : generate_data.o
	cc -o generate_data generate_data.o

generate_data.o : generate_data.c
	cc -c generate_data.c

run:
	./main

data_gen :
	./generate_data

clean :
	rm main main.o generate_data generate_data.o
