#!/usr/bin/python

import os
import re
import sys
import serial
import time

class Stationary(object):
    """ Ugly. Talks to the command line oriented serial interface the
    freakusb Stationary program presents.
    """

    verbose = False
    PROMPT = 'CHIBI >>' #'FREAKUSB >>'
    re_accel = re.compile('.*[^0-9\d](-?[0-9]+).*,.*[^0-9\-](-?[0-9]+).*,.*[^0-9\-](-?[0-9]+).*')

    def __init__(self, device_file):
        self._s = serial.Serial(device_file)
        # initial "enter" to get into a known state.
        self.write('')

    def close(self):
        self._s.close()

    def waitForPrompt(self):
        s = self._s
        found = False
        lines = []
        while not found:
            line = s.read(max(1, s.inWaiting()))
            lines.append(line)
            if self.verbose:
                print "got %r" % line
            if (''.join(lines)).find(self.PROMPT) != -1:
                found = True
        if self.verbose:
            line = s.read(s.inWaiting())
            print "got %r" % line
        return ''.join(lines) + line
        #import pdb; pdb.set_trace()

    def write(self, line):
        if len(line) == 0 or line[-1] != '\r':
            line = line + '\r'
        if self.verbose:
            print "writing %r" % line
        self._s.write(line)
        return self.waitForPrompt()
    
    def readline(self):
        return self._s.readline()

    # high level interface from here on

    def enable_print(self):
        self.write('enb 1')

    def disable_print(self):
        self.write('enb 0')

    def _parse_acceleration(self, line):
        # units are g (1 == 9.81 m/s^2)
        if self.verbose:
            print line
        return [float(x)/256 for x in re.match(self.re_accel, line).groups()]

    def accelerations(self):
        self.enable_print()
        while True:
            try:
                line = self._s.readline()
                acc_X, acc_Y, acc_Z = self._parse_acceleration(line)
                yield acc_X, acc_Y, acc_Z
            except KeyboardInterrupt:
                self.disable_print()
                raise StopIteration
            except Exception:
                # ignore any parsing errors resulting from non acceleration lines
                # (float float float) seen
                pass

def main():
    device_file = '/dev/ttyACM0'
    if len(sys.argv) > 1:
        device_file = sys.argv[-1]
    stationary = Stationary(device_file)
    # wait for something
    # to make sure this stops at an end of line, so we use s.readline() from now
    for acc_X, acc_Y, acc_Z in stationary.accelerations():
        print "%4.3f %4.3f %4.3f" % (acc_X, acc_Y, acc_Z)

if __name__ == '__main__':
    main()

