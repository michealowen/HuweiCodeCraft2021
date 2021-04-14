#ifndef _PUTINFO_H_
#define _PUTINFO_H_

#include "ServerBought.h"

namespace CodeCraft2021 {
	class PutInfo {

    public:

        // 被部署的已购买服务器
        ServerBought* serverBought;
        // 被部署在的节点
        int putNode;
        // 请求的序号
        int rank;

        PutInfo(ServerBought* serverBought, int node, int rank);

	};
}

#endif // !_PUTINFO_H_

