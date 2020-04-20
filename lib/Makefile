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
	$(CXX) src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) $(COVFLAGS) -o build/$(ARCHIVE).o -Wall -Wextra -std=c++17 -static
	$(CXX) src/sha256.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) $(COVFLAGS) -o build/sha256.o -Wall -Wextra -std=c++17 -static
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o build/sha256.o

build_lib_prod:
  ifeq ($(OS),Windows_NT)
    ifeq (exists, IF EXIST build (echo exists))
			md build/
    endif
  else
		mkdir -p build/
  endif
	arm-linux-gnueabihf-g++ src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -Wall -Wextra -std=c++17 -static
	arm-linux-gnueabihf-g++ src/sha256.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/sha256.o -Wall -Wextra -std=c++17 -static
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o build/sha256.o

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(ARCHIVE).*
		del build/sha256.*
  else
		rm build/$(ARCHIVE).*
		rm build/sha256.*
  endif

