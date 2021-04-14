#include "../include/Request.h"

namespace CodeCraft2021 {

    Request::Request(std::vector<std::string> strings, int rank) {
        
        // add 请求
        if (strings[0][0] == 'a') {
            this->operation = strings[0];
            this->vmType = strings[1];
            this->vmId = strings[2];
            this->rank = rank;
        }
        // del请求
        else {
            this->operation = strings[0];
            this->vmId = strings[1];
            this->rank = rank;
        }
    }

}