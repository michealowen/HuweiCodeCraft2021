#ifndef _VM_USED_
#define _VM_USED_

#include <string>

#include "Vm.h"
#include "ServerBought.h"

namespace CodeCraft2021 {

    class VmUsed
    {

    public:

        //虚拟机编号
        std::string vmId;
        //型号,虚拟机型号长度不超过 20，仅由数字，大小写英文字符和'.'构成
        std::string vmType;
        //在哪个节点上部署,0表示左节点，1表示右节点，2表示双节点
        int node;

        // 构造函数, 使用服务器编号与虚拟机构造
        VmUsed(std::string& vmId, std::string& vmType);

        // 无参的构造函数
        VmUsed();

        // VmUsed 作为key, 需要重载==
        bool operator==(const VmUsed& vm) const;

    };

    /*
    struct VmUsedHash {
        size_t operator()(const VmUsed* vm) const {
            return std::hash<std::string>()(vm->vmId);
        }
    };
    */
}


#endif // !_VM_USED_
