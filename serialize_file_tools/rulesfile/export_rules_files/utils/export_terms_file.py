import struct
def export_terms_textplain(term_file, terms):
	fd = open(term_file, "a")
	terms = invert_mapping(terms)
	terms_ids = terms.keys()
	terms_ids.sort()
	for term_id in terms_ids:
		fd.write("%d,%s\n"% (term_id, terms[term_id]))
	fd.close()

def export_phrase_textplain(phrases_file, phrases ):
	fd = open(phrases_file, "a")
	for words, phrase_id in phrases.items():
		words_text = ",".join([str(word) for word in words])
		fd.write("%s,%d\n" %(words_text, phrase_id))
	fd.close()

def invert_mapping(mapping):
	new_mapping = dict([(value, key) for key, value in mapping.items()])
	return new_mapping
