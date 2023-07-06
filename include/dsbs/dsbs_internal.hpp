#ifndef DSBS_INTERNAL_HPP
#define DSBS_INTERNAL_HPP



#include "dsbs.hpp"

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