test_allocators:
	gcc -o test/test_allocators test/test_allocators.c allocators.c test/unity/unity.c

.PHONY: clean

clean:
	rm -f allocators test/test_allocators