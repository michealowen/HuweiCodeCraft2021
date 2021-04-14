#ifndef _VIRTUAL_H_
#define _VIRTUAL_H_

#include <string>
#include <vector>


namespace CodeCraft2021 {
	
	class Vm
	{

	public:

		// 型号,唯一标识，虚拟机型号长度不超过 20，仅由数字，大小写英文字符和'.'构成
		std::string name;
		// CPU核数,大小不超过 1024
		long cpu;
		// 内存大小,大小不超过 1024
		long memory;
		//是否双节点部署,0表示单节点，1表示双节点
		int isBi;

		//在哪个节点上部署,0表示左节点，1表示右节点，2表示双节点
		//int node;

		
		// 构造函数,使用参数构造
		Vm(std::string& name, long cpu, long memory, int isBi);

		// 无参的构造函数, 用于 unordered_map的查找
		Vm();


		// 构造函数, 使用字符串构造
		Vm(std::vector<std::string>& vmString);

		// 析构函数
		~Vm();
		
		
		// 快捷输出函数
		void toString();

	};
}


#endif // !_VIRTUAL_H_

