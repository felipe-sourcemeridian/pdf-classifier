ACTIVE_NODE_ID_QUERY = "select outer_query.* from (select inner_query.*,rownum rn from (select NODEID, bridgenode FROM CORPUS INNER JOIN NODE ON corpus.corpusid = node.corpusid WHERE corpus.corpusactive = 1 and CORPUSTOCLASSIFY = 1) inner_query)outer_query where outer_query.rn > {low_index} and outer_query.rn < {upper_index}"

CORPUS_TERMS_QUERY = "select distinct corpusterms FROM CORPUS WHERE corpus.corpusactive = 1 and  CORPUSTOCLASSIFY = 1"


MUSTHAVE_QUERY = "SELECT NODEID,MUSTWORD FROM MUSTHAVE WHERE NODEID IN({node_ids})"

MUSTHAVEGAVE_TERMNS_QUERY = "SELECT DISTINCT MUSTWORD FROM NODE INNER JOIN NODEMUSTHAVEGATE ON NODEMUSTHAVEGATE.NODEID=NODE.NODEID WHERE NODE.NODEID IN({node_ids})"

SIGNATURE_QUERY = "SELECT NODE.NODEID,NODE.NODESIZE,NODE.NODETYPE,SIGNATUREWORD,SIGNATUREOCCURENCES FROM NODE INNER JOIN SIGNATURE ON SIGNATURE.NODEID=NODE.NODEID WHERE NODE.NODEID IN ({node_ids})"

NODE_ID = 0
BRIDGE = 1

SIGNATURE_NODE_ID = 0
SIGNATURE_NODE_SIZE = 1
SIGNATURE_NODETYPE = 2
SIGNATURE_TERM = 3
SIGNATURE_OCURRENCE = 4

MUSTHAVE_NODE_ID = 0
MUSTHAVE_TERM = 1

def find_signatures_by_node_ids(node_ids, con):
	ids = get_ids_string_from_node_ids(node_ids)
	query = SIGNATURE_QUERY.format(node_ids = ids)
	signatures = []
	try:
		cursor = con.cursor()
		cursor.execute(query)
		signatures = [signature for signature in cursor]
		return signatures
		
	finally:
		cursor.close()

def find_musthaves_by_node_ids(node_ids, con):
	ids = get_ids_string_from_node_ids(node_ids)
	query = MUSTHAVE_QUERY.format(node_ids = ids)
	try:
		cursor = con.cursor()
		cursor.execute(query)
		musthaves = [musthave for musthave in cursor]
		return musthaves
	finally:
		cursor.close()

def find_musthavegate_terms_by_node_ids(node_ids, con):
	return get_terms_by_query_and_node_ids(MUSTHAVEGAVE_TERMNS_QUERY, node_ids, con)

def find_active_nodes(con, low_index, upper_index):
	try:
		cursor = con.cursor()
		query = ACTIVE_NODE_ID_QUERY.format(low_index = low_index, upper_index = upper_index)
		cursor.execute(query)
		nodes = [(node_record[NODE_ID], int(node_record[BRIDGE])) for node_record in cursor]
		return nodes
	finally:
		cursor.close()

def find_corpus_terms(con):
	try:
		cursor = con.cursor()
		cursor.execute(CORPUS_TERMS_QUERY)
		return get_terms_from_cursor(cursor)
	finally:
		cursor.close()

def get_terms_by_query_and_node_ids(query, node_ids, con):
	ids = get_ids_string_from_node_ids(node_ids)
	try:
		cursor = con.cursor()
		sql_query = query.format(node_ids = ids) 
		cursor.execute(sql_query)
		return get_terms_from_cursor(cursor)
	finally:
		cursor.close()

def get_terms_from_cursor(cursor):
	terms = []
	for term in cursor:
		terms.append(term[0])
	return terms

def get_ids_string_from_node_ids(node_ids): 
	ids = ",".join([str(node_id) for node_id in node_ids])
	return ids
