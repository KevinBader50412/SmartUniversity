#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <set>
#include <string>
#include <vector>

class Graph {
public:
    void addVertex(const std::string& v);
    void addEdge(const std::string& a, const std::string& b);
    void clear();

    bool hasEdge(const std::string& a, const std::string& b) const;
    int degree(const std::string& v) const;
    std::vector<std::string> vertices() const;
    const std::set<std::string>& neighbors(const std::string& v) const;
    int edgeCount() const;

    std::map<std::string, int> greedyColoring(const std::vector<std::string>& order) const;

private:
    std::map<std::string, std::set<std::string>> adj;
};

#endif
