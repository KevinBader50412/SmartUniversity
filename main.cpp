#include <iostream>
#include <string>
#include "Scheduler.h"

using namespace std;

string ask(const string& label) {
    cout << label;
    string s;
    getline(cin, s);
    return s;
}

int askInt(const string& label) {
    string s = ask(label);
    try {
        return stoi(s);
    } catch (...) {
        return 0;
    }
}

void done(bool ok) {
    cout << (ok ? "Done.\n" : "Operation failed (check the IDs).\n");
}

void studentMenu(Scheduler& app) {
    cout << "\n1. Add  2. View  3. Modify  4. Delete\n";
    int c = askInt("> ");
    if (c == 1) {
        string id = ask("ID: "), name = ask("Name: "), major = ask("Major: ");
        done(app.addStudent(Student(id, name, major)));
    } else if (c == 2) {
        app.listStudents();
    } else if (c == 3) {
        string id = ask("ID: ");
        string name = ask("New name (empty to keep): "), major = ask("New major (empty to keep): ");
        done(app.updateStudent(id, name, major));
    } else if (c == 4) {
        done(app.removeStudent(ask("ID: ")));
    }
}

void courseMenu(Scheduler& app) {
    cout << "\n1. Add  2. View  3. Modify  4. Delete  5. Register student  6. Drop student  7. View registrations\n";
    int c = askInt("> ");
    if (c == 1) {
        string code = ask("Code: "), title = ask("Title: ");
        int credits = askInt("Credits: ");
        done(app.addCourse(Course(code, title, credits)));
    } else if (c == 2) {
        app.listCourses();
    } else if (c == 3) {
        string code = ask("Code: "), title = ask("New title (empty to keep): ");
        int credits = askInt("New credits (0 to keep): ");
        done(app.updateCourse(code, title, credits));
    } else if (c == 4) {
        done(app.removeCourse(ask("Code: ")));
    } else if (c == 5) {
        string id = ask("Student ID: "), code = ask("Course code: ");
        done(app.registerStudent(id, code));
    } else if (c == 6) {
        string id = ask("Student ID: "), code = ask("Course code: ");
        done(app.dropStudent(id, code));
    } else if (c == 7) {
        app.listRegistrations();
    }
}

void examMenu(Scheduler& app) {
    cout << "\n1. Create  2. View  3. Modify  4. Delete\n";
    int c = askInt("> ");
    if (c == 1) {
        string id = ask("Exam ID: "), course = ask("Course code: ");
        int minutes = askInt("Duration (minutes): ");
        string official = ask("University date/time (optional): ");
        done(app.addExam(Exam(id, course, minutes > 0 ? minutes : 120, official)));
    } else if (c == 2) {
        app.listExams();
    } else if (c == 3) {
        string id = ask("Exam ID: "), course = ask("New course (empty to keep): ");
        int minutes = askInt("New duration (0 to keep): ");
        string official = ask("New university time (empty to keep): ");
        done(app.updateExam(id, course, minutes, official));
    } else if (c == 4) {
        done(app.removeExam(ask("Exam ID: ")));
    }
}

void scheduleMenu(Scheduler& app) {
    cout << "\n1. As entered  2. Largest degree first  3. Smallest degree first  4. Random\n";
    int c = askInt("> ");
    if (c < 1 || c > 4) return;
    int slots = app.schedule(static_cast<Ordering>(c - 1));
    cout << "Exams scheduled in " << slots << " time slot(s).\n";
}

int main() {
    Scheduler app("data");
    app.load();

    while (true) {
        cout << "\n===== Smart University Exam Scheduler =====\n"
             << "1. Students\n"
             << "2. Courses & registrations\n"
             << "3. Exams\n"
             << "4. Conflict report\n"
             << "5. Generate timetable\n"
             << "6. View timetable\n"
             << "7. Student timetable\n"
             << "8. Compare orderings\n"
             << "9. Compare with university schedule\n"
             << "10. Save\n"
             << "0. Save and exit\n";

        int choice = askInt("> ");
        cout << "\n";

        switch (choice) {
            case 1: studentMenu(app); break;
            case 2: courseMenu(app); break;
            case 3: examMenu(app); break;
            case 4: app.conflictReport(); break;
            case 5: scheduleMenu(app); break;
            case 6: app.showTimetable(); break;
            case 7: app.showStudentTimetable(ask("Student ID: ")); break;
            case 8: app.compareOrderings(); break;
            case 9: app.compareWithOfficial(); break;
            case 10: app.save(); cout << "Saved.\n"; break;
            case 0: app.save(); cout << "Saved. Bye!\n"; return 0;
            default: cout << "Invalid choice.\n";
        }

        if (!cin) break;
    }

    app.save();
    return 0;
}
