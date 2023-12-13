# Relational-Graph-DB
Multimoding query based on extended relational model.

## 数据结构表示

### 1. Attr

使用Union实现对不同类型数据（**long long** 和 **char*** ）的统一存储

```C++
union Attr{
    char* str;
    long long number;
    void* pointerSet;
};
```

### 2. Tuple

```C++
int num1; // number of attribute
void * table; // 指向所在的table 由于声明比RG前，不得不用void*
vector<Attr> attribute; // 该元组存储的各个数据
```

### 3. RG

```C++
string name; // 表的名字
int num1; // 属性列数量
int num3; // 行数量（元组数量）
vector<Tuple> table; // 存储数据的表

vector<str>zero;
// 表头，zero[i]表示i列属性名（如id、name）；如果子图部分需要，可以在zero[num1]...zero[num1+num2-1]部分存储指针列的名称

map <str, int> attr; // name -> line no. 属性名 到 列号 的映射
map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:char* 该列存储的数据类型；0表示long long，1表示char*
```

### 4. Condition

#### （1）SelCondition

```C++
string attr; // attr name
int CMP; // compare type
char* value; // the constant to be compared
```

#### （2）JoinCondition

```C++
string attr1, attr2; // attr name for RG A and RG B
int CMP;
```

### 5. Step

```c++
int S1, S2; // best csg and cmp set
vector<JoinCondition> conditions; // join condition
vector<string> attr; // attr which should be contained
```




| CMP  | 对应比较操作       |
| ---- | ------------------ |
| 0    | ==constant_String  |
| 1    | == constant_number |
| 2    | <= constant_number |
| 3    | >= constant_number |
| 4    | edge join | 
| -    | < k   可写为<=k-1  |



## 参数传递约定

### 1. 查询条件

```C++
vector<SelCondition>conditions // 其中Condition之间是且关系，见Condition类
```

## 查询操作接口规范

几种操作均在**Exert**命名空间内，请使用如 `Exert::Projection()`格式调用



注意！！！！！！！！！

由于tuple需要存RG* table，所以所有函数必须返回RG*

### 1. Projection

```C++
RG* Projection(const RG &R, vector<int> attrs);
```

### 2. Join

```C++
RG* RGJoin(RG &a, RG &b, vector<JoinCondition> &conditions);
```

定义返回的RG内容：

排列顺序：zero从小到大：a的所有attr，b的所有attr，a的pointer，b的pointer

且此时由于可能出现多个相同的属性名，因此不能直接提取后面的属性名！

## TODO List

1. BuildQueryGraph 维护PointerSet
2. 增加反向指针，用来维护中间结果V、E表变化
3. 每一步需要Projection哪些Attr
4. 实现Do
5. Calc 中添加 projection 的相关计算
6. 实现Join（Edge）
8. Estimation

## 修改日志

请在每次提交时候在此处附上本次修改的详细内容

| 修改人 时间     | 修改内容                                                     |
| --------------- | ------------------------------------------------------------ |
| UUQ 11.29 17:15 | 修改Projection传入内容<br>修改Tuple中attr存储逻辑为union{char *, long long}，并初步修改输入（char\*的输入可能有问题） |
| UUQ 11.29 17:49 | 新增Condition类，新增toNumber函数，修改了部分由Union带来的错误 |
| UUQ 12.5 21:20  | 修改了Condition类为两种Condition类，实现了Selection操作（CMP函数）<br>修复了char*的各种bug |
| JBY 12.5 11:36  | JoinCondition 新添加代表 edge join 的 CMP, 更新了 Plan 中每一步 Step 的相关描述，增加了对于不同表中同名属性的处理，更改 attr 的表达形式 |
| UUQ 12.11 17:50 | 正在修改查询条件由行号->属性名、属性名前增加表名带来的问题，目前还在修复。<br>实现了getAttrName函数，用来从一个"tableName.attrName"中分割出attrName |
| UUQ 12.11 21:55 | 修复了Projection和Selection的bug                             |
| UUQ 12.11 23:05 | 增加Tuple中**void* table**指向所属表，并相应修改所有执行函数的返回类型为**RG*** |
| UUQ 12.12 0:04  | 完成了RGJoin，但产生了新的问题，见文档“**疑问**”第一、二、三条 |
| ST 12.12 2:04   | 新增加了一些代码，只放在readme文件里面，后面可以添加。       |
| JBY 12.12 2:46  | 完成了 BuildQueryGraph 中表的copy和改名，简化了询问的输入形式。完成对best的添加，实现了EmitCsgCmp的框架（仍缺少代价评估， |
| UUQ 12.12 12:25 | 修改关于属性名中表名的问题，约定查询条件的attr字符串中包含"tableName." |
| UUQ 12.12 14:16 | 初步实现edgeJoin，但因为InitGraph输入存在问题未能测试。      |
| JBY 12.12 21:36 | 更改了 union ，实现，pointerSet 和 attribute 的合并，增加对 pointerSet 元素的命名，修复了 InitGrapgh 的 bug |
| UUQ 12.13 1:50  | 修改pointerSet带来的影响，但由于传参可能产生拷贝构造等，edgejoin暂时无法成功匹配 |
| UUQ 12.13 11:27 | 12.13 11:26 成功实现EdgeJoin，但输入逻辑存在疑问：现在的图输入疑似是默认id顺序输入？不论是点id还是边id |
## 疑问

1. edge join是否有两种？  是判断某一个指针set中是否包含另一个元组即可？

## 备注

1. 操作执行的实现假设用户输入的条件（或者来自plan的condition）不存在类型错误（比如明明是char*字段却使用数字相等、不等的条件）
2. Selection暂时不支持对于pointerSet的Select（因为这玩意不知道怎么加条件）
3. 可能的问题：pointerSet的列号是num1+i还是从0开始的i  ——应该从0开始，只不过zero里面合并起来了

4. 主函数
````
   RG result = Calc(best);//我从这儿开始改
   Output(result);
````
5. 外面的
````
RG Selectcolumn(RG a, vector<string> b) {
    //实现筛选出一些列
}

RG Do(RG a, RG b, vector<JoinCondition> c, vector<string> d) {
    RG temp = RGJoin(a, b, c);
    RG result = Selectcolumn(temp, d);//给它一个表，还有一些要保留的列的名字，生成一个新表，怎么去实现？
    return result;
}

//从这儿开始改
RG Calc(int S) {

    /*class Step{
    public:
        int S1, S2; // csg and cmp set
        vector<JoinCondition> conditions; // join condition
        vector<string> attr; // attr which should be contained
    };*/

    if((S & (S - 1)) == 0) {//如果S是一个二进制数，就是简单的那个表
        return id_RG(S);//给它一个数，要输出一个RG表？？？怎么去实现
    }
    else {
        Step now = Query::Plan[S];//Plan的每一个元素是一个step
        int S1 = now.S1, S2 = now.S2;
        vector<Condition> condition = now.conditions;
        return Do(Calc(S1), Calc(S2), condition, attr);
        // return RG(); // for temporary debug
    }
}

#include <fstream>
void Output(RG a) {//对一个表进行输出
    int num1 = a.num1; // attr
    int num2 = a.num2; // pointer
    int num3 = a.num3; // tuple

    // 打开文件以进行写操作，如果文件不存在则创建，如果存在则覆盖
    ofstream outputFile("result.txt");

    // 重定向标准输出流到文件
    streambuf *coutbuf = cout.rdbuf(); // 保存原始的 cout 缓冲区指针
    cout.rdbuf(outputFile.rdbuf()); // 重定向 cout 到文件

    // 输出到控制台和文件
    for (auto i : a.zero) {
        cout << i << ' ';
    }
    cout << endl;
    for (int i = 0; i < num3; i++) {
        for (int j = 0; j < num1; j++) {
            if (a.attr_type[j] == 0) {
                cout << a.table[i].attribute[j].number << " ";
            } else if (a.attr_type[j] == 1) {
                cout << (a.table[i].attribute[j].str) << " ";
            } else {
                cout << "Unknown attribute type!";
            }
        }
        for (int j = 0; j < num2; j++) {
            cout << "pointer" << " "; // a.table[i].pointerSet[j]
        }
        cout << endl;
    }

    // 恢复标准输出流
    cout.rdbuf(coutbuf);

    // 关闭文件
    outputFile.close();
}
````