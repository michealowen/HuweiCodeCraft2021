#include <iostream>
#include <vector>
// 字符串与数值互转
#include <sstream>

#include "../include/Server.h"


namespace CodeCraft2021 {


	// 构造函数, 使用参数构造
	Server::Server(std::string& serverType, long cpu, long memory, long oricost, long dailycost) :
		serverType(serverType),
		cpu(cpu),
		memory(memory),
		oricost(oricost),
		dailycost(dailycost)
	{}


	// 构造函数, 使用字符串构造
	Server::Server(std::vector<std::string>& vmString) :
		serverType(vmString[0])
	{
		std::stringstream str2digt;
		str2digt << vmString[1];
		str2digt >> cpu;
		str2digt << vmString[2];
		str2digt >> memory;
		str2digt << vmString[3];
		str2digt >> oricost;
		str2digt << vmString[4];
		str2digt >> dailycost;
	}

	// 析构函数
	Server::~Server() {}

	void Server::toString() {
		std::cout << "Virtual{"
			<< "serverType='" << serverType
			<< ", cpu=" << cpu
			<< ", memory=" << memory
			<< ", oricost=" << oricost
			<< ", dailycost=" << dailycost
			<< '}';
	}

	// 无参构造函数, 用于 unordered_map的查找
	Server::Server() {}


}