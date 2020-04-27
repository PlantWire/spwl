run_test:
	cd test/ && make run_test

build_test: build_lib
	cd test/ && make build_test

clean_test:
	cd test/ && make clean

build_lib:
	cd lib/ && make build_lib

build_lib_prod:
	cd lib/ && make build_lib_prod

clean:
	cd lib/ && make clean
	cd test/ && make clean
lint:
	cpplint --filter=-legal/copyright,-build/c++11,-runtime/references --recursive --exclude=test/cute --exclude=lib/include/sha256.h --exclude=lib/src/sha256.c --exclude=.metadata .
