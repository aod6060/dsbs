#ifndef DSBS_HPP
#define DSBS_HPP

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

namespace dsbs {
    enum Operation
    {
        OP_BUILD = 0,
        OP_CLEAN,
        OP_REBUILD,
        OP_INIT,
        OP_RELEASE,
        OP_RUN
    };

    namespace command {
        struct Command {
            std::string command;
        };
        
        struct Project {
            std::string projectName;
            std::vector<Command> commands;
	    Command projectCommand;
        };
    }

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
            std::string sharedImportLibFlag;
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
            PT_SHARED_DLL_LIB,
            PT_UNKNOWN
        };

        struct Source
        {
            std::string srcDir;
            std::string objDir;
	    std::string cacheDir;
        };

        struct Project
        {
            // Project Information
            std::string name;
            ProjectType type;
            //std::string profile;
            profile::Profile profile;
            
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

    namespace util {
        void strSplit(std::string str, char delim, std::function<void(std::string)> cb);
        void loadFile(std::string path, std::function<void(std::string)> cb);
    }
    
    struct Context {
    	Operation op = Operation::OP_BUILD;
    	std::string solutionFile = "solution.json";
    	std::string projectName = "";
    	bool useMT = false;
    	int numThreads = 8;
    };
    
    /*
    	These are the function you'll need to be concerened about if you want to intergrate
    	dsbs into your application.
    */
    void initConfig();

    std::string getVersion();
    int getSolutionFileVersion();
    int getProfileFileVersion();

    //int run(Operation op, std::string solutionFile, std::string projectName, bool useMT = false);
    int run(Context& context);
}


#endif
