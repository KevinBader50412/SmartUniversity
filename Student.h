#ifndef STUDENT_H
#define STUDENT_H

#include <string>
#include "json.hpp"

using json = nlohmann::ordered_json;

class Student {
public:
    Student() = default;
    Student(const std::string& id, const std::string& name, const std::string& major);

    std::string getId() const { return id; }
    std::string getName() const { return name; }
    std::string getMajor() const { return major; }

    void setName(const std::string& n) { name = n; }
    void setMajor(const std::string& m) { major = m; }

    json toJson() const;
    static Student fromJson(const json& j);

private:
    std::string id;
    std::string name;
    std::string major;
};

#endif
