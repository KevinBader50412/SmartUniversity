#include "Student.h"

Student::Student(const std::string& id, const std::string& name, const std::string& major)
    : id(id), name(name), major(major) {}

json Student::toJson() const {
    return {{"id", id}, {"name", name}, {"major", major}};
}

Student Student::fromJson(const json& j) {
    return Student(j.at("id"), j.at("name"), j.value("major", ""));
}
