ARCHIVE = libSPWL

COVFLAGS += -fprofile-arcs
COVFLAGS += -ftest-coverage
COVFLAGS += -fPIC

all:	build_lib

build_lib:
  ifeq ($(OS),Windows_NT)
  ifeq (exists, IF EXIST build (echo exists))
		md build/
  endif
  else
		mkdir -p build/
  endif
	$(CXX) src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) $(COVFLAGS) -o build/$(ARCHIVE).o -lcrypto -Wall -Wextra -std=c++17 -static
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

build_lib_prod:
  ifeq ($(OS),Windows_NT)
    ifeq (exists, IF EXIST build (echo exists))
			md build/
    endif
  else
		mkdir -p build/
  endif
	arm-linux-gnueabihf-g++ src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -lcrypto -Wall -Wextra -std=c++17 -static
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(ARCHIVE).*
  else
		rm build/$(ARCHIVE).*
  endif

