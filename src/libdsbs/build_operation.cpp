#include <dsbs/dsbs_internal.hpp>

namespace dsbs {
    namespace internal {

        std::function<void(std::string)> createExtensionCheck(std::filesystem::directory_entry entry, solution::Source& source, solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
            return [&](std::string ext) {
                if(ext == entry.path().extension()) {
                    //std::cout << entry.path().string() << "\n";
                    solution::Source pairs;
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
                    if(project.type == solution::ProjectType::PT_SHARED_LIB) {
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
                    command::Command c;
                    c.command = command.str();
                    c.projectName = project.name;
                    //commandList.push_back(c);
                    projectCommand.command.push_back(c);
                }
            };
        }

        std::function<void(std::filesystem::directory_entry entry)> createDirectorySearch(solution::Source& source, solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
            return [&](std::filesystem::directory_entry entry) {
                    //std::cout << entry.path().string() << "\n";
                std::for_each(
                    project.profile.fileExtensions.sourceFileExtensions.begin(),
                    project.profile.fileExtensions.sourceFileExtensions.end(),
                    createExtensionCheck(entry, source, project, objects, projectCommand)
                );
            };
        }

        std::function<void(solution::Source&)> createSolutionIterator(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
            return [&](solution::Source& source) {
                // Iterate through directory and searching for correct source files
                ::util::iterateDirectory(std::filesystem::path(source.srcDir), createDirectorySearch(source, project, objects, projectCommand));
            };
        }

        void createExecutable(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
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

            command::Command c;
            c.command = cb.str();
            c.projectName = project.name;
            projectCommand.command.push_back(c);
            //commandList.push_back(c);

        }
        
        void createStaticLibrary(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
                std::stringstream cb;

                // ar [options] [archive-name] [objects]
                cb << project.profile.programs.archiver << " ";
                cb << project.profile.flags.staticLibFlags << " ";
                cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";
                // Adding in objects
                std::for_each(objects.begin(), objects.end(), [&](std::string o) {
                    cb << o << " ";
                });
                command::Command c;
                c.command = cb.str();
                c.projectName = project.name;
                //commandList.push_back(c);
                projectCommand.command.push_back(c);
        }

        void createSharedLibrary(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
            createStaticLibrary(project, objects, projectCommand);
        }

        void createProjectCommand(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
            // Build Project Command
            if(project.type == solution::ProjectType::PT_EXECUTABLE) {
                createExecutable(project, objects, projectCommand);
            } else if(project.type == solution::ProjectType::PT_STATIC_LIB) {
                createStaticLibrary(project, objects, projectCommand);
            } else if(project.type == solution::ProjectType::PT_SHARED_LIB) {
                createSharedLibrary(project, objects, projectCommand);
            }
        }
        
        void createCommandList(solution::Project& project, std::vector<command::Project>& projectCommandList) {
            command::Project projectCommand;
            projectCommand.projectName = project.name;

            std::vector<std::string> objects;

            std::for_each(project.sources.begin(), project.sources.end(), createSolutionIterator(project, objects, projectCommand));

            createProjectCommand(project, objects, projectCommand);
            
            projectCommandList.push_back(projectCommand);
        }
        
        int buildOperation(std::string projectName, solution::Solution& solution) {
            std::vector<command::Project> projectCommandList;

            if(!projectName.empty()) {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
                    if(project.name == projectName) {
                        createCommandList(project, projectCommandList);
                    }
                });
            } else {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
                    createCommandList(project, projectCommandList);
                });
            }

            return executeCommandList(projectCommandList);
        }

    }
}