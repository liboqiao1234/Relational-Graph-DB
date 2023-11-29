#include<bits/stdc++.h>

using namespace std;

int n; // number of table
int m; // number of graph

union Attr{
    char* string;
    long long number;
};

class Tuple{
public:
    int num1; // number of attribute
    int num2; // number of pointer set
    vector<Attr> attribute;
    vector<set<Tuple*>> pointerSet;

    Tuple (int num1 = 0, int num2 = 0) {
        attribute.resize(num1);
        for (int i = 0; i < num1; i++) {
            attribute[i].number = 0;
        }
        pointerSet.resize(num2);
        for (int i = 0; i < num2; i++) {
            pointerSet[i].clear();
        }
        this->num1 = num1;
        this->num2 = num2;
    }
};

class RG{
public:
    int num1; // number of attribute
    int num2; // number of pointer set
    int num3; // number of tuple
    vector<Tuple> table;
    vector<string>zero; // first row: attr name and pointer name;
    map <string, int> attr; // name -> line no.
    map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:string
    map <string, int> poi;
    RG (int num1 = 0, int num2 = 0, int num3 = 0) {
        table.resize(num3);
        attr.clear();
        poi.clear();
        zero.clear();
        for (int i = 0; i < num3; i++) {
            table[i] = Tuple(num1, num2);
        }
        this->num1 = num1;
        this->num2 = num2;
        this->num3 = num3;
    }
};


struct Opt{
    int type, a, b;
    Opt(int type = 0, int a = 0, int b = 0) : type(type), a(a), b(b) {}
};

vector<RG> Rgs;

class Condition{
    int attr; // line no.
    int cmp; // compare type
    char* value; // the constant
};

long long toNum(char *value) {
    long long res = 0;
    int i = 0;
    int flag = 1;
    while (value[i]==' ') ++i;
    if (value[i] == '-') {
        flag = -1;
        ++i;
    }
    while (value[i]!='\0') {
        res = (res << 1) + (res << 3) + (value[i] - '0');
    }
    return res * flag;
}

namespace Init{ 
    void InitTable() {
        cin >> n;
        for (int i = 0; i < n; i++) {
            int num1, num3;
            cin >> num1 >> num3;
            RG now = RG(num1, 0, num3);
            string attr;
            int attr_type;
            for (int k = 0; k < num1; k++) {
                cin >> attr >> attr_type;
                now.zero.push_back(attr);
                now.attr_type[k] = attr_type;
                now.attr[attr] = k;
            }
            for (int j = 0; j < num3; j++) {
                for (int k = 0; k < num1; k++) {
                    if(now.attr_type[k] == 0) {
                        cin >> now.table[j].attribute[k].number;
                    } else {
                        cin >> now.table[j].attribute[k].string;
                        // use cin to deal with input for char* type may cause error!!!!!
                    }
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
                V.table[j].attribute[0].number = id;
            }
            for(int j = 0; j < M; j++) {
                int x, y, id;
                cin >> x >> y >> id;
                E_in.table[j].attribute[0].number = id;
                E_in.table[j].pointerSet[0].insert(&V.table[x]);
                V.table[y].pointerSet[1].insert(&E_in.table[j]);

                E_out.table[j].attribute[0].number = id;
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
        tot = -1, RgtoId.clear(), IdtoRg.clear();
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
            if(Plan[S2 | s] != make_pair(0, 0))
                EmitCsgCmp(S1, S2 | s);
        }
        X |= N;
        for(int s = N; s ; s = (s - 1) & N) {
            EnumerateCmpRec(S1, S2 | s, X);
        }
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
            if(Plan[S1 | s] != make_pair(0, 0))
                EmitCsg(S1 | s);
        }
        
        for(int s = N; s ; s = (s - 1) & N) 
            EmitCsg(S1 | s);
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
    RG Projection(RG &R, vector<int> attrs, vector<int> pointers) {
        int num1 = attrs.size(); // num1: attr
        int num2 = pointers.size(); // num2: pointer
        int num3 = R.num3; // tuple num
        RG* res = new RG(num1, num2, num3);
        vector <int> selected_attr, selected_poi;
        string attr_name;
        for (int i = 0; i < num1; i++) {
            // Pointer remains unsolved. How to define RG.attr ?
            attr_name = R.zero[attrs[i]];
            res->attr[attr_name] = i;
            res->zero.push_back(attr_name);
            selected_attr.push_back(attrs[i]); // check the projection attrs' number
        }
        for (int i=0; i<num2;i++) {
            attr_name = R.zero[R.num1 + i]; // means the pointers set
            res->poi[attr_name] = i;
            res->zero.push_back(attr_name);
            selected_poi.push_back(pointers[i]); // pointers number
        }

        for (int i = 0; i < num3; i++) {
            auto tmpTuple = new Tuple(num1, num2);
            for (int j = 0; j < num1; j++) {
                (*tmpTuple).attribute[j]=R.table[i].attribute[selected_attr[j]];
            }
            for (int j = 0; j < num2; j++) {
                (*tmpTuple).pointerSet[j]=R.table[i].pointerSet[selected_poi[j]];
            }
            res->table[i]=*tmpTuple;
        }
        return *res;
    }
    RG Selection(RG &R, vector<Condition>conditions) {
        // unfinished
        RG* res = new RG();
        return *res;
    }
    RG RGJoin(RG &a, RG &b, vector<Condition>conditions) {
        RG *res = new RG();
        return *res;
    }
    RG EdgeJoin(RG &a, RG &b, vector<Condition>conditions) {
        RG *res = new RG();
        return *res;
    }
};

void Calc() {

}

void Output() {

}

namespace Debug {
    void outputRG(RG &a) {
        int num1 = a.num1; // attr
        int num2 = a.num2; // pointer
        int num3 = a.num3; // tuple
        for (auto i:a.zero) {
            cout << i << ' ';
        }
        cout << endl;
        for (int i=0;i<num3;i++) {
            for(int j=0;j<num1;j++) {
                if (a.attr_type[j] == 0){
                    cout << a.table[i].attribute[j].number << " ";
                } else if(a.attr_type[j] == 1) {
                    cout << a.table[i].attribute[j].string << " ";
                } else {
                    cout << "Unknown attribute type!" ;
                }

            }
            for(int j=0;j<num2;j++) {
                cout << "pointer" << " "; // a.table[i].pointerSet[j]
            }
            cout << endl;
        }
    }
};

int main() {
    Init :: Init();
    vector<int>qqqq;
    qqqq.push_back(1);// means "name"
    qqqq.push_back(0);// means "id"
    /*
     * test examples:
      1
      2 3
      id name
      1 123
      2 234
      3 345
      0
     */
    vector<int>aaaa;
    RG test = Exert ::Projection(Rgs[0],qqqq,aaaa);
    Debug::outputRG(test);
    Query :: Query();
    Calc();
    Output();
    return 0;
}