#include <iostream>
#include <vector>
// 字符串与数值互转
#include <sstream>

#include "../include/ServerBought.h"


namespace CodeCraft2021 {


    ServerBought::ServerBought(int serverId, std::string& serverType, Server& server):
        serverId(serverId),
        serverType(serverType),
        // 构造时将是否可以迁移设置为true
        canMig(true),
        cpusizel(server.cpu / 2),
        memsizel(server.memory / 2),
        cpusizer(server.cpu / 2),
        memsizer(server.memory / 2)
    {}

    // 构造函数, 延迟设置serverId
    ServerBought::ServerBought(std::string& serverType, Server& server):
        serverType(serverType),
        // 构造时将是否可以迁移设置为true
        canMig(true),
        cpusizel(server.cpu / 2),
        memsizel(server.memory / 2),
        cpusizer(server.cpu / 2),
        memsizer(server.memory / 2)
    {}

    // 无参的构造函数
    ServerBought::ServerBought() {}

    // ServerBought 作为key, 需要重载==
    bool ServerBought::operator==(const ServerBought& serverBought) const {
        return serverId == serverBought.serverId;
    }

}