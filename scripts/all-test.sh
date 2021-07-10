#!/bin/bash

function test {
    echo "|========================<Shared Memory Test>========================|"
    ./shared-memory-test.sh ${BIN}
    echo "|=====================<End of Shared Memory Test>====================|"
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo ""
    echo "|========================<Socket Test>========================|"
    ./socket-test.sh ${BIN}
    echo "|=====================<End of Socket Test>====================|"
}

BIN="../bin"

time test
echo "[ende gut, alles gut]"
