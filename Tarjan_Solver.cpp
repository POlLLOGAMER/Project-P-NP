#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

using Clause = vector<int>;
using Formula = vector<Clause>; 

unordered_map <int,vector<int>*> graph;
int vars = 0;

int id = 0;
unordered_map<int, int> ids;
unordered_map<int, int> low;
unordered_map<int, bool> inStack;
stack<int> s;

void implicationGraph(Formula &formula) {
    for (const auto &clause : formula){
        for (int literal : clause){
            vars = max (vars, abs(literal));
        }
    }
    for (int i = 1; i <= vars; i++) {
        if (graph.find(i) == graph.end()) graph[i] = new vector<int>();
        if (graph.find(-i) == graph.end()) graph[-i] = new vector<int>();
    }
    for (const auto &clause : formula) {
        int u = clause[0];
        int v = clause[1];
        // (u OR v)  =>  (!u -> v) AND (!v -> u)
        graph[-u]->push_back(v);
        graph[-v]->push_back(u);
    }
}

void DFS(int source) {
    s.push(source);
    inStack[source] = true;
    ids[source] = id;
    low[source] = id;
    id++;
    
    if (graph.find(source) != graph.end()){
        for (int v : *graph[source]) {
            if (ids.find(v) == ids.end() || ids[v] == -1)
                DFS(v);
            if (inStack[v])
                low[source] = min(low[source], low[v]);
        }
    }
    
    if (ids[source] == low[source]) {
        while (!s.empty()) {
            int node = s.top();
            s.pop();
            inStack[node] = false;
            low[node] = ids[source]; // Asignamos el ID del componente raíz
            if (node == source) break;
        }
    }
}

void SCC(){
    id = 0;
    ids.clear();
    low.clear();
    inStack.clear();
    while(!s.empty()) s.pop();

    for (int i = 1; i <= vars; i++){
        ids[i] = -1; ids[-i] = -1;
    }
    for (int i = 1; i <= vars; i++) {
        if (ids[i] == -1) DFS(i);
        if (ids[-i] == -1) DFS(-i);
    }
}

// LA FUNCIÓN CLAVE PARA LA ASIGNACIÓN
void printAssignment() {
    cout << "SAT <3" << endl;
    cout << "Asignacion encontrada:" << endl;
    for (int i = 1; i <= vars; i++) {
        // En Tarjan, si low[-i] < low[i], significa que -i se terminó de procesar después
        // o está en un componente "más alto" en el grafo de implicación.
        // Por lo tanto, i debe ser TRUE.
        if (low[i] < low[-i]) {
            cout << " Variable " << i << ": TRUE" << endl;
        } else {
            cout << " Variable " << i << ": FALSE" << endl;
        }
    }
}

bool twoSAT() {    
    SCC();
    for (int i = 1; i <= vars; i++){
        if (low[i] == low[-i]){
            return false;
        }
    }
    return true;
}

int main() {
    // Tu fórmula de ejemplo
    Formula formula = {
        {-1, -4}, {2, -4}, {-3, -4}, {4, -5}, {4, -6}, {4, -7}
    };

    implicationGraph(formula);
    
    if (twoSAT()) {
        printAssignment();
    } else {
        cout << "UNSAT :(" << endl;
    }

    // Limpieza de memoria (importante por los 'new')
    for (auto & entry : graph) delete entry.second;
    return 0;
}
