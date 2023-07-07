#include <dsbs/dsbs_internal.hpp>


namespace util {
    void loadStream(std::string path, std::function<void(std::ifstream&)> cb) {
        std::ifstream in(path);
        if(in.is_open()) {
            cb(in);
            in.close();
        }
    }

    void saveStream(std::string path, std::function<void(std::ofstream&)> cb) {
        std::ofstream out(path);
        cb(out);
        out.close();
    }

    void jsonIterator(Json::Value value, std::function<void(Json::Value)> cb) {
        for(int i = 0; i < value.size(); i++) {
            cb(value[i]);
        }
    }

    void jsonIteratorString(Json::Value value, std::function<void(std::string str)> cb) {
        for(int i = 0; i < value.size(); i++) {
            cb(value[i].asString());
        }
    }

    dsbs::solution::ProjectType toType(std::string type) {
        if(type == "executable") {
            return dsbs::solution::ProjectType::PT_EXECUTABLE;
        }
        else if(type == "static") {
            return dsbs::solution::ProjectType::PT_STATIC_LIB;
        }
        else if(type == "shared") {
            return dsbs::solution::ProjectType::PT_SHARED_LIB;
        }
        else {
            return dsbs::solution::ProjectType::PT_UNKNOWN;
        }
    }

    void iterateDirectory(std::filesystem::path path, std::function<void(std::filesystem::directory_entry)> cb) {
        for(const std::filesystem::directory_entry& e : std::filesystem::directory_iterator{path}) {
            cb(e);
        }
    }

    // I'll keep this and port it to windows
    // https://www.tutorialspoint.com/How-to-execute-a-command-and-get-the-output-of-command-within-Cplusplus-using-POSIX
    std::string exec(std::string cmd)
    {
        char buffer[128];
        std::string result;

        // windows _popen
        FILE *pipe = popen(cmd.c_str(), "r");

        if (!pipe)
        {
            return "popen failed";
        }

        while (!feof(pipe))
        {
            if (fgets(buffer, 128, pipe) != nullptr)
            {
                result += buffer;
            }
        }

        // _pclose windows
        pclose(pipe);

        return result;
    }
}


namespace dsbs {
    namespace util {
        void strSplit(std::string str, char delim, std::function<void(std::string)> cb) {
            std::stringstream ss(str);
            std::string temp;

            while(std::getline(ss, temp, delim)) {
                cb(temp);
            }
        }

        void loadFile(std::string path, std::function<void(std::string)> cb) {
            std::ifstream in(path);
            if(in.is_open()) {
                std::string temp;
                while(std::getline(in, temp)) {
                    cb(temp);
                }
                in.close();
            }
        }

    }
}