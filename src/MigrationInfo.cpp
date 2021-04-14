#include "../include/MigrationInfo.h"

namespace CodeCraft2021 {

    MigrationInfo::MigrationInfo(std::string& vmId, int toServerId, int toNode):
        vmId(vmId),
        toServerId(toServerId),
        toNode(toNode)
    {}


}