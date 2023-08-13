#include <dsbs/dsbs_internal.hpp>

namespace dsbs {
    namespace internal {
    
    	namespace rebuild {
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

				    ss.str("");

				    // Do a cache check to see if the file has changed...
				    ss << source.cacheDir << args[0] << ext << ".cache";

				    std::filesystem::path cacheObject = std::filesystem::path(ss.str());
				    //std::filesystem::path objObject = std::filesystem::path(pairs.objDir);

				    //std::cout << cacheObject << "\n";
				    //
				    //

				    if(!std::filesystem::exists(cacheObject)) {
					    std::filesystem::copy(entry.path(), cacheObject);
				    }


				    std::stringstream sourceCode, cacheCode;

				    util::loadFile(entry.path().string(), [&](std::string line) {
						    sourceCode << line << "\n";
				    });

				    util::loadFile(cacheObject.string(), [&](std::string line) {
						    cacheCode << line << "\n";
				    });

				    std::string sourceCodeStr = sourceCode.str();
				    std::string cacheCodeStr = cacheCode.str();

				    if(sourceCodeStr.size() != cacheCodeStr.size()) {
					    std::filesystem::remove(cacheObject);
					    std::filesystem::copy(entry.path(), cacheObject);
				    } else {
					    if(sourceCodeStr != cacheCodeStr) {
						    std::filesystem::remove(cacheObject);
						    std::filesystem::copy(entry.path(), cacheObject);
					    }
				    }

		            //std::cout << pairs.srcDir << " -> " << pairs.objDir << "\n";
		            // Build Command
			    //
			    	std::stringstream command;
			    	command << project.profile.programs.compiler << " ";
			    	if(project.type == solution::ProjectType::PT_SHARED_LIB) {
			        	command << project.profile.flags.sharedLibFlags << " ";
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
			    	//c.projectName = project.name;
			    	//commandList.push_back(c);
			    	projectCommand.commands.push_back(c);
			    
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

		    //command::Command c;
		    //c.command = cb.str();
		    //c.projectName = project.name;
		    //projectCommand.commands.push_back(c);
		    projectCommand.projectCommand.command = cb.str();

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
		    //command::Command c;
		    //c.command = cb.str();
		    //c.projectName = project.name;
		    //projectCommand.commands.push_back(c);
		    projectCommand.projectCommand.command = cb.str();
		}

		void createSharedLibrary(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
		    //createStaticLibrary(project, objects, projectCommand);

		    /*
		        all: foo.o bar.o main.o
		            gcc -shared foo.o -o libfoo.dll -Wl,--out-implib,libfoo.a
		            gcc -shared bar.o foo.o -o libbar.dll -Wl,--out-implib,libbar.a
		            gcc main.o  -Wl,-rpath=. -L. -lbar -lfoo -o main
		    */

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
		    // Import Library
		    //cb << project.profile.flags.sharedImportLibFlag << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";

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

		    //command::Command c;
		    //c.command = cb.str();
		    //c.projectName = project.name;
		    //projectCommand.commands.push_back(c);
		    projectCommand.projectCommand.command = cb.str();

		}

		void createSharedDLLLibrary(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
		    //createStaticLibrary(project, objects, projectCommand);

		    /*
		        all: foo.o bar.o main.o
		            gcc -shared foo.o -o libfoo.dll -Wl,--out-implib,libfoo.a
		            gcc -shared bar.o foo.o -o libbar.dll -Wl,--out-implib,libbar.a
		            gcc main.o  -Wl,-rpath=. -L. -lbar -lfoo -o main
		    */

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
		    // Import Library
		    cb << project.profile.flags.sharedImportLibFlag << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension << " ";

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

		    //command::Command c;
		    //c.command = cb.str();
		    //c.projectName = project.name;
		    //projectCommand.commands.push_back(c);
		    projectCommand.projectCommand.command = cb.str();

		}

		void createProjectCommand(solution::Project& project, std::vector<std::string>& objects, command::Project& projectCommand) {
		    // Build Project Command
		    if(project.type == solution::ProjectType::PT_EXECUTABLE) {
		        createExecutable(project, objects, projectCommand);
		    } else if(project.type == solution::ProjectType::PT_STATIC_LIB) {
		        createStaticLibrary(project, objects, projectCommand);
		    } else if(project.type == solution::ProjectType::PT_SHARED_LIB) {
		        createSharedLibrary(project, objects, projectCommand);
		    } else if(project.type == solution::ProjectType::PT_SHARED_DLL_LIB) {
		        createSharedDLLLibrary(project, objects, projectCommand);
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
		
		//int rebuildOperation(std::string projectName, solution::Solution& solution, bool useMT) {
		int rebuildOperation(Context& context, solution::Solution& solution) {
		    std::vector<command::Project> projectCommandList;

		    if(!context.projectName.empty()) {
		        std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
		            if(project.name == context.projectName) {
		                createCommandList(project, projectCommandList);
		            }
		        });
		    } else {
		        std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
		            createCommandList(project, projectCommandList);
		        });
		    }

		    //return executeCommandList(projectCommandList, useMT);
		    return executeCommandList(context, projectCommandList);
		}

	    }
	    
    }
}
