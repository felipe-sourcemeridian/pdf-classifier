import repo.node_repo as node_repo
import phrase_service
import utils.export_terms_file as export_terms_file
import utils.stemmer_util as stemmer_util
import time

def export_terms_and_phrases_from_corpus(terms_file, phrases_file, terms_mapping, phrases_mapping, con):
	terms = node_repo.find_corpus_terms(con)
	terms_splited = stemmer_util.split_and_normalize_terms(terms, ",| ")
	phrases = phrase_service.get_phrases_corpus_from_terms(terms)
	generate_and_export_terms(terms_file, terms_splited, terms_mapping, phrases_mapping)
	generate_and_export_phrases(phrases_file, phrases, terms_mapping, phrases_mapping)
	

def process_terms_phrases_and_export(terms, terms_file, phrases_file ,terms_mapping, phrases_mapping):
	terms_splited = stemmer_util.split_and_normalize_terms(terms, " ")
	phrases = phrase_service.get_phrases_from_terms(terms)
	generate_and_export_terms(terms_file, terms_splited, terms_mapping, phrases_mapping)
	generate_and_export_phrases(phrases_file, phrases, terms_mapping, phrases_mapping)	

def generate_and_export_phrases(phrases_file ,phrases, terms_mapping, phrases_mapping):
	
	term_id = get_term_id_from_mappings(terms_mapping, phrases_mapping)
	new_phrases_mapping = phrase_service.generate_phrases_mapping_from_term_mapping(phrases, terms_mapping, phrases_mapping, term_id)
	export_phrases(phrases_file, new_phrases_mapping)
	
def generate_and_export_terms(terms_file, terms, terms_mapping, phrases_mapping):
	term_id = get_term_id_from_mappings(terms_mapping, phrases_mapping)
	new_terms_mapping = generate_term_mapping_by_terms(terms, terms_mapping, term_id)
	export_terms(terms_file, new_terms_mapping)

def get_term_id_from_mappings(terms_mapping, phrases_mapping):
	return len(terms_mapping) + len(phrases_mapping)

def export_phrases(phrases_file, phrases):
	export_terms_file.export_phrase_textplain(phrases_file, phrases)

def export_terms(term_file, terms):
	export_terms_file.export_terms_textplain(term_file, terms)

def generate_term_mapping_by_terms(terms, term_mapping,  begin_id):
	new_mapping = {}
	terms_stemming = stemmer_util.lower_and_stem_terms(terms)
	for term_stemming in terms_stemming:
		if term_stemming in term_mapping:
			continue
		term_mapping[term_stemming] = begin_id
		new_mapping[term_stemming] = begin_id
		begin_id = begin_id + 1
	return new_mapping
