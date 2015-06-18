#ifndef __INIFILE_H__
#define __INIFILE_H__

#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

class IniFile {
public:
    typedef std::map<std::string, std::string> KeyType;
    typedef std::map<std::string, KeyType> SectionType;
public:
    IniFile() {}
    ~IniFile() {}
    inline bool Init(const std::string &file);
    std::string Get(const std::string &section, const std::string &key, std::string value) {
        SectionType::iterator sit = data_.find(section);
        if (sit != data_.end()) {
            KeyType::iterator kit = sit->second.find(key);
            if (kit != sit->second.end())
                value = kit->second;
        }
        return value;
    }
    int Get(const std::string &section, const std::string &key, int value) {
        std::string str_value = Get(section, key, "");
        if (str_value != "")
            value = strtoul(str_value.c_str(), NULL, 10);
        return value;
    }
    double Get(const std::string &section, const std::string &key, double value) {
        std::string str_value = Get(section, key, "");
        if (str_value != "")
            value = strtod(str_value.c_str(), NULL);
        return value;
    }
    const std::string &ErrorStr() { return error_; }
private:
    SectionType data_;    // < section, <key, value> >
    std::string error_;
};

bool IniFile::Init(const std::string &file)
{
    std::ifstream in(file.c_str());
    if (!in) {
        error_ = std::string("open file '") + file.c_str() + "' failed, " + strerror(errno) + "!";
        return false;
    }
    char c;
    std::string line, section, key, value;
    while (getline(in, line)) {
        std::istringstream is(line);
        if (is >> c) {
            if (c == '=') {
                error_ = std::string("can't be start with '=' in a line");
                return false;
            }
            if (c == '#' || c == ';')   // <commentary>
                continue;
            else if (c == '[') {        // section
                if ((is >> section) && (section[section.size() - 1] == ']')) {
                    section.erase(section.size() - 1);
                    data_[section];
                    continue;
                } else {
                    error_ = std::string("get section error, line '") + line + "'";
                    return false;
                }
            } else {
                is.putback(c);
                if (is >> key) {
                    std::string::size_type pos = key.find('=');
                    if (pos == std::string::npos) {     // key = value or key =value
                        if (is >> c >> value && c == '=') {
                            data_[section][key] = value;
                            continue;
                        }
                    } else if (pos == key.size() - 1) { // key= value
                        key.erase(key.size() - 1);
                        if (is >> value) {
                            data_[section][key] = value;
                            continue;
                        }
                    } else {                            // key=value
                        data_[section][key.substr(0, pos)] = key.substr(pos + 1);
                        continue;
                    }
                    error_ = std::string("get key-value error, line '") + line + "'";
                    return false;
                }
            }
        }
    }
    return true;
}
 
#endif
