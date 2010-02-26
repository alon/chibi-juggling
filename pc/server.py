#!/usr/bin/python

"""
Serve a single serial port to multiple clients. That's it - no parsing
is done here.
"""

import sys
import os
import time
from twisted.internet import reactor, protocol
from twisted.protocols import basic
from twisted.internet import task
from twisted.internet.serialport import SerialPort

import pdb
brk = pdb.set_trace

JUGGLING_SERVER_PORT = 7500

class LineBasedTeeProtocol(basic.LineReceiver):

    delimiter='\n' # I always fall on this point. grrr!

    def connectionMade(self):
        print "connected"
        self.factory.clients.append(self)

    def lineReceived(self, line):
        print "got", repr(line)
        self.factory.client_commands.append(line) # hope this is atomic :\
        reactor.callLater(0.0, self.factory.flush_lines_from_clients)


class TeeFactory(protocol.ServerFactory):
    """ Protocol for all connectors, and a convenient place for our single serial
    port, or rather serial protocol.
    """

    debug = False
    protocol = LineBasedTeeProtocol
    serial = None
    clients = []
    client_commands = []

    def on_line_from_serial(self, line):
        if self.debug:
            print "serial said", repr(line)
        for c in self.clients:
            c.transport.write(line+'\n')

    def flush_lines_from_clients(self):
        for cmd in self.client_commands:
            cmd = cmd.strip()+'\r'
            print "sending", repr(cmd)
            self.serial.transport.write(cmd)
        self.client_commands = []

class StationaryProtocol(basic.LineReceiver):
    delimiter = '\r' # extremely important.

    def __init__(self, teefactory):
        self.tee = teefactory
        self.tee.serial = self
    def connectionMade(self):
        pass
    def lineReceived(self, line):
        print line
        reactor.callLater(0.0, self.tee.on_line_from_serial, line)
    def request_full_scan(self):
        self.transport.write(self.full_scan_request)

def listenToTCPAndSerial(device):
    teefactory = TeeFactory()
    protocol = StationaryProtocol(teefactory)
    SerialPort(protocol, device, reactor, baudrate=115200)
    reactor.listenTCP(JUGGLING_SERVER_PORT, teefactory)
    return protocol


if __name__ == '__main__':
    device = '/dev/ttyACM0' if len(sys.argv) == 1 or not os.path.exists(sys.argv[-1]) else sys.argv[-1]
    print "Reading stationary from %s" % device
    protocol = listenToTCPAndSerial(device)
    reactor.run()

