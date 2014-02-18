#!/usr/bin/python
import socket
import struct
import sys
import argparse
import os
TITLE_PACKET=1
DOCUMENT_PACKET=2
END_PACKET=3
def classify_multiples_directorys(host, port, directory):
	thread_list=[]
	for dir_path,dir_names,file_names in os.walk(directory):
		for dir_name in dir_names:
			print "init work in directory %s" % directory + dir_name
			thread_work=threading.Thread(group = None,target = classify_directory, name = None, args = (host, port, directory + dir_name + "/"), kwargs = {})
			thread_work.start()
			thread_list.append(thread_work)
		break
def classify_directory(directory, server):
	file_classified = 0
	for dirnames, dirs, files in os.walk(directory):
		for file_name in files:
			try:
				classify_document_on_server(directory + file_name, server)
				files_classified = files_classified + 1
				print "files classified %d"% files_classified
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
def read_classifier_response_from_socket(sock):
	print "reading content from socket"
	fd = sock.makefile()
	classifier_response = fd.read()
	fd.close()
	return classifier_response
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
	parse.add_argument('--host', help="classifier's server host name", dest='host',required=True)
	parse.add_argument('-p', '--port', type=int, help="classifier's server port", dest='port',required=True)
	parse.add_argument('-f', '--file', help='file to classify', dest='file', required=False)
	parse.add_argument('-d', '--directory',help='classify whole files on directory', dest='directory', required=False)
	args=parse.parse_args()
	if args.file:
		classify_document_on_server(args.file, (args.host, args.port))
	elif args.directory:
		classify_directory(args.directory, (args.host, args.port))
	else:
		parse.print_help()
