prog=shell

ldflags:=-lreadline -lncurses -L. -ldeq -Wl,-rpath=.

include ../GNUmakefile

try: $(objs) libdeq.so
	gcc -o $@ $(objs) $(ldflags)

trytest: try
	Test/run

test: $(prog)
	Test/run

.PHONY: valgrind
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