CXX = g++
CXXFLAGS = -std=c++17 -Iinclude -I$(SYSTEMC_PATH)/include
LDFLAGS = -L$(SYSTEMC_PATH)/lib -lsystemc
LDLIBPATH = LD_LIBRARY_PATH=$(SYSTEMC_PATH)/lib

SRC := $(wildcard src/*.cpp)
TESTS := $(wildcard test/*.cpp)
OBJS := $(patsubst test/%.cpp, build/test_%, $(TESTS))

build:
	mkdir -p build

build/test_%: test/%.cpp $(SRC) | build
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@

all: $(OBJS)

run-%: build/test_%
	$(LDLIBPATH) $<

build/processor: processor.cpp $(SRC) | build
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@

run-processor: build/processor
	$(LDLIBPATH) $<

run: src/loader/loader
	@if [ -z "$(PROG)" ]; then \
		echo "Erro: Especifique o programa com PROG. Exemplo: make run PROG=programs/test_sub.txt"; \
		exit 1; \
	fi
	./src/loader/loader $(PROG)
	$(MAKE) run-processor

clean:
	rm -rf build
	rm -f src/loader/loader

src/loader/loader: src/loader/loader.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@
