#include <dsbs/dsbs_internal.hpp>
#include <windows.h>

namespace dsbs {
    namespace internal {

	/*
	void executeThread(int n, const command::Command& c) {
                 std::cout << "\t* " << c.command << "\n";
                 std::string output = ::util::exec(c.command);
               	 if(!output.empty()) {
                       	 std::cout << output << "\n";
		 }

	}
	*/
        int executeCommandList(std::vector<command::Project>& projectCommandList, bool useMT) {
            std::for_each(projectCommandList.begin(), projectCommandList.end(), [&](command::Project& project) {
                std::cout << "Building \""<< project.projectName <<"\"..." << "\n";
                
		// Execute Commands
		if(useMT) {
			std::cout << "Error: Current windows build can't use -useMT. Will need to figure out a better approach for this." << "\n";
		}
		
		std::for_each(project.commands.begin(), project.commands.end(), [&](command::Command& command) {
                    	std::cout << "\t* " << command.command << "\n";
                    	std::string output = ::util::exec(command.command);
                    	if(!output.empty()) {
                        	std::cout << output << "\n";
                    	}
                });

		// Execute Project Command
              	std::cout << "\t* " << project.projectCommand.command << "\n";
                std::string output = ::util::exec(project.projectCommand.command);
               	if(!output.empty()) {
                        std::cout << output << "\n";
               	}


                std::cout << "End Building \""<< project.projectName <<"\"" << "\n";
                std::cout << "\n";
            });
            return 0;
        }

    }
}
