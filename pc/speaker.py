#!/usr/bin/python

import os

import gst

from reader import stationary_factory
from sp import throws

def make_wav(num):
    filename = 'wav/%s.wav' % num
    if not os.path.exists(filename):
        os.system('flite -t %s -o wav/%s.wav' % (num, num))
    assert(os.path.exists(filename))
    return os.path.realpath(filename)

def make_playbin(num):
    filename = make_wav(num)
    p=gst.Pipeline("player")
    s=gst.element_factory_make("playbin", "stream") 
    s.set_property("uri", "file://%s" % filename)
    p.add(s)
    return p, s

g_p, g_s = make_playbin(0) # This is the global playbin used by play
g_p.set_state(gst.STATE_PLAYING)

def play(num):
    global g_p
    global g_s
    g_p.set_state(gst.STATE_CHANGE_PLAYING_TO_PAUSED)
    g_s.set_property("uri", "file://%s" % make_wav(num))
    g_p.set_state(gst.STATE_PLAYING)

def main():
    stationary = stationary_factory()
    for throw_num, thrown in throws(stationary.accelerations()):
        print throw_num, thrown
        if thrown:
            play(throw_num)

if __name__ == '__main__':
    main()

