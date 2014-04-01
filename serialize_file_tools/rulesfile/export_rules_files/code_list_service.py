import repo.node_document_repo as node_document_repo
import utils.stemmer_util as stemmer_util
def export_code_list_file(code_list_file, term_mapping, phrases_mapping, con):
	corpus_ids = node_document_repo.find_all_corpus_id(con)
	clean_active_term = lambda corpus_term : corpus_terms[node_document_repo.CORPUS_TERM] != None and len(corpus_term[node_document_repo.CORPUS_TERM].strip()) > 0
	code_list_mapping = {}
	for corpus_id in corpus_ids:
		corpus_terms = node_document_repo.find_active_term_by_corpus_id(con, corpus_id)
		corpus_terms = filter(clean_active_term, corpus_terms)
		process_corpus_terms(corpus_terms, code_list_mapping, term_mapping, phrases_mapping)
	export_code_list_file_plaintext(code_list_file, code_list_mapping)

def process_corpus_terms(corpus_terms, code_list_mapping,term_mapping, phrases_mapping):
	for corpus_term in corpus_terms:
		terms = corpus_term[node_document_repo.CORPUS_TERM]
		node_id = corpus_term[node_document_repo.CORPUS_NODE_ID]
		terms_ids = get_terms_ids_from_mappings(terms, term_mapping, phrases_mapping)
		for term_id in terms_ids:
			code_list_nodes = []
			if term_id in code_list_mapping:
				code_list_nodes = code_list_mapping[term_id]
			code_list_nodes.append(node_id)
			code_list_mapping[term_id] = code_list_nodes
	
def get_terms_ids_from_mappings(terms, term_mapping, phrases_mapping):
	terms = terms.replace("||",",")
	terms = terms.split(",")
	terms_ids = []
	for term in terms:
		term_id = stemmer_util.get_term_value_by_mappings(term, term_mapping, phrases_mapping)
		if not term_id:
			continue
		terms_ids.append(term_id)
	return terms_ids

def export_code_list_file_plaintext(code_list_file, code_list_mapping):
	fd = open(code_list_file, "a")
	for term_id, nodes in code_list_mapping.items():
		nodes = ",".join([str(node) for node in nodes])
		fd.write("%d,%s\n"%(term_id, nodes))
	fd.close()
