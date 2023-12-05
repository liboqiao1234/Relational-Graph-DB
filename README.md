# Relational-Graph-DB
Multimoding query based on extended relational model.

## 数据结构表示

### 1. Attr

使用Union实现对不同类型数据（**long long** 和 **char*** ）的统一存储

```C++
union Attr{
    char* str;
    long long number;
};
```

### 2. Tuple

```C++
int num1; // number of attribute
int num2; // number of pointer set
vector<Attr> attribute; // 该元组存储的各个数据
vector<set<Tuple*>> pointerSet; // 该元组存储的指针列，指向其他（一个或多个）元组
```

### 3. RG

```C++
string name; // 表的名字
int num1; // 属性列数量
int num2; // 指针列数量
int num3; // 行数量（元组数量）
vector<Tuple> table; // 存储数据的表

vector<str>zero;
// 表头，zero[i]表示i列属性名（如id、name）；如果子图部分需要，可以在zero[num1]...zero[num1+num2-1]部分存储指针列的名称

map <str, int> attr; // name -> line no. 属性名 到 列号 的映射
map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:char* 该列存储的数据类型；0表示long long，1表示char*
map <str, int> poi; // 指针列的  列名->列号（如果需要）
```

### 4. Condition

#### （1）SelCondition

```C++
string attrNo; // attr name
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


## 修改日志

请在每次提交时候在此处附上本次修改的详细内容

| 修改人 时间     | 修改内容                                                     |
| --------------- | ------------------------------------------------------------ |
| UUQ 11.29 17:15 | 修改Projection传入内容<br>修改Tuple中attr存储逻辑为union{char *, long long}，并初步修改输入（char\*的输入可能有问题） |
| UUQ 11.29 17:49 | 新增Condition类，新增toNumber函数，修改了部分由Union带来的错误 |
| UUQ 12.5 21:20  | 修改了Condition类为两种Condition类，实现了Selection操作（CMP函数）<br>修复了char*的各种bug |
| JBY 12.5        | JoinCondition 新添加代表 edge join 的 CMP, 更新了 Plan 中每一步 Step 的相关描述，增加了对于不同表中同名属性的处理，更改 attr 的表达形式|

## 备注

1. 操作执行的实现假设用户输入的条件（或者来自plan的condition）不存在类型错误（比如明明是char*字段却使用数字相等、不等的条件）
2. Selection暂时不支持对于pointerSet的Select（因为这玩意不知道怎么加条件）
