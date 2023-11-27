#include<bits/stdc++.h>

using namespace std;

int n; // number of table
int m; // number of graph

struct Tuple{
    int num1; // number of attribute
    int num2; // number of pointer set
    vector<int> attribute;
    vector<set<Tuple*>> pointerSet;

    Tuple (int num1 = 0, int num2 = 0) {
        attribute.resize(num1);
        for (int i = 0; i < num1; i++) {
            attribute[i] = 0;
        }
        pointerSet.resize(num2);
        for (int i = 0; i < num2; i++) {
            pointerSet[i].clear();
        }
    }
};

class RG{
public:
    int num1; // number of attribute
    int num2; // number of pointer set
    int num3; // number of tuple
    vector<Tuple> table;
    map <string, int> attr; // name -> line no.
    RG (int num1 = 0, int num2 = 0, int num3 = 0) {
        table.resize(num3);
        for (int i = 0; i < num3; i++) {
            table[i] = Tuple(num1, num2);
        }
    }
};


struct Opt{
    int type, a, b;
    Opt(int type = 0, int a = 0, int b = 0) : type(type), a(a), b(b) {}
};

vector<RG> Rgs;

namespace Init{ 
    void InitTable() {
        cin >> n;
        for (int i = 0; i < n; i++) {
            int num1, num3;
            cin >> num1 >> num3;
            RG now = RG(num1, 0, num3);
            for (int i = 0; i < num3; i++) {
                for (int j = 0; j < num1; j++) {
                    cin >> now.table[i].attribute[j];
                }
            }
            Rgs.push_back(now);
        }
    }

    void InitGraph() {
        cin >> m;
        for(int i = 0; i < m; i++) {
            int N, M; // number of nodes and edges
            cin >> N >> M;
            RG V(1, 2, N), E_in(1, 1, M), E_out(1, 1, M);
            for(int j = 0; j < N; j++) {
                int id;
                cin >> id;
                V.table[j].attribute[0] = id;
            }
            for(int j = 0; j < M; j++) {
                int x, y, id;
                cin >> x >> y >> id;
                E_in.table[j].attribute[0] = id;
                E_in.table[j].pointerSet[0].insert(&V.table[x]);
                V.table[y].pointerSet[1].insert(&E_in.table[j]);

                E_out.table[j].attribute[0] = id;
                E_out.table[j].pointerSet[0].insert(&V.table[y]);
                V.table[x].pointerSet[0].insert(&E_out.table[j]);
            }
            Rgs.push_back(V);
            Rgs.push_back(E_in);
            Rgs.push_back(E_out);
        }
    }

    void Init() {
        InitTable(), InitGraph();
    }
};

namespace Query{

    struct Edge{
        int to;
        Opt opt;
        Edge(int to = 0, Opt opt = Opt()) : to(to), opt(opt) {}
    };

    vector<vector<Edge> > G;
    map<int, int> RgtoId;
    map<int, int> IdtoRg;
    int tot;

    void BuildQueryGraph() {
        tot = -1, RgtoId.clear(), IdtoRg.clear(), pp.clear();
        int type;
        cin >> type;
        if(type == 0) {
            int N;
            cin >> N;
            for(int i = 1; i <= N; i++) {
                int x, y, a, b;
                cin >> x >> y >> a >> b;
                if(!RgtoId.count(x)) {
                    RgtoId[x] = ++tot;
                    IdtoRg[tot] = x;
                    G.push_back({});
                }
                if(!RgtoId.count(y)) {
                    RgtoId[y] = ++tot;
                    IdtoRg[tot] = y;
                    G.push_back({}); 
                }
                x = RgtoId[x], y = RgtoId[y];
                G[x].push_back(Edge(y, Opt(1, a, b)));
                G[y].push_back(Edge(x, Opt(1, b, a)));
            }
        } else {
            int graphN, graphM, graphId; 
            cin >> graphId >> graphN >> graphM;
            for(int i = 0; i < graphN; i++) {
                IdtoRg[++tot] = n + 3 * graphId;
                G.push_back({});
            }
            for(int i = 0; i < graphM; i++) {
                int x, y;
                cin >> x >> y;
                IdtoRg[++tot] = n + 3 * graphId + 1, G.push_back({});
                G[y].push_back(Edge(tot, Opt(2, -2, 0)));
                G[tot].push_back(Edge(tot, Opt(2, -1, 0)));
                IdtoRg[++tot] = n + 3 * graphId + 2, G.push_back({});
                G[x].push_back(Edge(tot, Opt(2, -2, 0)));
                G[tot].push_back(Edge(tot, Opt(2, -1, 0)));
            }
            int N;
            cin >> N;
            for(int i = 1; i <= N; i++) {
                int x, y, a, b;
                cin >> x >> y >> a >> b;
               if(!RgtoId.count(x)) {
                    RgtoId[x] = ++tot;
                    IdtoRg[tot] = x;
                    G.push_back({});
                }
                if(!RgtoId.count(y)) {
                    RgtoId[y] = ++tot;
                    IdtoRg[tot] = y;
                    G.push_back({}); 
                }
                x = RgtoId[x], y = RgtoId[y];
                G[x].push_back(Edge(y, Opt(1, a, b)));
                G[y].push_back(Edge(x, Opt(1, b, a)));
            }
            cin >> N;
            for(int i = 1; i <= N; i++) {                
                int x, y, a, b = 0;
                cin >> x >> y >> a;
                if(!RgtoId.count(x)) {
                    RgtoId[x] = ++tot;
                    IdtoRg[tot] = x;
                    G.push_back({});
                }
                x = RgtoId[x];
                G[x].push_back(Edge(y, Opt(1, a, b)));
                G[y].push_back(Edge(x, Opt(1, b, a)));
            } 
        }

    }

    vector<int> N;
    vector<long long> Cost;
    vector<pair<int, int> > Plan;

    int B(int x) {
        return (1 << tot + 1) ^ (x - 1);
    }

    int Min(int x) {
        return x & -x;
    }

    int Neighbor(int S, int X) {
        return N[S] ^ (N[S] & X); 
    }

    void EmitCsgCmp(int S1, int S2) {
        
    }

    void EnumerateCmpRec(int S1, int S2, int X) {
        int N = Neighbor(S2, X);
        for(int s = N; s ; s = (s - 1) & N) {
            if(plan[S2 | s] != make_pair(0, 0))
                EmitCsgCmp(S1, S2 | s);
        }
        X |= N;
        for(int s = N; s ; s = (s - 1) & N) 
            EnumerateCmpRec(S1, S2 | s, X);
    }

    void EmitCsg(int S1) {
        int X = S1 | B(Min(S1));
        int N = Neighbor(S1, X);
        for(int i = tot - 1; i >= 0; i--) {
            int S2 = (1 << i);
            EmitCsgCmp(S1, S2);
            EnumerateCmpRec(S1, S2, X);
        }
    }
        
    void EnumerateCsgRec(int S1, int X) {
        int N = Neighbor(S1, X);
        for(int s = N; s ; s = (s - 1) & N) {
            if(Plan[S1 | N] != make_pair(0, 0))
                EmitCsg(S1 | s);
        }
        
        for(int s = N; s ; s = (s - 1) & N) 
            EmitCsg(S1 | s, X | s);
    }

    void GetPlan() {
        int sta = (1 << tot + 1);
        Cost.clear(), Cost.resize(sta);
        Plan.clear(), Plan.resize(sta);
        N.clear(), N.resize(sta);        
        for(int i = 0; i <= tot; i++) {
            for(auto edge : G[i]) N[1 << i] |= edge.to;
        }
        for(int s = 1; s < sta; s++) {
            N[s] = N[s  ^ (s & -s)] | N[s & -s];
        }
        for(int i = tot; i >= 0; i--) {
            EmitCsg(1 << i);
            EnumerateCsgRec(1 << i , B(1 << i));
        }
    }

    void Query() {
        BuildQueryGraph();
        GetPlan();
    }

};

namespace Exert{
    RG * Projection(RG R, vector<string> attrs, vector<string> pointers) {
        int num1 = attrs.size(); // num1: attr
        int num2 = pointers.size(); // num2: pointer
        int num3 = R.num3; // tuple num
        RG* res = new RG(num1, num2, num3);
        vector <int> selected_attr, selected_poi;
        for (int i = 0; i < num1; i++) {
            // Pointer remains unsolved. How to define RG.attr ?
            selected_attr.push_back(R.attr[attrs[i]]); // check the projection attrs' number
        }
        for (int i=0; i<num2;i++) {
            selected_poi.push_back(R.attr[pointers[i]]); // pointers number
        }

        for (int i = 0; i < num3; i++) {
            auto tmpTuple = new Tuple(num1, num2);
            for (int j = 0; j < num1; j++) {
                (*tmpTuple).attribute.push_back(R.table[i].attribute[selected_attr[j]]);
            }
            for (int j = 0; j < num2; j++) {
                (*tmpTuple).pointerSet.push_back(R.table[i].pointerSet[selected_poi[j]]);
            }
            res->table.push_back(*tmpTuple);
        }
    }
};

void Calc() {

}

void Output() {

}

int main() {
    Init :: Init();
    Query :: Query();
    Calc();
    Output();
    return 0;
}