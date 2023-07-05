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
            profile.flags.libFlags = root["flags"]["lib-flag"].asString();
            profile.flags.compileFlag = root["flags"]["compile-flag"].asString();
            profile.flags.objectFlag = root["flags"]["object-flag"].asString();
            profile.flags.staticLibFlags = root["flags"]["static-lib-flag"].asString();
            profile.flags.sharedLibFlags = root["flags"]["shared-lib-flag"].asString();
            profile.flags.sharedLibFlagsLinker = root["flags"]["shared-lib-flag-linker"].asString();

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

    //int buildSourceObjectList(core::Operation op, std::string projectName, core::solution::Solution& solution);

    int operationsCheck(core::Operation op, std::string projectName, core::solution::Solution& solution);

    int loadSolutionProfile(core::Operation op, std::string solutionFile, std::string projectName) {

        // Load Solution
        core::solution::Solution solution;

        util::loadStream(solutionFile, loadSolution(solution));

        //return buildSourceObjectList(op, projectName, solution);
        return operationsCheck(op, projectName, solution);
    }

    int buildOperation(std::string projectName, core::solution::Solution& solution);
    int cleanOperation(std::string projectName, core::solution::Solution& solution);

    int operationsCheck(core::Operation op, std::string projectName, core::solution::Solution& solution) {
        if(op == core::Operation::OP_BUILD) {
            return buildOperation(projectName, solution);
        } else if(op == core::Operation::OP_CLEAN) {
            return cleanOperation(projectName, solution);
        }
        std::cout << "Error: The operation needs to be either \"build\" or \"clean\"" << "\n";
        return -1;
    }

    int executeCommandList(std::vector<core::command>& commandList);

    int buildOperation(std::string projectName, core::solution::Solution& solution) {
        //std::cout << "Building..." << "\n";

        std::vector<core::command> commandList;

        if(!projectName.empty()) {
            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& project) {
                if(project.name == projectName) {
                    // Grab sources
                    std::cout << "Building: " << project.name << "\n";

                    std::vector<std::string> objects;

                    std::for_each(project.sources.begin(), project.sources.end(), [&](core::solution::Source& source) {

                        

                        // Iterate through directory and searching for correct source files
                        util::iterateDirectory(std::filesystem::path(source.srcDir), [&](std::filesystem::directory_entry entry) {
                            //std::cout << entry.path().string() << "\n";
                            std::for_each(
                                project.profile.fileExtensions.sourceFileExtensions.begin(),
                                project.profile.fileExtensions.sourceFileExtensions.end(),
                                [&](std::string ext) {
                                    if(ext == entry.path().extension()) {
                                        //std::cout << entry.path().string() << "\n";
                                        core::solution::Source pairs;
                                        pairs.srcDir = entry.path().string();

                                        std::string temp = entry.path().filename().string();
                                        std::vector<std::string> args;

                                        util::strSplit(temp, '.', [&](std::string s) {
                                            args.push_back(s);
                                        });

                                        std::stringstream ss;
                                        ss << source.objDir << args[0] << project.profile.fileExtensions.objectFileExtension;

                                        pairs.objDir = ss.str();

                                        objects.push_back(pairs.objDir);

                                        //std::cout << pairs.srcDir << " -> " << pairs.objDir << "\n";
                                        // Build Command
                                        std::stringstream command;
                                        command << project.profile.programs.compiler << " ";
                                        if(project.type == core::solution::ProjectType::PT_SHARED_LIB) {
                                            //command << project.profile.flags.sharedLibFlags << " ";
                                        }
                                        command << project.profile.flags.compileFlag << " ";
                                        command << pairs.srcDir << " ";
                                        command << project.profile.flags.objectFlag << " ";
                                        command << pairs.objDir << " ";
                                        // Includes
                                        std::for_each(project.includeDirs.begin(), project.includeDirs.end(), [&](std::string inc) {
                                            command << project.profile.flags.includeDirFlag << inc << " ";
                                        });
                                        // Compiler Options
                                        std::for_each(project.compilerOptions.begin(), project.compilerOptions.end(), [&](std::string option) {
                                            command << option << " ";
                                        });

                                        //commandList.push_back(command.str());
                                        core::command c;
                                        c.command = command.str();
                                        c.projectName = project.name;
                                        commandList.push_back(c);
                                    }
                                }
                            );
                        });                    
                    });

                    // Build Project Command
                    if(project.type == core::solution::ProjectType::PT_EXECUTABLE) {
                        std::stringstream cb;

                        cb << project.profile.programs.linker << " ";
                        
                        // Adding in objects
                        std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                            cb << o << " ";
                        });

                        // Create Binary
                        cb << project.profile.flags.objectFlag << " ";
                        cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension << " ";

                        // Link Libraries
                        // Library Dirs
                        std::for_each(project.libDirs.begin(), project.libDirs.end(), [&](std::string s) {
                            cb << project.profile.flags.libDirFlag << s << " ";
                        });

                        // Libraries
                        std::for_each(project.libs.begin(), project.libs.end(), [&](std::string s) {
                            cb << project.profile.flags.libFlags << s << " ";
                        });

                        // Linker Options
                        std::for_each(project.linkerOptions.begin(), project.linkerOptions.end(), [&](std::string s) {
                            cb << s << " ";
                        });

                        core::command c;
                        c.command = cb.str();
                        c.projectName = project.name;

                        commandList.push_back(c);

                    } else if(project.type == core::solution::ProjectType::PT_STATIC_LIB) {
                        std::stringstream cb;

                        // ar [options] [archive-name] [objects]
                        cb << project.profile.programs.archiver << " ";
                        cb << project.profile.flags.staticLibFlags << " ";
                        cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";
                        // Adding in objects
                        std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                            cb << o << " ";
                        });
                        core::command c;
                        c.command = cb.str();
                        c.projectName = project.name;
                        commandList.push_back(c);
                    } else if(project.type == core::solution::ProjectType::PT_SHARED_LIB) {

                        std::cout << "TODO: Currently dsbs doesn't support shared but will in a future update. " << "\n";
                        
                        std::stringstream cb;

                        // ar [options] [archive-name] [objects]
                        cb << project.profile.programs.archiver << " ";
                        cb << project.profile.flags.staticLibFlags << " ";
                        cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";
                        // Adding in objects
                        std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                            cb << o << " ";
                        });
                        core::command c;
                        c.command = cb.str();
                        c.projectName = project.name;
                        commandList.push_back(c);
                        /*
                        std::stringstream cb;

                        cb << project.profile.programs.linker << " ";
                        cb << project.profile.flags.sharedLibFlagsLinker << " ";

                        // Adding in objects
                        std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                            cb << o << " ";
                        });

                        // Create Binary
                        cb << project.profile.flags.objectFlag << " ";
                        cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension << " ";

                        // Link Libraries
                        // Library Dirs
                        std::for_each(project.libDirs.begin(), project.libDirs.end(), [&](std::string s) {
                            cb << project.profile.flags.libDirFlag << s << " ";
                        });

                        // Libraries
                        std::for_each(project.libs.begin(), project.libs.end(), [&](std::string s) {
                            cb << project.profile.flags.libFlags << s << " ";
                        });

                        // Linker Options
                        std::for_each(project.linkerOptions.begin(), project.linkerOptions.end(), [&](std::string s) {
                            cb << s << " ";
                        });

                        core::command c;
                        c.command = cb.str();
                        c.projectName = project.name;

                        commandList.push_back(c);
                        */
                    }
                }
            });
        } else {
            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& project) {
                // Grab sources
                std::cout << "Building: " << project.name << "\n";

                std::vector<std::string> objects;

                std::for_each(project.sources.begin(), project.sources.end(), [&](core::solution::Source& source) {

                    

                    // Iterate through directory and searching for correct source files
                    util::iterateDirectory(std::filesystem::path(source.srcDir), [&](std::filesystem::directory_entry entry) {
                        //std::cout << entry.path().string() << "\n";
                        std::for_each(
                            project.profile.fileExtensions.sourceFileExtensions.begin(),
                            project.profile.fileExtensions.sourceFileExtensions.end(),
                            [&](std::string ext) {
                                if(ext == entry.path().extension()) {
                                    //std::cout << entry.path().string() << "\n";
                                    core::solution::Source pairs;
                                    pairs.srcDir = entry.path().string();

                                    std::string temp = entry.path().filename().string();
                                    std::vector<std::string> args;

                                    util::strSplit(temp, '.', [&](std::string s) {
                                        args.push_back(s);
                                    });

                                    std::stringstream ss;
                                    ss << source.objDir << args[0] << project.profile.fileExtensions.objectFileExtension;

                                    pairs.objDir = ss.str();

                                    objects.push_back(pairs.objDir);

                                    //std::cout << pairs.srcDir << " -> " << pairs.objDir << "\n";
                                    // Build Command
                                    std::stringstream command;
                                    command << project.profile.programs.compiler << " ";
                                    if(project.type == core::solution::ProjectType::PT_SHARED_LIB) {
                                        //command << project.profile.flags.sharedLibFlags << " ";
                                    }
                                    command << project.profile.flags.compileFlag << " ";
                                    command << pairs.srcDir << " ";
                                    command << project.profile.flags.objectFlag << " ";
                                    command << pairs.objDir << " ";
                                    // Includes
                                    std::for_each(project.includeDirs.begin(), project.includeDirs.end(), [&](std::string inc) {
                                        command << project.profile.flags.includeDirFlag << inc << " ";
                                    });
                                    // Compiler Options
                                    std::for_each(project.compilerOptions.begin(), project.compilerOptions.end(), [&](std::string option) {
                                        command << option << " ";
                                    });

                                    //commandList.push_back(command.str());
                                    core::command c;
                                    c.command = command.str();
                                    c.projectName = project.name;
                                    commandList.push_back(c);
                                }
                            }
                        );
                    });                    
                });

                // Build Project Command
                if(project.type == core::solution::ProjectType::PT_EXECUTABLE) {
                    std::stringstream cb;

                    cb << project.profile.programs.linker << " ";
                    
                    // Adding in objects
                    std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                        cb << o << " ";
                    });

                    // Create Binary
                    cb << project.profile.flags.objectFlag << " ";
                    cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension << " ";

                    // Link Libraries
                    // Library Dirs
                    std::for_each(project.libDirs.begin(), project.libDirs.end(), [&](std::string s) {
                        cb << project.profile.flags.libDirFlag << s << " ";
                    });

                    // Libraries
                    std::for_each(project.libs.begin(), project.libs.end(), [&](std::string s) {
                        cb << project.profile.flags.libFlags << s << " ";
                    });

                    // Linker Options
                    std::for_each(project.linkerOptions.begin(), project.linkerOptions.end(), [&](std::string s) {
                        cb << s << " ";
                    });

                    core::command c;
                    c.command = cb.str();
                    c.projectName = project.name;

                    commandList.push_back(c);

                } else if(project.type == core::solution::ProjectType::PT_STATIC_LIB) {
                    std::stringstream cb;

                    // ar [options] [archive-name] [objects]
                    cb << project.profile.programs.archiver << " ";
                    cb << project.profile.flags.staticLibFlags << " ";
                    cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";
                    // Adding in objects
                    std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                        cb << o << " ";
                    });
                    core::command c;
                    c.command = cb.str();
                    c.projectName = project.name;
                    commandList.push_back(c);
                } else if(project.type == core::solution::ProjectType::PT_SHARED_LIB) {

                    std::cout << "TODO: Currently dsbs doesn't support shared but will in a future update. " << "\n";

                    std::stringstream cb;

                    // ar [options] [archive-name] [objects]
                    cb << project.profile.programs.archiver << " ";
                    cb << project.profile.flags.staticLibFlags << " ";
                    cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";
                    // Adding in objects
                    std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                        cb << o << " ";
                    });
                    core::command c;
                    c.command = cb.str();
                    c.projectName = project.name;
                    commandList.push_back(c);
                    /*
                    std::stringstream cb;

                    cb << project.profile.programs.linker << " ";
                    cb << project.profile.flags.sharedLibFlagsLinker << " ";

                    // Adding in objects
                    std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                        cb << o << " ";
                    });

                    // Create Binary
                    cb << project.profile.flags.objectFlag << " ";
                    cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension << " ";

                    // Link Libraries
                    // Library Dirs
                    std::for_each(project.libDirs.begin(), project.libDirs.end(), [&](std::string s) {
                        cb << project.profile.flags.libDirFlag << s << " ";
                    });

                    // Libraries
                    std::for_each(project.libs.begin(), project.libs.end(), [&](std::string s) {
                        cb << project.profile.flags.libFlags << s << " ";
                    });

                    // Linker Options
                    std::for_each(project.linkerOptions.begin(), project.linkerOptions.end(), [&](std::string s) {
                        cb << s << " ";
                    });

                    core::command c;
                    c.command = cb.str();
                    c.projectName = project.name;

                    commandList.push_back(c);
                    */
                }

            });
        }

        return executeCommandList(commandList);
    }

    int executeCommandList(std::vector<core::command>& commandList) {
        std::for_each(commandList.begin(), commandList.end(), [&](core::command& command) {
            std::cout << "["<<command.projectName<<"] -> " << command.command << "\n";
            // Execute command with some sort of system level api call.
            std::string output = util::exec(command.command);
            if(!output.empty()) {
                std::cout << output << "\n";
            }
        });

        return 0;
    }

    int cleanOperation(std::string projectName, core::solution::Solution& solution) {
        std::vector<std::filesystem::path> paths;

        if(!projectName.empty()) {
            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& project) {

                if(projectName == project.name) {
                    // Scan through objects
                    std::for_each(project.sources.begin(), project.sources.end(), [&](core::solution::Source& source) {
                        util::iterateDirectory(source.objDir, [&](std::filesystem::directory_entry entry) {
                            if(entry.path().extension() == project.profile.fileExtensions.objectFileExtension) {
                                paths.push_back(entry.path());
                            }
                        });
                    });

                    if(project.type == core::solution::ProjectType::PT_EXECUTABLE) {
                        std::stringstream cb;
                        cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension;
                        paths.push_back(std::filesystem::path(cb.str()));

                    } else if(project.type == core::solution::ProjectType::PT_SHARED_LIB || project.type == core::solution::ProjectType::PT_STATIC_LIB) {
                        std::stringstream cb;
                        cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                        paths.push_back(std::filesystem::path(cb.str()));
                    }

                }
            });
        } else {
            std::for_each(solution.projects.begin(), solution.projects.end(), [&](core::solution::Project& project) {
                // Scan through objects
                std::for_each(project.sources.begin(), project.sources.end(), [&](core::solution::Source& source) {
                    util::iterateDirectory(source.objDir, [&](std::filesystem::directory_entry entry) {
                        if(entry.path().extension() == project.profile.fileExtensions.objectFileExtension) {
                            paths.push_back(entry.path());
                        }
                    });
                });

                if(project.type == core::solution::ProjectType::PT_EXECUTABLE) {
                    std::stringstream cb;
                    cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension;
                    paths.push_back(std::filesystem::path(cb.str()));

                } else if(project.type == core::solution::ProjectType::PT_SHARED_LIB || project.type == core::solution::ProjectType::PT_STATIC_LIB) {
                    std::stringstream cb;
                    cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                    paths.push_back(std::filesystem::path(cb.str()));
                }
            });
        }

        // Remove files
        std::for_each(paths.begin(), paths.end(), [&](std::filesystem::path p) {
            std::cout << "Removing: " << p.string() << "\n";
            std::filesystem::remove(p);
        });

        return 0;
    }

}
