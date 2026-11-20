#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <map>
#include <set>
#include <string>
#include <vector>
#include "Student.h"
#include "Course.h"
#include "Exam.h"
#include "Graph.h"

enum class Ordering { AsEntered, LargestDegree, SmallestDegree, Random };

class Scheduler {
public:
    explicit Scheduler(const std::string& dataDir);

    void load();
    void save() const;

    bool addStudent(const Student& s);
    bool updateStudent(const std::string& id, const std::string& name, const std::string& major);
    bool removeStudent(const std::string& id);
    Student* findStudent(const std::string& id);
    void listStudents() const;

    bool addCourse(const Course& c);
    bool updateCourse(const std::string& code, const std::string& title, int credits);
    bool removeCourse(const std::string& code);
    void listCourses() const;

    bool registerStudent(const std::string& studentId, const std::string& courseCode);
    bool dropStudent(const std::string& studentId, const std::string& courseCode);
    void listRegistrations() const;

    bool addExam(const Exam& e);
    bool updateExam(const std::string& id, const std::string& course, int duration, const std::string& official);
    bool removeExam(const std::string& id);
    void listExams() const;

    void buildConflictGraph();
    int schedule(Ordering order);
    void compareOrderings();

    void showTimetable() const;
    void showStudentTimetable(const std::string& studentId) const;
    void conflictReport();
    void compareWithOfficial();

private:
    std::string dir;
    std::vector<Student> students;
    std::vector<Course> courses;
    std::vector<Exam> exams;
    std::map<std::string, std::set<std::string>> registrations;
    Graph graph;
    int slotsUsed = 0;
    std::string orderName = "none";

    std::set<std::string> studentsInCourse(const std::string& code) const;
    std::vector<std::string> sharedStudents(const Exam& a, const Exam& b) const;
    std::vector<std::string> makeOrder(Ordering order) const;
    Exam* findExam(const std::string& id);
    std::string courseTitle(const std::string& code) const;
    static std::string slotLabel(int slot);
    static std::string orderingName(Ordering order);
};

#endif
