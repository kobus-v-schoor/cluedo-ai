ifeq ($(wildcard last_build),)
 ifndef gf
  gf = -std=c++11 -g -Wall
 endif
 ifndef go
  go = g++ -c $(gf)
 endif
else
 ifeq ($(shell cat last_build),debug)
  ifndef gf
   gf = -std=c++11 -g -Wall
  endif
  ifndef go
   go = g++ -c $(gf)
  endif
 else
  ifndef gf
   gf = -std=c++11 -O3 -DAI_RELEASE
  endif
  ifndef go
   go = g++ -c $(gf)
  endif
 endif
endif

SHELL = /bin/bash
.PHONY: auto debug release run gdb clean tar

auto: | last_build
	if [[ $$(cat last_build) == debug ]]; then \
		$(MAKE) debug; \
	else \
		$(MAKE) release; \
	fi

debug: | last_build
	export gf="-std=c++11 -g -Wall"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != debug ]] && $(MAKE) clean; \
	echo debug > last_build; \
	$(MAKE) test

release: | last_build
	export gf="-std=c++11 -O3 -DAI_RELEASE"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != release ]] && $(MAKE) clean; \
	echo release > last_build; \
	$(MAKE) test

last_build:
	echo debug > last_build

test: \
 test.o \
 tests/board.o \
 tests/bot.o \
 tests/position.o \
 src/board.o \
 src/bot.o \
 src/position.o
	g++ $(gf) test.o tests/board.o tests/bot.o tests/position.o src/board.o src/bot.o src/position.o -o test

test.o: \
 test.cpp \
 include/bot.h \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) test.cpp -o test.o

tests/board.o: \
 tests/board.cpp \
 include/board.h
	$(go) tests/board.cpp -o tests/board.o

tests/bot.o: \
 tests/bot.cpp \
 include/bot.h \
 include/tests.h \
 include/macros.h \
 include/position.h
	$(go) tests/bot.cpp -o tests/bot.o

tests/position.o: \
 tests/position.cpp \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) tests/position.cpp -o tests/position.o

src/board.o: \
 src/board.cpp \
 include/board.h
	$(go) src/board.cpp -o src/board.o

src/bot.o: \
 src/bot.cpp \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/bot.cpp -o src/bot.o

src/position.o: \
 src/position.cpp \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) src/position.cpp -o src/position.o

run: $(shell [[ -f last_build ]] && cat last_build || echo debug) | last_build
	./test

gdb: debug
	gdb test

clean:
	rm -f test.o tests/board.o tests/bot.o tests/position.o src/board.o src/bot.o src/position.o ai.tar.gz test

tar:
	tar -chvz test.cpp include/bot.h include/position.h include/board.h include/macros.h tests/board.cpp tests/bot.cpp include/tests.h tests/position.cpp src/board.cpp src/bot.cpp src/position.cpp makefile -f ai.tar.gz

doc:
	doxygen doxyfile

.PHONY: coverage
coverage:
	export gf="-std=c++11 -g -Wall --coverage -fprofile-arcs -ftest-coverage"; \
	export go="g++ -c $$gf"; \
	$(MAKE) clean; \
	$(MAKE) test; \
	./test; \
	gcov -r src/*.cpp; \
	lcov --directory src/ --capture --output-file coverage.info; \
	lcov --remove coverage.info -o coverage.info '/usr/include/*'; \
	lcov --list coverage.info; \
	echo coverage > last_build; \
	find . -name '*.gcda' -delete -or -name '*.gcno' -delete; \
	[[ -d coverage ]] && rm -rf coverage; \
	genhtml coverage.info --output-directory coverage; \
	rm coverage.info
