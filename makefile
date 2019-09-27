ifeq ($(wildcard last_build),)
 ifndef gf
  gf = -std=c++11 -g -Wall -DTESTING
 endif
 ifndef go
  go = g++ -c $(gf)
 endif
else
 ifeq ($(shell cat last_build),debug)
  ifndef gf
   gf = -std=c++11 -g -Wall -DTESTING
  endif
  ifndef go
   go = g++ -c $(gf)
  endif
 else
  ifndef gf
   gf = -std=c++11 -O3 -DTESTING -DAI_RELEASE
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
	export gf="-std=c++11 -g -Wall -DTESTING"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != debug ]] && $(MAKE) clean; \
	echo debug > last_build; \
	$(MAKE) test

release: | last_build
	export gf="-std=c++11 -O3 -DTESTING -DAI_RELEASE"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != release ]] && $(MAKE) clean; \
	echo release > last_build; \
	$(MAKE) test

last_build:
	echo debug > last_build

test: \
 test.o \
 tests/predictors/seen.o \
 tests/board.o \
 tests/bot.o \
 tests/deductors/no-show.o \
 tests/deductors/seen.o \
 tests/deductors/local-exclude.o \
 tests/position.o \
 src/predictors/seen.o \
 src/board.o \
 src/bot.o \
 src/deductors/no-show.o \
 src/deductors/seen.o \
 src/deductors/local-exclude.o \
 src/position.o
	g++ $(gf) test.o tests/predictors/seen.o tests/board.o tests/bot.o tests/deductors/no-show.o tests/deductors/seen.o tests/deductors/local-exclude.o tests/position.o src/predictors/seen.o src/board.o src/bot.o src/deductors/no-show.o src/deductors/seen.o src/deductors/local-exclude.o src/position.o -o test

test.o: \
 test.cpp \
 include/bot.h \
 include/position.h \
 include/board.h \
 include/deductors/local-exclude.h \
 include/deductors/no-show.h \
 include/deductors/seen.h \
 include/macros.h \
 include/deductor.h
	$(go) test.cpp -o test.o

tests/predictors/seen.o: \
 tests/predictors/seen.cpp \
 include/predictors/seen.h \
 include/predictor.h \
 include/bot.h \
 include/deck.h \
 include/macros.h \
 include/position.h
	$(go) tests/predictors/seen.cpp -o tests/predictors/seen.o

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

tests/deductors/no-show.o: \
 tests/deductors/no-show.cpp \
 include/deductors/no-show.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deductors/no-show.cpp -o tests/deductors/no-show.o

tests/deductors/seen.o: \
 tests/deductors/seen.cpp \
 include/deductors/seen.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deductors/seen.cpp -o tests/deductors/seen.o

tests/deductors/local-exclude.o: \
 tests/deductors/local-exclude.cpp \
 include/deductors/local-exclude.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deductors/local-exclude.cpp -o tests/deductors/local-exclude.o

tests/position.o: \
 tests/position.cpp \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) tests/position.cpp -o tests/position.o

src/predictors/seen.o: \
 src/predictors/seen.cpp \
 include/predictors/seen.h \
 include/predictor.h \
 include/bot.h \
 include/deck.h \
 include/macros.h \
 include/position.h
	$(go) src/predictors/seen.cpp -o src/predictors/seen.o

src/board.o: \
 src/board.cpp \
 include/board.h
	$(go) src/board.cpp -o src/board.o

src/bot.o: \
 src/bot.cpp \
 include/bot.h \
 include/deductor.h \
 include/deductors/local-exclude.h \
 include/deductors/no-show.h \
 include/deductors/seen.h \
 include/deck.h \
 include/predictor.h \
 include/predictors/seen.h \
 include/macros.h \
 include/position.h
	$(go) src/bot.cpp -o src/bot.o

src/deductors/no-show.o: \
 src/deductors/no-show.cpp \
 include/deductors/no-show.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deductors/no-show.cpp -o src/deductors/no-show.o

src/deductors/seen.o: \
 src/deductors/seen.cpp \
 include/deductors/seen.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deductors/seen.cpp -o src/deductors/seen.o

src/deductors/local-exclude.o: \
 src/deductors/local-exclude.cpp \
 include/deductors/local-exclude.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deductors/local-exclude.cpp -o src/deductors/local-exclude.o

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
	rm -f test.o tests/predictors/seen.o tests/board.o tests/bot.o tests/deductors/no-show.o tests/deductors/seen.o tests/deductors/local-exclude.o tests/position.o src/predictors/seen.o src/board.o src/bot.o src/deductors/no-show.o src/deductors/seen.o src/deductors/local-exclude.o src/position.o ai.tar.gz test

tar:
	tar -chvz test.cpp include/bot.h include/position.h include/board.h include/deductors/local-exclude.h include/deductors/no-show.h include/deductors/seen.h include/macros.h include/deductor.h tests/predictors/seen.cpp include/predictors/seen.h include/predictor.h include/deck.h tests/board.cpp tests/bot.cpp include/tests.h tests/deductors/no-show.cpp tests/deductors/seen.cpp tests/deductors/local-exclude.cpp tests/position.cpp src/predictors/seen.cpp src/board.cpp src/bot.cpp src/deductors/no-show.cpp src/deductors/seen.cpp src/deductors/local-exclude.cpp src/position.cpp makefile -f ai.tar.gz

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
	lcov --remove coverage.info -o coverage.info '*/include/*'; \
	lcov --list coverage.info; \
	echo coverage > last_build; \
	find . -name '*.gcda' -delete -or -name '*.gcno' -delete; \
	[[ -d coverage ]] && rm -rf coverage; \
	genhtml coverage.info --output-directory coverage; \
	rm coverage.info
