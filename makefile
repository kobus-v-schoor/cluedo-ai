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
   gf = -std=c++11 -O3 -DNO_LOGGING
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
	export gf="-std=c++11 -O3 -DNO_LOGGING"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != release ]] && $(MAKE) clean; \
	echo release > last_build; \
	$(MAKE) test

last_build:
	echo debug > last_build

test: \
 test.o \
 tests/board.o \
 tests/position.o \
 tests/game.o \
 tests/deductors/no-show.o \
 tests/deductors/card-count-exclude.o \
 tests/deductors/seen.o \
 tests/deductors/local-exclude.o \
 tests/predictors/seen.o \
 tests/deck.o \
 tests/bot.o \
 src/board.o \
 src/position.o \
 src/predictor.o \
 src/deductors/no-show.o \
 src/deductors/card-count-exclude.o \
 src/deductors/seen.o \
 src/deductors/local-exclude.o \
 src/predictors/seen.o \
 src/deck.o \
 src/bot.o
	g++ $(gf) test.o tests/board.o tests/position.o tests/game.o tests/deductors/no-show.o tests/deductors/card-count-exclude.o tests/deductors/seen.o tests/deductors/local-exclude.o tests/predictors/seen.o tests/deck.o tests/bot.o src/board.o src/position.o src/predictor.o src/deductors/no-show.o src/deductors/card-count-exclude.o src/deductors/seen.o src/deductors/local-exclude.o src/predictors/seen.o src/deck.o src/bot.o -o test

test.o: \
 test.cpp \
 include/bot.h \
 include/position.h \
 include/board.h \
 include/deductors/local-exclude.h \
 include/deductors/no-show.h \
 include/deductors/seen.h \
 include/deductors/card-count-exclude.h \
 include/predictors/seen.h \
 include/macros.h \
 include/deductor.h \
 include/predictor.h \
 include/deck.h
	$(go) test.cpp -o test.o

tests/board.o: \
 tests/board.cpp \
 include/board.h
	$(go) tests/board.cpp -o tests/board.o

tests/position.o: \
 tests/position.cpp \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) tests/position.cpp -o tests/position.o

tests/game.o: \
 tests/game.cpp \
 include/bot.h \
 include/board.h \
 include/macros.h \
 include/position.h
	$(go) tests/game.cpp -o tests/game.o

tests/deductors/no-show.o: \
 tests/deductors/no-show.cpp \
 include/deductors/no-show.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deductors/no-show.cpp -o tests/deductors/no-show.o

tests/deductors/card-count-exclude.o: \
 tests/deductors/card-count-exclude.cpp \
 include/deductors/card-count-exclude.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deductors/card-count-exclude.cpp -o tests/deductors/card-count-exclude.o

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

tests/predictors/seen.o: \
 tests/predictors/seen.cpp \
 include/predictors/seen.h \
 include/predictor.h \
 include/bot.h \
 include/deck.h \
 include/macros.h \
 include/position.h
	$(go) tests/predictors/seen.cpp -o tests/predictors/seen.o

tests/deck.o: \
 tests/deck.cpp \
 include/deck.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) tests/deck.cpp -o tests/deck.o

tests/bot.o: \
 tests/bot.cpp \
 include/bot.h \
 include/tests.h \
 include/deck.h \
 include/board.h \
 include/macros.h \
 include/position.h
	$(go) tests/bot.cpp -o tests/bot.o

src/board.o: \
 src/board.cpp \
 include/board.h
	$(go) src/board.cpp -o src/board.o

src/position.o: \
 src/position.cpp \
 include/position.h \
 include/board.h \
 include/macros.h
	$(go) src/position.cpp -o src/position.o

src/predictor.o: \
 src/predictor.cpp \
 include/predictor.h \
 include/bot.h \
 include/deck.h \
 include/macros.h \
 include/position.h
	$(go) src/predictor.cpp -o src/predictor.o

src/deductors/no-show.o: \
 src/deductors/no-show.cpp \
 include/deductors/no-show.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deductors/no-show.cpp -o src/deductors/no-show.o

src/deductors/card-count-exclude.o: \
 src/deductors/card-count-exclude.cpp \
 include/deductors/card-count-exclude.h \
 include/deductor.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deductors/card-count-exclude.cpp -o src/deductors/card-count-exclude.o

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

src/predictors/seen.o: \
 src/predictors/seen.cpp \
 include/predictors/seen.h \
 include/predictor.h \
 include/bot.h \
 include/deck.h \
 include/macros.h \
 include/position.h
	$(go) src/predictors/seen.cpp -o src/predictors/seen.o

src/deck.o: \
 src/deck.cpp \
 include/deck.h \
 include/bot.h \
 include/macros.h \
 include/position.h
	$(go) src/deck.cpp -o src/deck.o

src/bot.o: \
 src/bot.cpp \
 include/bot.h \
 include/board.h \
 include/deductor.h \
 include/deductors/local-exclude.h \
 include/deductors/no-show.h \
 include/deductors/seen.h \
 include/deductors/card-count-exclude.h \
 include/deck.h \
 include/predictor.h \
 include/predictors/seen.h \
 include/macros.h \
 include/position.h
	$(go) src/bot.cpp -o src/bot.o

run: $(shell [[ -f last_build ]] && cat last_build || echo debug) | last_build
	./test

gdb: debug
	gdb test

clean:
	rm -f test.o tests/board.o tests/position.o tests/game.o tests/deductors/no-show.o tests/deductors/card-count-exclude.o tests/deductors/seen.o tests/deductors/local-exclude.o tests/predictors/seen.o tests/deck.o tests/bot.o src/board.o src/position.o src/predictor.o src/deductors/no-show.o src/deductors/card-count-exclude.o src/deductors/seen.o src/deductors/local-exclude.o src/predictors/seen.o src/deck.o src/bot.o ai.tar.gz test

tar:
	tar -chvz test.cpp include/bot.h include/position.h include/board.h include/deductors/local-exclude.h include/deductors/no-show.h include/deductors/seen.h include/deductors/card-count-exclude.h include/predictors/seen.h include/macros.h include/deductor.h include/predictor.h include/deck.h tests/board.cpp tests/position.cpp tests/game.cpp tests/deductors/no-show.cpp tests/deductors/card-count-exclude.cpp tests/deductors/seen.cpp tests/deductors/local-exclude.cpp tests/predictors/seen.cpp tests/deck.cpp tests/bot.cpp include/tests.h src/board.cpp src/position.cpp src/predictor.cpp src/deductors/no-show.cpp src/deductors/card-count-exclude.cpp src/deductors/seen.cpp src/deductors/local-exclude.cpp src/predictors/seen.cpp src/deck.cpp src/bot.cpp makefile -f ai.tar.gz

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
	find . -name '*.gcda' -delete -or -name '*.gcno' -delete; \
	[[ -d coverage ]] && rm -rf coverage; \
	genhtml coverage.info --output-directory coverage; \
	rm coverage.info; \
	$(MAKE) clean

game: $(shell [[ -f last_build ]] && cat last_build || echo debug) | last_build
	./test [game]
