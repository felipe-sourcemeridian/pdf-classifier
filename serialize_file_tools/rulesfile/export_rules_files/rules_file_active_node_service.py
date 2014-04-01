import repo.node_repo as noderepo
import term_service
import time

PHRASES_FILE = 'phrases_file'
TERMS_FILE = 'terms_file'
MUSTHAVES_RAW_FILE = 'musthave_raw_file'
SIGNATURES_RAW_FILE = 'sigterms_raw_file'

SIGNAGTURE_RAW_FILE_FORMAT = "{node_id},{node_size},{node_type},{term},{ocurrence}\n"

MUSTHAVE_RAW_FILE_FORMAT = "{node_id},{term}\n"

get_term_from_signature = lambda signature: signature[noderepo.SIGNATURE_TERM]

get_term_from_musthave = lambda musthave : musthave[noderepo.MUSTHAVE_TERM]

active_node_bridge_filter = lambda active_node: active_node[noderepo.BRIDGE] == 0


def process_rules_file_for_active_nodes(active_nodes, terms_mapping, phrases_mapping, mapping_files, con):

	node_ids = get_node_ids_from_active_nodes(active_nodes)
	print_debug("getting signatures")
	signatures = noderepo.find_signatures_by_node_ids(node_ids, con)
	print_debug("signatures found %d"%(len(signatures)))
	print_debug("getting musthaves")
	musthaves = noderepo.find_musthaves_by_node_ids(node_ids, con)
	print_debug("getting musthave gate terms")
	musthave_gate_terms = noderepo.find_musthavegate_terms_by_node_ids(node_ids, con)
	all_terms = map(get_term_from_signature, signatures)
	all_terms.extend(map(get_term_from_musthave,musthaves))
	all_terms.extend(musthave_gate_terms)
	print_debug("export terms and phrases")
	export_terms_and_phrases_by_terms(all_terms, mapping_files[TERMS_FILE], mapping_files[PHRASES_FILE], terms_mapping, phrases_mapping)

	print_debug("export signatures")

	export_signatures_for_not_node_bridges(signatures, mapping_files[SIGNATURES_RAW_FILE], active_nodes)

	print_debug("export musthaves")

	export_musthaves_for_not_node_bridges(musthaves, mapping_files[MUSTHAVES_RAW_FILE], active_nodes)
	
	
	

def export_terms_and_phrases_by_terms(terms, terms_file, phrases_file, terms_mapping, phrases_mapping):
	term_service.process_terms_phrases_and_export(terms, terms_file, phrases_file ,terms_mapping, phrases_mapping)

def export_musthaves_for_not_node_bridges(musthaves, musthaves_file, active_nodes):
	node_ids = get_node_ids_not_bridge(active_nodes)
	musthave_not_bridge = lambda musthave : musthave[noderepo.MUSTHAVE_NODE_ID] in node_ids
	musthaves_to_export = filter(musthave_not_bridge, musthaves)	
	export_musthaves(musthaves_to_export, musthaves_file)

def export_musthaves(musthaves, musthaves_file):
	fd = open(musthaves_file, "a")
	for musthave in musthaves:
		musthave_content = MUSTHAVE_RAW_FILE_FORMAT.format(node_id = musthave[noderepo.MUSTHAVE_NODE_ID], term = musthave[noderepo.MUSTHAVE_TERM])
		fd.write(musthave_content)
	fd.close()

def export_signatures_for_not_node_bridges(signatures, signatures_file, active_nodes):	
	
	node_ids = get_node_ids_not_bridge(active_nodes)
	signature_not_bridge_filter = lambda signature: signature[noderepo.SIGNATURE_NODE_ID] in node_ids
	signatures_not_bridge  = filter(signature_not_bridge_filter, signatures)	
	export_signatures(signatures_not_bridge, signatures_file)
	
def get_node_ids_not_bridge(active_nodes):
	active_nodes_not_bridge = filter(active_node_bridge_filter, active_nodes)
	node_ids = get_node_ids_from_active_nodes(active_nodes_not_bridge)
	return node_ids
	
def export_signatures(signatures, signatures_file):
	fd = open(signatures_file, "a")
	for signature in signatures:
		node_id = signature[noderepo.SIGNATURE_NODE_ID]
		node_size = signature[noderepo.SIGNATURE_NODE_SIZE] or 0
		node_type = signature[noderepo.SIGNATURE_NODETYPE] or 0
		term = signature[noderepo.SIGNATURE_TERM] or ""
		ocurrence = signature[noderepo.SIGNATURE_OCURRENCE] or 0
		signature_content = SIGNAGTURE_RAW_FILE_FORMAT.format(node_id = node_id, node_size = node_size, node_type = node_type, term = term, ocurrence = ocurrence)
		fd.write(signature_content)
	fd.close()
		

def get_node_ids_from_active_nodes(active_nodes):
	node_ids = [active_node[noderepo.NODE_ID] for active_node in active_nodes]
	return node_ids

def print_debug(message):
	print "%s\t%s"%(message, time.asctime())
