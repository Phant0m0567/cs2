#pragma once

#include <string>

class Config {
public:
    static void Load();
    static void Save();

    static void LoadProfile(const std::string& profile_name);
    static void SaveProfile(const std::string& profile_name);
    static const std::string& GetCurrentProfile();

private:
    static bool LoadFromFile(const std::string& file_name);
    static void SaveToFile(const std::string& file_name);
    static std::string current_profile_;
};
