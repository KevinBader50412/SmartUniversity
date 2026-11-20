#include "Graph.h"

void Graph::addVertex(const std::string& v) {
    adj[v];
}

void Graph::addEdge(const std::string& a, const std::string& b) {
    if (a == b) return;
    adj[a].insert(b);
    adj[b].insert(a);
}

void Graph::clear() {
    adj.clear();
}

bool Graph::hasEdge(const std::string& a, const std::string& b) const {
    auto it = adj.find(a);
    return it != adj.end() && it->second.count(b);
}

int Graph::degree(const std::string& v) const {
    auto it = adj.find(v);
    return it == adj.end() ? 0 : it->second.size();
}

std::vector<std::string> Graph::vertices() const {
    std::vector<std::string> result;
    for (const auto& [v, _] : adj) result.push_back(v);
    return result;
}

const std::set<std::string>& Graph::neighbors(const std::string& v) const {
    static const std::set<std::string> empty;
    auto it = adj.find(v);
    return it == adj.end() ? empty : it->second;
}

int Graph::edgeCount() const {
    int total = 0;
    for (const auto& [v, n] : adj) total += n.size();
    return total / 2;
}

std::map<std::string, int> Graph::greedyColoring(const std::vector<std::string>& order) const {
    std::map<std::string, int> color;

    for (const auto& v : order) {
        std::set<int> used;
        for (const auto& n : neighbors(v)) {
            auto it = color.find(n);
            if (it != color.end()) used.insert(it->second);
        }

        int c = 0;
        while (used.count(c)) c++;
        color[v] = c;
    }

    return color;
}
