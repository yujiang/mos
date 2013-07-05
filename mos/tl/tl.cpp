
#include "image_zgp.h"
#include "map_tl.h"
#include "graph/graph.h"
#include "map/map.h"


void init_tl()
{
	get_graph()->regist_file_source(get_file_source_zgp());
	get_map()->register_map_source("map",&get_map_source_tl);
}