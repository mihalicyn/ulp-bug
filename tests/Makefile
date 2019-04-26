all:
	$(MAKE) ulp
	$(MAKE) vanilla

vanilla:
	gcc -g -Wall --std=gnu99 -o vanilla.out vanilla.c -O2

ulp:
	gcc -g -Wall --std=gnu99 -o ulp.out ulp.c -O2

clean:
	rm -rf *.out
