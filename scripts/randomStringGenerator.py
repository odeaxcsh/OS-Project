#!/bin/python3

import sys
import random
import string
from typing import List

def main(argv: List[str]):

    if len(argv) < 2:
        print(f"Usage: {argv[0]} <size: int>", file=sys.stderr)
        exit(1)

    size = int(argv[1])
    valid_letters = string.ascii_letters + string.digits
    result_str = ''.join(random.choices(valid_letters, k=size))
    print(f'{result_str}', file=sys.stdout)

if __name__ == '__main__':
    main(sys.argv)