#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        int operationsCheck(Operation op, std::string projectName, solution::Solution& solution) {
            if(op == Operation::OP_BUILD) {
                return buildOperation(projectName, solution);
            } else if(op == Operation::OP_CLEAN) {
                return cleanOperation(projectName, solution);
            }
            std::cout << "Error: The operation needs to be either \"build\" or \"clean\"" << "\n";
            return -1;
        }
    }
}