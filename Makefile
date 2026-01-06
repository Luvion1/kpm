CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -D_DEFAULT_SOURCE -D_BSD_SOURCE -I src/pkg -I src/util
SRC = src/pkg/unified_main.c \
      src/pkg/unified_package_manager.c \
      src/pkg/stateless_registry.c \
      src/pkg/kmod_parser.c \
      src/pkg/visual_utils.c \
      src/util/utils.c

TARGET = kpm
PREFIX = /usr/local

TEST_SRC = tests/test_parser.c \
           src/pkg/kmod_parser.c \
           src/util/utils.c
TEST_TARGET = run_tests

.PHONY: all clean install uninstall test benchmark

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: $(TEST_SRC)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC)
	./$(TEST_TARGET)
	rm -f $(TEST_TARGET)

benchmark: all
	@echo "Running performance benchmark..."
	@time ./kpm help > /dev/null
	@echo "Benchmark complete."

clean:
	rm -f $(TARGET) $(TEST_TARGET) kpm-x86_64 kpm-aarch64 *.o

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin
	# Install Man Page
	install -d $(DESTDIR)$(PREFIX)/share/man/man1
	install -m 644 kpm.1 $(DESTDIR)$(PREFIX)/share/man/man1
	# Install Bash Completion
	install -d $(DESTDIR)$(PREFIX)/share/bash-completion/completions
	install -m 644 completions/kpm.bash $(DESTDIR)$(PREFIX)/share/bash-completion/completions/kpm

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	rm -f $(DESTDIR)$(PREFIX)/share/man/man1/kpm.1
	rm -f $(DESTDIR)$(PREFIX)/share/bash-completion/completions/kpm