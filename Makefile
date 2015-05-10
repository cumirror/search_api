test: test.o search_api.o search_kmp.o
	gcc -o test test.o search_api.o search_kmp.o

clean:
	rm *.o test
