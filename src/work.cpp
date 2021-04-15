#include "../include/work.h"

namespace CodeCraft2021 {

	/**
	 * 处理函数
	 */
	void process() {

		//std::cout << requestLists.size() << std::endl;

		// 处理每一天的请求
		for (int day = 0; day < requestLists.size(); day++) {

			//std::cout << "第" << day << "天" << std::endl;

			/* 迁移 */
			migrate();

			/* 针对当天删除请求的迁移 */
			//migrate4del(day);

			/* 处理当日请求 */
			std::vector<Request> dayRequests = requestLists[day];
			// 对当日请求排序, 递减排序
			std::sort(dayRequests.begin(), dayRequests.end(), requestCmp);
			
			// 处理本日请求前的服务器个数
			int lastDayServerNum = ServerBoughtList.size();

			for (Request& request : dayRequests) {
				// add请求
				if (request.operation[0] == 'a') {

					// 先尝试从已有服务器中分配
					bool canStore = addMatch(request);
					//bool canStore = false;
					// 若尝试分配失败, 则扩容
					if (canStore == false) {
						std::vector<Server> serverListSelected = selectServer(request);
						expansion(request, serverListSelected);
					}
				}
				// del请求
				else {
					// 删除虚拟机
					delVm(request.vmId);
				}
			}

			/*把每天的操作信息都放入到日志中，最后统一输出*/
			write2Log(lastDayServerNum);
			// 计算能源消耗
			//calculatePower();
			// 检查映射关系
			checkRelation();
			// 检查是否溢出
			checkOverflow();
		}
		
		//std::cout << "总硬件成本:" << serverFee << std::endl;
		//std::cout << "总运行成本:" << powerFee << std::endl;
		//std::cout << purchaseNum << std::endl;
		//std::cout << logInfo.size() << std::endl;
	}

	/**
	 * 迁移
	 * 将服务器按照空闲空间比例由大到小排序, 从空闲比例大的服务器迁移到空闲比例小的服务器
	 */
	void migrate() {

		// 计算剩余迁移次数--虚拟机总数的千分之五的下界
		int leftMigNum = (int)std::floor(VmUsed2ServerBought.size() * 0.005);

		// 循环次数
		int cycleNum = 0;

		// 双指针
		int left = 0;
		int right = ServerBoughtList.size() - 1;

		while (cycleNum < 10) {

			// 先对已购买服务器按照空闲空间比例由大到小排序
			std::sort(ServerBoughtList.begin(), ServerBoughtList.end(), serverBoughtRatioGreaterCmp);
			
			while (left < right && leftMigNum > 0) {
				// 选出left与right指向的已购买服务器
				ServerBought* ServerBoughtLeft = ServerBoughtList[left];
				ServerBought* ServerBoughtRight = ServerBoughtList[right];

				// 尝试将虚拟机从ServerBoughtLeft迁往ServerBoughtRight
				int migNum = migrateServerBought(*ServerBoughtLeft, *ServerBoughtRight, leftMigNum);
				//int migNum = 0;
				leftMigNum -= migNum;
				// 若当前left服务器迁空
				if (serverBoughtIsFree(*ServerBoughtLeft) == true) {
					++left;
					right = ServerBoughtList.size() - 1;
				}
				else {
					--right;
				}
			}

			// 内层循环退出, 说明left服务器实在迁不动了
			++left;
			right = ServerBoughtList.size() - 1;
			++cycleNum;
		}

	}


	/**
	 * 将一台已购买服务器的虚拟机迁移到另一台已购买服务器, 返回迁移的虚拟机数量
	 */
	int migrateServerBought(ServerBought& serverBoughtFrom, ServerBought& serverBoughtTo, int leftMigNum) {

		// 迁出服务器上虚拟机个数
		int vmUsedListSz = ServerBought2VmUsed[&serverBoughtFrom].size();

		// 虚拟机迁移数量
		int curMigNum = 0;

		// 将要从 ServerBoughtFrom 中移除的虚拟机
		std::unordered_set<VmUsed*> vmUsedToDel;

		for (int i = 0; i < vmUsedListSz; ++i) {
			// 当前虚拟机
			VmUsed& vmUsed = *(ServerBought2VmUsed[&serverBoughtFrom][i]);
			// 虚拟机规格
			Vm& vm = VmInfoDict[vmUsed.vmType];
			// 当前虚拟机是否可以迁移
			bool flag = false;
			// 若能够迁移, 迁移到的节点
			int toNode;

			// 双节点
			if (vmUsed.node == 2) {
				if (std::min(serverBoughtTo.cpusizel, serverBoughtTo.cpusizer) >= vm.cpu / 2 &&
					std::min(serverBoughtTo.memsizel, serverBoughtTo.memsizer) >= vm.memory / 2) {
					
					flag = true;
					toNode = 2;
				}
			}
			// 单节点
			else {
				if (serverBoughtTo.cpusizel >= vm.cpu &&
					serverBoughtTo.memsizel >= vm.memory) {

					flag = true;
					toNode = 0;
				}
				else if (serverBoughtTo.cpusizer >= vm.cpu &&
					serverBoughtTo.memsizer >= vm.memory) {
					
					flag = true;
					toNode = 1;
				}
			}

			// 若当前虚拟机器能够迁移
			if (flag == true) {
				// 释放服务器资源
				releaseResource(vmUsed, serverBoughtFrom);
				// 将虚拟机部署在 ServerBoughtTo 上
				VmUsed2ServerBought[&vmUsed] = &serverBoughtTo;
				vmUsed.node = toNode;
				ServerBought2VmUsed[&serverBoughtTo].push_back(&vmUsed);
				consumeResource(serverBoughtTo, vmUsed, toNode);
				// 将该虚拟机加入待删除的列表
				vmUsedToDel.insert(&vmUsed);
				++curMigNum;
				// 写入迁移信息
				migrateInfo.push_back(MigrationInfo(vmUsed.vmId, serverBoughtTo.serverId, toNode));
			}
		}

		// 从ServerBoughtFrom中删除这些虚拟机, 边遍历边删除
		std::vector<VmUsed*>::iterator it = ServerBought2VmUsed[&serverBoughtFrom].begin();
		while (it != ServerBought2VmUsed[&serverBoughtFrom].end()) {
			if ( vmUsedToDel.find(*it)!= vmUsedToDel.end()) {
				// 删除服务器到虚拟机的映射
				ServerBought2VmUsed[&serverBoughtFrom].erase(it);
			}
			else
				++it;
		}

		// 返回迁移的虚拟机数量
		return curMigNum;
	}

	/**
	 * 将一台已购买服务器上的del虚拟机迁移到另一台, 返回迁移数量
	 */
	 int migrateDelServerBought(ServerBought& serverBoughtFrom, ServerBought& serverBoughtTo,
                                std::unordered_set<VmUsed*>& delVmUsedSet, int leftMigNum){

        // 迁出服务器上虚拟机个数
        int vmUsedListSz = ServerBought2VmUsed[&serverBoughtFrom].size();

        // 虚拟机迁移数量
        int curMigNum = 0;

        // 将要从 ServerBoughtFrom 中移除的虚拟机
        std::unordered_set<VmUsed*> vmUsedToDel;

        for (int i = 0; i < vmUsedListSz; ++i) {
            // 当前虚拟机
            VmUsed& vmUsed = *(ServerBought2VmUsed[&serverBoughtFrom][i]);
            // 若该虚拟不属于今日删除的虚拟机, continue
            if(delVmUsedSet.find(&vmUsed) == delVmUsedSet.end())
                continue;
            // 虚拟机规格
            Vm& vm = VmInfoDict[vmUsed.vmType];
            // 当前虚拟机是否可以迁移
            bool flag = false;
            // 若能够迁移, 迁移到的节点
            int toNode;

            // 双节点
            if (vmUsed.node == 2) {
                if (std::min(serverBoughtTo.cpusizel, serverBoughtTo.cpusizer) >= vm.cpu / 2 &&
                    std::min(serverBoughtTo.memsizel, serverBoughtTo.memsizer) >= vm.memory / 2) {

                    flag = true;
                    toNode = 2;
                }
            }
            // 单节点
            else {
                if (serverBoughtTo.cpusizel >= vm.cpu &&
                    serverBoughtTo.memsizel >= vm.memory) {

                    flag = true;
                    toNode = 0;
                }
                else if (serverBoughtTo.cpusizer >= vm.cpu &&
                         serverBoughtTo.memsizer >= vm.memory) {

                    flag = true;
                    toNode = 1;
                }
            }

            // 若当前虚拟机器能够迁移
            if (flag == true) {
                // 释放服务器资源
                releaseResource(vmUsed, serverBoughtFrom);
                // 将虚拟机部署在 ServerBoughtTo 上
                VmUsed2ServerBought[&vmUsed] = &serverBoughtTo;
                vmUsed.node = toNode;
                ServerBought2VmUsed[&serverBoughtTo].push_back(&vmUsed);
                consumeResource(serverBoughtTo, vmUsed, toNode);
                // 将该虚拟机加入待删除的列表
                vmUsedToDel.insert(&vmUsed);
                ++curMigNum;
                // 写入迁移信息
                migrateInfo.push_back(MigrationInfo(vmUsed.vmId, serverBoughtTo.serverId, toNode));
            }
        }

        // 从ServerBoughtFrom中删除这些虚拟机, 边遍历边删除
        std::vector<VmUsed*>::iterator it = ServerBought2VmUsed[&serverBoughtFrom].begin();
        while (it != ServerBought2VmUsed[&serverBoughtFrom].end()) {
            if ( vmUsedToDel.find(*it)!= vmUsedToDel.end()) {
                // 删除服务器到虚拟机的映射
                ServerBought2VmUsed[&serverBoughtFrom].erase(it);
            }
            else
                ++it;
        }

        // 返回迁移的虚拟机数量
        return curMigNum;

	 }

    /**
     * 将一台已购买服务器上的普通虚拟机迁移到另一台, 返回迁移数量
     */
    int migrateCommonServerBought(ServerBought& serverBoughtFrom, ServerBought& serverBoughtTo,
                               std::unordered_set<VmUsed*>& delVmUsedSet, int leftMigNum){

        // 迁出服务器上虚拟机个数
        int vmUsedListSz = ServerBought2VmUsed[&serverBoughtFrom].size();

        // 虚拟机迁移数量
        int curMigNum = 0;

        // 将要从 ServerBoughtFrom 中移除的虚拟机
        std::unordered_set<VmUsed*> vmUsedToDel;

        for (int i = 0; i < vmUsedListSz; ++i) {
            // 当前虚拟机
            VmUsed& vmUsed = *(ServerBought2VmUsed[&serverBoughtFrom][i]);
            // 若该虚拟属于今日删除的虚拟机, continue
            if(delVmUsedSet.find(&vmUsed) != delVmUsedSet.end())
                continue;
            // 虚拟机规格
            Vm& vm = VmInfoDict[vmUsed.vmType];
            // 当前虚拟机是否可以迁移
            bool flag = false;
            // 若能够迁移, 迁移到的节点
            int toNode;

            // 双节点
            if (vmUsed.node == 2) {
                if (std::min(serverBoughtTo.cpusizel, serverBoughtTo.cpusizer) >= vm.cpu / 2 &&
                    std::min(serverBoughtTo.memsizel, serverBoughtTo.memsizer) >= vm.memory / 2) {

                    flag = true;
                    toNode = 2;
                }
            }
                // 单节点
            else {
                if (serverBoughtTo.cpusizel >= vm.cpu &&
                    serverBoughtTo.memsizel >= vm.memory) {

                    flag = true;
                    toNode = 0;
                }
                else if (serverBoughtTo.cpusizer >= vm.cpu &&
                         serverBoughtTo.memsizer >= vm.memory) {

                    flag = true;
                    toNode = 1;
                }
            }

            // 若当前虚拟机器能够迁移
            if (flag == true) {
                // 释放服务器资源
                releaseResource(vmUsed, serverBoughtFrom);
                // 将虚拟机部署在 ServerBoughtTo 上
                VmUsed2ServerBought[&vmUsed] = &serverBoughtTo;
                vmUsed.node = toNode;
                ServerBought2VmUsed[&serverBoughtTo].push_back(&vmUsed);
                consumeResource(serverBoughtTo, vmUsed, toNode);
                // 将该虚拟机加入待删除的列表
                vmUsedToDel.insert(&vmUsed);
                ++curMigNum;
                // 写入迁移信息
                migrateInfo.push_back(MigrationInfo(vmUsed.vmId, serverBoughtTo.serverId, toNode));
            }
        }

        // 从ServerBoughtFrom中删除这些虚拟机, 边遍历边删除
        std::vector<VmUsed*>::iterator it = ServerBought2VmUsed[&serverBoughtFrom].begin();
        while (it != ServerBought2VmUsed[&serverBoughtFrom].end()) {
            if ( vmUsedToDel.find(*it)!= vmUsedToDel.end()) {
                // 删除服务器到虚拟机的映射
                ServerBought2VmUsed[&serverBoughtFrom].erase(it);
            }
            else
                ++it;
        }

        // 返回迁移的虚拟机数量
        return curMigNum;

    }


	/**
	 * 判断已购买服务器是否为空服务器
	 */
	bool serverBoughtIsFree(ServerBought& serverBought) {
		// 服务器规格
		Server& server = ServerInfoDict[serverBought.serverType];
		if (server.cpu == (serverBought.cpusizel + serverBought.cpusizer) &&
			server.memory == (serverBought.memsizel + serverBought.memsizer))
			return true;
		return false;
	}


	/**
	 * 释放服务器资源
	 */
	void releaseResource(VmUsed& vmUsed, ServerBought& serverBought) {
		
		// 虚拟机规格
		Vm& vm = VmInfoDict[vmUsed.vmType];
		// 双节点
		if (vmUsed.node == 2) {
			serverBought.cpusizel += vm.cpu / 2;
			serverBought.cpusizer += vm.cpu / 2;
			serverBought.memsizel += vm.memory / 2;
			serverBought.memsizer += vm.memory / 2;
		}
		// 左节点
		else if (vmUsed.node == 0) {
			serverBought.cpusizel += vm.cpu;
			serverBought.memsizel += vm.memory;
		}
		// 右节点
		else {
			serverBought.cpusizer += vm.cpu;
			serverBought.memsizer += vm.memory;
		}
	}

	/**
	 * 为当日删除虚拟机做出优化
	 * 首先将今日删除的虚拟机整理到同一台服务器上
	 * 其次将剩余虚拟机尽量整合到一起
	 */
	void migrate4del(const int day) {
		
		// 计算剩余迁移次数--虚拟机总数的千分之五的下界
        int leftMigNum = (int)std::floor(VmUsed2ServerBought.size() * 0.005);
		
		// 将服务器按照剩余空间从小到大排序
		std::sort(ServerBoughtList.begin(), ServerBoughtList.end(), serverBoughtFreeCapLessCmp);

		/* 将左侧(剩余空间小的服务器上的del虚拟机)迁往(右侧剩余空间大的服务器) */
		/* 将右侧剩余空间大的服务器上不是今天删除的虚拟机迁往左侧 */

		// 循环迁移次数
		int cycleNum = 0;

		// 选出今天删除的虚拟机
		std::unordered_set<VmUsed*> delVmUsedSet;
		//std::vector<VmUsed*> delVmUsedList;

		// 遍历今日请求, 选出今日删除的虚拟机
		for (Request& r : requestLists[day]) {	
			if (r.operation == "del") {
				delVmUsedSet.insert(VmUsedDict[r.vmId]);
			}
		}

		// 左右指针
		// delFrom为del虚拟机迁出的服务器, delTo为del虚拟机迁入的服务器
        int delFrom = 0, delTo = ServerBoughtList.size() - 1;
        // commonFrom为普通虚拟机迁出的服务器, commonTo为普通虚拟机迁入的服务器
		int commonFrom = ServerBoughtList.size() - 1, commonTo = 0;

		// 外层循环
		while (cycleNum < 10 && leftMigNum > 0) {
			// 循环次数自增
		    ++cycleNum;
            // 内循环
            while(delFrom < delTo &&  commonFrom > commonTo && leftMigNum > 0){
                // del 迁出迁入服务器
                ServerBought& delFromServerBought = *(ServerBoughtList[delFrom]);
                ServerBought& delToServerBought = *(ServerBoughtList[delTo]);
                // common 迁出迁入服务器
                ServerBought& commonFromServerBought = *(ServerBoughtList[commonFrom]);
                ServerBought& commonToServerBought = *(ServerBoughtList[commonTo]);

                // 尝试将 delFrom 的del虚拟机迁移到 delTo
                int migNum = migrateDelServerBought(delFromServerBought, delToServerBought, delVmUsedSet, leftMigNum);
                leftMigNum -= migNum;
                if(leftMigNum==0)
                    break;
                // 尝试将 commonFrom 的common虚拟机迁移到 commonTo
                migNum = migrateCommonServerBought(commonFromServerBought, commonToServerBought, delVmUsedSet, leftMigNum);
                leftMigNum -= migNum;
                if(leftMigNum==0)
                    break;

                // 若当前 delFrom 中的del虚拟机迁空, 或不能迁移
                if (serverBoughtIsFree(leftServerBought) == true) {
                    ++left;
                    right = ServerBoughtList.size() - 1;
                }
                else {
                    --right;
                }
            }


		}
	}


	/**
	 * 尝试从已有服务器中分配服务器给虚拟机
	 */
	bool addMatch(Request& request) {

		// 获取请求的虚拟机的规格
		Vm curVm = VmInfoDict[request.vmType];

		// flag 表示能否从已经购买的服务器中分配
		bool flag = false;

		// 若虚拟机得到部署, 部署的服务器(ServerBought)指针
		ServerBought* selServerBoughtPtr = nullptr;

		// 若虚拟机能得到部署, 部署的节点
		int selNode;

		// 若当前虚拟机为双节点
		if (curVm.isBi == 1) {

			/* 对当前已购买的服务器排序, 此次排序是为了存放双节点虚拟机 */
			std::sort(ServerBoughtList.begin(), ServerBoughtList.end(), serverBoughtCapLessCmpBi);

			for (int i = 0; i < ServerBoughtList.size(); ++i) {
				ServerBought& topServerBought = *(ServerBoughtList[i]);
				// 判断当前服务器是否可以放下该双节点虚拟机, 若能放下则放下, 不能放下继续循环
				if (topServerBought.cpusizel >= curVm.cpu / 2 && topServerBought.cpusizer >= curVm.cpu / 2
					&& topServerBought.memsizel >= curVm.memory / 2 && topServerBought.memsizer >= curVm.memory / 2) {
					selServerBoughtPtr = &topServerBought;
					selNode = 2;
					flag = true;
					break;
				}
			}
		}

		// 若当前虚拟机为单节点
		else {
			/* 对当前已购买的服务器排序, 此次排序是为了存放双节点虚拟机, 排序方式需要改 */
			std::sort(ServerBoughtList.begin(), ServerBoughtList.end(), serverBoughtCapLessCmpSin);

			for (int i = 0; i < ServerBoughtList.size(); ++i) {
				ServerBought& topServerBought = *(ServerBoughtList[i]);
				/* 对于单节点虚拟机, 先选择两个节点负载低(cpu + ram)的节点存放, 若放不下, 放入另一个节点*/
				// 先尝试放入左节点
				if ((topServerBought.cpusizel + topServerBought.memsizel) > (topServerBought.cpusizer + topServerBought.memsizer)) {
					// 判断左节点是否能存放
					if ((topServerBought.cpusizel >= curVm.cpu && topServerBought.memsizel >= curVm.memory)) {
						selServerBoughtPtr = &topServerBought;
						selNode = 0;
						flag = true;
						break;
					}
					// 若左节点不能存放, 判断右节点能否存放
					else if ((topServerBought.cpusizer >= curVm.cpu && topServerBought.memsizer >= curVm.memory)) {

						selServerBoughtPtr = &topServerBought;
						selNode = 1;
						flag = true;
						break;
					}
				}

				// 先尝试放入右节点
				else {
					// 判断右节点是否能存放
					if ((topServerBought.cpusizer >= curVm.cpu && topServerBought.memsizer >= curVm.memory)) {

						selServerBoughtPtr = &topServerBought;
						selNode = 1;
						flag = true;
						break;
					}
					// 若右节点不能存放, 判断左节点能否存放
					else if ((topServerBought.cpusizel >= curVm.cpu && topServerBought.memsizel >= curVm.memory)) {

						selServerBoughtPtr = &topServerBought;
						selNode = 0;
						flag = true;
						break;
					}
				}
			}

		}

		// 若可以在已购买服务器列表中分配服务器
		if (flag == true) {
			// 已使用虚拟机
			/* 也许需要将节点的确认放入构造函数 */
			VmUsed* vmUsed = new VmUsed(request.vmId, request.vmType);
			vmUsed->node = selNode;
			VmUsedDict.insert({ request.vmId, vmUsed });

			// 部署虚拟机
			allocVm(*vmUsed, *selServerBoughtPtr, selNode);

			// 输出部署信息
			putInfo.push_back(PutInfo(selServerBoughtPtr, selNode, request.rank));

			// 指针置为空
			selServerBoughtPtr = nullptr;
			//资源重组
			//resort(topserverbought);
		}

		return flag;
	}


	/**
	 * 待购买的服务器列表
	 */
	std::vector<Server> selectServer(Request& resuest) {
		// 对待购买的服务器进行排序
		std::sort(ServerList.begin(), ServerList.end(), serverOriLessCmp);
		return ServerList;
	}

	/**
	 * 扩容策略
	 */
	void expansion(Request& request, std::vector<Server>& serverListSelected) {

		//if (request.vmId == "568479100")
		//	std::cout << "ok" << std::endl;

		// 获取请求的虚拟机规格
		Vm curVm = VmInfoDict[request.vmType];

		// 是否有符合要求的服务器
		bool flag = false;

		// 被选中的服务器类型
		std::string selServerType;

		// 虚拟机被部署在的服务器节点
		int selNode;

		/* 双节点 */
		if (curVm.isBi == 1) {
			for (Server& server : serverListSelected) {
				/* 判断服务器是否满足筛选条件, 同时能够容纳虚拟机 */
				if (server.cpu * 1.0 >= curVm.cpu * 1.3 && server.memory * 1.0 >= curVm.memory * 1.3 &&
					server.cpu >= curVm.cpu && server.memory >= curVm.memory) {
					selServerType = server.serverType;
					// 双节点
					selNode = 2;
					flag = true;
					break;
				}
			}
			/* 若没有服务器满足筛选条件, 则只找出能够存放虚拟机的服务器 */
			if (flag == false) {
				for (Server& server : serverListSelected) {
					if (server.cpu >= curVm.cpu && server.memory >= curVm.memory) {
						selServerType = server.serverType;
						// 双节点
						selNode = 2;
						flag = true;
						break;
					}
				}
			}
		}
		/* 单节点 */
		else {
			for (Server& server : serverListSelected) {
				/* 判断服务器是否满足筛选条件, 同时能够容纳虚拟机 */
				if (server.cpu / 2.0 >= curVm.cpu * 1.3 && server.memory / 2.0 >= curVm.memory * 1.3 &&
					server.cpu / 2 >= curVm.cpu && server.memory / 2 >= curVm.memory) {
					selServerType = server.serverType;
					// 默认放到左节点
					selNode = 0;
					flag = true;
					break;
				}
			}
			/* 若没有服务器满足筛选条件, 则只找出能够存放虚拟机的服务器 */
			if (flag == false) {
				for (Server& server : serverListSelected) {
					if (server.cpu / 2 >= curVm.cpu && server.memory / 2 >= curVm.memory) {
						selServerType = server.serverType;
						// 默认放到左节点
						selNode = 0;
						flag = true;
						break;
					}
				}
			}
		}

		/* 若flag仍为false, 说明服务器列表中没有能满足虚拟机要求的服务器, 通常不会发生 */
		assert(flag == true);

		// 新建已购买服务器, 此时还无法确定serverId
		ServerBought* serverBoughtPtr = new ServerBought(selServerType, ServerInfoDict[selServerType]);
		// 将该购买服务器加入已购买服务器表
		//ServerBoughtDict.insert({ serverId, serverBoughtPtr });
		ServerBoughtList.push_back(serverBoughtPtr);
		// 已使用虚拟机
		/* 也许需要将节点的确认放入构造函数 */
		//VmUsed vmUsed(request.vmId, request.vmType);
		VmUsed* vmUsed = new VmUsed(request.vmId, request.vmType);
		vmUsed->node = selNode;
		VmUsedDict.insert({ request.vmId, vmUsed });
		// 部署虚拟机
		allocVm(*vmUsed, *serverBoughtPtr, selNode);
		// 添加购买信息
		addInfo[selServerType].push_back(serverBoughtPtr);
		
		// 记录硬件成本
		serverFee += ServerInfoDict[selServerType].oricost;

		//输出部署信息
		putInfo.push_back(PutInfo(serverBoughtPtr, selNode, request.rank));
		// serverBoughtPtr 置为空
		serverBoughtPtr = nullptr;
		vmUsed = nullptr;

	}

	/**
	 * 将虚拟机部署在服务器上
	 */
	void allocVm(VmUsed& vmUsed, ServerBought& serverBought, int node) {
		// 添加虚拟机到服务器的映射
		VmUsed2ServerBought.insert({ &vmUsed ,&serverBought });
		// 添加服务器到虚拟机列表的映射
		ServerBought2VmUsed[&serverBought].push_back(&vmUsed);
		// 消耗服务器资源
		consumeResource(serverBought, vmUsed, node);
	}

	/**
	 * 消耗服务器资源
	 * serverBought表示被消耗的服务器实例，vm表示要部署的虚拟机，node表示虚拟机被部署的节点
	 */
	void consumeResource(ServerBought& serverBought, VmUsed& vmUsed, int node) {
		// 虚拟机规格
		Vm vm = VmInfoDict[vmUsed.vmType];
		// 部署在的节点
		if (node == 2) {
			serverBought.cpusizel -= vm.cpu / 2;
			serverBought.cpusizer -= vm.cpu / 2;
			serverBought.memsizel -= vm.memory / 2;
			serverBought.memsizer -= vm.memory / 2;
		}
		else if (node == 1) {
			serverBought.cpusizer -= vm.cpu;
			serverBought.memsizer -= vm.memory;
		}
		else {
			serverBought.cpusizel -= vm.cpu;
			serverBought.memsizel -= vm.memory;
		}
	}


	/**
	 * 从服务器上删除虚拟机
	 */
	void delVm(std::string& vmId) {

		// 获取已购买的虚拟机对象
		VmUsed& vmUsed = *(VmUsedDict[vmId]);

		// 虚拟机所在的服务器, 服务器节点
		ServerBought& serverBought = *(VmUsed2ServerBought[&vmUsed]);
		int node = vmUsed.node;

		// 从已部署虚拟机表中删除虚拟机
		VmUsedDict.erase(vmId);

		// 从虚拟机与服务器映射中删除映射
		VmUsed2ServerBought.erase(&vmUsed);

		// 从服务器与虚拟机列表映射中删除虚拟机
		std::vector<VmUsed*>::iterator it = std::find(ServerBought2VmUsed[&serverBought].begin(),
			ServerBought2VmUsed[&serverBought].end(),
			&vmUsed);
		ServerBought2VmUsed[&serverBought].erase(it);

		// 修改所在服务器的cpu与ram
		if (node == 2) {
			serverBought.cpusizel += VmInfoDict[vmUsed.vmType].cpu / 2;
			serverBought.cpusizer += VmInfoDict[vmUsed.vmType].cpu / 2;
			serverBought.memsizel += VmInfoDict[vmUsed.vmType].memory / 2;
			serverBought.memsizer += VmInfoDict[vmUsed.vmType].memory / 2;
		}
		// 左节点
		else if (node == 0) {
			serverBought.cpusizel += VmInfoDict[vmUsed.vmType].cpu;
			serverBought.memsizel += VmInfoDict[vmUsed.vmType].memory;
		}
		// 右节点
		else {
			serverBought.cpusizer += VmInfoDict[vmUsed.vmType].cpu;
			serverBought.memsizer += VmInfoDict[vmUsed.vmType].memory;
		}

		// 删除后重组 暂时还不更改
		//reSort(serverBought);
	}


	/**
	 * 将每日的操作信息写入日志中
	 */
	void write2Log(int lastDayServerNum) {

		/* 购买日志 */
		// 输出购买的服务器个数
		std::stringstream ss;
		ss << "(purchase, " << addInfo.size() << ")";
		logInfo.push_back(ss.str());
		ss.str("");
		ss.clear();

		// 输出每种服务器购买数量
		std::unordered_map<std::string, std::vector<ServerBought*>>::iterator it = addInfo.begin();
		for (; it != addInfo.end(); ++it) {
			// 当前种类的已购买服务器列表
			std::vector<ServerBought*>& curServerTypeBought = it->second;
			ss << "(" << it->first << ", " << curServerTypeBought.size() << ")";
			// 给已购买服务器编号
			for (ServerBought* serverBought : curServerTypeBought) {
				serverBought->serverId = lastDayServerNum++;
			}
			logInfo.push_back(ss.str());
			ss.str("");
			ss.clear();
		}
		/* 每天的迁移日志 */
		ss << "(migration, " << migrateInfo.size() << ")";
		logInfo.push_back(ss.str());
		ss.str("");
		ss.clear();
		for (MigrationInfo migrationInfo : migrateInfo) {
			ss << "(" + migrationInfo.vmId << ", " << migrationInfo.toServerId;
			if (migrationInfo.toNode == 2)
				ss << ")";
			else if (migrationInfo.toNode == 0)
				ss << ", " << "A)";
			else
				ss << ", " << "B)";
			logInfo.push_back(ss.str());
			ss.str("");
			ss.clear();
		}

		// 对部署请求按照序号从小到大排序
		std::sort(putInfo.begin(), putInfo.end(), putInfoRankLessCmp);
		// 每天的部署日志
		for (PutInfo& p : putInfo) {
			if (p.putNode == 2) {
				ss << "(" << (p.serverBought)->serverId << ")";
				logInfo.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else if (p.putNode == 1) {
				ss << "(" << (p.serverBought)->serverId << ", B)";
				logInfo.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
			else {
				ss << "(" << (p.serverBought)->serverId << ", A)";
				logInfo.push_back(ss.str());
				ss.str("");
				ss.clear();
			}
		}
		//清空每日信息
		addInfo.clear();
		migrateInfo.clear();
		putInfo.clear();
	}


	/**
	 * 计算能源消耗
	 */
	void calculatePower() {

		std::vector<ServerBought*>::iterator it = ServerBoughtList.begin();
		for (; it != ServerBoughtList.end(); ++it) {
			// 已购买的服务器
			ServerBought& serverBought = **it;
			// 服务器型号
			std::string serverType = serverBought.serverType;
			if (serverBought.cpusizel != ServerInfoDict[serverType].cpu / 2 || serverBought.cpusizer != ServerInfoDict[serverType].cpu / 2
				|| serverBought.memsizel != ServerInfoDict[serverType].memory / 2 || serverBought.memsizer != ServerInfoDict[serverType].memory / 2) {
				powerFee += ServerInfoDict[serverType].dailycost;
			}
		}
	}


	/**
	 * 检查服务器是否溢出
	 */
	void checkOverflow() {
		// 遍历 ServerBought2VmUsed
		std::unordered_map<ServerBought*, std::vector<VmUsed*>>::iterator it = ServerBought2VmUsed.begin();
		for (; it != ServerBought2VmUsed.end(); ++it) {
			// 当前已购买服务器
			ServerBought* serverBought = it->first;
			// 当前服务器规格
			Server server = ServerInfoDict[serverBought->serverType];
			// 临时ServerBought, 用作资源判断
			ServerBought tmp(-1, server.serverType, server);
			// 服务器上的虚拟机列表
			std::vector<VmUsed*>& v = it->second;
			std::vector<VmUsed*>::iterator iit = v.begin();
			for (; iit != v.end(); ++iit) {
				// 当前虚拟机
				VmUsed* vmUsed = *iit;
				// 当前虚拟机规格
				Vm vm = VmInfoDict[vmUsed->vmType];
				// 减去当前虚拟机的资源
				if (vmUsed->node == 2) {
					tmp.cpusizel -= vm.cpu / 2;
					tmp.cpusizer -= vm.cpu / 2;
					tmp.memsizel -= vm.memory / 2;
					tmp.memsizer -= vm.memory / 2;
				}
				else if (vmUsed->node == 0) {
					tmp.cpusizel -= vm.cpu;
					tmp.memsizel -= vm.memory;
				}
				else {
					tmp.cpusizer -= vm.cpu;
					tmp.memsizer -= vm.memory;
				}
			}
			// 判断是否出现负数
			if (tmp.cpusizel < 0 || tmp.cpusizer < 0 || tmp.memsizel < 0 || tmp.memsizer < 0) {
				std::cout << "资源溢出" << std::endl;
			}
			// 判断临时ServerBought的状态与真实ServerBought的状态是否一致
			if (tmp.cpusizel != serverBought->cpusizel || tmp.cpusizer != serverBought->cpusizer
				|| tmp.memsizel != serverBought->memsizel || tmp.memsizer != serverBought->memsizer) {
				std::cout << "预计状态与真实状态不一致" << std::endl;
			}
		}
	}

	/**	
	 * 检验映射关系是否一致
	 */
	void checkRelation() {
		/* 两个映射关系 VmUsed2ServerBought ServerBought2VmUsed */
		// 遍历 ServerBought2VmUsed
		std::unordered_map<ServerBought*, std::vector<VmUsed*>>::iterator it = ServerBought2VmUsed.begin();
		for (; it != ServerBought2VmUsed.end(); ++it) {
			std::vector<VmUsed*>& v = it->second;
			std::vector<VmUsed*>::iterator iit = v.begin();
			for (; iit != v.end(); ++iit) {
				if (VmUsed2ServerBought[*iit] != it->first) {
					std::cout << "映射关系不一致" << std::endl;
				}
			}
		}
	}


}