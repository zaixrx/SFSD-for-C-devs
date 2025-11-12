#!/bin/bash

if [ $# -lt 1 ]; then
    echo "Usage: $0 <src> [-o <out>]"
    exit 1
fi

SOURCE=""
BINARY="bin.out"

while [[ $# -gt 0 ]]; do
    case "$1" in
        -o)
            if [[ -z "$2" || "$2" == -* ]]; then
                echo "Error: -o requires a out name."
                echo "Usage: $0 <src> [-o <out>]"
                exit 1
            fi
            BINARY="$2"
            shift 2
            ;;
        -*)
            echo "Warning: Unknown option '$1' ignored."
            shift
            ;;
        *)
            if [[ -z "$SOURCE" ]]; then
                SOURCE="$1"
            else
                echo "Warning: Extra argument '$1' ignored."
            fi
            shift
            ;;
    esac
done

if [[ -z "$SOURCE" ]]; then
    echo "Error: No source file provided."
    echo "Usage: $0 <src> [-o <out>]"
    exit 1
fi

cc -I./ -o "$BINARY" "$SOURCE" -Wall -Wextra -ggdb
