#ifndef _SERVER_H_
#define _SERVER_H_

#include <string>

namespace CodeCraft2021 {


    class Server
    {

    public:
        // 型号,唯一标识，长度不超过 20，仅由数字和大小写英文字符构成
        std::string serverType;
        // CPU核数
        long cpu;
        // 内存大小
        long memory;
        // 硬件成本
        long oricost;
        // 每日能耗成本
        long dailycost;

        // 构造函数, 使用参数构造
        Server(std::string& serverType, long cpu, long memory, long oricost, long dailycost);

        // 构造函数, 使用字符串构造
        Server(std::vector<std::string>& serverString);

        // 无参构造函数, 用于 unordered_map的查找
        Server();

        // 析构函数
        ~Server();

        void toString();
    };
}

#endif