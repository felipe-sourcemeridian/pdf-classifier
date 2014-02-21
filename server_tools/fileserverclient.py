#!/usr/bin/python
import os
import argparse
import socket
import errno
import os
import fcntl
def main():
	parse = argparse.ArgumentParser(description = "FileServer Testing Tool This Tools's Text Base, You Can Testing the FileServer with simples comand with the next format for command for exempla send message por all connectocion you write *:message for send message the first connection you type the next command 1:message, for close the connection you type 1:close")
	parse.add_argument('--host', help="FileServer Host's name", dest='host',required=False, default = 'localhost')
	parse.add_argument('-p', '--port', type=int, help="classifier's server port or default port is 5000", dest='port',required=False, default=5000)
	parse.add_argument('-c', '--clients', help='number of client to connect to server',type = int, dest='clients', required=False, default = 5)
	args=parse.parse_args()
	connections = open_connections(args.clients, args.host, args.port)
	while True:
		user_input = raw_input("cli:>")
		print user_input
		try:
			command = parse_user_input(user_input)
		except:
			print "the command's format is wrong"
			parse.print_help()
			continue
		client_connections = get_connections_from_command(command, connections)
		if command[1] == 'close':
			close_connections(client_connections)
		elif command[1] == 'read':
			read_messages(client_connections)
		else:
			send_messages(client_connections, command[1])

def close_connections(connections):
	for connection in connections:
		connection.close()
	print "connection close %d" %(len(connections))
def send_messages(connections, message):
	for connection in connections:
		connection.send(message)
	print "message sent %s"%(message)
def read_messages(connections):
	for i in range(0, len(connections)):
		print "%d:%s"%(i, read_message(connections[i]))
def read_message(connection):
	try:
		data = ''
		while True:
			data = data + connection.recv(4096)
	except socket.error,e:
		err = e.args[0]
		if err == errno.EAGAIN or err == eerno.EWOULDBLOCK:
			return data
	return data
		
def get_connections_from_command(command, connections):
	index = command[0]
	client_connection = []
	if index == '*':
		return connections
	else:
		i = int(command[0]) - 1
		client_connection.append(connections[i])
		return client_connection
def open_connections(number_of_connections, host, port):
	connections = []
	for i in range(number_of_connections):
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect((host, port))
		fcntl.fcntl(sock, fcntl.F_SETFL, os.O_NONBLOCK)
		connections.append(sock)
	return connections
def parse_user_input(user_input):
	data = user_input.split(':')
	return (data[0].strip(), data[1].strip())
if __name__ =='__main__':
	main()
