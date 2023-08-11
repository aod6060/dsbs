#include <dsbs/dsbs_internal.hpp>
#include <pthread.h>


namespace dsbs {
    namespace internal {
    
    	// Going to use pthread instead of thread
    	
    	// I'll alow up to 8 threads for now.
    	const static int MAX_THREADS = 8;
    	
    	/*
	void executeThread(int n, const command::Command& c) {
                 std::cout << "\t* " << c.command << "\n";
                 std::string output = ::util::exec(c.command);
               	 if(!output.empty()) {
                       	 std::cout << output << "\n";
		 }

	}
	*/
	
	struct Thread {
		pthread_t thread_id;
		std::vector<command::Command> commands;
	};
	
	void* compileThread(void* data) {
		//std::cout << "Hello, World" << "\n";
		
		Thread* t = (Thread*)data;
		
		std::for_each(t->commands.begin(), t->commands.end(), [&](command::Command& command) {
	    		std::cout << "\t* " << command.command << "\n";
	    		std::string output = ::util::exec(command.command);
	    		if(!output.empty()) {
				std::cout << output << "\n";
	    		}
		});
	}
	
        int executeCommandList(std::vector<command::Project>& projectCommandList, bool useMT) {
            std::for_each(projectCommandList.begin(), projectCommandList.end(), [&](command::Project& project) {
                std::cout << "Building \""<< project.projectName <<"\"..." << "\n";
                
		// Execute Commands
		if(useMT) {
			/*
			std::vector<std::thread> threads;

			for(int i = 0; i < project.commands.size(); i++) {
				threads.push_back(std::thread(executeThread, i, project.commands[i]));
			}

			for(std::thread& t : threads) {
				t.join();
			}
			*/
			
			//std::vector<std::vector<command::Command>> commands;
			//commands.resize(MAX_THREADS);
			
			std::vector<Thread> threads;
			threads.resize(MAX_THREADS);
			
			for(int i = 0; i < project.commands.size(); i++) {
				//commands[i % MAX_THREADS].push_back(project.commands[i]);
				threads[i % MAX_THREADS].commands.push_back(project.commands[i]);
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
            
            if(useMT) {
            	pthread_exit(nullptr);
            }
            
            return 0;
        }

    }
}
