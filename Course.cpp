#include "Course.h"

Course::Course(const std::string& code, const std::string& title, int credits)
    : code(code), title(title), credits(credits) {}

json Course::toJson() const {
    return {{"code", code}, {"title", title}, {"credits", credits}};
}

Course Course::fromJson(const json& j) {
    return Course(j.at("code"), j.at("title"), j.value("credits", 0));
}
