#include <dsbs/dsbs_internal.hpp>
#include <pthread.h>


namespace dsbs {
    namespace internal {
    
    	// Going to use pthread instead of thread
    	
    	// I'll alow up to 8 threads for now.
    	//const static int MAX_THREADS = 8;
	struct Thread {
		pthread_t thread_id;
		std::vector<command::Command> commands;
	};
	
	void* compileThread(void* data) {
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
			std::vector<Thread> threads;
			threads.resize(context.numThreads);
			
			for(int i = 0; i < project.commands.size(); i++) {
				//commands[i % MAX_THREADS].push_back(project.commands[i]);
				threads[i % context.numThreads].commands.push_back(project.commands[i]);
			}
			
			for(int i = 0; i < threads.size(); i++) {
				pthread_create(&threads[i].thread_id, nullptr, compileThread, &threads[i]);
			}
			
			for(int i = 0; i < threads.size(); i++) {
				pthread_join(threads[i].thread_id, nullptr);
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
            
            if(context.useMT) {
            	pthread_exit(nullptr);
            }
            
            return 0;
        }

    }
}
