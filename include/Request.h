#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <string>
#include <vector>

namespace CodeCraft2021 {

	class Request {

	public:

        std::string operation;
        std::string vmType;
        std::string vmId;
        int rank;

        Request(std::vector<std::string> strings, int rank);

	};

}


#endif // !_REQUEST_H_

