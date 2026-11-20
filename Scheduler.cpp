#include "Scheduler.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

Scheduler::Scheduler(const string& dataDir) : dir(dataDir) {}

static json readFile(const string& path) {
    ifstream in(path);
    if (!in) return json::array();
    try {
        json j;
        in >> j;
        return j;
    } catch (...) {
        cout << "Could not read " << path << "\n";
        return json::array();
    }
}

static void writeFile(const string& path, const json& j) {
    ofstream out(path);
    out << setw(4) << j << endl;
}

void Scheduler::load() {
    students.clear();
    courses.clear();
    exams.clear();
    registrations.clear();

    for (const auto& j : readFile(dir + "/students.json")) students.push_back(Student::fromJson(j));
    for (const auto& j : readFile(dir + "/courses.json")) courses.push_back(Course::fromJson(j));
    for (const auto& j : readFile(dir + "/exams.json")) exams.push_back(Exam::fromJson(j));
    for (const auto& j : readFile(dir + "/registrations.json"))
        registrations[j.at("student")].insert(j.at("course").get<string>());

    slotsUsed = 0;
    for (const auto& e : exams) slotsUsed = max(slotsUsed, e.getSlot() + 1);

    json t = readFile(dir + "/timetable.json");
    if (t.is_object()) orderName = t.value("ordering", "none");
}

void Scheduler::save() const {
    json s = json::array(), c = json::array(), e = json::array(), r = json::array();

    for (const auto& st : students) s.push_back(st.toJson());
    for (const auto& co : courses) c.push_back(co.toJson());
    for (const auto& ex : exams) e.push_back(ex.toJson());
    for (const auto& [id, list] : registrations)
        for (const auto& code : list) r.push_back({{"student", id}, {"course", code}});

    writeFile(dir + "/students.json", s);
    writeFile(dir + "/courses.json", c);
    writeFile(dir + "/exams.json", e);
    writeFile(dir + "/registrations.json", r);

    json t;
    t["ordering"] = orderName;
    t["totalSlots"] = slotsUsed;
    t["slots"] = json::array();
    for (int i = 0; i < slotsUsed; i++) {
        json slot = {{"slot", i + 1}, {"time", slotLabel(i)}, {"exams", json::array()}};
        for (const auto& ex : exams)
            if (ex.getSlot() == i) slot["exams"].push_back({{"id", ex.getId()}, {"course", ex.getCourseCode()}});
        t["slots"].push_back(slot);
    }
    writeFile(dir + "/timetable.json", t);
}

bool Scheduler::addStudent(const Student& s) {
    if (findStudent(s.getId())) return false;
    students.push_back(s);
    return true;
}

bool Scheduler::updateStudent(const string& id, const string& name, const string& major) {
    Student* s = findStudent(id);
    if (!s) return false;
    if (!name.empty()) s->setName(name);
    if (!major.empty()) s->setMajor(major);
    return true;
}

bool Scheduler::removeStudent(const string& id) {
    auto it = find_if(students.begin(), students.end(), [&](const Student& s) { return s.getId() == id; });
    if (it == students.end()) return false;
    students.erase(it);
    registrations.erase(id);
    return true;
}

Student* Scheduler::findStudent(const string& id) {
    for (auto& s : students)
        if (s.getId() == id) return &s;
    return nullptr;
}

void Scheduler::listStudents() const {
    if (students.empty()) {
        cout << "No students yet.\n";
        return;
    }
    cout << left << setw(10) << "ID" << setw(25) << "Name" << "Major\n";
    for (const auto& s : students)
        cout << setw(10) << s.getId() << setw(25) << s.getName() << s.getMajor() << "\n";
}

bool Scheduler::addCourse(const Course& c) {
    for (const auto& co : courses)
        if (co.getCode() == c.getCode()) return false;
    courses.push_back(c);
    return true;
}

bool Scheduler::updateCourse(const string& code, const string& title, int credits) {
    for (auto& c : courses) {
        if (c.getCode() != code) continue;
        if (!title.empty()) c.setTitle(title);
        if (credits > 0) c.setCredits(credits);
        return true;
    }
    return false;
}

bool Scheduler::removeCourse(const string& code) {
    auto it = find_if(courses.begin(), courses.end(), [&](const Course& c) { return c.getCode() == code; });
    if (it == courses.end()) return false;
    courses.erase(it);
    for (auto& [id, list] : registrations) list.erase(code);
    exams.erase(remove_if(exams.begin(), exams.end(), [&](const Exam& e) { return e.getCourseCode() == code; }),
                exams.end());
    return true;
}

void Scheduler::listCourses() const {
    if (courses.empty()) {
        cout << "No courses yet.\n";
        return;
    }
    cout << left << setw(10) << "Code" << setw(40) << "Title" << setw(9) << "Credits" << "Students\n";
    for (const auto& c : courses)
        cout << setw(10) << c.getCode() << setw(40) << c.getTitle() << setw(9) << c.getCredits()
             << studentsInCourse(c.getCode()).size() << "\n";
}

bool Scheduler::registerStudent(const string& studentId, const string& courseCode) {
    bool courseExists = any_of(courses.begin(), courses.end(),
                               [&](const Course& c) { return c.getCode() == courseCode; });
    if (!findStudent(studentId) || !courseExists) return false;
    return registrations[studentId].insert(courseCode).second;
}

bool Scheduler::dropStudent(const string& studentId, const string& courseCode) {
    auto it = registrations.find(studentId);
    return it != registrations.end() && it->second.erase(courseCode) > 0;
}

void Scheduler::listRegistrations() const {
    for (const auto& s : students) {
        cout << s.getName() << " (" << s.getId() << "): ";
        auto it = registrations.find(s.getId());
        if (it == registrations.end() || it->second.empty()) {
            cout << "-\n";
            continue;
        }
        for (const auto& code : it->second) cout << code << " ";
        cout << "\n";
    }
}

bool Scheduler::addExam(const Exam& e) {
    if (findExam(e.getId())) return false;
    exams.push_back(e);
    return true;
}

bool Scheduler::updateExam(const string& id, const string& course, int duration, const string& official) {
    Exam* e = findExam(id);
    if (!e) return false;
    if (!course.empty()) e->setCourseCode(course);
    if (duration > 0) e->setDuration(duration);
    if (!official.empty()) e->setOfficialTime(official);
    return true;
}

bool Scheduler::removeExam(const string& id) {
    auto it = find_if(exams.begin(), exams.end(), [&](const Exam& e) { return e.getId() == id; });
    if (it == exams.end()) return false;
    exams.erase(it);
    return true;
}

void Scheduler::listExams() const {
    if (exams.empty()) {
        cout << "No exams yet.\n";
        return;
    }
    cout << left << setw(8) << "ID" << setw(10) << "Course" << setw(10) << "Minutes"
         << setw(22) << "Official time" << "Slot\n";
    for (const auto& e : exams)
        cout << setw(8) << e.getId() << setw(10) << e.getCourseCode() << setw(10) << e.getDuration()
             << setw(22) << (e.getOfficialTime().empty() ? "-" : e.getOfficialTime())
             << (e.getSlot() < 0 ? "-" : to_string(e.getSlot() + 1)) << "\n";
}

void Scheduler::buildConflictGraph() {
    graph.clear();
    for (const auto& e : exams) graph.addVertex(e.getId());

    for (size_t i = 0; i < exams.size(); i++)
        for (size_t j = i + 1; j < exams.size(); j++)
            if (!sharedStudents(exams[i], exams[j]).empty())
                graph.addEdge(exams[i].getId(), exams[j].getId());
}

int Scheduler::schedule(Ordering order) {
    buildConflictGraph();
    auto colors = graph.greedyColoring(makeOrder(order));

    slotsUsed = 0;
    for (auto& e : exams) {
        e.setSlot(colors[e.getId()]);
        slotsUsed = max(slotsUsed, e.getSlot() + 1);
    }
    orderName = orderingName(order);
    return slotsUsed;
}

void Scheduler::compareOrderings() {
    buildConflictGraph();
    vector<Ordering> all = {Ordering::AsEntered, Ordering::LargestDegree, Ordering::SmallestDegree, Ordering::Random};

    cout << left << setw(22) << "Ordering" << setw(8) << "Slots" << "Order used\n";
    for (auto o : all) {
        auto order = makeOrder(o);
        auto colors = graph.greedyColoring(order);
        int used = 0;
        for (const auto& [v, c] : colors) used = max(used, c + 1);

        cout << setw(22) << orderingName(o) << setw(8) << used;
        for (const auto& v : order) cout << v << " ";
        cout << "\n";
    }
}

void Scheduler::showTimetable() const {
    if (slotsUsed == 0) {
        cout << "No timetable yet. Run the scheduler first.\n";
        return;
    }
    cout << "Ordering: " << orderName << "\n\n";
    for (int i = 0; i < slotsUsed; i++) {
        cout << "Slot " << i + 1 << "  [" << slotLabel(i) << "]\n";
        for (const auto& e : exams)
            if (e.getSlot() == i)
                cout << "   " << e.getId() << "  " << e.getCourseCode() << " - " << courseTitle(e.getCourseCode()) << "\n";
    }
    cout << "\nTotal time slots needed: " << slotsUsed << "\n";
}

void Scheduler::showStudentTimetable(const string& studentId) const {
    auto it = registrations.find(studentId);
    if (it == registrations.end()) {
        cout << "No registrations for this student.\n";
        return;
    }

    vector<Exam> mine;
    for (const auto& e : exams)
        if (it->second.count(e.getCourseCode())) mine.push_back(e);
    sort(mine.begin(), mine.end(), [](const Exam& a, const Exam& b) { return a.getSlot() < b.getSlot(); });

    for (const auto& e : mine)
        cout << left << setw(20) << (e.getSlot() < 0 ? "not scheduled" : slotLabel(e.getSlot()))
             << e.getCourseCode() << " - " << courseTitle(e.getCourseCode()) << "\n";
}

void Scheduler::conflictReport() {
    buildConflictGraph();
    int count = 0;

    for (size_t i = 0; i < exams.size(); i++) {
        for (size_t j = i + 1; j < exams.size(); j++) {
            auto shared = sharedStudents(exams[i], exams[j]);
            if (shared.empty()) continue;
            count++;
            cout << exams[i].getId() << " (" << exams[i].getCourseCode() << ") <-> "
                 << exams[j].getId() << " (" << exams[j].getCourseCode() << ")  shared: ";
            for (const auto& s : shared) cout << s << " ";
            cout << "\n";
        }
    }

    cout << "\nExams: " << exams.size() << "   Conflicts: " << count << "\n";
    for (const auto& e : exams)
        cout << "  " << e.getId() << " conflicts with " << graph.degree(e.getId()) << " exam(s)\n";
}

void Scheduler::compareWithOfficial() {
    buildConflictGraph();
    set<string> officialTimes;

    cout << left << setw(8) << "Exam" << setw(10) << "Course" << setw(22) << "University" << "Generated\n";
    for (const auto& e : exams) {
        if (!e.getOfficialTime().empty()) officialTimes.insert(e.getOfficialTime());
        cout << setw(8) << e.getId() << setw(10) << e.getCourseCode()
             << setw(22) << (e.getOfficialTime().empty() ? "-" : e.getOfficialTime())
             << (e.getSlot() < 0 ? "-" : slotLabel(e.getSlot())) << "\n";
    }

    int clashes = 0;
    for (size_t i = 0; i < exams.size(); i++)
        for (size_t j = i + 1; j < exams.size(); j++)
            if (!exams[i].getOfficialTime().empty() &&
                exams[i].getOfficialTime() == exams[j].getOfficialTime() &&
                graph.hasEdge(exams[i].getId(), exams[j].getId())) {
                clashes++;
                cout << "Clash in university schedule: " << exams[i].getCourseCode()
                     << " and " << exams[j].getCourseCode() << " at " << exams[i].getOfficialTime() << "\n";
            }

    cout << "\nUniversity schedule uses " << officialTimes.size() << " time slots with " << clashes << " clash(es).\n";
    if (slotsUsed > 0)
        cout << "Generated schedule uses " << slotsUsed << " time slots with 0 clashes.\n";
    else
        cout << "Generate a timetable first to compare.\n";
}

set<string> Scheduler::studentsInCourse(const string& code) const {
    set<string> result;
    for (const auto& [id, list] : registrations)
        if (list.count(code)) result.insert(id);
    return result;
}

vector<string> Scheduler::sharedStudents(const Exam& a, const Exam& b) const {
    auto sa = studentsInCourse(a.getCourseCode());
    auto sb = studentsInCourse(b.getCourseCode());
    vector<string> result;
    set_intersection(sa.begin(), sa.end(), sb.begin(), sb.end(), back_inserter(result));
    return result;
}

vector<string> Scheduler::makeOrder(Ordering order) const {
    vector<string> ids;
    for (const auto& e : exams) ids.push_back(e.getId());

    if (order == Ordering::LargestDegree)
        stable_sort(ids.begin(), ids.end(), [&](const string& a, const string& b) { return graph.degree(a) > graph.degree(b); });
    else if (order == Ordering::SmallestDegree)
        stable_sort(ids.begin(), ids.end(), [&](const string& a, const string& b) { return graph.degree(a) < graph.degree(b); });
    else if (order == Ordering::Random)
        shuffle(ids.begin(), ids.end(), mt19937(random_device{}()));

    return ids;
}

Exam* Scheduler::findExam(const string& id) {
    for (auto& e : exams)
        if (e.getId() == id) return &e;
    return nullptr;
}

string Scheduler::courseTitle(const string& code) const {
    for (const auto& c : courses)
        if (c.getCode() == code) return c.getTitle();
    return "?";
}

string Scheduler::slotLabel(int slot) {
    static const vector<string> times = {"08:00", "11:00", "14:00"};
    return "Day " + to_string(slot / 3 + 1) + " " + times[slot % 3];
}

string Scheduler::orderingName(Ordering order) {
    switch (order) {
        case Ordering::LargestDegree: return "Largest degree first";
        case Ordering::SmallestDegree: return "Smallest degree first";
        case Ordering::Random: return "Random";
        default: return "As entered";
    }
}
