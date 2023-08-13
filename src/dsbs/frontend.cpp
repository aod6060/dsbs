#include "sys.hpp"


namespace fe {
	dsbs::Context context;

    	bool isVersion = false;
    	bool isHelp = false;
    	bool useMT = false;

    int run(int argc, char** argv) {

        // Load from argv
        std::vector<std::string> arg;

        for(int i = 1; i < argc; i++) {
            arg.push_back(argv[i]);
        }

        dsbs::initConfig();

        std::for_each(arg.begin(), arg.end(), [&](std::string line) {
            std::vector<std::string> tags;
            dsbs::util::strSplit(line, '=', [&](std::string tag) {
                tags.push_back(tag);
            });

            if(tags[0] == "-op") {
                if(tags[1] == "build") {
                    	context.op = dsbs::Operation::OP_BUILD;
                } else if(tags[1] == "clean") {
                    	context.op = dsbs::Operation::OP_CLEAN;
                } else if(tags[1] == "rebuild") {
                	context.op = dsbs::Operation::OP_REBUILD;
                } else if(tags[1] == "init") {
                	context.op = dsbs::Operation::OP_INIT;
                } else if(tags[1] == "release") {
                	context.op = dsbs::Operation::OP_RELEASE;
                } else if(tags[1] == "run") {
                	context.op = dsbs::Operation::OP_RUN;
                }
            
            } else if(tags[0] == "-s") {
                context.solutionFile = tags[1];
            } else if(tags[0] == "-p") {
                context.projectName = tags[1];
            } else if(tags[0] == "-v") {
                isVersion = true;
            } else if(tags[0] == "-h") {
                isHelp = true;
            } else if(tags[0] == "-useMT") {
		    context.useMT = true;
	    } else if(tags[0] == "-numThreads") {
	    	context.numThreads = std::stoi(tags[1]);
	    }
        });

        if(isVersion || isHelp) {

            if(isVersion) {
                std::cout << "dsbs version: " << dsbs::getVersion() << "\n";
                std::cout << "solution file version: " << dsbs::getSolutionFileVersion() << "\n";
                std::cout << "profile file version: " << dsbs::getProfileFileVersion() << "\n";
            }

            if(isHelp) {
                //std::cout << "This will load up a help text file from ./dsbs_conf/help.txt" << "\n";
                dsbs::util::loadFile("./dsbs-config/config/help.txt", [&](std::string line) {
                    std::cout << line << "\n";
                });
            }

            return 0;
        }
        
        //dsbs::Context context;
        
        /*
        context.op = op;
        context.solutionFile = solutionFile;
        context.projectName = projectName;
        context.useMT = useMT;
        */
             
        return dsbs::run(context);
    }
}
