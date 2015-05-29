test: test.o search_api.o search_kmp.o search_bm.o search_ac2.o
	gcc -o test test.o search_api.o search_kmp.o search_bm.o search_ac2.o

clean:
	rm *.o test
