#ifndef IMAGE_ZGP_H
#define IMAGE_ZGP_H

//image_zgp使用zgp格式，256色每个图片分块（头部，上身，下身，装饰）存储，每个块都有一个调色板
//其文件格式是每个位置存两个信息——调色板序号+颜色值0-255
//zgp也有动画信息，某个动作的8个方向按照序列帧打包在一起

//渲染的时候，临时生成贴图，并且缓存下来

//get zgp info...
bool regist_zgp(const char* file,int& dir,int& frame);

class file_source;
file_source* get_file_source_zgp();
#endif