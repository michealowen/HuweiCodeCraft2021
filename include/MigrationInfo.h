#ifndef _MIGRATION_H_
#define _MIGRATION_H_

#include <string>

#include "ServerBought.h"

namespace CodeCraft2021 {
    class MigrationInfo {

    public:
           
        // 被迁移的虚拟机编号
        std::string vmId;
        // 被迁往的服务器Id
        int toServerId;
        // 被迁往的服务器节点
        int toNode;

        MigrationInfo(std::string& vmId, int toServerId, int toNode);

    };
}

#endif // !_MIGRATION_H_

