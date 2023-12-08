# Simple Makefile

# Rule for building the test_first_fit executable
test_first_fit:
	gcc -o test/test_first_fit test/test_first_fit.c first_fit.c test/unity/unity.c

.PHONY: clean

# Rule for cleaning up
clean:
	rm -f first_fit test/test_first_fit
