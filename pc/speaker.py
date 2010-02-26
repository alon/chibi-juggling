#!/usr/bin/python

import os
from reader import stationary_factory
from sp import throws

def main():
    stationary = stationary_factory()
    for throw_num, thrown in throws(stationary.accelerations()):
        print throw_num, thrown
        if thrown:
            os.system("flite -t %s &" % throw_num)

if __name__ == '__main__':
    main()

