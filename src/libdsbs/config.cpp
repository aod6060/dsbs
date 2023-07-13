#include <dsbs/dsbs_internal.hpp>


namespace dsbs {

    static std::string dsbsVersion;
    static int solutionFileVersion;
    static int profileFileVersion;

    void initConfig() {
        ::util::loadStream("./dsbs-config/config/config.json", [&](std::ifstream& in) {
            Json::Value root;

            in >> root;

            dsbsVersion = root["dsbs-version"].asString();
            solutionFileVersion = root["solution-file-version"].asInt();
            profileFileVersion = root["profile-file-version"].asInt();
        });
    }

    std::string getVersion() {
        return dsbsVersion;
    }

    int getSolutionFileVersion() {
        return solutionFileVersion;
    }

    int getProfileFileVersion() {
        return profileFileVersion;
    }

}