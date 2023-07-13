#include <dsbs/dsbs_internal.hpp>

namespace dsbs {
    /*
        Creating Backend... This is were the magic will begin...
    */
    int run(Operation op, std::string solutionFile, std::string projectName) {
        //std::cout << ((op == core::OP_BUILD) ? "build" : "cleaning") << " " << solutionFile << " " << projectName << "\n";
        return internal::loadSolutionProfile(op, solutionFile, projectName);
    }
}
