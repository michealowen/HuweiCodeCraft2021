#include <iostream>
#include <vector>
// 字符串与数值互转
#include <sstream>
#include <string>

#include "../include/VmUsed.h"

namespace CodeCraft2021 {


    VmUsed::VmUsed(std::string& vmId, std::string& vmType):
        vmId(vmId),
        vmType(vmType)
    {}

    // 无参的构造函数
    VmUsed::VmUsed() {}

    // VmUsed 作为key, 需要重载==
    bool VmUsed::operator==(const VmUsed& vm) const{
        return vmId == vm.vmId;
    }

}