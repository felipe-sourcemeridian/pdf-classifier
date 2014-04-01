from nltk.stem.porter import PorterStemmer
import re
NORMALIZE_TERM_REG = "[^a-zA-Z0-9 ]"
porter = PorterStemmer()
lower_term = lambda term: term.lower()
terms_filter = lambda term: term !=None and len(term.strip()) != 0
normalize = lambda term : re.sub(NORMALIZE_TERM_REG, "", term)
remove_space = lambda term: term.strip()
stem_terms = lambda term: porter.stem_word(term)



def split_and_normalize_terms(terms, pattern):
	new_terms = filter(terms_filter, terms)
	new_terms = split_terms(new_terms, pattern)
	new_terms = map(normalize, new_terms)
	new_terms = filter(terms_filter, new_terms)
	new_terms = map(remove_space, new_terms)
	return new_terms

def filter_and_normalize_terms(terms):
	new_terms = filter(terms_filter, terms)	
	new_terms = map(normalize, new_terms)
	new_terms = filter(terms_filter, new_terms)
	return new_terms

def split_terms(terms, pattern):
	new_terms = []
	for term in terms:
		splited_terms = re.split(pattern, term)
		new_terms.extend(splited_terms)
	return new_terms

def get_term_value_by_mappings(term, term_mapping, phrases_mapping):
	is_phrase = lambda term: len(term.strip().split(" ")) > 1
	if is_phrase(term):
		terms = get_terms_from_phrase(term, term_mapping)
		if terms in phrases_mapping:	
			return phrases_mapping[terms]
	else:
		term = stem_terms(term.lower())
		if term in term_mapping:
			return term_mapping[term]
	return None

def get_terms_from_phrase(phrase, term_mapping):	
	terms = phrase.split(" ")
	new_terms = []
	terms  = lower_and_stem_terms(terms)
	for term in terms:
		if term in term_mapping:
			new_terms.append(term_mapping[term])
	return tuple(new_terms)

def lower_and_stem_terms(terms):
	new_terms = map(lower_term, terms)
	return map(stem_terms, new_terms)
