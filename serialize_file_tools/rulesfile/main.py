#!/usr/bin/python
import cx_Oracle
import time
import export_rules_files.repo.node_repo as node_repo
import export_rules_files.rules_file_active_node_service as rules_service
import export_rules_files.term_service as term_service
import os
import test_utils.terms_and_phrases_util as terms_and_phrase_loader
import export_rules_files.musthave_terms_service as musthave_service


os.environ["NLS_LANG"] = ".UTF8"

def main():
	rules_active_nodes_files = {}
	rules_active_nodes_files[rules_service.PHRASES_FILE] = 'phrases.txt'
	rules_active_nodes_files[rules_service.TERMS_FILE] = 'terms.txt'
	rules_active_nodes_files[rules_service.MUSTHAVES_RAW_FILE] = 'musthave_raw.txt'
	rules_active_nodes_files[rules_service.SIGNATURES_RAW_FILE] = 'signatures_raw.txt'
	con = cx_Oracle.connect('sbooks/its@75.151.173.217:1522/medusa')
#	export_rules_for_active_nodes(con, rules_active_nodes_files)
	print_format_message("load terms mapping from file")
	terms_mapping = terms_and_phrase_loader.get_terms_mapping_from_file("terms.txt")
	print_format_message("load phrases mapping from file")
	phrases_mapping = terms_and_phrase_loader.get_phrases_mapping_from_file("phrases.txt")
	print_format_message("export code_list_node and code_list_tax")
	musthave_service.export_musthave_and_code_list_from_musthave_raw_file("musthave_raw.txt",  terms_mapping, phrases_mapping, "code_list_node.txt"
, "code_list_tax.txt")
	print_format_message("finish")
	con.close()
		
def export_rules_for_active_nodes(con, file_mapping):
	terms_file = file_mapping[rules_service.TERMS_FILE]
	phrases_file = file_mapping[rules_service.PHRASES_FILE]
	low_index = 0
	page = 1000
	has_more_result = True
	terms_mapping = {}
	phrases_mapping = {}
	print_format_message("exporting corpus's terms")
	term_service.export_terms_and_phrases_from_corpus(terms_file, phrases_file, terms_mapping, phrases_mapping, con)
	while has_more_result:
		print_format_message("getting active nodes from index %d"%low_index)
		active_nodes = node_repo.find_active_nodes(con, low_index, low_index + page + 1)
		print_format_message("export terms files")
		rules_service.process_rules_file_for_active_nodes(active_nodes, terms_mapping, phrases_mapping, file_mapping, con)
		has_more_result = len(active_nodes) == page
		low_index = low_index + len(active_nodes)
	return (terms_mapping, phrases_mapping)


def print_format_message(message):
	print "%s %s"%(message, time.asctime())
			
if __name__ == '__main__':
	main()
