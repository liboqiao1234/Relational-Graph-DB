#include<bits/stdc++.h>
using namespace std;

int n; // number of table
int m; // number of graph
int tmpTableCnt; // num of tmpTable

map<string, int> Table; // name -> id
map<string, int> GraphId;

union Attr{
    char* str;
    long long number;
    void* pointerSet;
};

class Tuple{
public:
    int num1; // number of attribute
    void* table; // use  (RG*) to convert!!
    vector<Attr*> attribute;
    set<Attr*> pointerFrom;

    explicit Tuple (void* fa, int num1) ;
    Tuple (void *fa, Tuple *a, Tuple *b);
};

class RG{
public:
    int num1; // number of attribute
    int num3; // number of tuple
    string name;
    vector<Tuple*> table;
    vector<string>zero; // first row: attr name and pointer name;
    map <string, int> attr; // name -> line no.
    map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:str
    explicit RG (string name = "EmptyName", int num1 = 0,  int num3 = 0);
};

Tuple::Tuple(void *fa, int num1 = 0){
    table = fa;
//        attribute.resize(num1);
    for (int i = 0; i < num1; i++) {
        attribute.emplace_back(new Attr());
    }
    pointerFrom.clear();
    this->num1 = num1;
}

RG::RG (string name, int num1, int num3) {
    attr.clear();
    zero.clear();
    for (int i = 0; i < num3; i++) {
    table.emplace_back(new Tuple(this, num1));
    }
    this->name = name;
    this->num1 = num1;
    this->num3 = num3;
}

class Graph{
public:
    int N, M;
    vector<long long> Node;
    vector<pair<pair<int, int>, long long>> Edge;
};

vector<Graph> Graphs;

void InsertNewAttr(RG *now, string name, int type) {
    now -> num1++;
    now -> zero.push_back(name);
    now -> attr[name] = (now -> num1) - 1;
    now -> attr_type[(now -> num1) - 1] = 2;
    for(auto o : now -> table) {
        o -> num1++;
        o -> attribute.push_back(new Attr());
    }
}

void outputRG(RG &a, const string& name = "") {
    int num1 = a.num1; // attr
    int num3 = a.num3; // tuple
    if(name == ""){
        cout << a.name << " addr:" << &a <<endl;
        for (auto i:a.zero) {
            cout << i << ' ';
        }
        cout << endl;
        for (int i=0;i<num3;i++) {
            cout<<"tupleAddr: "<<&(a.table[i])<<" ";
            for(int j=0;j<num1;j++) {
                if (a.attr_type[j] == 0){
                    cout << a.table[i]->attribute[j]->number << " ";
                } else if(a.attr_type[j] == 1) {
                    cout << (a.table[i]->attribute[j]->str) << " ";
                } else if(a.attr_type[j] == 2) {
                    cout << "Pointers to:" << ' ';
                    auto ps = static_cast<set<Tuple*>*>(a.table[i]->attribute[j]->pointerSet);
                    if (ps == nullptr) {
                        cout<<" empty! ";
                        continue;
                    }
                    for(auto i : (*ps)){
                        cout<<(i)<<" ";
                    }

                } else {
                    cout << " Unknown attribute type! " ;
                }

            }
            cout << "  fatherTableAddr: " << a.table[i]->table;
            cout << endl;
        }
    } else {
        cout << name <<endl;
        for (auto i:a.zero) {
            cout << i << ' ';
        }
        cout << endl;
        for (int i=0;i<num3;i++) {
            for(int j=0;j<num1;j++) {
                if (a.attr_type[j] == 0){
                    cout << a.table[i]->attribute[j]->number << " ";
                } else if(a.attr_type[j] == 1) {
                    cout << (a.table[i]->attribute[j]->str) << " ";
                } else if(a.attr_type[j] == 2) {
                    cout << "Pointers" << ' ';
                } else {
                    cout << " Unknown attribute type! " ;
                }
            }
            cout << endl;
        }
    }

}

vector<RG> Rgs;

class SelCondition{
public:
    string attr; // line no.
    int cmp; // compare type
    char* value; // the constant
};

class JoinCondition{
public:
    string attr1, attr2;
    int cmp{};
};

class Step{
public:
    int S1{}, S2{}; // csg and cmp set
    vector<JoinCondition> conditions; // join condition
};

long long toNum(const char *value) {
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

void updatePointer(Tuple *a, Tuple * ori) {
    for(auto &i:ori->pointerFrom) {

        if((set<Tuple*>*)(i->pointerSet) == nullptr) {
            (i->pointerSet) = new set<Tuple*>;
        }
        auto ps = static_cast<set<Tuple*>*>(i->pointerSet);

        ps->erase(ori);
        ps->insert(a);
    }
}

Tuple::Tuple (void *fa, Tuple *aa, Tuple *bb) { // a+b
    Tuple a = *aa;
    Tuple b = *bb;
    table = fa;
    num1 = a.num1 + b.num1;
    attribute.resize(a.num1 + b.num1);
    for (int i = 0; i < a.num1; i++) {
        attribute[i] = a.attribute[i];
    }
    for (int i = 0; i < b.num1; i++) {
        attribute[i + a.num1] = b.attribute[i];
    }
    pointerFrom.insert(a.pointerFrom.begin(), a.pointerFrom.end());
    pointerFrom.insert(b.pointerFrom.begin(), b.pointerFrom.end());
    updatePointer(this, &a);
    updatePointer(this, &b);
}

void updateFather(RG &t) {
    for(int i=0;i<t.num3;i++) {
        t.table[i]->table = &t;
    }
}

namespace Init{ 
    void InitTable() {
        cin >> n;
        for (int i = 0; i < n; i++) {
            string name;
            cin >> name;
            int num1, num3;
            cin >> num1 >> num3;
            RG now = RG(name, num1, num3);
            string attr;
            int attr_type;
            for (int k = 0; k < num1; k++) {
                cin >> attr >> attr_type; // attr_type  0 longlong 1 str
                attr = name + "." + attr;
                now.zero.push_back(attr);
                now.attr_type[k] = attr_type;
                now.attr[attr] = k;
            }
            for (int j = 0; j < num3; j++) {
                for (int k = 0; k < num1; k++) {
                    if(now.attr_type[k] == 0) {
                        cin >> now.table[j]->attribute[k]->number;
                    } else {
                        now.table[j]->attribute[k]->str = new char();
                        cin >> now.table[j]->attribute[k]->str;
                    }
                }
            }
            Table[now.name] = Rgs.size();
            Rgs.push_back(std::move(now));
        }
    }

    void InitGraph() {
        cin >> m;
        for(int i = 0; i < m; i++) {
            string name;
            cin >> name;
            GraphId[name] = i;
            Graph now;
            cin >> now.N >> now.M;
            now.Node.resize(now.N);
            for(int j = 0; j < now.N; j++) {
                cin >> now.Node[j];
            }
            for(int j = 0; j < now.M; j++) {
                int x, y;
                long long id;
                cin >> x >> y >> id;
                x--;
                y--;
                now.Edge.push_back(make_pair(make_pair(x, y), id));
            }
            Graphs.push_back(now);
        }
    }

    void Init() {
        InitTable(), InitGraph();
    }
};

namespace Query{

    struct Edge{
        int to;
        JoinCondition condition;
        explicit Edge(int to = 0, JoinCondition condition = *(new JoinCondition())) : to(to), condition(std::move(condition)) {}
    };

    int tot, cnt;
    vector<RG*> tables;
    map<string, int> TableId;
    vector<vector<Edge> > G;
    vector<vector<int> > uG;
    vector<int> checkId;
    vector<long long>Cardi;
    long long GetCost(Step now) {
        return Cardi[now.S1]*Cardi[now.S2]*now.conditions.size();
    }

    void BuildQueryGraph() {
        tot = 0, cnt = 0;
        int type;
        cin >> type;
        if(type == 1) {
            // build graphMatch
            int graphN, graphM, graphId;
            string graphName;
            cin >> graphName >> graphN >> graphM;
            graphId = GraphId[graphName];
            Graph g = Graphs[graphId];
            int N = g.N, M = g.M;
            for(int i = 0; i < graphN; i++) {
                RG* V = new RG("V" + to_string(i), 1, N);
                V -> zero.push_back(V -> name + ".id"), V -> attr[V -> name + ".id"] = 0, V -> attr_type[0] = 0;
                for(int j = 0; j < N; j++) V -> table[j]->attribute[0]->number = g.Node[j];
                TableId["V" + to_string(i)] = tot++;
                uG.emplace_back(), G.emplace_back(), tables.emplace_back(V), checkId.push_back(cnt++);
                updateFather(*tables[tables.size()-1]);
            }
            for(int i = 0; i < graphM; i++) {
                int x, y;
                cin >> x >> y;
                JoinCondition condition1, condition2;
                condition1.cmp = condition2 .cmp = 4;

                RG* E_rev = new RG("E_rev" + to_string(i), 2, M);

                E_rev -> zero.push_back(E_rev -> name + ".id"), E_rev -> attr[E_rev -> name + ".id"] = 0, E_rev -> attr_type[0] = 0;
                E_rev -> zero.push_back(E_rev -> name + ".dst"), E_rev -> attr[E_rev -> name + ".dst"] = 1, E_rev -> attr_type[1] = 2;
                TableId["E_rev" + to_string(i)] = tot;

                InsertNewAttr(tables[x], "V" + to_string(x) + "E" + to_string(i) + ".out", 2);
                InsertNewAttr(tables[y], "V" + to_string(y) + "E" + to_string(i) + ".in", 2);

                for(int j = 0; j < M; j++) {
                    int u = g.Edge[j].first.first;
                    int v = g.Edge[j].first.second;
                    long long id = g.Edge[j].second;
                    E_rev -> table[j]->attribute[0]->number = id;
                    if (E_rev -> table[j]->attribute[1]->pointerSet == nullptr) {
                        E_rev -> table[j]->attribute[1]->pointerSet = new set<Tuple*>;
                    }
                    static_cast<set<Tuple*>*>(E_rev -> table[j]->attribute[1]->pointerSet) -> insert((tables[x] -> table[u]));
                    (tables[x] -> table[u])->pointerFrom.insert(E_rev -> table[j]->attribute[1]);
                    if ((tables[y] -> table[v])->attribute.back()->pointerSet == nullptr) {
                        (tables[y] -> table[v])->attribute.back()->pointerSet = new set<Tuple*>;
                    }
                    static_cast<set<Tuple*>*>((tables[y] -> table[v])->attribute.back()->pointerSet) -> insert(E_rev -> table[j]);
                    E_rev -> table[j]->pointerFrom.insert((tables[y] -> table[v])->attribute.back());
                }
                uG.emplace_back(), G.emplace_back();
                tables.push_back(E_rev);
                updateFather(*tables[tables.size()-1]);
                checkId.push_back(cnt);

                condition1.attr1 = "E_rev" + to_string(i) + ".dst";
                condition2.attr1 = "V" + to_string(y) + "E" + to_string(i) + ".in";
                uG[y].push_back(tot), uG[tot].push_back(y), uG[x].push_back(tot), uG[tot].push_back(x);
                G[y].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(x, condition1));
                tot++;

                RG* E_ord = new RG("E_ord" + to_string(i), 2, M);
                E_ord -> zero.push_back(E_ord -> name + ".id"), E_ord -> attr[E_ord -> name + ".id"] = 0, E_ord -> attr_type[0] = 0;
                E_ord -> zero.push_back(E_ord -> name + ".dst"), E_ord -> attr[E_ord -> name + ".dst"] = 1, E_ord -> attr_type[1] = 2;
                TableId["E_ord" + to_string(i)] = tot;
                for(int j = 0; j < M; j++) {
                    int u = g.Edge[j].first.first;
                    int v = g.Edge[j].first.second;
                    long long id = g.Edge[j].second;
                    E_ord -> table[j]->attribute[0]->number = id;
                    if (E_ord -> table[j]->attribute[1]->pointerSet == nullptr) {
                        E_ord -> table[j]->attribute[1]->pointerSet = new set<Tuple*>;
                    }
                    static_cast<set<Tuple*>*>(E_ord -> table[j]->attribute[1]->pointerSet)->insert(tables[y]->table[v]);
                    (tables[y] -> table[v])->pointerFrom.insert(E_ord -> table[j]->attribute[1]);
                    if ((tables[x] -> table[u])->attribute.back()->pointerSet == nullptr) {
                        (tables[x] -> table[u])->attribute.back()->pointerSet = new set<Tuple*>;
                    }
                    static_cast<set<Tuple*>*>((tables[x] -> table[u])->attribute.back()->pointerSet) -> insert(E_ord -> table[j]);
                    E_ord -> table[j]->pointerFrom.insert((tables[x] -> table[u])->attribute.back());
                }
                uG.emplace_back(), G.emplace_back(), tables.push_back(E_ord), checkId.push_back(cnt++);
                updateFather(*tables[tables.size()-1]);
                condition1.attr1 = "E_ord" + to_string(i) + ".dst";
                condition2.attr1 = "V" + to_string(x) + "E" + to_string(i) + ".out";
                uG[y].push_back(tot), uG[tot].push_back(y), uG[x].push_back(tot), uG[tot].push_back(x);
                G[x].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(y, condition1));
                tot++;
            }
        }
        int N;
        cin >> N;
        for(int i = 1; i <= N; i++) {
            string attr1, attr2;
            int x, y, cmp;
            cin >> attr1 >> attr2 >> cmp;
            string tableName1 = attr1.substr(0, attr1.find_first_of('.'));
            string tableName2 = attr2.substr(0, attr2.find_first_of('.'));
            if(!TableId.count(tableName1)) {
                TableId[tableName1] = tot++;
                uG.emplace_back(), G.emplace_back(), tables.push_back(&Rgs[Table[tableName1]]), checkId.push_back(cnt++);
            }
            if(!TableId.count(tableName2)) {
                TableId[tableName2] = tot++;
                uG.emplace_back(), G.emplace_back(), tables.push_back(&Rgs[Table[tableName2]]), checkId.push_back(cnt++);
            }
            x = TableId[tableName1];
            y = TableId[tableName2];
            JoinCondition condition;
            condition.attr1 = attr1;
            condition.attr2 = attr2;
            condition.cmp = cmp;
            uG[x].push_back(y), uG[y].push_back(x);
            G[x].push_back(Edge(y, condition));
        }
    }

    int best;
    map<int, int> Id;
    vector<int> N;
    vector<long long> Cost;
    vector<Step> Plan;
    vector<int> Can;

    int B(int x) {
        return x | (x - 1);
    }

    int Min(int x) {
        return x & -x;
    }

    int Neighbor(int S, int X) {
        return N[S] ^ (N[S] & X);
    }

    Step GetStep(int S1, int S2) {
        Step res;
        res.S1 = S1, res.S2 = S2, res.conditions.clear();
        for(int t = S1; t; t ^= Min(t))  {
            int i = Id[Min(t)];
            for(auto [j, condition] : G[i]) if(S2 >> j & 1) {
                res.conditions.push_back(condition);
            }
        }
        for(int t = S2; t; t ^= Min(t)) {
            int i = Id[Min(t)];
            for(auto [j, condition] : G[i]) if(S1 >> j & 1) {
                JoinCondition condition1 = condition;
                if(condition1.cmp == 2 || condition1.cmp == 3)
                    condition1.cmp = 5 - condition1.cmp;
                swap(condition1.attr1, condition1.attr2);
                res.conditions.push_back(condition1);
            }
        }
        return res;
    }

    void EmitCsgCmp(int S1, int S2) {
        // cout << S1 << ' ' << S2 << '\n';
        int Sta = S1 | S2;
        if(!Can[Sta]) return ;
        Step now = GetStep(S1, S2);
        Cardi[Sta] = Cardi[now.S1]*Cardi[now.S2]/(now.conditions.size()+1);
        if(Plan[Sta].S1 == 0 || Cost[Sta] > Cost[S1] + Cost[S2] + GetCost(now)) {
            Plan[Sta] = now;
        }

        if(Can[Sta] == 2) {
            if(best == -1 || Cost[Sta] < Cost[best]) {
                best = Sta;
            }
        }
    }

    void EnumerateCmpRec(int S1, int S2, int X) {
        int N = Neighbor(S2, X);
        for(int s = N; s ; s = (s - 1) & N) {
            if(Plan[S2 | s].S1)
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
        for(int i = tot - 1; i>= 0; i--) if(N >> i & 1) {
            int S2 = 1 << i;
            EmitCsgCmp(S1, S2);
            EnumerateCmpRec(S1, S2, X);
        }
    }

    void EnumerateCsgRec(int S1, int X) {
        int N = Neighbor(S1, X);
        for(int s = N; s ; s = (s - 1) & N) {
            if(Plan[S1 | s].S1)
                EmitCsg(S1 | s);
        }

        for(int s = N; s ; s = (s - 1) & N)
            EnumerateCsgRec(S1 | s, X | Neighbor(S1, X));
    }

    void GetPlan() {
        int sta = (1 << tot);
        best = -1;
        Cost.clear(), Cost.resize(sta);
        Cardi.clear(), Cardi.resize(sta);
        Plan.clear(), Plan.resize(sta);
        N.clear(), N.resize(sta);
        Can.clear(), Can.resize(sta);
        for(int s = 0; s < sta; s++) {
            vector<int> vis(cnt, 0);
            for(int i = 0; i < tot; i++)
                if(s >> i & 1) vis[checkId[i]]++;
            Can[s] = *max_element(vis.begin(), vis.end()) <= 1;
            if(*min_element(vis.begin(), vis.end()) >= 1) Can[s] = 2;
        }

        for(int i = 0; i < tot; i++) {
            Cardi[1<<i] = tables[i]->num3;
//            cout<<"";
            Id[1 << i] = i, Plan[1 << i].S1 = 1 << i;
            for(auto edge : uG[i]) N[1 << i] |= 1 << edge;
            // , cout << edge << ' ';
            // cout << N[1 << i] << '\n';
        }
//        cout << '\n';
        for(int s = 1; s < sta; s++) {
            N[s] = N[s  ^ (s & -s)] | N[s & -s];
        }
        for(int i = tot - 1; i >= 0; i--) {
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
    RG* Projection(RG &R, vector<string> attrs1) { // if const R, can't access the R.attr[]
        vector<string> attrs;
        int temp = R.num1;
        int temp1 = attrs1.size();
        for (int i=0;i<temp1;i++) {
            int flag = 0;
            for (int j=0;j<temp;j++) {
                if(R.zero[j] == attrs1[i]) {
                    flag = 1;
                    break;
                }
            }
            if(flag==1) {
                attrs.emplace_back(attrs1[i]);
            }
        }
        tmpTableCnt++;
        int num1 = attrs.size(); // num1: attr
        int num3 = R.num3; // tuple num
        RG* res = new RG("__tmpTable" + to_string(tmpTableCnt), num1, num3); // Name!!!
        vector <int> selected_attr;
        string attr_name;
        int line_no;
        for (int i = 0; i < num1; i++) {
            attr_name = attrs[i];
            line_no = R.attr[attrs[i]];
            res->attr[attr_name] = i;
            res->zero.push_back(attr_name);
            res->attr_type[i] = R.attr_type[line_no];
            selected_attr.push_back(line_no); // check the projection attrs' number
        }

        for (int i = 0; i < num3; i++) {
            auto tmpTuple = new Tuple(res, num1);
            tmpTuple->pointerFrom.insert(R.table[i]->pointerFrom.begin(), R.table[i]->pointerFrom.end());
            for (int j = 0; j < num1; j++) {
                (*tmpTuple).attribute[j] = R.table[i]->attribute[selected_attr[j]];
            }
            updatePointer(tmpTuple,R.table[i]);
            res->table[i] = tmpTuple;
        }
        return res;
    }

    bool CMP(const SelCondition &condition, const Tuple &a, int lineNo) { // whether the tuple fits the condition.
        int cmpop = condition.cmp;
        if (lineNo >= a.num1) {
            cout << "Select the pointerSet, not supported yet." << endl;
            return false;
        }
        char *con = (char*)malloc(4);
        Attr* be = new Attr(*a.attribute[lineNo]);
        strcpy(con,condition.value);
        switch (cmpop){
            case 0:{
                if (strcmp(con, be->str)==0) {
                    return true;
                }
                break;
            }
            case 1:{
                if (be->number == toNum(con)) {
                    return true;
                }
                break;
            }
            case 2:{
                if (be->number <= toNum(con)) {
                    return true;
                }
                break;
            }
            case 3:{
                if (be->number >= toNum(con)) {
                    return true;
                }
                break;
            }
            default:{
                cout << "Unknown compare type" <<endl;
            }
        }
        return false;
    }

    bool CMP(const JoinCondition &condition, Tuple *aa, Tuple *bb, int lineNo1, int lineNo2) {
        auto &a = *aa;
        auto &b = *bb;
        int cmpop = condition.cmp;
        Attr* av;
        if (lineNo1 != -1) {
            av = new Attr(*a.attribute[lineNo1]);
        }
        Attr* bv;
        if (lineNo2 != -1) {
            bv = new Attr(*b.attribute[lineNo2]);
        }
        if (cmpop !=4 && (lineNo1 ==-1 || lineNo2==-1)) cout<<"Join wrong: no such attr in no-edge join!"<<endl;
        switch (cmpop) {
            case 0:{
                if (strcmp(av->str, bv->str)==0){
                    return true;
                }
                break;
            }
            case 1:{
                if (av->number == bv->number) {
                    return true;
                }
                break;
            }
            case 2:{
                if (av->number <= bv->number) {
                    return true;
                }
                break;
            }
            case 3:{
                if (av->number >= bv->number) {
                    return true;
                }
                break;
            }
            case 4:{
                if (a.attribute[lineNo1]->pointerSet != nullptr) {
                    auto s = static_cast<set<Tuple*>*>(a.attribute[lineNo1]->pointerSet);
                    if ((*((set<Tuple*>*)(a.attribute[lineNo1]->pointerSet))).count((Tuple*)&b) == 1) {
                        return true;
                    }
                }
                break;
            }
            default:{
                cout << "unknown join" <<endl;
            }
        }
        return false;
    }
    RG* Selection(RG &R, vector<SelCondition> &conditions) {
        tmpTableCnt++;
        string name = "__tmpTable" + to_string(tmpTableCnt);
        int num1 = R.num1;
        RG* res = new RG(name, num1, 0);
        string AttrName;
        int line_no;
        res->zero = R.zero;
        res->attr = R.attr;
        res->attr_type = R.attr_type;

        // Exert selection.
        int tot = R.num3;
        int flag = 1;
        int num3 = 0;
        for (int i = 0; i < tot; i++) {
            flag = 1;
            for (auto & condition : conditions) {
                int lineNo = R.attr[condition.attr];
                if(!CMP(condition, *R.table[i], lineNo)) {
                    flag = 0;
                    break;
                }
            }
            if (flag) {
                num3++;
                auto tmpTuple = R.table[i]; // remained bugable!
                tmpTuple->table = res;
                updatePointer(tmpTuple, R.table[i]);
                (res->table).push_back(tmpTuple); // beware bugs of copy construction ?
            }
        }
        res->num3 = num3;
        return res;
    }
    RG* RGJoin(RG &a, RG &b, vector<JoinCondition> &conditions) { // simple n^2 join, slow but right
        tmpTableCnt++;
        string name = "__tmpTable" + to_string(tmpTableCnt);
        int tot = 0;
        RG *res = new RG(name, a.num1 + b.num1, 0);
        string AttrName;
        for (int i = 0; i < a.num1; i++) {
            AttrName = a.zero[i];
            res -> attr[AttrName] = i;
            assert(i==res->zero.size()); // test!
            res -> attr_type[i] = a.attr_type[i];
            res -> zero.push_back(AttrName);
        }
        for (int i = 0; i < b.num1; i++) {
            AttrName = b.zero[i];
            res -> attr[AttrName] = i + a.num1; // or i+a.num1
            assert(i+a.num1==res->zero.size());
            res -> attr_type[i + a.num1] = b.attr_type[i];
            res -> zero.push_back(AttrName);
        }
        // data maintain
        int flag = 1;
        for (int i = 0; i < a.num3; i++) {
            for (int j = 0; j < b.num3; j++) {
                flag = 1;
                for (auto & condition: conditions) {
                    int lineNo1 = -2;
                    try{
                        lineNo1 = a.attr.at(condition.attr1);
                    }catch (...){
                        lineNo1 = -1;
                    }
                    int lineNo2 = -2;
                    try{
                        lineNo2 = b.attr.at(condition.attr2);
                    }catch (...){
                        lineNo2 = -1;
                    }
                    if (condition.attr1.empty() && condition.cmp == 4){
                        if (!CMP(condition, b.table[j], a.table[i], lineNo2, lineNo1)) {
                            flag = 0;
                            break;
                        }
                    } else if(!condition.attr1.empty()){
                        if (!CMP(condition, a.table[i], b.table[j], lineNo1, lineNo2)) {
                            flag = 0;
                            break;
                        }
                    } else{
                        cout << "wrong!!!!" << endl;
                    }

                }
                if (flag) {
                    assert(res->attr.size()==a.num1+b.num1);
                    auto tmpTuple = new Tuple(res, a.table[i], b.table[j]);
                    res->table.emplace_back(tmpTuple);
                    tot++;
                }
            }
        }

        res->num3 = tot;
        return res;
    }
};

RG *Selectcolumn(RG &a, vector<string> b) {
    RG* result = Exert::Projection(a, b);
    return result;
}

RG *Do(RG *a, RG *b, vector<JoinCondition> c, vector<string> d) {
    RG* temp = Exert::RGJoin(*a, *b, c);
    RG *result = Selectcolumn(*temp, d);
    return result;
}

RG *Calc(int S, vector<string> attr) {

    /*class Step{
    public:
        int S1, S2; // csg and cmp set
        vector<JoinCondition> conditions; // join condition
        vector<string> attr; // attr which should be contained
    };*/

    if((S & (S - 1)) == 0) {//如果S是一个二进制数，就是简单的那个表
        int temp = log2(S);
        return (Query::tables[temp]);
    }
    else {
        sort(attr.begin(), attr.end());
        attr.resize(unique(attr.begin(), attr.end()) - attr.begin());
        Step now = Query::Plan[S];//Plan的每一个元素是一个step
        int S1 = now.S1, S2 = now.S2;
        vector<JoinCondition> condition = now.conditions;
        int num = condition.size();
        vector<string> attr1;
        attr1 = attr;

        vector<string> attr2;
        attr2 = attr;

        for (int i =0;i<num;i++) {
            string str1 = condition[i].attr1;
            string str2 = condition[i].attr2;
            attr1.emplace_back(str1);
            attr2.emplace_back(str2);
        }

        RG *s1 = Calc(S1, attr1);
        RG *s2 = Calc(S2, attr2);
        

        RG *res = Do(s1, s2, condition, attr);
        return res;
    }
}

int main() {
    Init :: Init();
    Query :: Query();

    vector<string> readattr;
    getchar();
    string input;
    getline(cin, input);
    istringstream iss(input);
    string token;
    while (getline(iss, token, ' ')) {
        readattr.push_back(token);
    }
    cout<<endl;

    RG *result = Calc(Query::best, readattr);
    outputRG(*result,"Final Output Table");
    return 0;
}
/*
2
table1
2 3
id 0
name 1
1 Name
2 Name1
3 345
table2
2 3
id 0
name 1
321 Name
213 Name1
3 3456

1
graph1
4 5
1 2 3 4
1 2 3
2 3 0
1 3 1
1 4 2
4 3 4


1
graph1
3 3
0 1
0 2
1 2

1
table1.id V0.id 1
V0.id V1.id V2.id

 */
