#ifndef __CONFIG_DATA_H__
#define __CONFIG_DATA_H__

#include "ini_file.h"

class ConfigData {
public:
    static bool Init(const std::string &file) {
        return Inst().ini_file_.Init(file);
    }
    template <typename T>
    static T Get(const std::string &section, const std::string &key, T value) {
        return Inst().ini_file_.Get(section, key, value);
    }
    static std::string Get(const std::string &section, const std::string &key, const char *value) {
        return Inst().ini_file_.Get(section, key, std::string(value));
    }
    static const std::string &ErrorStr() { return Inst().ini_file_.ErrorStr(); }
private:
    static ConfigData &Inst() {
        static ConfigData instance;
        return instance;
    }
    ConfigData() {}
    ~ConfigData() {}
private:
    IniFile ini_file_;
};

#endif

