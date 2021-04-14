#ifndef _SERVER_BOUGHT_
#define _SERVER_BOUGHT_


#include <string>
#include <vector>

#include "VmUsed.h"
#include "Server.h"

namespace CodeCraft2021 {

    class ServerBought
    {
    public:

        //唯一标识,服务器编号
        int serverId;
        // 服务器类型
        std::string serverType;
        
        //左节点CPU核数,大小不超过 1024
        long cpusizel;
        //左节点内存大小,大小不超过 1024
        long memsizel;
        //右节点CPU核数,大小不超过 1024
        long cpusizer;
        //右节点内存大小,大小不超过 1024
        long memsizer;
        //是否可以迁移,true表示可以,false表示不可以
        bool canMig;

        // 构造函数
        ServerBought(int serverId, std::string& serverType, Server& server);

        // 构造函数, 延迟设置serverId
        ServerBought(std::string& serverType, Server& server);

        // 无参的构造函数
        ServerBought();

        // ServerBought 作为unordered_amp的key, 需要重载==
        bool operator==(const ServerBought& serverBought) const;

        // ServerBought 作为map的key, 需要重载<
        //bool operator<(const ServerBought& serverBought) const;

    };

    struct ServerBoughtHash {
        size_t operator()(const ServerBought& serverBought) const {
            return std::hash<int>()(serverBought.serverId);
        }
    };

}



#endif