#include<bits/stdc++.h>

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
    string name;
    vector<Tuple> table;
    vector<string>zero; // first row: attr name and pointer name;
    map <string, int> attr; // name -> line no.
    map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:str
    map <string, int> poi;
    RG (string name = "EmptyName", int num1 = 0, int num2 = 0, int num3 = 0) {
        table.resize(num3);
        attr.clear();
        poi.clear();
        zero.clear();
        for (int i = 0; i < num3; i++) {
            table[i] = Tuple(num1, num2);
        }
        this->name = name;
        this->num1 = num1;
        this->num2 = num2;
        this->num3 = num3;
    }
};

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
    vector<string> attr; // attr which should be contained
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
                        // already fixed at 23.12.15
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
        Edge(int to = 0, JoinCondition condition = *(new JoinCondition())) : to(to), condition(condition) {}
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
                string attr1, attr2;
                int x, y, a, b, cmp;
                cin >> attr1 >> attr2 >> cmp;
                x = Table[attr1.substr(0, attr1.find_first_of('.'))];
                y = Table[attr1.substr(0, attr2.find_first_of('.'))];
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
                JoinCondition condition;
                condition.attr1 = attr1;
                condition.attr2 = attr2;
                condition.cmp = cmp;
                G[x].push_back(Edge(y, condition));
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
                JoinCondition condition1, condition2;
                condition1.cmp = condition2 .cmp = 4;
                condition1.attr1 = -1, condition2.attr2 = -2;
     //           condition1.attr2 = condition2.attr2 = 0;
                G[y].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(tot, condition1));
                IdtoRg[++tot] = n + 3 * graphId + 2, G.push_back({});
                G[x].push_back(Edge(tot, condition2));
                G[tot].push_back(Edge(tot, condition1));
            }
            int N;
            cin >> N;
            for(int i = 1; i <= N; i++) {
                string attr1, attr2;
                int x, y, a, b, cmp;
                cin >> attr1 >> attr2 >> cmp;
                x = Table[attr1.substr(0, attr1.find_first_of('.'))];
                y = Table[attr1.substr(0, attr2.find_first_of('.'))];
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
                JoinCondition condition;
                condition.attr1 = attr1;
                condition.attr2 = attr2;
                condition.cmp = cmp;
                G[x].push_back(Edge(y, condition));
            }
            cin >> N;
            for(int i = 1; i <= N; i++) {                
                int x, y, a, b, cmp;
                cin >> x >> y >> a >> b >> cmp;
                if(!RgtoId.count(x)) {
                    RgtoId[x] = ++tot;
                    IdtoRg[tot] = x;
                    G.push_back({});
                }
                x = RgtoId[x];
                JoinCondition condition;
                condition.attr1 = a;
                condition.attr2 = b;
                condition.cmp = cmp;
                G[x].push_back(Edge(y, condition));
            } 
        }

    }
    int best;
    vector<int> N;
    vector<long long> Cost;
    vector<Step> Plan;

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
    RG Projection(RG &R, vector<string> attrs, vector<string> pointers) { // if const R, can't access the R.attr[]
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
            attr_name = res->name+'.'+attrs[i];
            line_no = R.attr[R.name+'.'+attrs[i]];
            res->attr[attr_name] = i;
            res->zero.push_back(attr_name);
            res->attr_type[i] = R.attr_type[line_no];
            selected_attr.push_back(line_no); // check the projection attrs' number
        }
        for (int i=0; i < num2;i++) {
            attr_name = R.name+'.'+R.zero[R.num1 + i]; // means the pointers set
            line_no = R.poi[attr_name];
            res->poi[attr_name] = i;
            res->zero.push_back(attr_name);
            selected_poi.push_back(line_no); // pointers number
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

    bool CMP(const SelCondition &condition, const Tuple &a, int lineNo) { // whether the tuple fits the condition.
        int cmpop = condition.cmp;
        //int lineNo = condition.attr;
        if (lineNo >= a.num1) {
            cout << "Select the pointerSet, not supported yet." << endl;
            return false;
        }
        char *con = (char*)malloc(1);
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
    }
    RG Selection(RG &R, vector<SelCondition> &conditions) {
        tmpTableCnt++;
        string name = "__tmpTable" + to_string(tmpTableCnt);
        RG* res = new RG(name, R.num1, R.num2, 0);
        int num1 = R.num1;
        int num2 = R.num2;
        string AttrName;
        int line_no;
        for (int i = 0; i < num1; i++) {
            AttrName = name + '.' + getAttrName(R.zero[i]); // split .?
            res->zero.push_back(AttrName);
            res->attr[AttrName] = i;
        }
        for (int i = 0; i < num2; i++) {
            AttrName = name + '.' + getAttrName(R.zero[i+num1]);
            res->zero.push_back(AttrName);
            res->poi[AttrName] = i+num1;
        }
        res->attr_type = R.attr_type; // because the line_no doesn't change.
                             // need to be rewrite!!!!!!!!
        // Exert selection.
        int tot = R.num3;
        int flag = 1;
        int num3 = 0;
        for (int i = 0; i < tot; i++) {
            flag = 1;
            for (int j = 0; j < conditions.size(); j++) {
                int lineNo = R.attr[R.name+'.'+conditions[j].attr];
                if(!CMP(conditions[j], R.table[i], lineNo)) {
                    flag = 0;
                    break;
                }
            }
            if (flag) {
                num3++;
                (res->table).push_back(R.table[i]); // beware bugs of copy construction ?
            }
        }
        res->num3 = num3;
        return *res;
    }
    RG RGJoin(RG &a, RG &b, vector<JoinCondition>conditions) {
        RG *res = new RG();
        return *res;
    }
    RG EdgeJoin(RG &a, RG &b/*, vector<Condition>conditions*/) {
        RG *res = new RG();
        return *res;
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
            cout << endl;
        }
    }
};

int main() {
    Init :: Init();
    vector<string>ProjectAttrs;
    ProjectAttrs.emplace_back("name");// means "name"
    ProjectAttrs.emplace_back("id");// means "id"
    /*
     * test examples:
      1
      table1
      2 3
      id 0
      name 1
      1 Name
      2 Name1
      3 345
      0
     */
    vector<string>ProjectPointers;
    RG test = Exert ::Projection(Rgs[0], ProjectAttrs, ProjectPointers);
    Debug::outputRG(test);

    cout<<endl<<endl;


    vector<SelCondition>SelectionCon;
    SelCondition a{};// .name == "Name1"
    a.attr = "name"; // name
    a.value = "Name1";
    a.cmp = 0;
    SelectionCon.push_back(a);
    RG testSel = Exert::Selection(Rgs[0], SelectionCon);

    Debug::outputRG(testSel);
    cout<<endl<<endl;
    RG testMix = Exert::Projection(testSel, ProjectAttrs, ProjectPointers);
    Debug::outputRG(testMix);
    Query :: Query();
    Calc(0);
    Output();
    return 0;
}