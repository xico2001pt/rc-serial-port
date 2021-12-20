all: application

$(shell mkdir -p build)

application:
	gcc source/* -o build/application

debug:
	gcc -DDEBUG source/* -o build/application

efficiency:
	gcc -DDEBUG -DFER=$(FER) -DT_PROP=$(T_PROP) -DMAX_FRAME_SIZE=$(MAX_FRAME_SIZE) source/* -o build/application

clean:
	rm -rf ./build