SCRIPTS = *-test.sh
UTILS = randomStringGenerator.py

test-cp:
	-mkdir "${BIN}/test" 
	-cp ${SCRIPTS} "${UTILS}" "${BIN}/test/"
	sed -i 's/BIN\=.*/BIN=".."/' "${BIN}/test/all-test.sh"
