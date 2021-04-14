#include "../include/IO.h"

// 在这里写 work.cpp 会导致链接错误
#include "../include/work.h"

using namespace CodeCraft2021;

int main(int argc, char* argv[]){

	const char* file_name = "../data/training-2.txt";

	// 读取文件数据
	read_file(file_name);
	// 读取标准输入
	//read_std();
	// 主处理函数
	process();
	// 标准输出
	write();
	return 0;
}
