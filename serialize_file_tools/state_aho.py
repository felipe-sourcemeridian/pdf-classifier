#!/usr/bin/python
from collections import deque
from struct import *
from sys import stdout
import time
BASE_FORMAT_WITHOUTPOINTER="<QIIQIBB2s"
BASE_FORMAT=BASE_FORMAT_WITHOUTPOINTER+"Q"
POINTER_SIZE=8
"""
typedef struct _state_aho
{
 struct _state_aho *parent;
 uint32_t word_id;
 unint32_t value;
 struct _state_aho *fail;
 unint32_t size;
 unsigned char has_result;
 unsigned char is_null_node;
 char padding[2];
 struct _state_aho *childs[0];
}state_aho;
"""
HEADER_SIZE=calcsize(BASE_FORMAT_WITHOUTPOINTER)
class Node(object):
	def __init__(self):
		self.value=None
		self.data=0
		self.childs=[]
		self.fail=None
		self.size=0
		self.nullNode=0
		self.offset=0
		self.parentOffset=0
		self.parent=None	
	def node_to_binary(self):	
		has_result=0
		pack_node=None			
		if(self.value):
			has_result=1
		value=self.value or 0
		if(self.fail==None):
			self.fail=Node()
			self.fail.offset=0
		pack_node=pack(BASE_FORMAT_WITHOUTPOINTER,self.parentOffset,self.data,value,self.fail.offset,self.size,has_result,self.nullNode,"sh")
		if(self.size!=0):
			for child in self.childs:
				pack_node=pack_node+pack("<Q",child.offset)
		else:
			pack_node=pack_node+pack("<Q",0)
		return pack_node
		
def read_file(file_name):
	fd=open(file_name,"r")
	lines=fd.readlines()
	fd.close()
	lines=[line.strip() for line in lines]
	return lines
def node_child(node, data):
	for child in node.childs:
		if(child.data == data):
			return child
	return None
def found_fail_node(node,root):
	parent=node.parent
	fail=parent.fail
	while(fail!=root):
		parent=fail
		fail=node_child(fail,node.data)
		if(fail):
			return fail
		fail=parent.fail		
	return None
def create_state_aho_fail(root):
	queue=deque([node for node in root.childs])
	current_node=queue.popleft()
	fail=None	
	while(current_node):		
		if(current_node.parent==root):
			[queue.append(node) for node in current_node.childs]
			current_node=queue.popleft()
			continue		
		current_node.fail=found_fail_node(current_node,root) or root
		try:
			current_node=queue.popleft()
		except:
			current_node=None
def create_state_aho(lines):
	root=Node()
	root.fail=root
	count=0
	size=len(lines)
	percent=0.0
	for line in lines:
		count=count+1
		current_node=root
		data=line.split(",")
		for i in range(1,len(data)):
			try:
				child=node_child(current_node,int(data[i]))
			except:
				print "Error remove this node for transition In Line %d"%(count)
				break
			if(child!=None):
				current_node=child
			else:
				node=Node()
				try:
					node.data=int(data[i])
				except:
					print "Error remove this node for transition In Line %d"%(count)
					break
				if(current_node==root):
					node.fail=root
				node.parent=current_node
				current_node.childs.append(node)
				current_node=node
		try:
			current_node.value=int(data[0])
		except:
			current_node.value=0
		stdout.write("\r lines process %d/%d "%(count,size))
		stdout.flush()
	return root
def search(root,words):
	result=[]
	current_node=root
	node=None
	for word in words:
		node=None
		node=node_child(current_node,word) or node_child(current_node.fail,word)
		if(node):
			current_node=node
			if(node.value):
				result.append(node.value)
		else:
			current_node=current_node.fail
	return result	
