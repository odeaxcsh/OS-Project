BIN=bin

all: shared-memory socket
	-mkdir $(BIN)
	(cd shared-memory && make out BIN="../$(BIN)")
	(cd socket && make out BIN="../$(BIN)")
	(cd scripts && make test-cp BIN="../$(BIN)")

clean:
	(cd shared-memory && make clean)
	(cd socket && make clean)
	-rm -r $(BIN)