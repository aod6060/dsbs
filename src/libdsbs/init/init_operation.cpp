#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
	namespace internal {
		namespace init {
			//int initOperation(solution::Solution& solution) {
			int initOperation(Context& context, solution::Solution& solution) {
				// TODO ~ Work on initOperation
				//std::cout << "TODO ~ Work on initOperation" << "\n";
				
				std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& p) {
					// Bin Dirs
					if(!std::filesystem::exists(std::filesystem::path(p.binDir))) {
						std::filesystem::create_directories(std::filesystem::path(p.binDir));
					}
					
					// Sources
					std::for_each(p.sources.begin(), p.sources.end(), [&](solution::Source& s) {
						
						// src-dir
						if(!std::filesystem::exists(std::filesystem::path(s.srcDir))) {
							std::filesystem::create_directories(std::filesystem::path(s.srcDir));
						}
						// cache dir
						if(!std::filesystem::exists(std::filesystem::path(s.cacheDir))) {
							std::filesystem::create_directories(std::filesystem::path(s.cacheDir));
						}						
						// obj dir
						if(!std::filesystem::exists(std::filesystem::path(s.objDir))) {
							std::filesystem::create_directories(std::filesystem::path(s.objDir));
						}
					});
					
					// Includes Dirs
					std::for_each(p.includeDirs.begin(), p.includeDirs.end(), [&](std::string inc) {
						if(!std::filesystem::exists(std::filesystem::path(inc))) {
							std::filesystem::create_directories(std::filesystem::path(inc));
						}
					});
					// Lib Dirs
					std::for_each(p.libDirs.begin(), p.libDirs.end(), [&](std::string l) {
						if(!std::filesystem::exists(std::filesystem::path(l))) {
							std::filesystem::create_directories(std::filesystem::path(l));
						}
					});
				});
				
				return 0;
			}
		}
	}
}
