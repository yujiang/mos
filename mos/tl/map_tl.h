#ifndef MAP_TL_H
#define MAP_TL_H

//map_tl使用.map格式，就是jpg分块并且存alpha通道
//并且在内存中生成带通道的A8888贴图
//绘制方法：先禁用alphablend绘制地图，在绘制sprite的过程中，如果跟alpha有交集，就把交集再绘制一次。


class map_source;
map_source* get_map_source_tl();
#endif