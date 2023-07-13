#ifndef DSBS_INTERNAL_HPP
#define DSBS_INTERNAL_HPP



#include "dsbs.hpp"


namespace dsbs {
    namespace internal {
        int loadSolutionProfile(Operation op, std::string solutionFile, std::string projectName);
        int operationsCheck(Operation op, std::string projectName, solution::Solution& solution);
        int buildOperation(std::string projectName, solution::Solution& solution);
        int executeCommandList(std::vector<command::Project>& projectCommandList);
        int cleanOperation(std::string projectName, solution::Solution& solution);
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