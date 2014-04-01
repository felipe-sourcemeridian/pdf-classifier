#!/usr/bin/python
split_line = lambda line: line.strip().split(",")
invert_term_key = lambda term: (term[1].strip(), int(term[0]))
def main():
	terms_mapping = get_terms_from_file("terms_mapping.txt")
	original_terms_mapping = get_terms_from_file("../original_terms/words.txt")
	diff_terms = get_diff_terms(terms_mapping, original_terms_mapping)
	save_terms_mapping("term_diff.txt", diff_terms)

def save_terms_mapping(diff_file_name, terms_mapping):
	fd = open(diff_file_name, "w")
	terms_keys = terms_mapping.keys()
	for term in terms_keys:
		term_id = terms_mapping[term]
		fd.write("%s,%s\n"%(term, term_id))
	fd.close()

def get_terms_from_file(file_name):	
	fd = open(file_name, "r")	
	lines = fd.readlines()
	fd.close()
	splited_lines = map(split_line, lines)
	terms = map(invert_term_key, splited_lines)
	terms = dict(terms)
	return terms
def get_diff_terms(terms_mapping, terms_mapping_compare):
	diff_terms = {}
	for term in terms_mapping.keys():
		term_id = terms_mapping[term]
		if not term in terms_mapping_compare:
			diff_terms[term] = term_id
	return diff_terms

def get_terms_missing(terms_mapping, terms_mapping_compare):
	return get_terms_by_predicate(terms_mapping, terms_mapping_compare, is_term_missing)


def get_terms_not_equal(terms_mapping, terms_mapping_compare):
	return get_terms_by_predicate(terms_mapping, terms_mapping_compare, is_term_not_equal)

def is_term_not_equal(term, term_id, terms_mapping_compare):
	return False

def is_term_missing(term, term_id, terms_mapping_compare):
	return not term in terms_mapping_compare



def get_terms_by_predicate(terms_mapping, terms_mapping_compare, predicate):
	diff_terms_mapping = {}
	count = 0
	for term, term_id in terms_mapping.items():
		if predicate(term, term_id, terms_mapping_compare):
			diff_terms_mapping[term_id] = term				
	return diff_terms_mapping	

if __name__ == '__main__':
	main()
