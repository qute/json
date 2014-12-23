
BIN ?= qjson
MAIN ?= main.c
SRC ?= $(filter-out $(MAIN), $(wildcard *.c))
DEPS ?= $(wildcard deps/*/*.c)
OBJS ?= $(SRC:.c=.o) $(DEPS:.c=.o)
TESTS ?= $(wildcard test/*.c)
PREFIX ?= /usr/local

LIB ?= libjson.a
CFLAGS += -Ideps -std=c99

$(BIN): $(LIB)
	$(CC) $(CFLAGS) $(DEPS) $(SRC) $(MAIN) -o $(@)

$(LIB): $(OBJS)
	ar crus $(@) $(OBJS)

$(OBJS):
	$(CC) $(CFLAGS) -c $(@:.o=.c) -o $(@)

test: $(TESTS)

.PHONY: $(TESTS)
$(TESTS):
	@$(CC) $(CFLAGS) -I. $(OBJS) $(@) -o $(@:.c=)
	@echo ">$(@:.c=)"
	@./$(@:.c=)

install: $(BIN)
	install $(BIN) $(PREFIX)/bin
	install $(LIB) $(PREFIX)/lib
	install json.h $(PREFIX)/include

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
	rm -f $(PREFIX)/lib/$(LIB)
	rm -f $(PREFIX)/include/json.h

clean:
	rm -f $(TESTS:.c=)
	rm -f $(BIN)
	rm -f $(LIB)
	rm -f $(OBJS)

.PHONY: $(BIN) $(LIB) $(OBJS) test $(TESTS)
