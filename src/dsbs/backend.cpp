#include "sys.hpp"

namespace be {


    /*
        Creating Backend... This is were the magic will begin...
    */

    int loadSolutionProfile(core::Operation op, std::string solutionFile, std::string projectName);

    int run(core::Operation op, std::string solutionFile, std::string projectName) {
        //std::cout << ((op == core::OP_BUILD) ? "build" : "cleaning") << " " << solutionFile << " " << projectName << "\n";
        return loadSolutionProfile(op, solutionFile, projectName);
    }


    int loadSolutionProfile(core::Operation op, std::string solutionFile, std::string projectName) {

        // Load Solution
        core::solution::Solution solution;

        util::loadStream(solutionFile, [&](std::ifstream& in) {
            Json::Value root;
            in >> root;

            solution.type = root["type"].asString();
            solution.version = root["version"].asInt();

            if(solution.type != "solution") {
                std::cout << "Error: type isn't solution will try to load" << "\n";
            }

            if(solution.version != SOLUTION_FILE_VERSION) {
                std::cout << "Error: wrong version will try to load" << "\n";
            }

            solution.name = root["name"].asString();



            util::jsonIterator(root["projects"], [&](Json::Value v) {

                core::solution::Project project;

                project.name = v["name"].asString();
                project.type = util::toType(v["type"].asString());
                project.profile = v["profile"].asString();
                
                // Directories
                project.binDir = v["bin-dir"].asString();

                util::jsonIterator(v["sources"], [&](Json::Value v2) {
                    core::solution::Source source;
                    source.srcDir = v2["src-dir"].asString();
                    source.objDir = v2["obj-dir"].asString();
                    project.sources.push_back(source);
                });

                util::jsonIteratorString(v["include-dirs"], [&](std::string str) {
                    project.includeDirs.push_back(str);
                });

                util::jsonIteratorString(v["lib-dirs"], [&](std::string str) {
                    project.libDirs.push_back(str);
                });

                util::jsonIteratorString(v["libs"], [&](std::string str) {
                    project.libs.push_back(str);
                });
                // Options
                util::jsonIteratorString(v["compiler-options"], [&](std::string str) {
                    project.compilerOptions.push_back(str);
                });

                util::jsonIteratorString(v["archiver-options"], [&](std::string str) {
                    project.archiverOptions.push_back(str)
                });

                util::jsonIteratorString(v["linker-options"], [&](std::string str) {
                    project.linkerOptions.push_back(str);
                });

                solution.projects.push_back(project);
            });

        });

        //// Load Profile
        //core::profile::Profile profile;
        

        return 0;
    }
}
