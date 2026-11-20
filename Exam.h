#ifndef EXAM_H
#define EXAM_H

#include <string>
#include "json.hpp"

using json = nlohmann::ordered_json;

class Exam {
public:
    Exam() = default;
    Exam(const std::string& id, const std::string& courseCode, int duration,
         const std::string& officialTime = "");

    std::string getId() const { return id; }
    std::string getCourseCode() const { return courseCode; }
    int getDuration() const { return duration; }
    std::string getOfficialTime() const { return officialTime; }
    int getSlot() const { return slot; }

    void setCourseCode(const std::string& c) { courseCode = c; }
    void setDuration(int d) { duration = d; }
    void setOfficialTime(const std::string& t) { officialTime = t; }
    void setSlot(int s) { slot = s; }

    json toJson() const;
    static Exam fromJson(const json& j);

private:
    std::string id;
    std::string courseCode;
    int duration = 120;
    std::string officialTime;
    int slot = -1;
};

#endif
