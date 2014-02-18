#!/usr/bin/python
import struct
from sets import Set
import argparse
import os
def main():
	
	parse = argparse.ArgumentParser(description = 'Classifier File Serializer')
	parse.add_argument('-t','--type', help="choise the file's type you want to serialize the options are w for words's file,n for generate node sig terms's file,l for list node's file,u unfiltered nodes's file,m for node must have terms's file,p for serialize phrase list file", dest='type',required=False)
	parse.add_argument('-o', help="the path to file output or directory when you choise", dest='output',required=True)
	parse.add_argument('-f', '--file', help='file to serialize', dest='file', required=False)
	parse.add_argument('--tax',  help='when choise the option -t l the file option is for file code_list_node.txt the option --tax is for file code_list_tax.txt', dest='tax_file', required=False)
	parse.add_argument('-d','--directory', help="the directory where find all files to serialize", dest='directory',required=False)
	args=parse.parse_args()
	if args.file:
		file_name = args.file
		output_file_name = args.output
		file_type = args.type
		serialize_file(file_name, output_file_name, file_type, args.tax_file)
	elif args.directory:
		serialize_all_files(args.directory, args.output)
	else:
		parse.print_help()
def serialize_all_files(dir_name, output_dir_name):
	words_file_name = os.path.join(dir_name, 'words.txt') 
	words_file_output = os.path.join(output_dir_name, 'words.data')
	node_sig_terms_file = os.path.join(dir_name, 'node_sig_vector.txt')
	node_sig_terms_output = os.path.join(output_dir_name, 'node_sig_vector.data')
	code_list_node_file_name = os.path.join(dir_name, 'code_list_node.txt')
	code_list_tax_file_name = os.path.join(dir_name, 'code_list_tax.txt')
	node_list_output = os.path.join(output_dir_name, 'node_list.data')
	unfiltered_node_file_name = os.path.join(dir_name, 'unfiltered_node_id.txt')
	unfiltered_node_output = os.path.join(output_dir_name, 'unfiltered_node_list.data')
	node_must_terms_file_name = os.path.join(dir_name, 'node_musthave.txt')
	node_must_terms_output = os.path.join(output_dir_name, 'node_musthave.data')
	serialize_words_file(words_file_name, words_file_output)
	serialize_node_sig_terms(node_sig_terms_file, node_sig_terms_output)
	serialize_list_node(code_list_node_file_name, code_list_tax_file_name, node_list_output)
	serialize_unfiltered_nodes(unfiltered_node_file_name, unfiltered_node_output)
	serialize_node_musthave_terms(node_must_terms_file_name,node_must_terms_output)
def serialize_file(file_name, output_file_name, file_type, tax_file):
	if file_type == 'w':
		serialize_words_file(file_name,output_file_name)
	elif file_type == 'n':
		serialize_node_sig_terms(file_name, output_file_name)
	elif file_type == 'l' and tax_file:
		serialize_list_node(file_name, tax_file, output_file_name)
	elif file_type == 'u':
		serialize_unfiltered_nodes(file_name, output_file_name)
	elif file_type == 'm':	
		serialize_node_musthave_terms(file_name, output_file_name)
	else:
		parse.print_help()	
def serialize_words_file(file_name, word_file):
	read_lines = read_file(file_name)
	fd=open(word_file,"wb")
	word_in_file = len(read_lines)
	fd.write(struct.pack("<I", word_in_file))
	print "********* words in file %d *******" % (word_in_file)
	for line in read_lines:
		data=line.split(",")
		data[1]=data[1].strip()
		data[0]=int(data[0])
		offset=len(data[1])+1
		binary_format="<IB"+str(offset)+"s"
		binary_data=struct.pack(binary_format, data[0], offset, data[1])
		fd.write(binary_data)
	fd.close()
def serialize_node_sig_terms(file_name, node_sig_vector_file):
	read_lines=read_file(file_name)
	fd=open(node_sig_vector_file,"wb")
	padding="emn"
	padding2="aluz"
	node_sig_term_size=len(read_lines)
	print "********* node sig term in file %d *******"%(node_sig_term_size)
	fd.write(struct.pack("<I",node_sig_term_size))
	for line in read_lines:
		line=line.strip()
		data=line.split(",")
		size=(len(data)-2)/2
		node_type=int(data[1])
		node_id=int(data[0])	
		binary_format="<B3sII"
		fd.write(struct.pack(binary_format,node_type,padding,node_id,size))
		binary_format="<If"
		data=data[2:]
		map_node_terms={}
		for i in range(0,size):
			term_id=int(data[i*2])
			weight=float(data[(i*2)+1])
			if term_id in map_node_terms:
				map_node_terms[term_id].append(struct.pack(binary_format,term_id,weight))
			else:
				map_node_terms[term_id]=[struct.pack(binary_format,term_id,weight)]
		keys=sorted(map_node_terms.keys())
		for key in keys:
			term_node_list=map_node_terms[key]
			for term_node in term_node_list:
				fd.write(term_node)
		fd.write(struct.pack("<4s",padding2))
	fd.close()
def parse_code_list_node(file_name):
	read_lines=read_file(file_name)
	data_mapping={}
	for line in read_lines:
		line=line.replace(":",",")
		line=line.strip()
		data=line.split(",")
		data_mapping[data[0].strip()]=data[1:]
	return data_mapping
def parse_word_to_binary(words, sort=False):
	data=""
	words=[int(word) for word in words]
	if(sort):
		words=sorted(words)
	for word in words:
		data=data+struct.pack("<I",word)
	return data
def create_code_list(key, mapping):
	size=0
	code_list=""
	if key in mapping:
		code_list=mapping[key]
		size=len(code_list)
		code_list=parse_word_to_binary(code_list)
	return (size,code_list)
def serialize_list_node(code_list_node_file, code_list_tax_file, output):
	print "************* serialize list node ************"
	binary_format="<III"
	mapping_code_list_node = parse_code_list_node(code_list_node_file)
	mapping_code_list_tax = parse_code_list_node(code_list_tax_file)
	keys = mapping_code_list_node.keys() + mapping_code_list_tax.keys()
	keys = Set(keys)
	records_len = len(keys)
	fd = open(output,"wb")
	fd.write(struct.pack("<I",records_len))
	nextWord = False
	for key in keys:
		size_code_list_node, code_list_node=create_code_list(key,mapping_code_list_node)
		size_code_list_tax,code_list_tax = create_code_list(key,mapping_code_list_tax)
		fd.write(struct.pack(binary_format, int(key), size_code_list_node, size_code_list_tax))
		fd.write(code_list_node)
		fd.write(code_list_tax)	
	fd.close()
def serialize_phrases(phrases_file_name, output):
	binary_format="<I4s"
	lines=read_file(phrases_file_name)
	fd=open(output,"wb")
	print "******************* phrases in file %d *****************"%(len(lines))
	fd.write(struct.pack(binary_format,len(lines),"emna"))
	for line in lines:	
		data=line.split(",")
		phrase_id=int(data[0])
		data=data[1:]
		fd.write(struct.pack("<II",phrase_id,len(data)))
		fd.write(parse_word_to_binary(data))
	fd.flush()
	fd.close()
def serialize_unfiltered_nodes(unfiltered_file_name, output):
	lines=read_file(unfiltered_file_name)
	print "******************* unfiltered nodes in file %d *****************"%(len(lines))
	fd=open(output,"wb")
	fd.write(struct.pack("<I",len(lines)))
	[fd.write(struct.pack("<I",int(word))) for word in lines]
	fd.flush()
	fd.close()
def serialize_node_musthave_terms(file_name, node_musthave_file):
	lines=read_file(file_name)
	print "******************* node musthave in file %d *****************"%(len(lines))
	fd=open(node_musthave_file,"wb")
	fd.write(struct.pack("<I",len(lines)))
	for line in lines:
		data=line.split(",")
		term_id=int(data[0])
		data=data[1:]
		fd.write(struct.pack("<II",term_id,len(data)))
		fd.write(parse_word_to_binary(data))
	fd.flush()
	fd.close()
def read_file(file_name):
	fd=open(file_name,"r")
	read_lines=fd.readlines()
	fd.close()
	new_read_lines = [line.strip() for line in read_lines if(len(line.strip())>0)]
	return new_read_lines
if __name__=="__main__":
	try:
		main()
 	except Exception as e:
		print e
