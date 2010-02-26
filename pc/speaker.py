#!/usr/bin/python

import os

import gst

from reader import stationary_factory
from sp import throws

def make_wav(num):
    filename = 'wav/%s.wav' % num
    if not os.path.exists(filename):
        os.system('flite -t %s -o %s.wav' % (num, num))
    assert(os.path.exists(filename))
    return filename

def make_playbin(num):
    filename = make_wav(num)
    p=gst.Pipeline("player")
    s=gst.element_factory_make("playbin", "stream") 
    s.set_property("uri", "file:///home/alon/accounts/linkedin/1.wav")
    p.add(s)
    return p

def play(num):
    p = make_playbin(num)
    p.set_state(gst.STATE_PLAYING)

def main():
    stationary = stationary_factory()
    for throw_num, thrown in throws(stationary.accelerations()):
        print throw_num, thrown
        if thrown:
            play(throw_num)

if __name__ == '__main__':
    main()

