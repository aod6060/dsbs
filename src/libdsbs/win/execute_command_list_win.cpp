#include <dsbs/dsbs_internal.hpp>
#include <windows.h>

namespace dsbs {
    namespace internal {
    	const static int MAX_THREADS = 8;
    	
    	struct Thread {
    		HANDLE thread_id;
    		std::vector<command::Command> commands;
    	};
    	
	DWORD WINAPI compileThread(LPVOID data) {
		Thread* t = (Thread*)data;
		
		std::for_each(t->commands.begin(), t->commands.end(), [&](command::Command& command) {
	    		std::cout << "\t* " << command.command << "\n";
	    		std::string output = ::util::exec(command.command);
	    		if(!output.empty()) {
				std::cout << output << "\n";
	    		}
		});
	}
	
        //int executeCommandList(std::vector<command::Project>& projectCommandList, bool useMT) {
        int executeCommandList(Context& context, std::vector<command::Project>& projectCommandList) {
            std::for_each(projectCommandList.begin(), projectCommandList.end(), [&](command::Project& project) {
                std::cout << "Building \""<< project.projectName <<"\"..." << "\n";
                
		// Execute Commands
		if(context.useMT) {
			//std::cout << "Error: Current windows build can't use -useMT. Will need to figure out a better approach for this." << "\n";
			
			
			std::vector<Thread> threads;
			threads.resize(context.numThreads);
			
			// Setup Data
			for(int i = 0; i < project.commands.size(); i++) {
				threads[i % numThreads].commands.push_back(project.commands[i]);
			}
			
			for(int i = 0; i < threads.size(); i++) {
				threads[i].thread_id = CreateThread(
					nullptr,
					0,
					compileThread,
					&threads[i],
					0,
					nullptr
				);
			}
			
			
			for(int i = 0; i < threads.size(); i++) {
				WaitForSingleObject(threads[i].thread_id, INFINITE);
			}
			
			
			// Cleanup Threads
			for(int i = 0; i < threads.size(); i++) {
				CloseHandle(threads[i].thread_id);
			}
			
			//WaitForMultipleObjects(MAX_THREADS, 
		}
		
		else {
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
