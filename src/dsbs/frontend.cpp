#include "sys.hpp"


namespace fe {
    core::Operation op = core::Operation::OP_BUILD;
    std::string projectName = "";
    std::string solutionFile = "solution.json";

    bool isVersion = false;
    bool isHelp = false;

    int run(int argc, char** argv) {

        // Load from argv
        std::vector<std::string> arg;

        for(int i = 1; i < argc; i++) {
            arg.push_back(argv[i]);
        }

        std::for_each(arg.begin(), arg.end(), [&](std::string line) {
            std::vector<std::string> tags;
            util::strSplit(line, '=', [&](std::string tag) {
                tags.push_back(tag);
            });

            if(tags[0] == "-op") {
                if(tags[1] == "build") {
                    op = core::Operation::OP_BUILD;
                } else if(tags[1] == "clean") {
                    op = core::Operation::OP_CLEAN;
                }
            } else if(tags[0] == "-s") {
                solutionFile = tags[1];
            } else if(tags[0] == "-p") {
                projectName = tags[1];
            } else if(tags[0] == "-v") {
                isVersion = true;
            } else if(tags[0] == "-h") {
                isHelp = true;
            }
        });

        if(isVersion || isHelp) {

            if(isVersion) {
                std::cout << "dsbs version: " << DSBS_VERSION << "\n";
                std::cout << "solution file version: " << SOLUTION_FILE_VERSION << "\n";
                std::cout << "profile file version: " << PROFILE_FILE_VERSION << "\n";
            }

            if(isHelp) {
                //std::cout << "This will load up a help text file from ./dsbs_conf/help.txt" << "\n";
                util::loadFile("./dsbs-config/config/help.txt", [&](std::string line) {
                    std::cout << line << "\n";
                });
            }

            return 0;
        }


        return be::run(op, solutionFile, projectName);
    }
}