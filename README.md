# Relational-Graph-DB
Multimoding query based on extended relational model.

## 数据结构表示

### 1. Attr

使用Union实现对不同类型数据（**long long** 和 **char*** ）的统一存储

```C++
union Attr{
    char* string;
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
int num1; // 属性列数量
int num2; // 指针列数量
int num3; // 行数量（元组数量）
vector<Tuple> table; // 存储数据的表

vector<string>zero;
// 表头，zero[i]表示i列属性名（如id、name）；如果子图部分需要，可以在zero[num1]...zero[num1+num2-1]部分存储指针列的名称

map <string, int> attr; // name -> line no. 属性名 到 列号 的映射
map <int, int> attr_type; // line no. -> 0/1 0:long long, 1:char* 该列存储的数据类型；0表示long long，1表示char*
map <string, int> poi; // 指针列的  列名->列号（如果需要）
```

### 4. Condition



## 参数传递约定

### 1. 查询条件

```C++
vector<Condition>conditions // 其中Condition之间是且关系，见Condition类
```







## 修改日志

请在每次提交时候在此处附上本次修改的详细内容

| 修改人 时间     | 修改内容                                                     |
| --------------- | ------------------------------------------------------------ |
| UUQ 11.29 17:15 | 修改Projection传入内容<br>修改Tuple中attr存储逻辑为union{char *, long long}，并初步修改输入（char\*的输入可能有问题） |
| UUQ 11.29 17:49 | 新增Condition类，新增toNumber函数，修改了部分由Union带来的错误 |

