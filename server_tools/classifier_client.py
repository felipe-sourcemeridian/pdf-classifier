#!/usr/bin/python
import socket
import struct
import sys
import argparse
import os
TITLE_PACKET=1
DOCUMENT_PACKET=2
END_PACKET=3
DOCUMENT_RESPONSE_PACKET = 126
END_DOCUMENT_RESPONSE = 127
def classify_directory(directory, server):
	files_classified = 0
	for dirnames, dirs, files in os.walk(directory):
		for file_name in files:
			try:
				classify_document_on_server(directory + file_name, server)
				files_classified = files_classified + 1
				print "files classified %d"% files_classified
				print "files classified %s"% file_name
			except Exception as e:
				print e.message	

def classify_document_on_server(document_name, server):
	try:
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect(server)
		data = read_document(document_name)
		send_document_content_to_classify_to_server(data, sock)
		response = read_classifier_response_from_socket(sock)
		print response
	finally:
		sock.close()
def read_classifier_response_from_socket(socket):
	classifier_header = read_header_response_from_socket(socket)
	document_response = ''
	while classifier_header[1] != END_DOCUMENT_RESPONSE:
		document_response += socket.recv(classifier_header[0])
		classifier_header = read_header_response_from_socket(socket)
	return document_response
def read_header_response_from_socket(socket):	
	header = socket.recv(8)
	header = struct.unpack("<IB3s", header)
	return (header[0], header[1])
def read_document(document_name):
	fd = open(document_name, "r")
	content = fd.readlines()
	fd.close()
	return content
def send_document_content_to_classify_to_server(document_content, sock):
	line=document_content[0]
	size=len(line)
	sock.send(struct.pack("IB3s",size,TITLE_PACKET,"car"))
	sock.send(line)
	for line in document_content[1:]:
		size=len(line)
		sock.send(struct.pack("IB3s", size, DOCUMENT_PACKET, "car"))
		sock.send(line)
	sock.send(struct.pack("IB3s" ,0, END_PACKET, "car"))
if __name__ == '__main__':
	parse = argparse.ArgumentParser(description = 'Classifier Client')
	parse.add_argument('--host', help="classifier's server host name", dest='host', default='localhost')
	parse.add_argument('-p', '--port', type=int, help="classifier's server port", dest='port', default=5000)
	parse.add_argument('-f', '--file', help='file to classify', dest='file', required=False)
	parse.add_argument('-d', '--directory',help='classify whole files on directory', dest='directory', required=False)
	args=parse.parse_args()
	if args.file:
		classify_document_on_server(args.file, (args.host, args.port))
	elif args.directory:
		classify_directory(args.directory, (args.host, args.port))
	else:
		parse.print_help()
