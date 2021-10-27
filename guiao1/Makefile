all: emitter receiver

$(shell mkdir -p build)

emitter:
    gcc emitter.c -o build/emitter

receiver:
    gcc receiver.c -o build/receiver

clean:
    rm -rf ./build