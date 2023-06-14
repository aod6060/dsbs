#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <random>
#include <chrono>
#include <filesystem>

#include <cstdio>

#include <json/json.h>


const std::string br_type = "bref";
const uint32_t br_version = 1;

const std::string b_type = "builder";
const uint32_t b_version = 1;


struct BuilderRef {
    std::string type;
    uint32_t version;
    std::vector<std::string> ref;
};

struct Builder {
    std::string type;
    uint32_t version;
    std::string name;
    std::string compiler;
    std::string bin_dir;
    std::string src_dir;
    std::string obj_dir;
    std::string extensions;
    std::vector<std::string> include_dirs;
    std::vector<std::string> lib_dirs;
    std::vector<std::string> libs;
    std::vector<std::string> options;
    std::vector<std::string> source_files;
};

//void use_builder_file()

void use_builder(std::string type, std::string fileName);
void use_builder_reference(std::string type);

int main(int argc, char** argv) {
    if(argc == 3) {
        // d_build [build, clean] [builder-file-name]

        std::string type = argv[1];
        std::string fileName = argv[2];

        use_builder(type, fileName);

    } else if(argc == 2) {
        // d_build [build, clean] 
        // This will load the all.bref.json file
        // instead of the builder file. The it will
        // go through its self 

        std::string type = argv[1];

        use_builder_reference(type);

    } else {
        std::cout << "d_build [build, rebuild, clean]" << "\n";
        std::cout << "d_build [build, rebuild, clean] [fileName]" << "\n";
    }

    return 0;
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

void json_value_iterator(Json::Value value, std::function<void(Json::Value)> cb)
{
    for (uint32_t i = 0; i < value.size(); i++)
    {
        cb(value[i]);
    }
}

void directory_filter(std::filesystem::path path, std::string ext, std::function<void(std::string)> cb) {
    for(std::filesystem::directory_entry e : std::filesystem::directory_iterator{path}) {
        std::filesystem::path p = e.path();
        if(p.extension().string() == ext) {
            cb(p.filename().string());
        }
    }
}

void directory_filter_path(std::filesystem::path path, std::string ext, std::function<void(std::filesystem::path)> cb) {
    for(std::filesystem::directory_entry e : std::filesystem::directory_iterator{path}) {
        std::filesystem::path p = e.path();
        if(p.extension().string() == ext) {
            cb(p);
        }
    }
}

void string_split(std::string str, char delim, std::function<void(std::string)> cb) {
    std::stringstream ss(str);
    std::string temp;

    while(std::getline(ss, temp, delim)) {
        cb(temp);
    }
}

bool load_builder(Builder* builder, std::string fileName) {
    std::ifstream in(fileName);

    if(!in.is_open()) {
        return false;
    }

    Json::Value root;
    in >> root;
    in.close();

    builder->type = root["type"].asString();
    builder->version = root["version"].asUInt();

    if(builder->type != b_type) {
        std::cout << "Error: builder file has the wrong type will try to load." << "\n";
    }

    if(builder->version != b_version) {
        std::cout << "Error: builder file has the wrong version will try to load." << "\n";
    }

    builder->name = root["name"].asString();
    builder->compiler = root["compiler"].asString();
    builder->bin_dir = root["bin-dir"].asString();
    builder->src_dir = root["src-dir"].asString();
    builder->obj_dir = root["obj-dir"].asString();
    builder->extensions = root["extensions"].asString();

    json_value_iterator(root["include-dirs"], [&](Json::Value v) {
        builder->include_dirs.push_back(v.asString());
    });

    json_value_iterator(root["lib-dirs"], [&](Json::Value v) {
        builder->lib_dirs.push_back(v.asString());
    });

    json_value_iterator(root["libs"], [&](Json::Value v) {
        builder->libs.push_back(v.asString());
    });

    json_value_iterator(root["options"], [&](Json::Value v) {
        builder->options.push_back(v.asString());
    });

    json_value_iterator(root["source-files"], [&](Json::Value v) {
        builder->source_files.push_back(v.asString());
    });

    return true;
}

std::string convertToString(std::string str, std::vector<std::string>& lines, std::string e) {
    std::stringstream ss;
    std::for_each(lines.begin(), lines.end(), [&](std::string s) {
        ss << str << s << e;
    });
    return ss.str();
}

void build_builder_link(Builder* builder, std::vector<std::string>& obj_files) {
    std::string libDir = convertToString("-L", builder->lib_dirs, " ");
    std::string libs = convertToString("-l", builder->libs, " ");
    std::string objs = convertToString(builder->obj_dir + "/", obj_files, " ");

    std::stringstream ss;

    ss << builder->compiler << " " << objs << " -o " << builder->bin_dir << "/" << builder->name << " " << libDir << libs;

    std::cout << exec(ss.str()) << std::endl;

    std::cout << ss.str() << std::endl;
}

void build_builder(Builder* builder) {
    // Compile Source to Object files
    std::stringstream idss;

    // Include Paths
    std::for_each(builder->include_dirs.begin(), builder->include_dirs.end(), [&](std::string ip) {
        idss << "-I" << ip << " ";
    });

    std::stringstream option_ss;
    std::for_each(builder->options.begin(), builder->options.end(), [&](std::string o) {
        option_ss << o << " ";
    });

    std::vector<std::string> obj_files;

    // Create Obj_files
    std::for_each(builder->source_files.begin(), builder->source_files.end(), [&](std::string sf) {
        std::vector<std::string> args;

        string_split(sf, '.', [&](std::string arg) {
            args.push_back(arg);
        });

        std::stringstream temp;

        temp << args[0] << ".o";

        obj_files.push_back(temp.str());
    });

    // g++ -c $(source) -o $(obj) $(includes)
    for(uint32_t i = 0; i < builder->source_files.size(); i++) {
        std::string src = builder->source_files[i];
        std::string obj = obj_files[i];

        std::stringstream ss;

        ss << builder->compiler << " "
           << "-c " << builder->src_dir << "/" << src << " "
           << "-o " << builder->obj_dir << "/" << obj << " "
           << idss.str() << " "
           << option_ss.str();

        std::cout << exec(ss.str()) << std::endl;

        std::cout << ss.str() << "\n";
    }

    build_builder_link(builder, obj_files);
}

void clean_builder(Builder* builder) {
    directory_filter_path(std::filesystem::path(builder->obj_dir), ".o", [&](std::filesystem::path fileName) {
        std::cout << "Removing " << fileName.string() << "\n";
        std::filesystem::remove(fileName);
    });

    // Remove binary
    std::stringstream bb;
    bb << builder->bin_dir << "/" << builder->name;
    
    std::filesystem::path bbp(bb.str());

    std::cout << "Removing " << bbp.string() << "\n";
    std::filesystem::remove(bbp);
}

void use_builder(std::string type, std::string fileName) {
    Builder builder;

    load_builder(&builder, fileName);

    if(type == "build") {
        build_builder(&builder);
    } else if(type == "clean") {
        clean_builder(&builder);
    }
}

bool load_build_reference(BuilderRef* ref) {
    std::ifstream in("all.bref.json");

    if(!in.is_open()) {
        return false;
    }

    Json::Value root;
    in >> root;
    in.close();

    ref->type = root["type"].asString();
    ref->version = root["version"].asUInt();

    if(ref->type != br_type) {
        std::cout << "Error: bref file wrong type will try to load" << "\n";
    }

    if(ref->version != br_version) {
        std::cout << "Error: bref file wrong version will try to load" << "\n";
    }

    json_value_iterator(root["ref"], [&](Json::Value v) {
        ref->ref.push_back(v.asString());
    });

    return true;
}

void run_build_reference(BuilderRef* ref, std::string type) {
    std::for_each(ref->ref.begin(), ref->ref.end(), [&](std::string file) {
        std::stringstream ss;
        ss << "./d_builder " << type << " " << file;
        std::string cmd = ss.str();
        std::cout << exec(cmd) << "\n";
    });
}

void use_builder_reference(std::string type) {
    BuilderRef ref;
    if(!load_build_reference(&ref)) {
        std::cout << "Error: all.bref.json doesn't exist... Please run d_solution to create the solution." << "\n";
        return;
    }
    run_build_reference(&ref, type);
}