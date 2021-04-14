#include "../include/Structure.h"

namespace CodeCraft2021 {

	// 虚拟机信息表, key为虚拟机型号, value为虚拟机对象
	std::unordered_map<std::string, Vm> VmInfoDict;

	// 服务器信息表, key为服务器型号, value为服务器对象
	std::unordered_map<std::string, Server> ServerInfoDict;

	// 服务器列表, 按照某种优先原则排序
	std::vector<Server> ServerList;

	// 已部署虚拟机表, key为虚拟机编号, value为VmUsed
	std::unordered_map<std::string, VmUsed*> VmUsedDict;

	// 已购买服务器表, key为服务器编号, value为ServerBought
	std::unordered_map<int, ServerBought*> ServerBoughtDict;

	// 已部署虚拟机与已购买服务器的映射, key为已部署虚拟机, value为已购买服务器
	std::unordered_map<VmUsed*, ServerBought*> VmUsed2ServerBought;

	// 已购买服务器与已部署虚拟机列表的映射, key为已购买服务器的指针, value为服务器上的虚拟机(指针)列表
	std::unordered_map<ServerBought*, std::vector<VmUsed*>> ServerBought2VmUsed;

	// 已购买服务器id与已部署虚拟机列表的映射, key为已购买服务器id, value为服务器上的虚拟机列表
	//std::unordered_map<int, std::vector<VmUsed>, ServerBoughtHash> ServerId2VmUsed;

	// 已购买服务器列表
	std::vector<ServerBought*> ServerBoughtList;

	// 请求信息列表
	std::vector<std::vector<Request>> requestLists;

	// 保存所有的输出信息
	std::vector<std::string> logInfo;

	// 保存每日的购买信息, key为服务器型号, value为已购买服务器
	std::unordered_map<std::string, std::vector<ServerBought*>> addInfo;

	// 保存部署信息
	std::vector<PutInfo> putInfo;

	//保存迁移信息
	std::vector<MigrationInfo> migrateInfo;

	// 系统能源消耗
	int powerFee = 0;

	// 服务器硬件成本
	int serverFee = 0;

	// 服务器按照硬件成本从小到大的排序方法
	bool serverOriLessCmp(Server& s_1, Server& s_2) {
		return s_1.oricost < s_2.oricost;
	}

	// 为了安置双节点虚拟机, 对已购买服务器排序方法, 按照剩余空间从小到大排序
	bool serverBoughtCapLessCmpBi(ServerBought* sb_1, ServerBought* sb_2) {
		//return (*sb_1).cpusizel + (*sb_1).cpusizer + (*sb_1).memsizel + (*sb_1).memsizer - 
			//(*sb_2).cpusizel - (*sb_2).cpusizer - (*sb_2).memsizel - (*sb_2).memsizer;
		return std::min((*sb_1).cpusizel + (*sb_1).memsizel, (*sb_1).cpusizer + (*sb_1).memsizer) <
			std::min((*sb_2).cpusizel + (*sb_2).memsizel, (*sb_2).cpusizer + (*sb_2).memsizer);
	}

	// 为了安置单节点虚拟机, 对已购买服务器排序方法, 按照剩余空间从小到大排序
	bool serverBoughtCapLessCmpSin(ServerBought* sb_1, ServerBought* sb_2) {
		// 对剩余空间大的节点进行比较
		return std::max((*sb_1).cpusizel + (*sb_1).memsizel, (*sb_1).cpusizer + (*sb_1).memsizer) <
			std::max((*sb_2).cpusizel + (*sb_2).memsizel, (*sb_2).cpusizer + (*sb_2).memsizer);
	}

	// 对服务器总的剩余空间排序, 从小到大
	bool serverBoughtFreeCapLessCmp(ServerBought* sb_1, ServerBought* sb_2) {
		return ((*sb_1).cpusizel + (*sb_1).cpusizer + (*sb_1).memsizel + (*sb_1).memsizer) > 
			((*sb_2).cpusizel + (*sb_2).cpusizer + (*sb_2).memsizel + (*sb_2).memsizer);
	}


	// 部署日志,暗中请求序号排序
	bool putInfoRankLessCmp(PutInfo& p_1, PutInfo& p_2) {
		return p_1.rank < p_2.rank;
	}


	// 对请求的处理顺序排序
	bool requestCmp(Request& r_1, Request& r_2) {
		// 若请求类型一样
		if (r_1.operation == r_2.operation) {
			// add 请求资源大的放前面
			if (r_1.operation == "add") {
				Vm vm_1 = VmInfoDict[r_1.vmType];
				Vm vm_2 = VmInfoDict[r_2.vmType];
				return (vm_1.cpu + vm_1.memory) < (vm_2.cpu + vm_2.memory);
			}
			// del 请求按照rank排序
			else {
				return r_1.rank < r_2.rank;
			}
		}
		// 若请求类型不一样, add排在前面, del排在后面
		else {
			if (r_1.operation == "add")
				return true;
			else
				return false;
		}
	}

	// 对已购买服务器排序, 按照空闲空间比例排序, 从大到小
	bool serverBoughtRatioGreaterCmp(ServerBought* sb_1, ServerBought* sb_2) {
		// 剩余空间比例 = 1 - 已占用空间/总空间
		Server& s_1 = ServerInfoDict[sb_1->serverType];
		Server& s_2 = ServerInfoDict[sb_2->serverType];
		double ratio_1 = 1 - (sb_1->cpusizel + sb_1->cpusizer + sb_1->memsizel + sb_1->memsizer) * 1.0
			/ (s_1.cpu + s_1.memory);
		double ratio_2 = 1 - (sb_2->cpusizel + sb_2->cpusizer + sb_2->memsizel + sb_2->memsizer) * 1.0
			/ (s_2.cpu + s_2.memory);
		return ratio_1 > ratio_2;
	}

}