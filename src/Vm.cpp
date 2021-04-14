#include <iostream>
#include <vector>
// 字符串与数值互转
#include <sstream>

#include "Vm.h"


namespace CodeCraft2021 {

	// 构造函数, 使用参数构造
	Vm::Vm(std::string& name, long cpu, long memory, int isBi) :
		name(name),
		cpu(cpu),
		memory(memory),
		isBi(isBi)
	{}

	// 无参的构造函数, 用于 unordered_map的查找
	Vm::Vm() {}


	// 构造函数, 使用字符串构造
	Vm::Vm(std::vector<std::string>& vmString) :
		name(vmString[0])
	{
		std::stringstream str2digt;
		str2digt << vmString[1];
		str2digt >> cpu;
		str2digt << vmString[2];
		str2digt >> memory;
		str2digt << vmString[3];
		str2digt >> isBi;
	}

	// 析构函数
	Vm::~Vm(){}

	void Vm::toString() {
		std::cout <<  "Virtual{" 
			<< "name='" << name 
			<< ", cpu=" << cpu 
			<< ", memory=" << memory 
			<< ", isBi=" << isBi 
			<<	'}';
	}
}