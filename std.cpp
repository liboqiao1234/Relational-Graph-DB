#include<bits/stdc++.h>

#include <utility>

using namespace std;

int n; // number of table
int m; // number of graph
int tmpTableCnt; // num of tmpTable

map<string, int> Table; // name -> id

union Attr{
    char* str;
    long long number;
};
class Tuple{
public:
    int num1; // number of attribute
    int num2; // number of pointer set
    void* table; // use  (RG*) to convert!!
    vector<Attr> attribute;
    vector<set<Tuple*>> pointerSet;

    explicit Tuple (void* fa, int num1 = 0, int num2 = 0) {
        table = fa;
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
    Tuple (void *fa, Tuple &a, Tuple &b) { // a+b
        table = fa;
        num1 = a.num1 + b.num1;
        num2 = a.num2 + b.num2;
        attribute.resize(a.num1 + b.num1);
        for (int i = 0; i < a.num1; i++) {
            attribute[i] = a.attribute[i];
        }
        for (int i = 0; i < b.num1; i++) {
            attribute[i + a.num1] = b.attribute[i];
        }
        for (int i = 0; i < a.num2; i++) {
            pointerSet[i] = a.pointerSet[i];
        }
        for (int i = 0; i < b.num2; i++) {
            pointerSet[i + a.num2] = b.pointerSet[i];
        }
    }
};

class RG{
public:
    int num1; // number of attribute
    int num2; // number of pointer set
    int num3; // number of tuple
    string name;
    vector<Tuple> table;
    vector<string>zero; // first row: attr name and pointer name;
    map <string, int> attr; // name -> line no.
    map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:str
    map <string, int> poi;
    RG (string name = "EmptyName", int num1 = 0, int num2 = 0, int num3 = 0) {
        //table.resize(num3);
        attr.clear();
        poi.clear();
        zero.clear();
        for (int i = 0; i < num3; i++) {
            table.push_back(Tuple(this,num1, num2));
        }
        this->name = name;
        this->num1 = num1;
        this->num2 = num2;
        this->num3 = num3;
    }
};

void ChangeTableName(RG &now, string newName) {    
    now.name = newName;
    now.attr.clear();
    for(int i = 0 ; i < now.zero.size(); i++) {
        string name = now.zero[i];
        int number = now.attr[name];
        now.attr.erase(name);
        name = now.name + name.substr(name.find_first_of('.') + 1, name.size());
        now.zero[i] = name, now.attr[name] = number;
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
    int cmp;
};

class Step{
public:
    int S1, S2; // csg and cmp set
    vector<JoinCondition> conditions; // join condition
    // vector<string> attr; // attr which should be contained
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

string getAttrName(string AttrName) { // to split real AttrName from "TableName.AttrName"
    int loc = AttrName.find('.');
    string res = AttrName.substr(loc + 1, AttrName.npos);
    return res;
}

namespace Init{ 
    void InitTable() {
        cin >> n;
        for (int i = 0; i < n; i++) {
            string name;
            cin >> name;
            int num1, num3;
            cin >> num1 >> num3;
            RG now = RG(name, num1, 0, num3);
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
                        cin >> now.table[j].attribute[k].number;
                    } else {
                        now.table[j].attribute[k].str = new char();
                        cin >> now.table[j].attribute[k].str;
                        // use cin to deal with input for char* type may cause error!!!!!
                        // already fixed at 23.12.1
                    }
                }
            }
            Table[now.name] = Rgs.size(), Rgs.push_back(now);
        }
    }

    void InitGraph() {
        cin >> m;
        for(int i = 0; i < m; i++) {
            string name;
            cin >> name;
            int N, M; // number of nodes and edges
            cin >> N >> M;
            RG V(name + "V", 1, 2, N), E_in(name + "E_in", 1, 1, M), E_out(name + "E_out", 1, 1, M);
            V.zero.push_back(V.name + ".id"), V.attr[V.name + ".id"] = 0, V.attr_type[0] = 0;
            for(int j = 0; j < N; j++) {
                long long id;
                cin >> id;
                V.table[j].attribute[0].number = id;
            }
            E_in.zero.push_back(E_in.name + ".id"), E_in.attr[E_in.name + ".id"] = 0, E_in.attr_type[0] = 0;
            E_out.zero.push_back(E_out.name + ".id"), E_out.attr[E_out.name + ".id"] = 0, E_out.attr_type[0] = 0;
            for(int j = 0; j < M; j++) {
                int x, y; long long id;
                cin >> x >> y >> id; 
                E_in.table[j].attribute[0].number = id;
                E_in.table[j].pointerSet[0].insert(&V.table[x]);
                V.table[y].pointerSet[1].insert(&E_in.table[j]);

                E_out.table[j].attribute[0].number = id;
                E_out.table[j].pointerSet[0].insert(&V.table[y]);
                V.table[x].pointerSet[0].insert(&E_out.table[j]);
            }
            Table[V.name] = Rgs.size(), Rgs.push_back(V);
            Table[E_in.name] = Rgs.size(), Rgs.push_back(E_in);
            Table[E_out.name] = Rgs.size(), Rgs.push_back(E_out);
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
        Edge(int to = 0, JoinCondition condition = *(new JoinCondition())) : to(to), condition(std::move(condition)) {}
    };

    int tot, cnt;
    vector<RG> tables;
    map<string, int> TableId;
    vector<vector<Edge> > G;
    vector<int> checkId;

    long long GetCost(Step now) {
        return 0;
    }



    void BuildQueryGraph() {
        tot = 0, cnt = 0;
        // RgtoId.clear(), IdtoRg.clear();
        int type;
        cin >> type;
        if(type == 1) {
            // build graphMatch
            int graphN, graphM, graphId; 
            cin >> graphId >> graphN >> graphM;
            for(int i = 0; i < graphN; i++) {
                RG now = Rgs[n + 3 * graphId];
                ChangeTableName(now, "V" + i);
                TableId["V" + i] = tot++;
                G.push_back({}), tables.push_back(now), checkId.push_back(cnt++);
            }
            for(int i = 0; i < graphM; i++) {
                int x, y;
                cin >> x >> y;
                JoinCondition condition1, condition2;
                condition1.cmp = condition2 .cmp = 4;
                condition1.attr1 = -1, condition2.attr2 = -2;

                RG now = Rgs[n + 3 * graphId + 1];
                ChangeTableName(now, "E_rev" + i);
                TableId["E_rev" + i] = tot;
                G.push_back({}), tables.push_back(now), checkId.push_back(cnt);

                G[y].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(x, condition1));
                tot++;

                now = Rgs[n + 3 * graphId + 2];
                ChangeTableName(now, "E_ord" + i);
                TableId["E_ord" + i] = tot;
                G.push_back({}), tables.push_back(now), checkId.push_back(cnt++);

                G[x].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(y, condition1));
                tot++;
            }
        }
        int N;
        cin >> N;
        for(int i = 1; i <= N; i++) {
            string attr1, attr2;
            int x, y, a, b, cmp;
            cin >> attr1 >> attr2 >> cmp;
            string tableName1 = attr1.substr(0, attr1.find_first_of('.'));
            string tableName2 = attr2.substr(0, attr2.find_first_of('.'));
            if(!TableId.count(tableName1)) {
                TableId[tableName1] = tot++;
                G.push_back({}), tables.push_back(Rgs[x]), checkId.push_back(cnt++);
            }
            if(!TableId.count(tableName2)) {
                TableId[tableName2] = tot++;
                G.push_back({}), tables.push_back(Rgs[x]), checkId.push_back(cnt++);
            }
            x = TableId[tableName1];
            y = TableId[tableName2];
            JoinCondition condition;
            condition.attr1 = attr1;
            condition.attr2 = attr2;
            condition.cmp = cmp;
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
        return ((1 << tot) - 1) ^ (x - 1);
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
            int i = Id[t];
            for(auto [j, condition] : G[i]) if(S2 >> j & 1) {
                res.conditions.push_back(condition);
            }
        }
        for(int t = S2; t; t ^= Min(t)) {
            int i = Id[t];
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
        int Sta = S1 | S2;
        if(!Can[Sta]) return ;
        Step now = GetStep(S1, S2);

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
        for(int i = tot - 1; i >= 0; i--) {
            int S2 = (1 << i);
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
            EmitCsg(S1 | s);
    }

    void GetPlan() {
        int sta = (1 << tot);
        best = -1;
        Cost.clear(), Cost.resize(sta);
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
            Id[1 << i] = i;
            for(auto edge : G[i]) N[1 << i] |= edge.to;
        }
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
    RG* Projection(RG &R, vector<string> attrs, vector<string> pointers) { // if const R, can't access the R.attr[]
        // attrs include the RGName!!!!!!!!!!!!!! f**k...
        tmpTableCnt++;
        int num1 = attrs.size(); // num1: attr
        int num2 = pointers.size(); // num2: pointer
        int num3 = R.num3; // tuple num
        RG* res = new RG("__tmpTable" + to_string(tmpTableCnt), num1, num2, num3); // Name!!!
        vector <int> selected_attr, selected_poi;
        string attr_name;
        int line_no;
        for (int i = 0; i < num1; i++) {
            // Pointer remains unsolved. How to define RG.attr ?
            attr_name = attrs[i];
            line_no = R.attr[attrs[i]];
            res->attr[attr_name] = i;
            res->zero.push_back(attr_name);
            res->attr_type[i] = R.attr_type[line_no];
            selected_attr.push_back(line_no); // check the projection attrs' number
        }
        for (int i = 0; i < num2; i++) {
            attr_name = R.zero[R.num1 + i]; // means the pointers set
            line_no = R.poi[attr_name];
            res->poi[attr_name] = i;
            res->zero.push_back(attr_name);
            selected_poi.push_back(line_no); // pointers number
        }

        for (int i = 0; i < num3; i++) {
            auto tmpTuple = new Tuple(res, num1, num2);
            for (int j = 0; j < num1; j++) {
                (*tmpTuple).attribute[j] = R.table[i].attribute[selected_attr[j]];
            }
            for (int j = 0; j < num2; j++) {
                (*tmpTuple).pointerSet[j] = R.table[i].pointerSet[selected_poi[j]];
            }
            res->table[i] = *tmpTuple;
        }
        return res;
    }

    bool CMP(const SelCondition &condition, const Tuple &a, int lineNo) { // whether the tuple fits the condition.
        int cmpop = condition.cmp;
        //int lineNo = condition.attr;
        if (lineNo >= a.num1) {
            cout << "Select the pointerSet, not supported yet." << endl;
            return false;
        }
        char *con = (char*)malloc(4);
        Attr* be = new Attr(a.attribute[lineNo]);
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

    bool CMP(const JoinCondition &condition, const Tuple &a, const Tuple &b, int lineNo1, int lineNo2) {
        // unfinished
        int cmpop = condition.cmp;
        if(lineNo1 >= a.num1 && lineNo2>=b.num2) {
            // remain unfinished : edge join
            cout << "Edge join, unfinished" << endl;
            return false;
        }
        Attr* av = new Attr(a.attribute[lineNo1]);
        Attr* bv = new Attr(b.attribute[lineNo2]);
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
                cout << "edge join unfinished" <<endl;
                return false;
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
        int num2 = R.num2;
        RG* res = new RG(name, num1, num2, 0);
        string AttrName;
        int line_no;
        res->zero = R.zero;
        res->attr = R.attr;
        res->poi = R.poi;
//        for (int i = 0; i < num1; i++) {
//            AttrName = name + '.' + getAttrName(R.zero[i]); // split .?
//            res->zero.push_back(AttrName);
//            res->attr[AttrName] = i;
//        }
//        for (int i = 0; i < num2; i++) {
//            AttrName = name + '.' + getAttrName(R.zero[i+num1]);
//            res->zero.push_back(AttrName);
//            res->poi[AttrName] = i+num1;
//        }
        res->attr_type = R.attr_type; // because the line_no doesn't change.
                             // need to be rewrite!!!!!!!!
        // Exert selection.
        int tot = R.num3;
        int flag = 1;
        int num3 = 0;
        for (int i = 0; i < tot; i++) {
            flag = 1;
            for (auto & condition : conditions) {
                int lineNo = R.attr[condition.attr];
                if(!CMP(condition, R.table[i], lineNo)) {
                    flag = 0;
                    break;
                }
            }
            if (flag) {
                num3++;
                auto tmpTuple = R.table[i];
                tmpTuple.table = res;
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
        RG *res = new RG(name, a.num1 + b.num1, a.num2 + b.num2, 0);
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
        for (int i = 0; i < a.num2; i++){
            AttrName = a.zero[i+a.num1];
            res -> poi[AttrName] = i;
            res -> zero.push_back(AttrName);
        }
        for (int i = 0; i < b.num2; i++){
            AttrName = b.zero[i+b.num1];
            res -> poi[AttrName] = i + a.num2;
            res -> zero.push_back(AttrName);
        }
        // data maintain
        int flag = 1;
        for (int i = 0; i < a.num3; i++) {
            for (int j = 0; j < b.num3; j++) {
                flag = 1;
                for (auto & condition: conditions) {
                    int lineNo1 = a.attr[condition.attr1];
                    int lineNo2 = b.attr[condition.attr2];
                    if (!CMP(condition, a.table[i], b.table[j], lineNo1, lineNo2)) {
                        flag = 0;
                        break;
                    }
                }
                if (flag) {
                    assert(res->attr.size()==a.num1+b.num1);
                    auto tmpTuple = new Tuple(res, a.table[i], b.table[j]);
                    res->table.push_back(*tmpTuple);
                    tot++;
                }
            }
        }

        res->num3 = tot;
        return res;
    }
    RG* EdgeJoin(RG &a, RG &b/*, vector<Condition>conditions*/) {
        RG *res = new RG();
        return res;
    }
};

RG Calc(int S) {
    return RG(); // for temporary debug
    Step now = Query::Plan[S];
    int S1 = now.S1, S2 = now.S2;
    //vector<Condition> condition = now.conditions;
    //return Do(Calc(S1), Calc(S2), condition, attr);

}

void Output() {

}

namespace Debug {
    void outputRG(RG &a) {
        int num1 = a.num1; // attr
        int num2 = a.num2; // pointer
        int num3 = a.num3; // tuple
        cout << a.name << " addr:" << &a <<endl;
        for (auto i:a.zero) {
            cout << i << ' ';
        }
        cout << endl;
        for (int i=0;i<num3;i++) {
            for(int j=0;j<num1;j++) {
                if (a.attr_type[j] == 0){
                    cout << a.table[i].attribute[j].number << " ";
                } else if(a.attr_type[j] == 1) {
                    cout << (a.table[i].attribute[j].str) << " ";
                } else {
                    cout << "Unknown attribute type!" ;
                }

            }
            for(int j=0;j<num2;j++) {
                cout << "pointer" << " "; // a.table[i].pointerSet[j]
            }
            cout << "  fatherTableAddr: " << a.table[i].table;
            cout << endl;
        }
    }
};

int main() {
    setbuf(stdout, NULL); // for CLion user UUQ
    Init :: Init();
    vector<string>ProjectAttrs;
    ProjectAttrs.emplace_back("table1.name");// means "name"
    ProjectAttrs.emplace_back("table1.id");// means "id"
    /*
     * test examples
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

0
     */
    vector<string>ProjectPointers;
    RG *test = Exert ::Projection(Rgs[0], ProjectAttrs, ProjectPointers);
    Debug::outputRG(*test);

    cout<<endl<<endl;


    vector<SelCondition>SelectionCon;
    SelCondition a{};// .name == "Name1"
    a.attr = "table1.name"; // name
    a.value = "Name1";
    a.cmp = 0;
    SelectionCon.push_back(a);
    RG *testSel = Exert::Selection(Rgs[0], SelectionCon);

    Debug::outputRG(*testSel);
    cout<<endl<<endl;
    RG *testMix = Exert::Projection(*testSel, ProjectAttrs, ProjectPointers);
    Debug::outputRG(*testMix);
    cout<<endl<<endl;


    vector<JoinCondition> JoinConditions;
    JoinCondition jc = {"table1.name", "table2.name", 0}; // name equal
    JoinConditions.push_back(jc);
    RG *testJoin1 = Exert::RGJoin(Rgs[0],Rgs[1],JoinConditions);
    Debug::outputRG(*testJoin1);
    cout<<endl<<endl;


    JoinConditions.clear();
    JoinConditions.push_back({"table1.id","table2.id",3}); // A.id >= B.id
    RG *testJoin2 = Exert::RGJoin(Rgs[0],Rgs[1],JoinConditions);
    Debug::outputRG(*testJoin2);
    cout<<endl<<endl;

    Query :: Query();
    Calc(0);
    Output();
    return 0;
}