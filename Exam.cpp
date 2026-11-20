#include "Exam.h"

Exam::Exam(const std::string& id, const std::string& courseCode, int duration,
           const std::string& officialTime)
    : id(id), courseCode(courseCode), duration(duration), officialTime(officialTime) {}

json Exam::toJson() const {
    return {{"id", id}, {"course", courseCode}, {"duration", duration},
            {"officialTime", officialTime}, {"slot", slot}};
}

Exam Exam::fromJson(const json& j) {
    Exam e(j.at("id"), j.at("course"), j.value("duration", 120), j.value("officialTime", ""));
    e.setSlot(j.value("slot", -1));
    return e;
}
