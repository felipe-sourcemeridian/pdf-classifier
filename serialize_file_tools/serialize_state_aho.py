#!/usr/bin/python
from state_aho import *
import sys
import mmap
FILE_NAME_STATE_AHO="state.aho"
def serialize_node(root,fd):
	def serialize_node_aux(current_node):
		current_node.size=len(current_node.childs)
		fd.write(current_node.node_to_binary())
		if(current_node.size!=0):
			for child in current_node.childs:
				serialize_node_aux(child)
			node=Node()
			node.nullNode=1
			fd.write(node.node_to_binary())
	serialize_node_aux(root)
def calcule_offset():
	print "deserialize node"
	root=deserialize_node(FILE_NAME_STATE_AHO)
	print "make fail function"
	create_state_aho_fail(root)
	fd=open(FILE_NAME_STATE_AHO,"wb")
	serialize_node(root,fd)
	fd.flush()
	fd.close()
def parse_header(data):
	data=unpack(BASE_FORMAT_WITHOUTPOINTER,data)
	node=Node()
	node.parent=None
	node.data=data[1]
	node.value=data[2]
	node.size=data[4]
	node.fail=None
	if(data[5]==0):
		node.value=None
	node.nullNode=data[6]	
	return node
def deserialize_node(file_name):		
	fd=open(file_name,"rb")
	data=fd.read(HEADER_SIZE)
	root=parse_header(data)
	root.offset=0
	root.parent=root
	root.fail=root
	parentNodeStack=[root]
	fd.read(POINTER_SIZE*root.size)
	while(data!=''):
		data=fd.read(HEADER_SIZE)
		if(len(data)==0):
			continue
		node=parse_header(data)
		if(node.nullNode==1):
			data=fd.read(POINTER_SIZE)
			parentNodeStack.pop()
			continue
		node.offset=fd.tell()-HEADER_SIZE
		node.parentOffset=parentNodeStack[-1].offset
		node.parent=parentNodeStack[-1]
		if(node.parent==root):
			node.fail=root
		node.parent.childs.append(node)
		if(node.size>0):
			parentNodeStack.append(node)
			data=fd.read(POINTER_SIZE*node.size)
		else:
			data=fd.read(POINTER_SIZE)
	fd.close()
	return root
def save_serialize_state_aho(file_name):
	print "create state aho"
	root=create_state_aho(read_file(file_name))
	print "serialize node"
	fd=open(FILE_NAME_STATE_AHO,"wb")
	serialize_node(root,fd)
	fd.flush()
	fd.close()
def main(argv=sys.argv):
	file_name="phrases.txt"
	try:
		op=argv[1]
	except Exception as error:
		op=""
	if(op!='build'):
		save_serialize_state_aho(file_name)
	else:
		calcule_offset()
if __name__=='__main__':
	main()
