import repo.node_document_repo as node_document_repo
import utils.stemmer_util as stemmer_util
SIGTERM_NODE_ID = 0
SIGTERM_NODE_TYPE = 1
SIGTERM_TERM = 2
SIGTERM_WEIGHT = 3
SIGTERM_VALUE = 4
SIGTER_FORMAT = "{node_id},{node_type},{term_value},{weight}\n"
def export_sigterm_file_by_nodes(sigterm_file, nodes, term_mapping, phrases_mapping, con, max_nodes_by_query = 1000):
	filter_sigterm = lambda sigterm : len(sigterm) == 5
	for sigterms in get_sigterm_by_nodes(nodes, con, max_nodes_by_query):
		sigterms = map(create_sigterm_from_cursor_raw_data, sigterms)
		process_sigterms(sigterms, term_mapping, phrases_mapping)
		sigterm = filter(filter_sigterm, sigterms)
		export_sigterm_plaintext(sigterm_file, sigterm)
		
def process_sigterms(sigterms, term_mapping, phrases_mapping):
	is_phrase = lambda term: len(term.strip().split(" ")) > 1
	for sigterm in sigterms:		
		term = sigterm[SIGTERM_TERM]
		value = stemmer_util.get_term_value_by_mappings(term, term_mapping, phrases_mapping) 
		if value != None:
			sigterm.append(value)
		
def get_sigterm_by_nodes(nodes, con, max_nodes_by_query):
	for i in range(0, len(nodes), max_nodes_by_query):
		current_nodes = nodes[i:i+max_nodes_by_query]
		sig_terms = node_document_repo.find_sigterms_by_nodes(current_nodes, con)
		yield sig_terms

def create_sigterm_from_cursor_raw_data(cursor_raw_data):
	sigterm = []
	ocurrences = cursor_raw_data[node_document_repo.SIGNATUREOCCURENCES] or 0
	node_size = cursor_raw_data[node_document_repo.NODE_SIZE] or 1
	weight = float(ocurrences) / node_size
	sigterm.append(cursor_raw_data[node_document_repo.NODE_ID] or 0)
	sigterm.append(cursor_raw_data[node_document_repo.NODE_TYPE] or 0)
	sigterm.append(cursor_raw_data[node_document_repo.SIGNATUREWORD] or "")
	sigterm.append(weight)
	return sigterm

def export_sigterm_plaintext(sigterm_file, sigterms):
	fd = open(sigterm_file, "a")
	for sigterm in sigterms:
		weight_format = "%.4f"%(sigterm[SIGTERM_WEIGHT])
		weight_format = weight_format.replace(",", ".")
		data = SIGTER_FORMAT.format(node_id = sigterm[SIGTERM_NODE_ID], node_type = sigterm[SIGTERM_NODE_TYPE],term_value = sigterm[SIGTERM_VALUE], weight = weight_format)
		fd.write(data)
	fd.close()
