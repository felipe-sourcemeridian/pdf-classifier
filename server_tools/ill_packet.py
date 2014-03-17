#!/usr/bin/python
import socket
import struct
import sys
import argparse
import os
def send_wrong_request(server, packet_type, packet_size):
	try:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect(server)
		print "send packet's size %d and packet's type %d" % (packet_size, packet_type)
		sock.send(struct.pack("IB3s", packet_size, packet_type,"car"))
		sock.recv(packet_size)
	finally:
		sock.close()
if __name__ == '__main__':
	parse = argparse.ArgumentParser(description = 'Classifier Client')
	parse.add_argument('--host', help="classifier's server host name", dest='host', default='localhost')
	parse.add_argument('-p', '--port', type=int, help="classifier's server port", dest='port', default=5000)
	parse.add_argument('-t', '--type', type=int, help="classifier's packet type", dest='type', required = True)
	parse.add_argument('-s', '--size', type=int, help="classifier's packet size", dest='size', required = True)
	args=parse.parse_args()
	send_wrong_request((args.host, args.port), args.type, args.size)
