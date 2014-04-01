import repo.node_document_repo as node_document_repo
def export_unfiltered_node(unfiltered_node_file, con, max_unfiltered_node = 1000):
	for unfiltered_nodes in node_document_repo.find_all_unfiltered_nodes(con, max_unfiltered_node):
		export_unfiltered_node_texplain(unfiltered_node_file, unfiltered_nodes)

def export_unfiltered_node_texplain(unfiltered_node_file, unfiltered_nodes)		:
	fd = open(unfiltered_node_file, "a")
	for unfiltered_node in unfiltered_nodes:
		fd.write("%d\n" %(unfiltered_node))
	fd.close()
