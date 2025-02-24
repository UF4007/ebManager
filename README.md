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

# ebManager

[English(pending)](README-en.md)  
### 提供了对有环图、继承等复杂数据结构的整体管理（序列化、跨文件引用、内存标记与全体析构）方案。

### 支持几乎所有STL容器及其任意嵌套：例如std::unordered_map<int, std::variant<std::vector< int >, std::string>>

### 出于数据结构的便捷性与稳定性考虑，不支持多线程。

#### 如何使用
这是一个headonly库。把ebManager文件夹复制到源目录下，在源代码中`#include "ebManager/ebManager.h"` 即可

**C++标准：** 17或更高

#### 示例代码

```C++
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

**详细Demo兼单元测试 :** 

```C++
#include "ebManager/demo.h"

eb_testmain();
```

---  

### 文档目录

- [eb::base](#eb::base)
- [eb::manager](#eb::manager)
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
  - [非侵入式，任意结构体的内存直接序列化](#非侵入式，任意结构体的内存直接序列化)
  - [子结构体序列化](#子结构体序列化)
  - [自定义二进制序列化](#自定义二进制序列化)
- [memPtr系列](#memPtr系列)
  - [dumbPtr](#dumbPtr)
  - [memPtr](#memptr)
  - [impPtr](#impPtr)
- [序列化原理](#序列化原理)
- [反序列化构造原理](#反序列化构造原理)
- [析构原理](#析构原理)
- [自定义格式](#自定义格式)
- [JSON序列化与反序列化](#json序列化与反序列化)
- [静态反射](#静态反射)
- [跨文件引用：出入口机制](#跨文件引用出入口机制)
  - [Ingress](#Ingress)
  - [pIngress](#pIngress)
  - [Egress](#Egress)
  - [pEgress](#pEgress)
- [文件读写接口](#文件读写接口)
- [线程安全](#线程安全)
- [使用的库](#使用的库)
- [联系作者](#联系作者)

---

## 线程安全

- 单线程使用，包括构造与析构。
  
### 使用的库：
* **rapidJson**  ：用于JSON支持

### 联系作者：
* 在此项目中提出issue
* 或+QQ：2219647655
