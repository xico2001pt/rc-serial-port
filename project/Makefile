all: application

$(shell mkdir -p build)

application:
	gcc source/* -o build/application

debug:
	gcc -DDEBUG source/* -o build/application

clean:
	rm -rf ./build