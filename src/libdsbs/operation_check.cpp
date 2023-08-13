#include <dsbs/dsbs_internal.hpp>

/*
/usr/bin/ld: ./test//libdsbs.a(operation_check.o): in function `dsbs::internal::operationCheck(dsbs::Context&, dsbs::solution::Solution&)':
operation_check.cpp:(.text+0x4c): undefined reference to `dsbs::internal::clean::cleanOperation(dsbs::Context&, dsbs::solution::Solution&)'
/usr/bin/ld: operation_check.cpp:(.text+0xd2): undefined reference to `dsbs::internal::run::runOperation(dsbs::Context&, dsbs::solution::Solution&)'
collect2: error: ld returned 1 exit status

*/
namespace dsbs {
    namespace internal {
        int operationCheck(Context& context, solution::Solution& solution) {
            if(context.op == Operation::OP_BUILD) {
                return build::buildOperation(context, solution);
            } 
            else if(context.op == Operation::OP_CLEAN) {
                return clean::cleanOperation(context, solution);
            } 
            else if(context.op == Operation::OP_REBUILD) {
            	return rebuild::rebuildOperation(context, solution);
            } 
            else if(context.op == Operation::OP_INIT) {
       		return init::initOperation(context, solution);
            } 
            else if(context.op == Operation::OP_RELEASE) {
            	return release::releaseOperation(context, solution);
            } 
            else if(context.op == Operation::OP_RUN) {
		return run::runOperation(context, solution);
	    }
            std::cout << "Error: The operation needs to be either \"build\" or \"clean\"" << "\n";
            return -1;
        }
    }
}
