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

    std::function<void(std::ifstream&)> loadProfile(core::profile::Profile& profile) {
        return [&](std::ifstream& in) {
            Json::Value root;
            in >> root;

            profile.type = root["type"].asString();
            profile.version = root["version"].asInt();

            if(profile.type != "profile") {
                std::cout << "Error: type isn't profile will try to load" << "\n";
            }

            if(profile.version != PROFILE_FILE_VERSION) {
                std::cout << "Error: version isn't correct will try to load" << "\n";
            }

            // Program
            profile.programs.compiler = root["programs"]["compiler"].asString();
            profile.programs.archiver = root["programs"]["archiver"].asString();
            profile.programs.linker = root["programs"]["linker"].asString();


            // File Extensions
            // Source File Extensions
            util::jsonIteratorString(root["file-extensions"]["source-file-extensions"], [&](std::string ext) {
                profile.fileExtensions.sourceFileExtensions.push_back(ext);
            });
            profile.fileExtensions.objectFileExtension = root["file-extensions"]["object-file-extension"].asString();
            profile.fileExtensions.binaryFileExtension = root["file-extensions"]["binary-file-extension"].asString();
            profile.fileExtensions.staticLibExtension = root["file-extensions"]["static-lib-extension"].asString();
            profile.fileExtensions.sharedLibExtension = root["file-extensions"]["shared-lib-extension"].asString();

            // Flags
            profile.flags.includeDirFlag = root["flags"]["include-dir-flag"].asString();
            profile.flags.libDirFlag = root["flags"]["lib-dir-flag"].asString();
            profile.flags.compileFlag = root["flags"]["compile-flag"].asString();
            profile.flags.objectFlag = root["flags"]["object-flag"].asString();
            profile.flags.staticLibFlags = root["flags"]["static-lib-flag"].asString();
            profile.flags.sharedLibFlags = root["flags"]["shared-lib-flag"].asString();

        };
    }

    std::function<void(std::ifstream&)> loadSolution(core::solution::Solution& solution) {
        return [&](std::ifstream& in) {
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
                //project.profile = v["profile"].asString();
                // Handle Profile
                util::loadStream(v["profile"].asString(), loadProfile(project.profile));


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
                    project.archiverOptions.push_back(str);
                });

                util::jsonIteratorString(v["linker-options"], [&](std::string str) {
                    project.linkerOptions.push_back(str);
                });

                solution.projects.push_back(project);
            });

        };
    }

    int buildSourceObjectList(core::Operation op, std::string projectName, core::solution::Solution& solution);

    int loadSolutionProfile(core::Operation op, std::string solutionFile, std::string projectName) {

        // Load Solution
        core::solution::Solution solution;

        util::loadStream(solutionFile, loadSolution(solution));

        return buildSourceObjectList(op, projectName, solution);
    }

    int createCommandList(core::Operation op, core::solution::Solution& solution, std::vector<core::solution::Source>& sourceList, std::vector<std::string>& objectList);

    int buildSourceObjectList(core::Operation op, std::string projectName, core::solution::Solution& solution) {
        std::vector<core::solution::Source> sourceList;
        std::vector<std::string> objectList;

        if(!projectName.empty()) {
            core::solution::Project* project = nullptr;

            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& p) {
                if(p.name == projectName) {
                    project = &p;
                }
            });

            if(project != nullptr) {
                std::for_each(project->sources.begin(), project->sources.end(), [&](core::solution::Source& s) {
                    util::iterateDirectory(std::filesystem::path(s.srcDir), 
                    [&](std::filesystem::directory_entry e) 
                    {
                        std::for_each(project->profile.fileExtensions.sourceFileExtensions.begin(), project->profile.fileExtensions.sourceFileExtensions.end(), [&](std::string ext) {
                            if(e.path().extension() == ext) {
                                //std::cout << e.path().string() << "\n";
                                core::solution::Source source;
                                source.srcDir = e.path().string();
                                std::vector<std::string> args;
                                std::string fileName = e.path().filename().string();
                                util::strSplit(fileName, '.', [&](std::string line) {
                                    args.push_back(line);
                                });
                                std::stringstream ss;
                                ss << s.objDir << args[0] << project->profile.fileExtensions.objectFileExtension;
                                source.objDir = ss.str();

                                sourceList.push_back(source);
                                objectList.push_back(source.objDir);

                                args.clear();
                            }
                        });
                    });
                });
            } else {
                std::cout << "Error: This project " << projectName << " doesn't exist!" << "\n";
                return -1;
            }
        } else {
            // This means that I'm focusing on the entire solution
            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& project) {
                std::for_each(project.sources.begin(), project.sources.end(), [&](core::solution::Source& s) {
                    util::iterateDirectory(std::filesystem::path(s.srcDir), 
                    [&](std::filesystem::directory_entry e) 
                    {
                        std::for_each(project.profile.fileExtensions.sourceFileExtensions.begin(), project.profile.fileExtensions.sourceFileExtensions.end(), [&](std::string ext) {
                            if(e.path().extension() == ext) {
                                //std::cout << e.path().string() << "\n";
                                core::solution::Source source;
                                source.srcDir = e.path().string();
                                std::vector<std::string> args;
                                std::string fileName = e.path().filename().string();
                                util::strSplit(fileName, '.', [&](std::string line) {
                                    args.push_back(line);
                                });
                                std::stringstream ss;
                                ss << s.objDir << args[0] << project.profile.fileExtensions.objectFileExtension;
                                source.objDir = ss.str();

                                sourceList.push_back(source);
                                objectList.push_back(source.objDir);

                                args.clear();
                            }
                        });
                    });
                });
            });
        }

        return createCommandList(op, solution, sourceList, objectList);
    }

    int createBuildCommandList(core::solution::Solution& solution, std::vector<core::solution::Source>& sourceList, std::vector<std::string>& objectList);

    int createCleanCommandList(core::solution::Solution& solution, std::vector<std::string>& objectList);

    int createCommandList(
        core::Operation op, 
        core::solution::Solution& solution, 
        std::vector<core::solution::Source>& sourceList, 
        std::vector<std::string>& objectList) {
        if(op == core::Operation::OP_BUILD) {
            return createBuildCommandList(solution, sourceList, objectList);
        } else if(op == core::Operation::OP_CLEAN) {
            return createCleanCommandList(solution, objectList);
        }
    }

    int createBuildCommandList(core::solution::Solution& solution, std::vector<core::solution::Source>& sourceList, std::vector<std::string>& objectList) {
        return 0;
    }

    int createCleanCommandList(core::solution::Solution& solution, std::vector<std::string>& objectList) {
        return 0;
    }

}
