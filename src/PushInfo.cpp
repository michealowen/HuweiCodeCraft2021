#include "../include/PutInfo.h"

namespace CodeCraft2021 {

    PutInfo::PutInfo(ServerBought* serverBought, int node, int rank) :
            serverBought(serverBought),
            putNode(node),
            rank(rank)
    {}

}