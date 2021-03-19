CFLAGS = -pthread -std=c11 -O0 -g3 -I src-c\
 -D_DEFAULT_SOURCE=__STRICT_ANSI__ -D_FORTIFY_SOURCE=2\
 -pedantic -pedantic-errors -Wall -Wextra -Werror -Wconversion -Wcast-align -Wcast-qual -Wdisabled-optimization -Wlogical-op\
 -Wmissing-declarations -Wmissing-include-dirs -Wredundant-decls -Wshadow -Wsign-conversion -Wswitch-default -Wundef\
 -Wwrite-strings -Wfloat-equal -fmessage-length=0

COMMON_SRCS =\
 src-c/chat.c\
 src-c/command_parser.c\
 src-c/net_buff.c\
 src-c/map_of_strings.c\
 src-c/preferences.c\
 src-c/set_of_strings.c

SRCS_EXE = $(COMMON_SRCS)\
 src-c/main.c

SRCS_TESTS = $(COMMON_SRCS)\
 test/command_parser_test.c\
 test/main.c\
 test/preferences_test.c\
 test/set_of_strings_test.c\
 test/tests_utils.c

OBJS_EXE   = $(SRCS_EXE:%c=BUILD/%o)
OBJS_TESTS = $(SRCS_TESTS:%c=BUILD/%o)

.PHONY: all
all: chat tests

.PHONY: validate
validate: tests
	@./tests

.PHONY: validate-valgrind
validate-valgrind: tests
	valgrind --leak-check=full --show-leak-kinds=all ./tests

.PHONY: clean
clean:
	rm -fr BUILD

chat: $(OBJS_EXE)
	gcc $^ -o $@ -pthread

tests: $(OBJS_TESTS)
	gcc $^ -o $@ -pthread

BUILD/%.o: %.c
	@mkdir -p $$(dirname $@)
	gcc $(CFLAGS) -c -MMD -MP -MF"$(@:%.o=%.d)" -MT $@ -o $@ $<

-include $(DEPS_EXE) $(DEPS_TESTS)
