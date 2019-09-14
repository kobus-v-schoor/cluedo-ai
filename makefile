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
   gf = -std=c++11 -O3
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
	export gf="-std=c++11 -O3"; \
	export go="g++ -c $$gf"; \
	[[ $$(cat last_build) != release ]] && $(MAKE) clean; \
	echo release > last_build; \
	$(MAKE) test

last_build:
	echo debug > last_build

test: \
 test.o \
 t_ai.o \
 ai.o
	g++ $(gf) test.o t_ai.o ai.o -o test

test.o: \
 test.cpp \
 ai.h
	$(go) test.cpp -o test.o

t_ai.o: \
 t_ai.cpp \
 ai.h
	$(go) t_ai.cpp -o t_ai.o

ai.o: \
 ai.cpp \
 ai.h
	$(go) ai.cpp -o ai.o

run: $(shell [[ -f last_build ]] && cat last_build || echo debug) | last_build
	./test

gdb: debug
	gdb test

clean:
	rm -f test.o t_ai.o ai.o ai.tar.gz test

tar:
	tar -chvz test.cpp ai.h t_ai.cpp ai.cpp makefile -f ai.tar.gz

