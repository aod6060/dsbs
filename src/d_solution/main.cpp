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

const std::string g_type = "solution";
const uint32_t g_version = 1;

struct Project
{
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
};

struct Solution
{
    std::string type;
    uint32_t version;
    std::string name;
    std::vector<Project> projects;
};

bool solution_load(std::string filePath, Solution *s);

void generate_bref(Solution* s);

void generate_builder(Solution* s);

void directory_filter(std::filesystem::path path, std::string ext, std::function<void(std::string)> cb);

int main(int argc, char **argv)
{
    Solution solution;

    std::string fileName = "solution.json";
    
    if(argc == 2) {
        fileName = argv[1];
    }

    if(!solution_load(fileName, &solution)) {
        std::cout << "Error: Please create a solution.json file for your project!" << "\n";
        return -1;
    }

    // Generate all.bref.json
    generate_bref(&solution);

    // Generate Builder Files
    generate_builder(&solution);

    return 0;
}

void json_value_iterator(Json::Value value, std::function<void(Json::Value)> cb)
{
    for (uint32_t i = 0; i < value.size(); i++)
    {
        cb(value[i]);
    }
}

bool solution_load(std::string fileName, Solution *s)
{
    Json::Value root;
    std::ifstream in(fileName);

    if(!in.is_open()) {
        return false;
    }

    in >> root;
    in.close();

    std::string type = root["type"].asString();

    if (type != g_type)
    {
        std::cout << "Error: Type isn't a solution will try to load..."
                  << "\n";
    }

    s->type = type;

    uint32_t version = root["version"].asUInt();

    if (version != g_version)
    {
        std::cout << "Error: Isn't the correct version will try to load"
                  << "\n";
    }

    s->version = version;

    s->name = root["name"].asString();

    // Handle Projects
    Json::Value projects = root["projects"];

    for (uint32_t i = 0; i < projects.size(); i++)
    {
        Json::Value v = projects[i];

        Project temp;
        temp.name = v["name"].asString();
        temp.compiler = v["compiler"].asString();
        temp.bin_dir = v["bin-dir"].asString();
        temp.src_dir = v["src-dir"].asString();
        temp.obj_dir = v["obj-dir"].asString();
        temp.extensions = v["extensions"].asString();

        // Includes Dirs
        json_value_iterator(v["include-dirs"], [&](Json::Value s)
                            { temp.include_dirs.push_back(s.asString()); });

        // Lib Dirs
        // Json::Value ld = v["lib-dirs"];
        json_value_iterator(v["lib-dirs"], [&](Json::Value s)
                            { temp.lib_dirs.push_back(s.asString()); });

        // Libs
        // Json::Value ls = v["libs"];
        json_value_iterator(v["libs"], [&](Json::Value s)
                            { temp.libs.push_back(s.asString()); });

        // Options
        // Json::Value ops = v["options"];
        json_value_iterator(v["options"], [&](Json::Value s)
                            { temp.options.push_back(s.asString()); });

        s->projects.push_back(temp);
    }

    return true;
}

void generate_bref(Solution* s) {
    // Create all.bref.json
    Json::Value root;

    root["type"] = "bref";
    root["version"] = 1;

    root["ref"] = Json::Value(Json::arrayValue);
    
    for(int i = 0; i < s->projects.size(); i++) {
        std::stringstream ss;

        ss << s->projects[i].name << ".builder.json";

        std::string str = ss.str();

        root["ref"].append(Json::Value(str.c_str()));
    }

    std::ofstream out("all.bref.json");

    out << root;

    out.close();
}

void generate_builder(Solution* s) {
    // Create Builder
    std::for_each(s->projects.begin(), s->projects.end(), [&](Project& p) {
        std::stringstream ss;
        ss << p.name << ".builder.json";
        std::string fileName = ss.str();

        Json::Value root;
        root["type"] = "builder";
        root["version"] = 1;
        root["name"] = p.name;
        root["compiler"] = p.compiler;
        root["src-dir"] = p.src_dir;
        root["bin-dir"] = p.bin_dir;
        root["obj-dir"] = p.obj_dir;
        root["extensions"] = p.extensions;

        // Include Dirs
        root["include-dirs"] = Json::Value(Json::arrayValue);

        for(std::string str : p.include_dirs) {
            root["include-dirs"].append(Json::Value(str));
        }

        // Lib Dirs
        root["lib-dirs"] = Json::Value(Json::arrayValue);
        for(std::string str : p.lib_dirs) {
            root["lib-dirs"].append(Json::Value(str));
        }

        // Libs
        root["libs"] = Json::Value(Json::arrayValue);
        for(std::string str : p.libs) {
            root["libs"].append(Json::Value(str));
        }

        // Options
        root["options"] = Json::Value(Json::arrayValue);
        for(std::string str : p.options) {
            root["options"].append(Json::Value(str));
        }

        // Source Files
        root["source-files"] = Json::Value(Json::arrayValue);
        directory_filter(std::filesystem::path(p.src_dir), p.extensions, [&](std::string fileName) {
            root["source-files"].append(fileName);
        });


        std::ofstream out(fileName);
        out << root;
        out.close();
    });
}

void directory_filter(std::filesystem::path path, std::string ext, std::function<void(std::string)> cb) {
    //std::filesystem::directory_iterator(path);

    for(std::filesystem::directory_entry e : std::filesystem::directory_iterator{path}) {
        std::filesystem::path p = e.path();
        if(p.extension().string() == ext) {
            cb(p.filename().string());
        }
    }
}
