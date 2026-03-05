prog=melonsh
objs=$(addsuffix .o,$(basename $(wildcard *.c)))

defines=-D_GNU_SOURCE
ccflags=-g -Wall -MMD $(defines)
ldflags=-g -lreadline -lncurses

.SUFFIXES:

%.o: %.c ; gcc -o $@ -c $< $(ccflags)

$(prog): $(objs) ; gcc -o $@ $^ $(ldflags)

.PHONY: clean run test valgrind

clean: ; rm -f $(prog) *.o *.d *.i

run: $(prog) ; ./$< $(args)

test: $(prog)
	Test/run

valgrind: $(prog)
	@for t in Test/Test_[0-9]*; do \
		[ -d "$$t" ] || continue; \
		[ -f "$$t/setup" ] && bash "$$t/setup"; \
		echo "=== $$t ==="; \
		valgrind --leak-check=full --show-leak-kinds=definite,indirect,possible \
			--suppressions=rl.supp \
			./$(prog) < "$$t/inp" 2>&1 | grep -v "^==.*==$$" | grep "^=="; \
		echo ""; \
	done

sinclude *.d