#ifndef _IO_H_
#define _IO_H_

#include <string>
#include <iostream>
#include <vector>

#include "Server.h"
#include "Vm.h"
#include "Structure.h"

//#include "Structure.h"

namespace CodeCraft2021 {
	
	/**
	 * 读取文件
	 */
	void read_file(const char* file_name);

	/**
	 * 读取标准输入
	 */
	void read_std();

	/**
	 * 标准输出
	 */
	void write();

}

#endif
