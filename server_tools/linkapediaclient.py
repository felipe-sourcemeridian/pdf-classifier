#!/usr/bin/python
import socket
import struct
import argparse
import time
PADDING = "car"
def main():
	parse = argparse.ArgumentParser(description = "Linkapedia Server Testing Tool")
	parse.add_argument('--host', help="LinkapediaServer Host's name", dest='host',required=False, default = 'localhost')
	parse.add_argument('-p', '--port', type=int, help="Linkapedia Server's server port or default port is 5000", dest='port',required=False, default=5000)
	parse.add_argument('-t', '--type', help="classifier's server test type when this option you can do automatic testing or get console for testing manual features the options are -t automatic or -t console", dest='test_type', required=False, default="automatic")
	args = parse.parse_args()
	automatic_testing(args.host, args.port)
def automatic_testing(host, port):
#	test_wrong_packet_type(host, port)
#	test_packet_size_limit(host, port)
	test_slow_consumer(host, port)
def test_slow_consumer(host, port):
	if send_request_slow_consumer(host, port):
		print_test_ok("Test Slow Consumer Should Be Close Connection")
	else:
		print_test_fail("Test Slow Consumer Should Be Close Connection")
def test_wrong_packet_type(host, port):
	if send_pack_wrong_type(host, port):
		print_test_ok("Wrong Packet Type Should Be Close Connection")
	else:
		print_test_fail("Wrong Packet Type Should Be Close Connection")
def test_packet_size_limit(host, port):
	if send_pack_with_size_not_allowed(host, port):
		print_test_ok("Sent Packet's Size Dont Allowed Should be Close Connection")
	else:
		print_test_fail("Sent Packet's Size Dont Allowed Should be Close Connection")
def print_test_ok(test_name):
	print "\33[1;32m%s \t[OK]\33[1;m" % (test_name)

def print_test_fail(test_name):
	print "\33[1;31m%s \t[FAIL]\33[1;m" % (test_name)

def send_request_slow_consumer(host, port):
	socket = open_connection(host, port)
	send_title_packet(socket, "test title")	
	time.sleep(3)
	send_document_packet(socket, "test document")
	time.sleep(3)
	send_end_packet(socket)
	time.sleep(3)
	return socket.recv(1) == ''
def send_title_packet(socket, message):
	send_header(socket, len(message), 1)
	socket.send(message)
def send_document_packet(socket, message):
	send_header(socket, len(message), 2)
	socket.send(message)
def send_end_packet(socket):
	send_header(socket, 0, 3)
def send_pack_wrong_type(host, port):
	sock = open_connection(host, port)
	send_header(sock, len("test"), 4)
	time.sleep(1)
	return sock.recv(1) == ''
def send_pack_with_size_not_allowed(host, port):
	sock = open_connection(host, port)
	send_header(sock, 64000, 1)
	time.sleep(1)
	return sock.recv(1) == ''
def send_header(socket, length, packet_type):
	socket.send(struct.pack("<IB3s", length, packet_type, PADDING))
def open_connection(host, port):
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, port))
	return sock
if __name__ == '__main__':
	main()
