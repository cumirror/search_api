srcs := $(wildcard *.c)
test:
	gcc -o test $(srcs)

clean:
	rm -rf *.o test
