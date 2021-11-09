all: emitter receiver

$(shell mkdir -p build)

emitter:
	gcc emitter.c serial_config.c state_machine.c -o build/emitter

receiver:
	gcc receiver.c serial_config.c state_machine.c -o build/receiver

clean:
	rm -rf ./build