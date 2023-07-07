#include <dsbs/dsbs_internal.hpp>

namespace dsbs {
    namespace internal {
        int executeCommandList(std::vector<command::Project>& projectCommandList) {
            std::for_each(projectCommandList.begin(), projectCommandList.end(), [&](command::Project& project) {
                std::cout << "Building \""<< project.projectName <<"\"..." << "\n";
                std::for_each(project.command.begin(), project.command.end(), [&](command::Command& command) {
                    std::cout << "\t* " << command.command << "\n";
                    std::string output = ::util::exec(command.command);
                    if(!output.empty()) {
                        std::cout << output << "\n";
                    }
                });
                std::cout << "End Building \""<< project.projectName <<"\"" << "\n";
                std::cout << "\n";
            });
            return 0;
        }

    }
}