#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_

#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <functional>

#include "Vm.h"
#include "Server.h"
#include "VmUsed.h"
#include "ServerBought.h"
#include "Request.h"
#include "PutInfo.h"
#include "MigrationInfo.h"

namespace CodeCraft2021 {

	// 虚拟机信息表, key为虚拟机型号, value为虚拟机对象
	extern std::unordered_map<std::string, Vm> VmInfoDict;

	// 服务器信息表, key为服务器型号, value为服务器对象
	extern std::unordered_map<std::string, Server> ServerInfoDict;

	// 服务器列表, 按照某种优先原则排序
	extern std::vector<Server> ServerList;

	// 已部署虚拟机表, key为虚拟机编号, value为VmUsed
	extern std::unordered_map<std::string, VmUsed*> VmUsedDict;

	// 已购买服务器表, key为服务器编号, value为ServerBought
	extern std::unordered_map<int, ServerBought*> ServerBoughtDict;

	// 已部署虚拟机与已购买服务器的映射, key为已部署虚拟机的指针, value为已购买服务器的指针
	extern std::unordered_map<VmUsed*, ServerBought*> VmUsed2ServerBought;

	// 已购买服务器与已部署虚拟机列表的映射, key为已购买服务器, value为服务器上的虚拟机列表
	extern std::unordered_map<ServerBought*, std::vector<VmUsed*>> ServerBought2VmUsed;
 
	// 已购买服务器id与已部署虚拟机列表的映射, key为已购买服务器id, value为服务器上的虚拟机列表
	//extern std::unordered_map<int, std::vector<VmUsed>, ServerBoughtHash> ServerId2VmUsed;

	// 已购买服务器列表
	extern std::vector<ServerBought*> ServerBoughtList;

	// 请求信息列表
	extern std::vector<std::vector<Request>> requestLists;

	// 保存所有的输出信息
	extern std::vector<std::string> logInfo;


	// 保存每日的购买信息, key为服务器型号, value为已购买服务器
	extern std::unordered_map<std::string, std::vector<ServerBought*>> addInfo;

	// 保存部署信息
	extern std::vector<PutInfo> putInfo;

	//保存迁移信息
	extern std::vector<MigrationInfo> migrateInfo;

	// 系统能源消耗
	extern int powerFee;

	// 服务器硬件成本
	extern int serverFee;

	// 服务器按照硬件成本从小到大的排序方法
	bool serverOriLessCmp(Server& s_1, Server& s_2);

	// 为了安置双节点虚拟机, 对已购买服务器排序方法, 按照剩余空间从小到大排序
	bool serverBoughtCapLessCmpBi(ServerBought* sb_1, ServerBought* sb_2);

	// 为了安置单节点虚拟机, 对已购买服务器排序方法, 按照剩余空间从小到大排序
	bool serverBoughtCapLessCmpSin(ServerBought* sb_1, ServerBought* sb_2);

	// 对服务器总的剩余空间排序, 从小到大
	bool serverBoughtFreeCapLessCmp(ServerBought* sb_1, ServerBought* sb_2);

	// 部署日志,暗中请求序号排序
	bool putInfoRankLessCmp(PutInfo& p_1, PutInfo& p_2);

	// 对请求的处理顺序排序
	bool requestCmp(Request& r_1, Request& r_2);

	// 对已购买服务器排序, 按照空闲空间比例排序
	bool serverBoughtRatioGreaterCmp(ServerBought* sb_1, ServerBought* sb_2);

}


#endif // !_STRUCTURE_H_

