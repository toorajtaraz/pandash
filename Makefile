PROGRAM = pandash
CC = gcc
BUILD = build
FILES = main.c utils/* 
CFLAGS = -lreadline -lpthread -o $(BUILD)/$(PROGRAM)

all: pandash

pandash:
	mkdir -p $(BUILD);
	$(CC) $(FILES) $(CFLAGS);

clean:
	rm -fr $(BUILD)

install:
	cp $(BUILD)/$(PROGRAM) /bin/$(PROGRAM);
	chmod 777 /bin/$(PROGRAM);

