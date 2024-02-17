checker/main: main.c
	gcc -o checker/main main.c

check: checker/main
	cd checker && ./checker.sh

.PHONY: clean
clean:
	rm -f checker/main
