NODE_BRIDGE_ACTIVE_QUEY = "select NODEID, FROM CORPUS INNER JOIN NODE ON corpus.corpusid = node.corpusid and node.bridgenode=0 WHERE corpus.corpusactive = 1 and  CORPUSTOCLASSIFY = 1"

NODE_BRIDGE_ACTIVE_QUERY_BY_PAGE = " select * from ( select inner_query.*, rownum rn from (select NODEID FROM CORPUS INNER JOIN NODE ON corpus.corpusid = node.corpusid and node.bridgenode=0 WHERE corpus.corpusactive = 1 and  CORPUSTOCLASSIFY = 1) inner_query)outer_query where outer_query.rn > {low_index} and outer_query.rn < {upper_index}"


SIGTERM_QUERY = "SELECT   NODE.NODEID,NODE.NODESIZE,NODE.NODETYPE,SIGNATUREWORD,SIGNATUREOCCURENCES FROM NODE INNER JOIN SIGNATURE ON SIGNATURE.NODEID=NODE.NODEID WHERE NODE.NODEID IN ({node_ids})"


UNFILTERED_NODE_QUERY =  "select NODEID FROM CORPUS INNER JOIN NODE ON corpus.corpusid = node.corpusid and node.bridgenode=0 WHERE corpus.corpusactive = 1 and  CORPUSTOCLASSIFY = 1 and corpusterms is null  and node.NODEMUSTHAVEGATEINHERITANCE is null"

CORPUS_ID_QUERY = "SELECT CORPUSID FROM CORPUS WHERE CORPUSACTIVE =1 and  CORPUSTOCLASSIFY = 1 and CORPUSID != 1000094 and CORPUSID!=1000003"

TERM_BY_CORPUS_QUERY = "SELECT NODE.NODEID,CASE nvl(NODE.NODEMUSTHAVEGATEINHERITANCE, -1) WHEN -1 THEN (SELECT CORPUS.CORPUSTERMS FROM CORPUS WHERE CORPUS.CORPUSID={corpus_id}) WHEN 0    THEN (LINKAPEDIA.GET_MUSTHAVEGATE_WORDS(NODE.NODEID)) ELSE (LINKAPEDIA.GET_MUSTHAVE_WORDS((SELECT PARENT_NODE.NODEID FROM NODE PARENT_NODE WHERE LEVEL = NODE.NODEMUSTHAVEGATEINHERITANCE START WITH PARENT_NODE.NODEID = NODE.PARENTID CONNECT BY PRIOR PARENT_NODE.PARENTID = PARENT_NODE.NODEID))) END AS TERM FROM NODE INNER JOIN CORPUS ON CORPUS.CORPUSID = NODE.CORPUSID WHERE NODE.bridgenode=0 and NODE.CORPUSID= {corpus_id}"


NODE_ID = 0
NODE_SIZE = 1
NODE_TYPE = 2

SIGNATUREWORD = 3
SIGNATUREOCCURENCES = 4 

MUSTHAVE_NODE_ID = 0
MUSTHAVE_TERM = 1

CORPUS_NODE_ID = 0
CORPUS_TERM = 1

def find_node_bridges_active(con, low_index, upper_index):
	nodes = []
	try:
		cursor = con.cursor()
		query = NODE_BRIDGE_ACTIVE_QUERY_BY_PAGE.format(low_index = low_index, upper_index = upper_index)
		cursor.execute(query)
		for node in cursor:
			nodes.append(node[0])
		return nodes
	finally:
		cursor.close()
	
def find_all_nodes_bridge_active(con):
	try:
		nodes = []
		cursor_nodes = con.cursor()
		cursor_nodes.execute(NODE_BRIDGE_ACTIVE_QUEY)
		for node in cursor_nodes:
			nodes.append(node[0])
		return nodes
	finally:
		cursor_nodes.close()

def find_musthave_terms_by_nodes(nodes, con):
	musthave_terms = []
	ids = ",".join([str(node) for node in nodes])
	musthave_query = MUSTHAVE_TERM_QUERY.format(node_ids = ids)
	try:
		cursor = con.cursor()
		cursor.execute(musthave_query)
		for musthave in cursor:
			node_id = musthave[MUSTHAVE_NODE_ID]
			term = musthave[MUSTHAVE_TERM] or ""
			musthave_terms.append([node_id, term])
	finally:
		cursor.close()
	return musthave_terms

def find_sigterms_by_nodes(nodes, con):
	sigterms = []
	sigterms_ids = ",".join([str(node) for node in nodes])
	sigterm_query = SIGTERM_QUERY.format(node_ids = sigterms_ids)
	try:
		cursor_sigterm = con.cursor()
		cursor_sigterm.execute(sigterm_query)
		for sigterm in cursor_sigterm:
			sigterms.append(sigterm)
		return sigterms
	finally:
		cursor_sigterm.close()

def find_all_corpus_id(con):
	try:
		corpus_ids = []
		cursor = con.cursor()
		cursor.execute(CORPUS_ID_QUERY)
		for corpus in cursor:
			corpus_ids.append(corpus[0])
		return corpus_ids
	finally:
		cursor.close()

def find_active_term_by_corpus_id(con, corpus_id):
	try:
		active_terms = []
		cursor = con.cursor()
		query = TERM_BY_CORPUS_QUERY.format(corpus_id = corpus_id)
		cursor.execute(query)
		for term in cursor:
			active_terms.append(term)
		return active_terms
	finally:
		cursor.close()

def find_all_unfiltered_nodes(con, max_corpus_id):
	try:
		cursor = con.cursor()
		cursor.execute(UNFILTERED_NODE_QUERY)
		unfiltered_nodes = []
		for unfiltered_node in cursor:
			unfiltered_nodes.append(unfiltered_node[0])
			if len(unfiltered_nodes) == max_corpus_id:
				yield unfiltered_nodes
				unfiltered_nodes = []
		yield unfiltered_nodes
	finally:
		cursor.close()
