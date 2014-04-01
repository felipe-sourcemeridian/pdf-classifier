import repo.node_repo as noderepo
import utils.stemmer_util as stemmer_util

def export_musthave_and_code_list_from_musthave_raw_file(musthave_raw_file,  terms_mapping, phrases_mapping, musthave_file, code_list_file):
	fd = open(musthave_raw_file, "r")
	musthave_mapping = {}
	code_list_mapping = {}
	for line in fd:
		if len(line.strip()) == 0:
			continue
		musthave = get_musthave_from_line(line)
		process_musthave_term(musthave, musthave_mapping, code_list_mapping, terms_mapping, phrases_mapping)
	save_code_list(code_list_mapping, code_list_file)
	save_musthave(musthave_mapping, musthave_file)


def get_musthave_from_line(line):
	musthave_data = line.split(",")
	node_id = musthave_data[noderepo.MUSTHAVE_NODE_ID]
	term =  musthave_data[noderepo.MUSTHAVE_TERM]
	return (int(node_id), term.strip())

		
def process_musthave_term(musthave, musthave_mapping, code_list_mapping, term_mapping, phrases_mapping):
		term = musthave[noderepo.MUSTHAVE_TERM]
		node_id = musthave[noderepo.MUSTHAVE_NODE_ID]
		value = stemmer_util.get_term_value_by_mappings(term, term_mapping, phrases_mapping)
		if value == None:
			return
		insert_code_list(code_list_mapping, value, node_id)
		insert_musthave_mapping(musthave_mapping, value, node_id)	

def insert_musthave_mapping(musthave_mapping, term_id, node_id):
	terms_list = []
	if node_id in musthave_mapping:
		terms_list = musthave_mapping[node_id]
	if term_id in terms_list:
		return
	terms_list.append(term_id)
	musthave_mapping[node_id] = terms_list

def insert_code_list(code_list_mapping, term_id, node_id):
	code_list = []
	if term_id in code_list_mapping:
		code_list = code_list_mapping[term_id]
	if node_id in code_list:
		return
	code_list.append(node_id)
	code_list_mapping[term_id] = code_list

def save_code_list(code_list_mapping, code_list_file):
	fd = open(code_list_file, "w")
	terms = code_list_mapping.keys()
	terms.sort()
	for term_id in terms:
		code_list = code_list_mapping[term_id]
		code_list.sort()
		code_list_text = get_ids_text(code_list)
		fd.write("%d,%s\n" % (term_id, code_list_text))
	fd.close()
	

def save_musthave(musthave_mapping, musthave_file):
	fd = open(musthave_file, "w")
	node_ids = musthave_mapping.keys()
	node_ids.sort()
	for node_id in node_ids:
		terms = musthave_mapping[node_id]
		terms.sort()
		terms_text = get_ids_text(terms)
		fd.write("%d,%s\n" % (node_id, terms_text))
	fd.close()

def get_ids_text(ids):
	return ",".join([str(list_id) for list_id in ids])
