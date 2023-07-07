#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        int cleanOperation(std::string projectName, solution::Solution& solution) {
            std::vector<std::filesystem::path> paths;

            if(!projectName.empty()) {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {

                    if(projectName == project.name) {
                        // Scan through objects
                        std::for_each(project.sources.begin(), project.sources.end(), [&](solution::Source& source) {
                            ::util::iterateDirectory(source.objDir, [&](std::filesystem::directory_entry entry) {
                                if(entry.path().extension() == project.profile.fileExtensions.objectFileExtension) {
                                    paths.push_back(entry.path());
                                }
                            });
                        });

                        if(project.type == solution::ProjectType::PT_EXECUTABLE) {
                            std::stringstream cb;
                            cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension;
                            paths.push_back(std::filesystem::path(cb.str()));

                        } else if(project.type == solution::ProjectType::PT_SHARED_LIB || project.type == solution::ProjectType::PT_STATIC_LIB) {
                            std::stringstream cb;
                            cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                            paths.push_back(std::filesystem::path(cb.str()));
                        }

                    }
                });
            } else {
                std::for_each(solution.projects.begin(), solution.projects.end(), [&](solution::Project& project) {
                    // Scan through objects
                    std::for_each(project.sources.begin(), project.sources.end(), [&](solution::Source& source) {
                        ::util::iterateDirectory(source.objDir, [&](std::filesystem::directory_entry entry) {
                            if(entry.path().extension() == project.profile.fileExtensions.objectFileExtension) {
                                paths.push_back(entry.path());
                            }
                        });
                    });

                    if(project.type == solution::ProjectType::PT_EXECUTABLE) {
                        std::stringstream cb;
                        cb << project.binDir << project.name << project.profile.fileExtensions.binaryFileExtension;
                        paths.push_back(std::filesystem::path(cb.str()));

                    } else if(project.type == solution::ProjectType::PT_SHARED_LIB || project.type == solution::ProjectType::PT_STATIC_LIB) {
                        std::stringstream cb;
                        cb << project.binDir << project.name << project.profile.fileExtensions.staticLibExtension;
                        paths.push_back(std::filesystem::path(cb.str()));
                    }
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