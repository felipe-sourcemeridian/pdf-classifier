TERM = 1
TERM_ID = 0

def get_terms_mapping_from_file(terms_file):
	fd = open(terms_file, "r")
	terms_mapping = {}
	for line in fd:
		if len(line.strip()) == 0:
			continue
		terms = line.split(",")
		terms_mapping[terms[TERM].strip()] = int(terms[TERM_ID].strip())
	fd.close()
	return terms_mapping

def get_phrases_mapping_from_file(phrases_file):
	fd = open(phrases_file, "r")
	phrases_mapping = {}
	phrase_filter = lambda phrase : len(phrase.strip()) != 0
	phrase_remove_space = lambda phrase: phrase.strip()
	for line in fd:
		if len(line.strip()) == 0:
			continue
		phrases = line.split(",")
		phrases = filter(phrase_filter, phrases)
		if(len(phrases) == 1):
			continue
		phrases = map(phrase_remove_space, phrases)
		terms_id = tuple([int(term_id) for term_id in phrases[:-1]])
		phrases_mapping[terms_id] = int(phrases[-1])
	fd.close()
	return phrases_mapping
	
