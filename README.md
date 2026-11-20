# Smart University Exam Scheduler

C++ console application that builds a university exam timetable with no conflicts using greedy graph coloring.

Each exam is a vertex in a graph. Two exams are connected when at least one student is registered in both courses, so they can't happen at the same time. The greedy algorithm then gives every exam the smallest time slot not used by its neighbours.

## Features

- Add, view, modify and delete students, courses and exams
- Register / drop students from courses
- Conflict detection and conflict report (shared students per pair of exams)
- Timetable generation with greedy graph coloring
- Four exam orderings (as entered, largest degree first, smallest degree first, random) and a comparison of how many slots each one needs
- Personal timetable for each student
- Comparison between the generated timetable and the university's official schedule
- All data stored in JSON files inside `data/`

## Build and run

```
g++ -std=c++17 -O2 *.cpp -o scheduler
./scheduler
```

Run it from the project folder so it finds the `data/` directory.

## Project structure

| File | Role |
|------|------|
| `Student` | student information |
| `Course` | course information |
| `Exam` | exam info and the assigned slot |
| `Graph` | adjacency list and greedy coloring |
| `Scheduler` | manages everything, conflicts, reports and JSON storage |
| `main.cpp` | console menu |
| `json.hpp` | [nlohmann/json](https://github.com/nlohmann/json) library |

## Data files

`students.json`, `courses.json`, `registrations.json`, `exams.json`, `timetable.json`
