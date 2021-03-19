CFLAGS = -pthread -std=c11 -O0 -g3 -I src-c -fvisibility=hidden -D_DEFAULT_SOURCE=__STRICT_ANSI__ -D_FORTIFY_SOURCE=2 -fPIC\
 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion -Wcast-align -Wcast-qual -Wdisabled-optimization -Wlogical-op\
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef\
 -Wwrite-strings -Wfloat-equal -fmessage-length=0

SRCS_LIB =\
 src-c/chat/chat.c\
 src-c/chat/command_parser.c\
 src-c/utils/net_buff.c\
 src-c/utils/map_of_strings.c\
 src-c/utils/preferences.c\
 src-c/utils/set_of_strings.c

SRCS_EXE = src-c/main.c
SRCS_TESTS =\
 test/command_parser_test.c\
 test/main.c\
 test/preferences_test.c\
 test/set_of_strings_test.c\
 test/tests_utils.c

OBJS_LIB   = $(SRCS_LIB:%c=BUILD/%o)
OBJS_EXE   = $(SRCS_EXE:%c=BUILD/%o)
OBJS_TESTS = $(SRCS_TESTS:%c=BUILD/%o)
DEPS_LIB   = $(SRCS_LIB:%c=BUILD/%d)
DEPS_EXE   = $(SRCS_EXE:%c=BUILD/%d)
DEPS_TESTS = $(SRCS_TESTS:%c=BUILD/%d)

.PHONY: all
all: libchat.so chat tests

.PHONY: validate
validate: tests
	@LD_LIBRARY_PATH=. ./tests

.PHONY: validate-valgrind
validate-valgrind: tests
	LD_LIBRARY_PATH=. valgrind --leak-check=full --show-leak-kinds=all ./tests

.PHONY: clean
clean:
	rm -fr BUILD bin depcache build Debug Release

libchat.so: $(OBJS_LIB)
	gcc $^ -shared -o $@

chat: $(OBJS_EXE) libchat.so
	gcc $(OBJS_EXE) -o $@ -pthread -L. -lchat

tests: $(OBJS_TESTS) libchat.so
	gcc $(OBJS_TESTS) -o $@ -pthread -L. -lchat

BUILD/%.o: %.c
	@mkdir -p $$(dirname $@)
	gcc $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

-include $(DEPS_EXE) $(DEPS_TESTS)
