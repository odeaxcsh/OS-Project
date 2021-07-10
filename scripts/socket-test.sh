#!/bin/bash

function help()
{
    echo "Usage [options] <path to server binary file> <path to client binary file>"
    echo "Usage [options] <path to binary files folder>                                 if executables name are socket-server and socket-client"
    echo "Usage [options]                                                               if executables are placed on the current folder"
    echo ""
    echo "which options are:"
    echo "--size=<comma separated integers>                  set manually size of test cases; default=100,1024,2048,4096,8192"
    echo "--port=<int>                                       server port number; defualt=8090"
    echo "--host=<IP>                                        server IP; defualt=127.0.0.1"
    echo "--help                                                prints this help page"
    echo "--version"
    echo ""
    echo "Example: ../bin/ --size=100,200,1000,3000 --port=8080 --host='192.168.43.53'"
    echo "Example: ../bin/socket-cleint ../bin/socket-server --size=1"
    exit 0;
}

# initializer variables to thier default values
SIZE=(100 1024 2048 4096 8192)
HOST="127.0.0.1"
PORT=8010

SERVERFILE=""
CLIENTFILE=""

# parse command line arguments
for i in "$@"; do
    case $i in
    --size=*)
        SIZESTR="${i#*=}"
        SIZE=(${SIZESTR//,/ })
        shift
        ;;
    --port=*)
        PORT="${i#*=}"
        shift
        ;;
    --host=*)
        HOST="${i#*=}"
        shift
        ;;
    --help)
        help
        shift
        ;;
    --version | -v)
        echo "v1.0.0"
        echo "written by odeaxcsh"
        echo "https://github.com/odeaxcsh/OS-project"
        exit 0;
        ;;
    *)
        if [ -z "$SERVERFILE" ]; then
            SERVERFILE=${i}
        elif [ -z "$CLIENTFILE" ]; then
            CLIENTFILE=${i}
        else
            echo "ignoring invalid argument" $i
        fi
        ;;
    esac
done

if [ -z "$SERVERFILE" ]; then
    SERVERFILE="."
fi

if [ -z "$CLIENTFILE" ]; then
    FOLDER="$SERVERFILE"

    SERVERFILE="${FOLDER}/socket-server"
    CLIENTFILE="${FOLDER}/socket-client"
fi

$SERVERFILE -h $HOST -p $PORT -n ${#SIZE[@]} > /dev/null  &

sleep .1

for s in ${SIZE[@]}; do
    echo "test ${s} bytes"
    $CLIENTFILE -h $HOST -p $PORT `./randomStringGenerator.py $s`
    echo ""
done
