#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        int operationsCheck(Operation op, std::string projectName, solution::Solution& solution, bool useMT) {
            if(op == Operation::OP_BUILD) {
                return build::buildOperation(projectName, solution, useMT);
            } else if(op == Operation::OP_CLEAN) {
                return clean::cleanOperation(projectName, solution, useMT);
            }
            std::cout << "Error: The operation needs to be either \"build\" or \"clean\"" << "\n";
            return -1;
        }
    }
}
