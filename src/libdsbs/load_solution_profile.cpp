#include <dsbs/dsbs_internal.hpp>


namespace dsbs {
    namespace internal {
        std::function<void(std::ifstream&)> loadProfile(profile::Profile& profile) {
            return [&](std::ifstream& in) {
                Json::Value root;
                in >> root;

                profile.type = root["type"].asString();
                profile.version = root["version"].asInt();

                if(profile.type != "profile") {
                    std::cout << "Error: type isn't profile will try to load" << "\n";
                }

                if(profile.version != dsbs::getProfileFileVersion()) {
                    std::cout << "Error: version isn't correct will try to load" << "\n";
                }

                // Program
                profile.programs.compiler = root["programs"]["compiler"].asString();
                profile.programs.archiver = root["programs"]["archiver"].asString();
                profile.programs.linker = root["programs"]["linker"].asString();


                // File Extensions
                // Source File Extensions
                ::util::jsonIteratorString(root["file-extensions"]["source-file-extensions"], [&](std::string ext) {
                    profile.fileExtensions.sourceFileExtensions.push_back(ext);
                });
                profile.fileExtensions.objectFileExtension = root["file-extensions"]["object-file-extension"].asString();
                profile.fileExtensions.binaryFileExtension = root["file-extensions"]["binary-file-extension"].asString();
                profile.fileExtensions.staticLibExtension = root["file-extensions"]["static-lib-extension"].asString();
                profile.fileExtensions.sharedLibExtension = root["file-extensions"]["shared-lib-extension"].asString();

                // Flags
                profile.flags.includeDirFlag = root["flags"]["include-dir-flag"].asString();
                profile.flags.libDirFlag = root["flags"]["lib-dir-flag"].asString();
                profile.flags.libFlags = root["flags"]["lib-flag"].asString();
                profile.flags.compileFlag = root["flags"]["compile-flag"].asString();
                profile.flags.objectFlag = root["flags"]["object-flag"].asString();
                profile.flags.staticLibFlags = root["flags"]["static-lib-flag"].asString();
                profile.flags.sharedLibFlags = root["flags"]["shared-lib-flag"].asString();
                profile.flags.sharedLibFlagsLinker = root["flags"]["shared-lib-flag-linker"].asString();
                profile.flags.sharedImportLibFlag = root["flags"]["shared-import-lib-flag"].asString();
            };
        }

        std::function<void(std::ifstream&)> loadSolution(solution::Solution& solution) {
            return [&](std::ifstream& in) {
                Json::Value root;
                in >> root;

                solution.type = root["type"].asString();
                solution.version = root["version"].asInt();

                if(solution.type != "solution") {
                    std::cout << "Error: type isn't solution will try to load" << "\n";
                }

                if(solution.version != dsbs::getSolutionFileVersion()) {
                    std::cout << "Error: wrong version will try to load" << "\n";
                }

                solution.name = root["name"].asString();



                ::util::jsonIterator(root["projects"], [&](Json::Value v) {

                    solution::Project project;

                    project.name = v["name"].asString();
                    project.type = ::util::toType(v["type"].asString());
                    //project.profile = v["profile"].asString();
                    // Handle Profile
                    ::util::loadStream(v["profile"].asString(), loadProfile(project.profile));


                    // Directories
                    project.binDir = v["bin-dir"].asString();

                    ::util::jsonIterator(v["sources"], [&](Json::Value v2) {
                        solution::Source source;
                        source.srcDir = v2["src-dir"].asString();
                        source.objDir = v2["obj-dir"].asString();
                        project.sources.push_back(source);
                    });

                    ::util::jsonIteratorString(v["include-dirs"], [&](std::string str) {
                        project.includeDirs.push_back(str);
                    });

                    ::util::jsonIteratorString(v["lib-dirs"], [&](std::string str) {
                        project.libDirs.push_back(str);
                    });

                    ::util::jsonIteratorString(v["libs"], [&](std::string str) {
                        project.libs.push_back(str);
                    });
                    // Options
                    ::util::jsonIteratorString(v["compiler-options"], [&](std::string str) {
                        project.compilerOptions.push_back(str);
                    });

                    ::util::jsonIteratorString(v["archiver-options"], [&](std::string str) {
                        project.archiverOptions.push_back(str);
                    });

                    ::util::jsonIteratorString(v["linker-options"], [&](std::string str) {
                        project.linkerOptions.push_back(str);
                    });

                    solution.projects.push_back(project);
                });

            };
        }
        int loadSolutionProfile(Operation op, std::string solutionFile, std::string projectName) {

            // Load Solution
            solution::Solution solution;

            ::util::loadStream(solutionFile, loadSolution(solution));

            //return buildSourceObjectList(op, projectName, solution);
            return operationsCheck(op, projectName, solution);
        }

    }
}