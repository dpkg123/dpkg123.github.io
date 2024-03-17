.PHONY: all
all: httpd
LIBS = -lpthread -static #-lsocket
httpd: httpd.c
	gcc -g -W -Wall $(LIBS) -o $@ $<

.PHONY: clean
clean:
	rm httpd
