#ifndef SYS_HPP
#define SYS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <filesystem>

#include <cstdio>

#include <json/json.h>

#define DSBS_VERSION "1.0.1"
#define SOLUTION_FILE_VERSION 2
#define PROFILE_FILE_VERSION 1

namespace core {
    enum Operation
    {
        OP_BUILD = 0,
        OP_CLEAN
    };

    struct command {
        std::string projectName;
        std::string command;
    };

    namespace profile
    {

        struct Programs
        {
            std::string compiler;
            std::string archiver;
            std::string linker;
        };

        struct FileExtensions
        {
            std::vector<std::string> sourceFileExtensions;
            std::string objectFileExtension;
            std::string binaryFileExtension;
            std::string staticLibExtension;
            std::string sharedLibExtension;
        };

        struct Flags
        {
            std::string includeDirFlag;
            std::string libDirFlag;
            std::string compileFlag;
            std::string objectFlag;
            std::string libFlags;
            std::string staticLibFlags;
            std::string sharedLibFlags;
            std::string sharedLibFlagsLinker;
        };

        struct Profile
        {
            std::string type;
            int version;
            Programs programs;
            FileExtensions fileExtensions;
            Flags flags;
        };
    }

    namespace solution
    {

        enum ProjectType
        {
            PT_EXECUTABLE = 0,
            PT_STATIC_LIB,
            PT_SHARED_LIB,
            PT_UNKNOWN
        };

        struct Source
        {
            std::string srcDir;
            std::string objDir;
        };

        struct Project
        {
            // Project Information
            std::string name;
            ProjectType type;
            //std::string profile;
            core::profile::Profile profile;
            
            // Directories
            std::string binDir;
            std::vector<Source> sources;
            std::vector<std::string> includeDirs;
            std::vector<std::string> libDirs;
            std::vector<std::string> libs;
            // Options
            std::vector<std::string> compilerOptions;
            std::vector<std::string> archiverOptions;
            std::vector<std::string> linkerOptions;
        };

        struct Solution
        {
            std::string type;
            int version;
            std::string name;
            std::vector<Project> projects;
        };
    }
}

namespace util {
    void strSplit(std::string str, char delim, std::function<void(std::string)> cb);
    void loadFile(std::string path, std::function<void(std::string)> cb);

    void loadStream(std::string path, std::function<void(std::ifstream &)> cb);
    void saveStream(std::string path, std::function<void(std::ofstream &)> cb);

    void jsonIterator(Json::Value value, std::function<void(Json::Value)> cb);
    void jsonIteratorString(Json::Value value, std::function<void(std::string str)> cb);

    core::solution::ProjectType toType(std::string type);

    void iterateDirectory(std::filesystem::path path, std::function<void(std::filesystem::directory_entry)> cb);

    std::string exec(std::string cmd);
}

namespace fe {
    int run(int argc, char **argv);
}

namespace be {
    int run(core::Operation op, std::string solutionFile, std::string projectName);
}

#endif