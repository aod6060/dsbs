#include <dsbs/dsbs_internal.hpp>

namespace dsbs {
    namespace internal {


	void executeThread(int n, const command::Command& c) {
                 std::cout << "\t* " << c.command << "\n";
                 std::string output = ::util::exec(c.command);
               	 if(!output.empty()) {
                       	 std::cout << output << "\n";
		 }

	}

        int executeCommandList(std::vector<command::Project>& projectCommandList, bool useMT) {
            std::for_each(projectCommandList.begin(), projectCommandList.end(), [&](command::Project& project) {
                std::cout << "Building \""<< project.projectName <<"\"..." << "\n";
                
		// Execute Commands
		if(useMT) {
			std::vector<std::thread> threads;

			for(int i = 0; i < project.commands.size(); i++) {
				threads.push_back(std::thread(executeThread, i, project.commands[i]));
			}

			for(std::thread& t : threads) {
				t.join();
			}

		} else {
			std::for_each(project.commands.begin(), project.commands.end(), [&](command::Command& command) {
                    		std::cout << "\t* " << command.command << "\n";
                    		std::string output = ::util::exec(command.command);
                    		if(!output.empty()) {
                        		std::cout << output << "\n";
                    		}
                	});
		}
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
