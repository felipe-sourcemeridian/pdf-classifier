import utils.stemmer_util as stemmer_util
PHRASE_DELIMITER = " "
PHRASE_CORPUS_DELIMITER = ","

def generate_phrases_mapping_from_term_mapping(phrases, term_mapping, phrases_mapping, begin_id):
	new_phrases_mapping = {}
	for phrase in phrases:
		terms = get_term_ids_from_phrase(phrase, term_mapping) 
		if terms in phrases_mapping:
			continue
		phrases_mapping[terms] = begin_id
		new_phrases_mapping[terms] = begin_id
		begin_id = begin_id + 1
	return new_phrases_mapping

def get_phrases_corpus_from_terms(terms):
	phrases = []
	for term in terms:
		current_terms = term.split(PHRASE_CORPUS_DELIMITER)
		phrases.extend(get_phrases_from_terms(current_terms))
	return phrases

def get_phrases_from_terms(terms):
	is_phrase = lambda term : len(term.split(PHRASE_DELIMITER)) > 1
	phrases = []
	for term in terms:
		if is_phrase(term):
			phrases.append(term)
	return phrases
	

def get_term_ids_from_phrase(phrase, term_mapping):
	term_ids = []
	terms = phrase.split(PHRASE_DELIMITER)
	terms = stemmer_util.filter_and_normalize_terms(terms)
	terms = stemmer_util.lower_and_stem_terms(terms) 
	for term in terms:
		term_ids.append(term_mapping[term])
	return tuple(term_ids)

