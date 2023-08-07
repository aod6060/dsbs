#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        int operationsCheck(Operation op, std::string projectName, solution::Solution& solution, bool useMT) {
            if(op == Operation::OP_BUILD) {
                return build::buildOperation(projectName, solution, useMT);
            } else if(op == Operation::OP_CLEAN) {
                return clean::cleanOperation(projectName, solution, useMT);
            } else if(op == Operation::OP_REBUILD) {
            	return rebuild::rebuildOperation(projectName, solution, useMT);
            } else if(op == Operation::OP_INIT) {
       		return init::initOperation(solution);
            } else if(op == Operation::OP_RELEASE) {
            	return release::releaseOperation(solution);
            } else if(op == Operation::OP_RUN) {
		return run::runOperation(projectName, solution, useMT);
	    }
            std::cout << "Error: The operation needs to be either \"build\" or \"clean\"" << "\n";
            return -1;
        }
    }
}
