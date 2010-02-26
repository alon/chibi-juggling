#!/usr/bin/python

import os
import time

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

class Player:
    min_delta = 2.0 # TODO: how to do this safely (getting segfaults on zaru)
    def __init__(self):
        #self._init_gst() # not working on zaru (segfault, needs investigating)
        #self._doplay = self._gstplay
        self.last = time.time()
        self._doplay = self._aplay

    def _init_gst(self):
        self.p, self.s = make_playbin(0) # This is the global playbin used by play
        self.p.set_state(gst.STATE_PLAYING)

    def play(self, num):
        # TODO - fix this
        #if self.p.get_state()[0] != gst.STATE_READY:
        #    print self.p.get_state()
        #    print 'not ready'
        #    return
        now = time.time()
        if now - self.last < self.min_delta: return
        self.last = now
        filename = make_wav(num)
        self._doplay(filename)
    
    def _aplay(self, filename):
        os.system("aplay %s" % filename)

    def _gstplay(self, filename):
        self.p.set_state(gst.STATE_READY)
        self.s.set_property("uri", "file://%s" % filename)
        self.p.set_state(gst.STATE_PLAYING)

wav_player = Player()
play = wav_player.play

def main():
    stationary = stationary_factory()
    for throw_num, thrown in throws(stationary.accelerations()):
        print throw_num, thrown
        if thrown:
            play(throw_num)

if __name__ == '__main__':
    main()

