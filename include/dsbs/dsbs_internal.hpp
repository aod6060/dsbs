#ifndef DSBS_INTERNAL_HPP
#define DSBS_INTERNAL_HPP



#include "dsbs.hpp"


namespace dsbs {
    namespace internal {
        //int loadSolutionProfile(Operation op, std::string solutionFile, std::string projectName, bool useMT);
        int loadSolutionProfile(Context& context);
        
        //int operationsCheck(Operation op, std::string projectName, solution::Solution& solution, bool useMT);
        int operationCheck(Context& context, solution::Solution& solution);
        
        int executeCommandList(Context& context, std::vector<command::Project>& projectCommandList);
        
	namespace init {
		int initOperation(Context& context, solution::Solution& solution);
	}

	namespace build {
		int buildOperation(Context& context, solution::Solution& solution);
	}

	namespace rebuild {
		int rebuildOperation(Context& context, solution::Solution& solution);	
	}

	namespace clean {
		int cleanOperation(Context& context, solution::Solution& solution);
	}

	namespace release {
		int releaseOperation(Context& context, solution::Solution& solution);
	}
	
	namespace run {
		int runOperation(Context& context, solution::Solution& solution);
	}
    }
}

namespace util {
    void loadStream(std::string path, std::function<void(std::ifstream &)> cb);
    void saveStream(std::string path, std::function<void(std::ofstream &)> cb);

    void jsonIterator(Json::Value value, std::function<void(Json::Value)> cb);
    void jsonIteratorString(Json::Value value, std::function<void(std::string str)> cb);

    dsbs::solution::ProjectType toType(std::string type);

    void iterateDirectory(std::filesystem::path path, std::function<void(std::filesystem::directory_entry)> cb);

    std::string exec(std::string cmd);
}

#endif
