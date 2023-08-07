#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        
        void handleCleanOperations(dsbs::solution::Project& project, std::vector<std::filesystem::path>& paths) {
            // Scan through objects
            std::for_each(project.sources.begin(), project.sources.end(), [&](solution::Source& source) {
                ::util::iterateDirectory(source.objDir, [&](std::filesystem::directory_entry entry) {
                    if(entry.path().extension() == project.profile.fileExtensions.objectFileExtension) {
                        paths.push_back(entry.path());
                    }
                });

		// Delete Cache
		::util::iterateDirectory(source.cacheDir, [&](std::filesystem::directory_entry entry) {
		    if(entry.path().extension() == ".cache") {
		    	paths.push_back(entry.path());
		    }
		});
            });

            if(project.type == solution::ProjectType::PT_EXECUTABLE) {
                std::stringstream cb;
                cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension;
                paths.push_back(std::filesystem::path(cb.str()));
            } else if(project.type == solution::ProjectType::PT_STATIC_LIB) {
                std::stringstream cb;
                cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                //cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension;
                paths.push_back(std::filesystem::path(cb.str()));
            } else if(project.type == solution::ProjectType::PT_SHARED_LIB) {
                std::stringstream cb;
                cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension;
                paths.push_back(std::filesystem::path(cb.str()));
            } else if(project.type == solution::ProjectType::PT_SHARED_DLL_LIB) {
                std::stringstream cb;
                cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                //cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension;
                paths.push_back(std::filesystem::path(cb.str()));
                cb.str("");
                cb << project.binDir << project.name << project.profile.fileExtensions.sharedLibExtension;
                paths.push_back(std::filesystem::path(cb.str()));
            }
        }

        int cleanOperation(std::string projectName, solution::Solution& solution, bool useMT) {
            std::vector<std::filesystem::path> paths;

            if(!projectName.empty()) {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
                    if(projectName == project.name) {
                        handleCleanOperations(project, paths);
                    }
                });
            } else {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
                    handleCleanOperations(project, paths);
                });
            }

            // Remove files
            std::for_each(paths.begin(), paths.end(), [&](std::filesystem::path p) {
                std::cout << "Removing: " << p.string() << "\n";
                std::filesystem::remove(p);
            });

            return 0;
        }

    }
}
