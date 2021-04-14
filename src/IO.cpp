#include "../include/IO.h"

namespace CodeCraft2021 {

	/**
	 * 读取文件
	 */
	void read_file(const char* file_name) {

		std::freopen(file_name, "rb", stdin);

		// 服务器信息输入
		int serverNum;
		// 目前的读入会读取名称前的括号,但是无影响
		std::string serverType;
		int serverCpu, serverMemory, oricost, dailycost;
		scanf("%d", &serverNum);


		// '(', ')'
		char leftParentheses, rightParentheses;
		// ','
		char comma;

		for (int i = 0; i < serverNum; ++i)
		{
			// 读入请求的服务器类型, serverType="hostVQJT7,", 需要去掉结尾的逗号
			std::cin >> leftParentheses >> serverType;
			serverType = std::string(serverType.begin(), serverType.end() - 1);
			scanf("%d,", &serverCpu);
			scanf("%d,", &serverMemory);
			scanf("%d,", &oricost);
			scanf("%d)", &dailycost);
			// 保存服务器信息
			Server s(serverType, serverCpu, serverMemory, oricost, dailycost);
			ServerInfoDict.insert({ serverType, s });
			ServerList.push_back(s);
		}

		// 虚拟机信息输入
		int vmNum = 0;
		scanf("%d", &vmNum);

		std::string vmType;
		int vmCpu, vmRam, isBi;

		for (int i = 0; i < vmNum; ++i) {
			// 读入请求的虚拟机类型, reqVmType="vmJVH7U,", 需要去掉结尾的逗号
			std::cin >> leftParentheses >> vmType;
			vmType = std::string(vmType.begin(), vmType.end() - 1);
			scanf("%d,", &vmCpu);
			scanf("%d,", &vmRam);
			scanf("%d)", &isBi);
			// 保存虚拟机信息
			Vm v(vmType, vmCpu, vmRam, isBi);
			VmInfoDict.insert({ vmType, v });
		}

		// 请求读取
		int requestDays = 0, dailyRequestNum = 0;
		scanf("%d", &requestDays);
		// 请求格式
		std::string op, reqVmType, vmId;

		for (int day = 0; day < requestDays; ++day) {
			// 当日请求次数 例如(add, vmLQMN4, 796395948)
			scanf("%d", &dailyRequestNum);
			// 读取当日请求
			// 当日的请求
			std::vector<Request> requests;
			for (int i = 0; i < dailyRequestNum; ++i) {
				std::cin >> leftParentheses >> op;
				// op = "add," 或 "del," 需要去掉结尾的逗号
				op = std::string(op.begin(), op.end()-1);
				// 判断是否为add请求
				if (op[0] == 'a') {
					// 读入请求的虚拟机类型, reqVmType="vmJVH7U,", 需要去掉结尾的逗号
					std::cin >> reqVmType;
					reqVmType = std::string(reqVmType.begin(), reqVmType.end() - 1);
					// 读入虚拟机id, vmId="686825538)", 需要去掉结尾的逗号
					std::cin >> vmId;
					vmId = std::string(vmId.begin(), vmId.end() - 1);
					// 保存add请求
					Request r(std::vector<std::string>{op, reqVmType, vmId}, i);
					requests.push_back(r);
				}
				else {
					// 读入虚拟机id, vmId="686825538)", 需要去掉结尾的逗号
					std::cin >> vmId;
					vmId = std::string(vmId.begin(), vmId.end() - 1);
					// 保存del请求
					Request r(std::vector<std::string>{op, vmId}, i);
					requests.push_back(r);
				}
			}
			requestLists.push_back(requests);
		}

	}
	
	/**
	 * 读取标准输入
	 */
	void read_std() {

		// 服务器信息输入
		int serverNum;
		// 目前的读入会读取名称前的括号,但是无影响
		std::string serverType;
		int serverCpu, serverMemory, oricost, dailycost;
		scanf("%d", &serverNum);

		// '(', ')'
		char leftParentheses;
		// ','
		char comma;

		for (int i = 0; i < serverNum; ++i)
		{
			std::cin >> leftParentheses >> serverType;
			scanf("%d,", &serverCpu);
			scanf("%d,", &serverMemory);
			scanf("%d,", &oricost);
			scanf("%d)", &dailycost);
			// 保存服务器信息
			Server s(serverType, serverCpu, serverMemory, oricost, dailycost);
			ServerInfoDict.insert({ serverType, s });
			ServerList.push_back(s);
		}

		// 虚拟机信息输入
		int vmNum = 0;
		scanf("%d", &vmNum);

		std::string vmType;
		int vmCpu, vmRam, isBi;

		for (int i = 0; i < vmNum; ++i) {
			std::cin >> leftParentheses >> vmType;
			scanf("%d,", &vmCpu);
			scanf("%d,", &vmRam);
			scanf("%d)", &isBi);
			// 保存虚拟机信息
			Vm v(vmType, vmCpu, vmRam, isBi);
			VmInfoDict.insert({ vmType, v });
		}

		// 请求读取
		int requestDays = 0, dailyRequestNum = 0;
		scanf("%d", &requestDays);
		// 请求格式
		std::string op, reqVmType, vmId;

		for (int day = 0; day < requestDays; ++day) {
			// 当日请求次数 例如(add, vmLQMN4, 796395948)
			scanf("%d", &dailyRequestNum);
			// 读取当日请求
			std::vector<Request> requests;
			for (int i = 0; i < dailyRequestNum; ++i) {
				std::cin >> leftParentheses >> op;
				// 判断是否为add请求
				if (op[0] == 'a') {
					// 读入请求的虚拟机类型
					std::cin >> reqVmType >> comma;
					// 读入虚拟机id
					std::cin >> vmId;
					// 保存add请求
					Request r(std::vector<std::string>{op, reqVmType, vmId}, i);
					requests.push_back(r);
				}
				else {
					std::cin >> vmId;
					// 保存del请求
					Request r(std::vector<std::string>{op, vmId}, i);
					requests.push_back(r);
				}
			}
			requestLists.push_back(requests);
		}

	}
	
	/**
	 * 标准输出
	 */
	void write() {
		for (std::string log : logInfo) {
			std::cout << log << std::endl;
		}
	}

}