#ifndef _WORK_H_
#define _WORK_H_

#include <functional>
#include <iostream>
#include <iterator> 
#include <unordered_set>
#include <algorithm>
// 日志保存
#include <sstream>
#include <assert.h>


#include "Structure.h"
#include "Request.h"

namespace CodeCraft2021 {

	/**
	 * 处理函数
	 */
	void process();

	/**
	 * 待购买的服务器列表
	 */
	std::vector<Server> selectServer(Request& resuest);

	/**
	 * 迁移
	 */
	void migrate();

	/**
	 * 将一台已购买服务器的虚拟机迁移到另一台已购买服务器
	 */
	int migrateServerBought(ServerBought& ServerBoughtFrom, ServerBought& ServerBoughtTo, int leftMigNum);

	/**
	 * 判断已购买服务器是否为空服务器
	 */
	bool serverBoughtIsFree(ServerBought& ServerBought);

	/**
	 * 为当日删除虚拟机做出优化
	 */
	void migrate4del(const int day);

	/**
	 * 尝试从已有服务器中分配服务器给虚拟机
	 */
	bool addMatch(Request& request);

	/**
	 * 扩容策略
	 */
	void expansion(Request& request, std::vector<Server>& serverListSelected);

	/**
	 * 将虚拟机部署在服务器上
	 */
	void allocVm(VmUsed& vmUsed, ServerBought& serverBought, int node);

	/**
	 * 消耗服务器资源
	 * serverBought表示被消耗的服务器实例，vm表示要部署的虚拟机，node表示虚拟机被部署的节点
	 */
	void consumeResource(ServerBought& serverBought, VmUsed& vmUsed, int node);


	/**
	 * 释放服务器资源
	 */
	void releaseResource(VmUsed& vmUsed, ServerBought& serverBought);

	/**
	 * 从服务器上删除虚拟机
	 */
	void delVm(std::string& vmId);

	/**
	 * 将每日的操作信息写入日志中,参数为昨日的服务器个数
	 */
	void write2Log(int lastDayServerNum);

	/**
	 * 计算每日的消耗
	 */
	void calculatePower();

	// 购买天数
	//extern int purchaseNum;

	/**
	 * 检验映射关系是否一致
	 */
	void checkRelation();

	/**
	 * 检查服务器是否溢出
	 */
	void checkOverflow();

}

#endif
