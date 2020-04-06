ARCHIVE = libSPWL

all:	build_lib

build_lib:
  ifeq ($(OS),Windows_NT)
  ifeq (exists, IF EXIST build (echo exists))
		md build/
  endif
  else
		mkdir -p build/
  endif
	$(CXX) src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -Wall -Wextra -std=c++17
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

build_lib_prod:
  ifeq ($(OS),Windows_NT)
    ifeq (exists, IF EXIST build (echo exists))
			md build/
    endif
  else
		mkdir -p build/
  endif
	arm-linux-gnueabihf-g++ src/SPWL.cpp -c $(CFLAGS) -g $(CXXFLAGS) $(CPPFLAGS) -o build/$(ARCHIVE).o -Wall -Wextra -std=c++17
	ar rcs build/$(ARCHIVE).a build/$(ARCHIVE).o

clean:
  ifeq ($(OS),Windows_NT)
		del build/$(ARCHIVE).*
  else
		rm build/$(ARCHIVE).*
  endif

