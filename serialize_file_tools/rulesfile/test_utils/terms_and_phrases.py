#!/usr/bin/python
import export_rules_files.utils.stemmer_util as stemmer_util
TERM_CONTENT_FORMAT = "{term_id},{term}\n"
def main():
	terms_mapping = {}
	print "get terms"
	sigterms = get_terms_from_file("../original_terms/sigterms.txt")
	generate_terms(sigterms, terms_mapping, " ")
	inverse_terms_mapping = inverse_mapping_terms(terms_mapping)
	save_terms_file("terms_mapping.txt", inverse_terms_mapping)

def inverse_mapping_terms(terms_mapping):
	new_terms_mapping = {}
	for term, term_id in terms_mapping.items():
		new_terms_mapping[term_id] = term
	return new_terms_mapping

def generate_terms(terms, terms_mapping, pattern_splited):
	term_id = len(terms_mapping)
	print "split terms and normalizar"
	terms = stemmer_util.split_and_normalize_terms(terms, pattern_splited)
	print "stemming terms"
	terms = map(stemmer_util.stem_terms, terms)
	for term in terms:
		if term in terms_mapping:
			continue
		terms_mapping[term] = term_id
		term_id = term_id + 1

def save_terms_file(file_name, terms_mapping):
	fd = open(file_name, "w")	
	for term_id in range(0,len(terms_mapping)):
		term_content = TERM_CONTENT_FORMAT.format(term_id = term_id, term = terms_mapping[term_id])
		fd.write(term_content)
	fd.close()
def get_terms_from_file(file_name):
	fd = open(file_name, "r")
	lines = fd.readlines()
	fd.close()
	return lines

if __name__ == '__main__':
	main()
