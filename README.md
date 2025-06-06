<div>
	<a href="">
		<img src="https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white" alt="cpp">
	</a>
	<a href="">
		<img src="https://ci.appveyor.com/api/projects/status/1acb366xfyg3qybk/branch/develop?svg=true" alt="building">
	</a>
	<a href="https://github.com/UF4007/memManager/blob/main/License.txt">
		<img src="https://img.shields.io/badge/license-MIT-blue" alt="MIT">
	</a>
	<a href="https://www.microsoft.com/en-us/windows">
		<img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="win">
	</a>
	<a href="https://www.debian.org/">
		<img src="https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black" alt="linux">
	</a>
	<a href="https://www.espressif.com/en">
		<img src="https://www.espressif.com/sites/all/themes/espressif/logo-black.svg" alt="esp" width="75" height="25">
	</a>
</div>

# eb::manager 整体数据结构管理

- 提供了对有环图、继承等复杂数据结构的整体管理（序列化、跨文件引用、内存标记与全体析构）方案。

- 支持几乎所有STL容器及其任意嵌套：例如 `std::unordered_map<int, std::variant<std::vector< int >, std::string>>`

- 出于数据结构的便捷性与稳定性考虑，不支持多线程。

## 此库为作者在初学C++时制作的库，水平较差，适用范围较小，不建议在彻底了解其原理前使用。

#### 如何使用
这是一个headonly库。把ebManager文件夹复制到源目录下，在源代码中`#include "ebManager/ebManager.h"` 即可

**C++标准：** 17

#### 示例代码

```cpp
#include "ebManager/ebManager.h"

struct testU : public eb::base {			//继承eb::base

	int id;						//定义成员变量
	std::string name;

	void save_fetch(eb::para para) override {	//实现纯虚函数save_fetch，把所有要保存的变量写进去
		GWPP("id", id, para);
		GWPP("name", name, para);
	}

	testU(eb::manager* m) :base(m) {}		//实现此签名的构造函数并传递eb::manager指针

	MEM_PERMISSION					//权限宏(如果是public可以不写)
};

struct testM : public eb::manager {			//继承eb::manager

	std::vector<eb::memPtr<testU>> vec;		//此库能正确序列化memPtr系列智能指针的指向关系
	
	void save_fetch(eb::para para) override {
		GWPP("vec", vec, para);
	}
};

int main(){
	testM* a = new testM();

	a->vec.push_back(new testU(a));
	a->vec.push_back(new testU(a));

	a->vec[0]->id = 42;
	a->vec[0]->name = "Hello world";
	a->vec[1]->id = 36;

	a->setUrl("D:\\test");		//设置一个有效的目录
	a->download();

	testM* b = new testM();		//序列化->反序列化 接抛球
	b->setUrl("D:\\test");
	b->upload();

	if (b->vec.size())
	{
		std::cout << b->vec[0]->id << std::endl;
		std::cout << b->vec[0]->name << std::endl;
		std::cout << b->vec[1]->id << std::endl;
	}
	else
	{
		std::cout << "failed." << std::endl;
	}
}
```

**Demo兼单元测试 :** 

```C++
#include "ebManager/demo.h"

eb_testmain();
```

---  

### 文档目录

- [eb::base](#ebbase)
- [eb::manager](#ebmanager)
- [序列化原理](#序列化原理)
- [反序列化构造原理](#反序列化构造原理)
- [析构原理](#析构原理)
- [JSON序列化与反序列化](#json序列化与反序列化)
- [memPtr系列](#memPtr系列)
  - [dumbPtr](#dumbPtr)
  - [memPtr](#memptr)
  - [impPtr](#impPtr)
- [支持的数据类型](#支持的数据类型)
  - [算术类型与枚举](#算术类型与枚举)
  - [原生数组](#原生数组)
  - [STL容器](#stl容器)
  - [原子量](#原子量)
  - [字符串](#字符串)
  - [memPtr系列指针](#memPtr系列指针)
  - [文件出入口](#文件出入口)
  - [variant](#variant)
  - [pair](#pair)
  - [tuple](#tuple)	（暂不，拟支持）
  - [optional](#optional)
  - [pFunction](#pfunction)
  - [非侵入式，任意结构体的内存直接序列化](#非侵入式任意结构体的内存直接序列化)
  - [子结构体序列化](#子结构体序列化)
  - [自定义二进制序列化](#自定义二进制序列化)
- [跨文件引用：出入口机制](#跨文件引用出入口机制)
  - [Ingress](#Ingress)
  - [pIngress](#pIngress)
  - [Egress](#Egress)
  - [pEgress](#pEgress)
- [文件读写接口](#文件读写接口)
- [线程安全](#线程安全)

---

## eb::base

`eb::base`是ebManager库中所有可管理对象的基类。所有需要被ebManager管理的对象都必须继承自`eb::base`，并实现其纯虚函数`save_fetch`。

### 典型用法

1. 继承eb::base
2. 实现save_fetch函数，在其中用`GWPP`声明所有需要序列化的成员变量
3. 构造函数需接收manager指针并传递给基类

**注意**： `eb::base`的派生类必须使用`new`申请在堆中，若为栈中成员、全局变量、其他结构体成员时，**行为未定义**。堆中的`eb::base`使用memPtr系列指针进行管理。

```cpp
#include "ebManager/ebManager.h"

struct testU : public eb::base {
    int id;
    std::string name;

    // 实现纯虚函数save_fetch，把所有要保存的变量写进去
    void save_fetch(eb::para para) override {
        GWPP("id", id, para);
        GWPP("name", name, para);
    }

    // 需实现此签名的构造函数，传递manager指针
    testU(eb::manager* m) : base(m) {}
};
```

### 说明

- `save_fetch`函数在序列化和反序列化时都会被自动调用，`GWPP`会根据para参数自动判断是读还是写。
- 只需在`save_fetch`中声明需要序列化的成员变量，其他工作由库自动完成。

## eb::manager

`eb::manager`是ebManager库中用于统一管理对象生命周期、序列化和反序列化的核心类。它继承自`eb::base`，代表一个对象集合的"根"，通常对应于一个文件或数据块。所有被管理的对象都需要传递`eb::manager*`进行关联。

### 典型用法

1. 继承`eb::manager`，并实现`save_fetch`函数，声明所有需要管理的成员对象。
2. 使用`download()`将对象树保存到文件，使用`upload()`从文件恢复对象树。
3. 通过`setUrl`设置文件路径。

- 在实际使用中，一个`eb::manager`通常归档为磁盘上的一个文件。

## 序列化原理

### 递归算法（DFS）

ebManager的序列化和反序列化采用**深度优先遍历（DFS）**递归算法。每个对象通过递归调用自身及其成员的`save_fetch`方法，自动按顺序遍历整个对象图。对于容器、嵌套结构、指针等成员，均会递归处理，确保所有数据被完整遍历和序列化。

### 有环图处理机制

- 序列化过程中，库内部维护一个"指针表"或"对象表"，为每个对象分配唯一标识（如内存地址或序列化偏移）。
- 当遇到指针成员（如memPtr）时，先查表：
  - 如果该对象未被序列化，则递归序列化并登记。
  - 如果已序列化，则只写入引用信息，避免重复递归，防止死循环。
- 该机制可安全处理任意复杂的有环图结构，无需用户手动干预。

### eb::base的二进制序列化API

- `void serialize(std::vector<uint8_t>* bc)`
  - 将当前对象（不包括指针成员指向的其他对象）序列化为二进制数据流。
- `bool deserialize(uint8_t* Ptr, uint32_t StringSize)`
  - 从二进制数据流反序列化恢复当前对象（不包括指针成员指向的其他对象）。

**说明**
- 只处理本对象本身，不递归处理指针成员指向的其他对象。
- 适合用于单一对象的存取、调试或自定义场景。

### eb::manager的二进制序列化API

- `void serialize(std::vector<uint8_t>* bc)`
  - 将整个对象树（包括所有成员对象及其引用关系）递归序列化为二进制数据流。适合完整保存和持久化。
- `bool deserialize(uint8_t* Ptr, uint32_t StringSize)`
  - 从二进制数据流反序列化恢复整个对象树及其引用关系。
- `bool download()`
  - 将整个manager及其管理的对象树保存到文件（二进制格式）。
- `bool upload()`
  - 从文件（二进制格式）加载并恢复对象树。
- `template<typename T> void serializeSub(T& sub, std::vector<uint8_t>* bc)`
  - 序列化指定子对象（通常用于部分数据导出）。
- `template<typename T> bool deserializeSub(T& sub, uint8_t* Ptr, uint32_t StringSize)`
  - 反序列化指定子对象。

**说明**
- 会递归处理所有被管理的对象及其引用关系，适合整体数据持久化。
- 使用`eb::manager`的序列化接口进行整体数据管理。

## 反序列化构造原理

ebManager的反序列化过程能够自动恢复复杂对象树、容器、指针关系和有环结构。其核心目标是：不仅恢复数据本身，还能正确重建对象间的引用关系。

### 主要机制

- 反序列化时，首先扫描二进制数据，批量创建所有对象实例，并为每个对象分配唯一标识。
- 对于指针成员（如memPtr），根据序列化时记录的引用信息，查找并恢复指向的目标对象。
- 支持有环图结构：即使对象间存在循环引用，也能正确恢复，不会出现悬空指针或重复构造。
- 对于容器、variant、optional等复杂类型，递归恢复其所有元素和嵌套结构。

### 关键点

- 反序列化过程分为"对象实例创建"和"指针关系恢复"两个阶段，确保所有引用都能正确指向。
- 用户无需手动管理指针或对象关系，库会自动处理所有细节。
- 支持跨文件引用（配合Ingress/Egress机制）。
- 只需调用`eb::manager`或`eb::base`的`deserialize`/`upload`等API，即可自动完成整个对象树的恢复。

## 析构原理

ebManager通过统一的析构机制，确保所有被管理对象能够安全、完整地释放，防止内存泄漏和悬空指针，尤其适用于有环图和复杂引用关系的数据结构。

### 主要机制

- 所有被`eb::manager`管理的对象，都会在manager的红黑树中有记录，并在manager析构时统一析构。
- manager析构将**无序**地遍历其管理的eb::base派生类对象集合，调用每个eb::base派生类对象的虚析构函数，并清理指针表。
- 对于memPtr等智能指针，引用计数归零时自动释放对象，确保循环引用也能被正确析构。
- 子文件（Subfile）、跨文件引用等特殊对象也会被统一管理和析构。

## JSON序列化与反序列化

JSON序列化与反序列化同样支持复杂嵌套结构、容器、指针关系，但是不支持有环图。

### 主要机制

- 通过rapidjson库实现高效的JSON读写。
- 递归遍历对象树，将所有成员变量、容器、指针等自动转换为JSON结构。
- 支持多种时间类型的JSON表达（如秒、毫秒、日期字符串等）。

## memPtr系列

memPtr系列是ebManager自定义的智能指针体系，专为对象间复杂引用关系、序列化和有环图管理而设计。它们在序列化、反序列化、析构等环节中起到核心作用。

所有指针类型均有引用计数和自动析构，防止内存泄漏和悬空指针，但是**都不是线程安全**的。

### dumbPtr

- 用法：`dumbPtr<T>`，T必须继承`eb::base`
- 只做引用计数，不参与序列化和反序列化。
- 适合临时引用场景。
- 不会自动保存和恢复指向关系，生命周期随引用计数自动管理。

### memPtr

- 用法：`memPtr<T>`，T必须继承`eb::base`，且必须实现eb::manager*签名的构造函数
- 支持完整的序列化、反序列化和引用关系恢复。
- 适合需要持久化、自动管理的对象引用，是最常用的指针类型。

### impPtr

- 用法：`impPtr<T>`，T必须继承`eb::base`，T可以是不能构造的多态类型
- 适合多态场景。
- 反序列化时，仅用于引用已存在对象。所以，在序列化遍历到它之前，**需要确保其所指的对象已经创建**，否则没有类型信息（多态），创建不了。

## 支持的数据类型

ebManager支持绝大多数C++常用类型的序列化与反序列化，能够满足复杂数据结构的管理需求。

### 算术类型与枚举

- 所有C++标准算术类型：如`int`、`float`、`double`、`char`、`bool`、`short`、`long`、`unsigned`等。
- 所有枚举类型（enum class、enum），包括带底层类型的枚举。

#### 二进制序列化行为

- 直接以内存二进制形式存储。

#### JSON序列化行为

- 以标准JSON数值类型输出（如整数、浮点数、布尔值）。
- 枚举类型会被序列化为其底层整数值。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("name", var, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    int a;
    float b;
    enum class Color { Red, Green, Blue } color;
    void save_fetch(eb::para para) override {
        GWPP("a", a, para);
        GWPP("b", b, para);
        GWPP("color", color, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "a": 42,
  "b": 3.14,
  "color": 1
}
```

---  

### 原生数组

- 支持所有C++原生定长数组类型，如`int arr[10]`、`float buf[256]`等。
- 支持多维数组（如`int mat[3][3]`），但建议优先使用STL容器。

#### 二进制序列化行为

- 以数组元素逐个递归序列化，最终以紧凑的二进制形式存储。

#### JSON序列化行为

- 以标准JSON数组形式输出，每个元素递归序列化为JSON值。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("name", arr, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    int arr[5] = {1, 2, 3, 4, 5};
    float mat[2][2] = {{1.1f, 2.2f}, {3.3f, 4.4f}};
    void save_fetch(eb::para para) override {
        GWPP("arr", arr, para);
        GWPP("mat", mat, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "arr": [1, 2, 3, 4, 5],
  "mat": [
    [1.1, 2.2],
    [3.3, 4.4]
  ]
}
```

---  

### STL容器

- 支持以下STL标准容器的序列化与反序列化：
  - `std::vector`
  - `std::list`
  - `std::deque`
  - `std::set`
  - `std::unordered_set`
  - `std::map`
  - `std::unordered_map`
  - `std::multiset`
  - `std::unordered_multiset`
  - `std::multimap`
  - `std::unordered_multimap`
  - `std::array`
  - `std::forward_list`
- 容器元素类型可以是任意可序列化类型，支持多级嵌套。

#### 二进制序列化行为

- 以容器元素逐个递归序列化，存储元素数量和内容。

#### JSON序列化行为

- 以标准JSON数组或对象形式输出，元素递归序列化为JSON值。
- `map`/`unordered_map`等以**键值对数组**形式输出，每个元素为`{"First": key, "Second": value}`。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("name", container, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::vector<int> vec;
    std::map<int, std::string> dict;
    std::array<float, 3> arr;
    void save_fetch(eb::para para) override {
        GWPP("vec", vec, para);
        GWPP("dict", dict, para);
        GWPP("arr", arr, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "vec": [1, 2, 3],
  "dict": [
    {"First": 1, "Second": "one"},
    {"First": 2, "Second": "two"}
  ],
  "arr": [1.1, 2.2, 3.3]
}
```

---  

### 原子量

- 支持`std::atomic<T>`类型的序列化与反序列化，T可以是任意可序列化的算术类型（如`int`、`float`、`double`等）。
- 适合需要原子操作的多线程场景（但本库本身不保证线程安全，仅保证原子类型可被序列化）。

#### 二进制序列化行为

- 直接load，存储其当前值。

#### JSON序列化行为

- 以标准JSON数值类型输出，表现与普通算术类型一致。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("name", atom, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::atomic<double> atom;
    void save_fetch(eb::para para) override {
        GWPP("atom", atom, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "atom": 3.1415
}
```

---  

### 字符串

- 支持以下标准字符串类型的序列化与反序列化：
  - std::string
  - std::wstring
  - std::u16string
  - std::u32string

#### 二进制序列化行为

- 以"长度+内容"方式存储字符串，长度为元素个数，内容为实际字符数据（不含结尾0）。
- 宽字符（如std::wstring）会以平台字节序直接存储，跨平台时需注意编码兼容。
- 反序列化时自动恢复字符串内容和长度。

#### JSON序列化行为

- 以标准JSON字符串输出，支持Unicode字符。
- 宽字符和多字节字符串会自动转换为UTF-8编码后输出为JSON字符串。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("name", str, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::string name;
    std::wstring wname;
    void save_fetch(eb::para para) override {
        GWPP("name", name, para);
        GWPP("wname", wname, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "name": "hello",
  "wname": "世界"
}
```

---  

### memPtr系列指针

- 支持eb::memPtr<T>、eb::impPtr<T>、eb::dumbPtr<T>等自定义智能指针类型，T需继承自eb::base。
- 在ebManager中，结构体成员**不能直接嵌套为另一个eb::base子类对象**，只能通过指针（memPtr、impPtr）来管理子对象。

**注意**：若序列化/反序列化递归时，递归到不属于此`ebManager`的`eb::base`子类对象，则忽略。跨文件引用请使用Ingress/Egress机制，或自定义跨文件接口。

#### 二进制序列化行为

- memPtr：自动递归序列化被指对象，反序列化时能主动创建对象。
- impPtr：反序列化时只匹配已存在对象，不新建对象，适合多态场景。
- dumbPtr：仅做引用计数，不参与序列化。
- 反序列化时自动恢复指针关系，支持有环图。

#### JSON序列化行为

- 若指针无效，则序列化为null
- 以对象内容输出，memPtr会递归输出所指对象，impPtr不输出任何内容。
- 不支持有环图（JSON不支持循环结构），如有环则输出字符串“Recurring Object”。
- 若从待序列化结构体出发，存在两个指针指向同一个对象，此时JSON无法正确表达这种结构，这个对象将会被序列化两次，在反序列化时，也会被构造成不同对象！

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("ptr", ptr, para);`声明即可。
- 示例：

```cpp
struct Other : public eb::base {
    size_t id;
    std::string name;
    void save_fetch(eb::para para) override {
        GWPP("id", id, para);
        GWPP("name", name, para);
    }
    Other(eb::manager* m) : base(m) {}
};
struct Example : public eb::base {
    eb::memPtr<Other> ptr;
    eb::memPtr<Other> ptrNull;
    void save_fetch(eb::para para) override {
        GWPP("ptr", ptr, para);
        GWPP("ptrNull", ptrNull, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例（memPtr指向对象）：

```json
{
  "ptr": {
    "id": 1,
    "name": "hello"
  },
  "ptrNull": null
}
```

---  

### 文件出入口

- 指向被另一个`eb::manager`所管理数据的接口。

#### 二进制序列化行为

- 仅在`eb::manager`整体序列化时有效。

#### JSON序列化行为

- 不能序列化出入口。

---  

### variant

- 支持std::variant类型的序列化与反序列化，支持任意可序列化类型的组合。
- 若改变std::variant中类型顺序，序列化索引也将改变，先前保存的数据将无法正确读出。

#### 二进制序列化行为

- 保存当前激活类型的索引和值，反序列化时自动恢复类型和值。

#### JSON序列化行为

- 以对象形式输出，包含类型索引`"Variant Type"`和值`"Value"`。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("v", v, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::variant<int, std::string> v;
    void save_fetch(eb::para para) override {
        GWPP("v", v, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "v": {
    "Variant Type": 1,
    "Value": "hello"
  }
}
```

---  

### pair

- 支持`std::pair<T1, T2>`类型的序列化与反序列化，T1/T2可为任意可序列化类型。

#### 二进制序列化行为

- 依次序列化first和second成员，反序列化时自动恢复。

#### JSON序列化行为

- 以对象形式输出。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("p", p, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::pair<int, std::string> p;
    void save_fetch(eb::para para) override {
        GWPP("p", p, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "p": {
    "First": 1,
    "Second": "hello"
  }
}
```

---  

### tuple

#### （暂不，拟支持）

---  

### optional

- 支持`std::optional<T>`类型的序列化与反序列化，T可为任意可序列化类型。

#### 二进制序列化行为

- 首先序列化一个标志位（是否有值），若有值则序列化内容。
- 反序列化时自动判断并恢复optional的状态和值。

#### JSON序列化行为

- 序列化时：若有值，则输出内容；若无值时，JSON此项不存在。
- 反序列化时：若有值，标志置位；若无值时，清空标志。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("opt", opt, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    std::optional<std::string> opt;
    void save_fetch(eb::para para) override {
        GWPP("opt", opt, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "opt": "hello"
}
```

- 若无值时，JSON此项不存在

---  

### pFunction

- `eb::pFunction<Ret(Args...), ID>`类型实际上是一个简单的32位无符号索引，间接引用一个函数指针。
- 任何需要被pFunction管理的函数指针，必须使用宏`INITIALIZE_PFUNCTION(_Signature,_ID_, ...)`,在全局空间中注册此类型的所有函数指针的索引。
- 索引按以上宏的变参顺序，从1开始依次创建，若改变函数指针的位置，索引也将改变。

#### 二进制序列化行为

- 序列化索引。

#### JSON序列化行为

- 以base64序列化索引。

#### 说明

- 使用方法：只需在`save_fetch`中用`GWPP("f", f, para);`声明即可。
- 示例：

```cpp
struct Example : public eb::base {
    eb::pFunction<void(int)> f;
    void save_fetch(eb::para para) override {
        GWPP("f", f, para);
    }
    Example(eb::manager* m) : base(m) {}
};
INITIALIZE_PFUNCTION(void(int), 0, func)
```

- 序列化为JSON时的样例：

```json
{
  "f": "AAAAAA=="
}
```

---  

### 非侵入式，任意结构体的内存直接序列化

- 支持任意POD（Plain Old Data）类型、C风格结构体、无虚函数/无指针成员的简单结构体。

#### 二进制序列化行为

- 直接以内存块（memcpy）方式序列化整个结构体，按字节存储所有内容。
- 反序列化时直接恢复为原始内存内容。
- 若结构体中包含指针、虚函数、动态分配成员等复杂情况时，将导致程序行为异常。

#### JSON序列化行为

- 以base64序列化字节流。

#### 说明

- 使用方法：在`save_fetch`中用`GWPP_Any("any", anyStruct, para);`声明。
- 示例：

```cpp
struct POD { int a; float b; };
struct Example : public eb::base {
    POD any;
    void save_fetch(eb::para para) override {
        GWPP_Any("any", any, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "any": "R1dQUEFueV8="
}
```

---  

### 子结构体序列化

- 子结构体（sub）定义：任意实现了`void save_fetch_sub(eb::base* b, const char* key, eb::para& p)`成员函数的结构体。
- 在`save_fetch_sub`函数中，使用`GWPP_sub(b, key, "var name", var, p);`函数管理结构体。
- 子结构体必须直接使用`GWPP`函数序列化，**不允许存在`std::vector<Sub>`这样的嵌套**。

#### 行为 & 说明

- 将子结构体进行展开，两个名称拼接在一起。
- 示例：

```cpp
struct Sub { 
    int x;
    std::string y; 
    void save_fetch_sub(eb::base* mu, const char* key, eb::para& para)
    {
        GWPP_sub(mu, key, "x", x, para);
        GWPP_sub(mu, key, "y", y, para);
    }
};
struct Example : public eb::base {
    Sub sub;
    void save_fetch(eb::para para) override {
        GWPP("sub_", sub, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 其序列化/反序列化行为等价于：

```cpp
struct Example : public eb::base {
    int x;
    std::string y; 
    void save_fetch(eb::para para) override {
        GWPP("sub_x", x, para);
        GWPP("sub_y", y, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "sub_x": 1,
  "sub_x": "hello"
}
```

---  

### 自定义二进制序列化

#### 支持范围

- 支持自定义结构体类型，只要实现了成员函数`void save_fetch_struct(uint8_t* content, eb::para& para)`和常量`save_fetch_size`。

#### 二进制序列化行为

- 调用结构体的`save_fetch_struct`方法，由用户自定义序列化/反序列化过程。
- 按`save_fetch_size`指定的字节数进行存储。

#### JSON序列化行为

- 以base64序列化字节流。

#### 说明

- 示例：

```cpp
struct Custom {
    int a;
    double b;
    static constexpr size_t save_fetch_size = sizeof(int) + sizeof(double);
    void save_fetch_struct(uint8_t* content, eb::para para) {
        GWPP_memcpy(content, a, para);
        GWPP_memcpy(content, b, para);
    }
};
struct Example : public eb::base {
    Custom custom;
    void save_fetch(eb::para para) override {
        GWPP("custom", custom, para);
    }
    Example(eb::manager* m) : base(m) {}
};
```

- 序列化为JSON时的样例：

```json
{
  "custom": "R1dQUF9tZW1jcHlf"
}
```

---  

## 跨文件引用：出入口机制

### Ingress

### pIngress

### Egress

### pEgress

## 文件读写接口

## 线程安全

- 单线程使用，包括构造与析构。
