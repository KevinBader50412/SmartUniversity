#ifndef COURSE_H
#define COURSE_H

#include <string>
#include "json.hpp"

using json = nlohmann::ordered_json;

class Course {
public:
    Course() = default;
    Course(const std::string& code, const std::string& title, int credits);

    std::string getCode() const { return code; }
    std::string getTitle() const { return title; }
    int getCredits() const { return credits; }

    void setTitle(const std::string& t) { title = t; }
    void setCredits(int c) { credits = c; }

    json toJson() const;
    static Course fromJson(const json& j);

private:
    std::string code;
    std::string title;
    int credits = 0;
};

#endif
