//�ڴ���浵����ϵͳ
//Super Super fast json!			------Head-Only------

#pragma once
#define MEM_MANAGER_DISABLE_WARINIGS 4244 4267
#pragma warning(disable: MEM_MANAGER_DISABLE_WARINIGS)
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <cassert>
#include <sstream>
#include <locale>
#include <codecvt>
#include "memUnitMacro.h"
#include <type_traits>
#include <windows.h>

#define NOMINMAX 0

#define ORD_SAVE 0				//�����ݴ��ڴ汣�浽Ӳ��
#define ORD_FETCH 1				//��Ӳ���������ݵ��ڴ�
#define ORD_REFLECTION_R 2		//���������
#define ORD_REFLECTION_W 3		//����д������������
#define ORD_SERIALIZE 4			//���л�
#define ORD_DESERIALIZE 5		//�����л�

#define MEM_EMPTY_EGRESS		3	//�����ǿյ�
#define MEM_NOTFOUND_FILE		2	//����ڰ󶨵����memManagerû�м���
#define MEM_NOTFOUND_KEYWORDS	1	//û�������memManager�ҵ����ڹؼ���
#define MEM_EMPTY_INGRESS		4	//���memManager�ҵ����ؼ����ҵ������������ָ����ڴ��ǿյ�
#define MEM_SUCCESS				0	//�ɹ��ӳ����л�ȡ����

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__) || defined(__powerpc64__) || defined(__s390x__)
#define POINTER_L long long
#define _wtop _wtoll
#define _ptow_s _lltow_s
#else
#define POINTER_L long
#define _wtop _wtol
#define _ptow_s _ltow_s
#endif

inline namespace mem {
inline errno_t _lltow_s(long long value, wchar_t* str, size_t sizeOfstr, const int radix) {
	assert(radix == 10 || !("radix must be 10"));

	std::wstringstream wss;
	wss << value;

	std::wstring wstr = wss.str();
	if (wstr.size() + 1 > sizeOfstr)
		return 1; // Error: Insufficient buffer size

	wcsncpy_s(str, sizeOfstr, wstr.c_str(), sizeOfstr);
	return 0; // Success
}
template <size_t size>
inline errno_t _lltow_s(long long value, wchar_t(&str)[size], const int radix) {
	assert(radix == 10 || !("radix must be 10"));

	std::wstringstream wss;
	wss << value;

	std::wstring wstr = wss.str();
	if (wstr.size() + 1 > size)
		return 1; // Error: Insufficient buffer size

	wcscpy_s(str, size, wstr.c_str());
	return 0; // Success
}

	//ȫ�ֱ���
	const UINT maxURL = MAX_PATH;
	const UINT maxKey = 25;
	const UINT maxKW = 25;
	const UINT defaultWcs = 50;

	//ȫ�֡��ļ����ر�
	class memManager;
	inline HANDLE global_mutex = NULL;
	inline std::vector<memManager*> global_load = {};

	//����ǰ������
	struct memApp;
	class ReflectResultKeyValue;
	struct ReflectResult;
	struct memPara {
		union {
			std::vector<memApp*>* appSegment;
			ReflectResult* reflection;
			ReflectResultKeyValue* reflection_single;
		};
		SHORT order;
		inline bool isConstruct()
		{
			switch (order)
			{
			case ORD_FETCH:
			case ORD_REFLECTION_R:
			case ORD_DESERIALIZE:
				return true;
			}
			return false;
		}
	};
	struct memPtrCorr;
	class memUnit;
	class memWcs;
	template<class mu, bool releaseable = true> class memVector;
	class Egress;
	class Ingress;
	class Subfile;
	template<typename First, typename...Args> class pVariant;
	template<class cast> class memVectorEgress;
	template<typename... Args> class memVectorVariant;
	struct memPtrComm {
		int count;
		memUnit* content;
		inline memPtrComm(memUnit* m)
		{
			count = 1;
			content = m;
		}
	};
	//��ĳ��Ҫ�̳�memPtr����ʹ��vectorʱ������дһ��
	template<typename mu, bool releaseable = true>
	class memPtr{
		memPtrComm* ptr;
		inline memPtr* getPtr()
		{
			return this;
		}
		inline void cdd() {
			if (ptr)
				if (ptr->count)
				{
					if (ptr->count == 1)
					{
						if (ptr->content)
							if (ptr->content->mngr)
								delete(ptr->content);
						delete ptr;
						ptr = NULL;
					}
					else
						(ptr->count)--;
				}
		}
		inline void cdd(memPtrComm* ptrOperation);
		//�˻�������ָ�����ô����Լ���������Ϊ0ʱ���������memUnit����������ʹmemUnit�ص�û�й�������ָ���״̬��
		inline void degeneracy()
		{
			if (ptr)
				if (ptr->count)
				{
					if (ptr->count == 1)
					{
						if (ptr->content)
							ptr->content->sharedPtr = nullptr;
						delete ptr;
					}
					else
						(ptr->count)--;
				}
			ptr = NULL;
		}
		template<class any, bool _r>
		inline void equalAny(const memPtr<any, _r>& mp)
		{
			cdd();
			if (mp.isEmpty())
			{
				ptr = NULL;
			}
			else
			{
				ptr = mp.ptr;
				(ptr->count)++;
			}
		}
	public:
		inline memPtr()
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			ptr = NULL;
		}
		inline memPtr(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			if (pmu)
			{
				if (((memUnit*)pmu)->sharedPtr)
				{
					ptr = ((memUnit*)pmu)->sharedPtr;
					(ptr->count)++;
				}
				else
				{
					ptr = new memPtrComm((memUnit*)pmu);
					((memUnit*)pmu)->sharedPtr = ptr;
				}
			}
			else
			{
				ptr = NULL;
			}
		}
		inline memPtr(const memPtr<mu, releaseable>& mp)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			if (mp.isEmpty())
			{
				ptr = NULL;
			}
			else
			{
				ptr = mp.ptr;
				(ptr->count)++;
			}
		}
		inline ~memPtr(){
			cdd();
		}
		friend class memUnit;
		friend class memManager;
		template<typename First, typename...Args> friend class pVariant;
		template<class cast> friend struct pEgress;
		template<class any, bool _r> friend class memPtr;
		void operator=(const memPtr<mu, releaseable>& mp);
		void operator=(mu* pmu);
		void swap(memPtr& sw);
		inline bool isEmpty() const
		{
			return !(ptr && ptr->content);
		}
		inline bool isFilled() const
		{
			return (ptr && ptr->content);
		}
		inline bool operator==(const memPtr& right) const
		{
			if (isFilled())
				return ptr == right.ptr;
			else if (right.isEmpty())
				return true;
			else
				return false;
		}
		inline bool operator!=(const memPtr& right) const
		{
			return !(this->operator==(right));
		}
		inline mu* operator*() const
		{
			if (isFilled())
				return (mu*)(ptr->content);
			else
				return NULL;
		}
		inline mu* operator->() const
		{
			if (isFilled())
				return (mu*)(ptr->content);
			else
				return NULL;
		}
		inline void release()
		{
			static_assert(releaseable == true, "this memPtr was definded to cannot be release");
			if (isFilled())
			{
				memUnit* i = ptr->content;	//���ﲻ���Ż�����Ȼ��memUnit���������������Ҳ��ִ����ͬ�Ĳ���������C++����ִ������������������������������������������У���ָ��δ��������������δ������Ϊ��
				ptr->content = NULL;
				delete(i);
			}
		}
	};
	template<typename T>
	struct is_template : std::false_type {};
	template<template<typename...> class T, typename... Args>
	struct is_template<T<Args...>> : std::true_type {};
	template<typename T>
	struct extract_template_memUnit { using type = void; };
	template<typename T, bool releaseable>
	struct extract_template_memUnit<memPtr<T, releaseable>> { using type = T; };
	template<typename T>
	struct is_based_on_memPtr {
	private:
		template <class U>
		static std::true_type test(const memPtr<U>&);
		static std::false_type test(...);
	public:
		static constexpr bool value = decltype(test(std::declval<T>()))::value;
	};
	class ReflectResultKeyValue {
		friend class memUnit;
		template<bool _void>
		inline void MatchVariant(void* vtptr, UINT& type, memPtr<memUnit>& ptr)
		{
			type = 0;
			ptr = NULL;
		}
		template<bool _void, typename IterFirst, typename... IterArgs>
		inline void MatchVariant(void* vtptr, UINT& type, memPtr<memUnit>& ptr)
		{
			if (memUnit::get_vtable_ptr<IterFirst>::ptr() == vtptr)
			{
				key[0] = L'\0';
				ptr = value.ptr;
			}
			else
			{
				type++;
				MatchVariant<false, IterArgs...>(vtptr, type, ptr);
			}
		}
		template<bool isSet, class _T, class e_T>
		inline void SetOrGetType(_T& thisSide, _T& memUnitSide, e_T et) {
			if constexpr (isSet)
			{
				thisSide = memUnitSide;
				type = et;
			}
			else
			{
				key[0] = L'\0';
				memUnitSide = thisSide;
			}
		}
		template <bool isSet> inline void TypeFunc(bool& va) { SetOrGetType<isSet>(value._bool, va, bool_); };
		template <bool isSet> inline void TypeFunc(char& va) { SetOrGetType<isSet>(value._char, va, char_);};
		template <bool isSet> inline void TypeFunc(uint8_t& va) { SetOrGetType<isSet>(value.u8, va, u8);};
		template <bool isSet> inline void TypeFunc(int8_t& va) { SetOrGetType<isSet>(value.i8, va, i8);};
		template <bool isSet> inline void TypeFunc(uint16_t& va) { SetOrGetType<isSet>(value.u16, va, u16);};
		template <bool isSet> inline void TypeFunc(int16_t& va) { SetOrGetType<isSet>(value.i16, va, i16);};
		template <bool isSet> inline void TypeFunc(uint32_t& va) { SetOrGetType<isSet>(value.u32, va, u32);};
		template <bool isSet> inline void TypeFunc(int32_t& va) { SetOrGetType<isSet>(value.i32, va, i32);};
		template <bool isSet> inline void TypeFunc(unsigned long& va) { SetOrGetType<isSet>(value.u32, (uint32_t&)va, u32);};
		template <bool isSet> inline void TypeFunc(long& va) { SetOrGetType<isSet>(value.i32, (int32_t&)va, i32);};
		template <bool isSet> inline void TypeFunc(uint64_t& va) { SetOrGetType<isSet>(value.u64, va, u64);};
		template <bool isSet> inline void TypeFunc(int64_t& va) { SetOrGetType<isSet>(value.i64, va, i64);};
		template <bool isSet> inline void TypeFunc(float& va) { SetOrGetType<isSet>(value.f32, va, f32);};
		template <bool isSet> inline void TypeFunc(double& va) { SetOrGetType<isSet>(value.f64, va, f64);};
		template <bool isSet, class _MU, bool re> inline void TypeFunc(memPtr<_MU, re>& va) {
			if constexpr (isSet)
			{
				type = p_memUnit;
				value.ptr = reinterpret_cast<memUnit*>(*va);
			}
			else
			{
				//if (typeid(_MU) == typeid(*value.ptr))
				if(memUnit::get_vtable_ptr<_MU>::ptr() == *(void**)std::addressof(*value.ptr))		//��������RTTI����ʹ�����ָ�����жϡ�
				{
					key[0] = L'\0';
					va = reinterpret_cast<_MU*>(value.ptr);			//Ӳ��ֵ�ڶ��ؼ̳е�ʱ����ܻᱨ������ûд������memUnit*��ֵmemPtr���ȵ����������ٴ���
				}
			}
		};
		template<bool isSet, class First, class ...Args>inline void TypeFunc(pVariant<First, Args...>& va) {
			if constexpr (isSet)
			{
				type = p_variant;
				value.ptr = reinterpret_cast<memUnit*>(*(va.ptr));
			}
			else
			{
				va.type = 1;
				MatchVariant<false, First, Args...>(*(void**)std::addressof(*value.ptr), va.type, va.ptr);
			}
		}
		template<class _T>
		inline void WriteMU(const WCHAR* muProperty_key, _T& muProperty){
			if (wcscmp(muProperty_key, key) == 0)
			{
				TypeFunc<false>(muProperty);
			}
		}
		inline void WriteMU(const WCHAR* muProperty_key, WCHAR* muProperty, UINT size) {
			if (wcscmp(muProperty_key, key) == 0)
			{
				key[0] = L'\0';
				wcscpy_s(muProperty, size, value.wchar);
			}
		}
	public:
		wchar_t key[maxKey];
		enum {
			void_,
			p_wchar,
			bool_,
			char_,
			u8,
			i8,
			u16,
			i16,
			u32,
			i32,
			u64,
			i64,
			f32,
			f64,
			enum_,
			p_memUnit,
			p_variant
			/*p_memVector,
			p_generic,
			p_memVectorVariant,
			p_egress,
			p_egressVector*/
		}type;
		union {
			wchar_t* wchar;
			bool _bool;
			char _char;
			unsigned char u8;
			signed char i8;
			unsigned short u16;
			short i16;
			unsigned int u32;
			int i32;
			unsigned long long u64;
			long long i64;
			float f32;
			double f64;
			memUnit* ptr;
		}value;
		inline ReflectResultKeyValue() {
			key[0] = L'\0';
			type = void_;
			value.u64 = 0;
		}
		template<class _T, typename std::enable_if<!std::is_convertible<_T, const wchar_t*>::value && !std::is_convertible<_T, memUnit*>::value, int>::type = 0>
		inline ReflectResultKeyValue(const WCHAR* _key, _T& va) {
			wcscpy_s(key, _key);
			TypeFunc<true>(va);
		}
		inline ReflectResultKeyValue(const WCHAR* _key, const wchar_t* va) {
			wcscpy_s(key, _key);
			value.wchar = (wchar_t*)va;
			type = p_wchar;
		}
		inline ReflectResultKeyValue(const WCHAR* _key, memUnit* va) {
			wcscpy_s(key, _key);
			value.ptr = va;
			type = p_memUnit;
		}
	};
	struct ReflectResult {
		std::vector<ReflectResultKeyValue> context;
	};
	class memUnit {
		memPtrComm* sharedPtr;
		memManager* mngr;
	public:
		template<class mu, bool releaseable> static mu* memCopy(const memPtr<mu, releaseable>& varMU);
		template<typename T>
		struct get_vtable_ptr {
			static inline void* ptr() {
				static_assert(std::is_polymorphic<T>::value, "T must be a polymorphic type");
				static void* ret = []() {
					if constexpr (std::is_default_constructible<T>::value)	//memManager
					{
						T temp = T();
						return *(void**)std::addressof(temp);
					}
					else													//memUnit
					{
						T temp = T(NULL);
						return *(void**)std::addressof(temp);
					}
				}();
				return ret;
			}
		};
		friend class Ingress;
		friend class Egress;
		friend class Subfile;
		template<class T> friend void GWPP_Struct(memUnit* mu, const WCHAR* key1, const WCHAR* key2, T& var, memPara para);
		friend class memWcs;
		template<class mu, bool releaseable> friend class memPtr;
		template<typename First, typename...Args> friend class pVariant;
		friend class memManager;
		inline memUnit(memManager* manager);
		inline memManager* getManager()
		{
			return mngr;
		}
		inline void reflectionRead(ReflectResult* output)
		{
			memPara para;
			para.reflection = output;
			para.order = ORD_REFLECTION_R;
			this->save_fetch(para);
		}
		//����д�뵥����true ��ʾ�ɹ���false ��ʾʧ��
		inline bool reflectionWrite(ReflectResultKeyValue inputKW)
		{
			memPara para;
			para.reflection_single = &inputKW;
			para.order = ORD_REFLECTION_W;
			this->save_fetch(para);
			return !(inputKW.key[0]);		//���д��ɹ���GWPP�Ὣkey�ĵ�һ���ַ���Ϊ'\0'
		}
		bool deserialize(WCHAR* Ptr, UINT StringSize);
		void serialize(std::vector<WCHAR>* bc);
		virtual ~memUnit();
	protected:
		virtual void save_fetch(memPara para) = 0;
		//fetchInit �� ���캯��������
		//fetchInitÿ��ָһ�ξ�ִ��һ�Σ����캯��ֻ��ִ��һ�Ρ�
		static constexpr BOOL isFetchInit = FALSE;
		virtual void fetchInit() {};
		void GWPP(const WCHAR* key, WCHAR* varWC, UINT size, memPara para);
		void GWPP(const WCHAR* key, bool& var, memPara para);
		void GWPP(const WCHAR* key, char& var, memPara para);
		void GWPP(const WCHAR* key, INT8& var, memPara para);
		void GWPP(const WCHAR* key, unsigned char& var, memPara para);
		void GWPP(const WCHAR* key, short& var, memPara para);
		void GWPP(const WCHAR* key, unsigned short& var, memPara para);
		void GWPP(const WCHAR* key, INT32& var, memPara para);
		void GWPP(const WCHAR* key, UINT32& var, memPara para);
		void GWPP(const WCHAR* key, LONG& varLONG, memPara para);
		void GWPP(const WCHAR* key, ULONG& varLONG, memPara para);
		void GWPP(const WCHAR* key, INT64& varLONG, memPara para);
		void GWPP(const WCHAR* key, UINT64& varLONG, memPara para);
		void GWPP(const WCHAR* key, FLOAT& var, memPara para);
		void GWPP(const WCHAR* key, double& var, memPara para);
		template<class _enum>
		std::enable_if_t<std::is_enum<_enum>::value, void>
		GWPP(const WCHAR* key, _enum& var, memPara para);
		template<typename memStruct>
		std::enable_if_t<!is_based_on_memPtr<memStruct>::value && !std::is_enum<memStruct>::value, void>
		GWPP(const WCHAR* key, memStruct& varMU, memPara para);
		template<class mu, bool releaseable>
		void GWPP(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para);
		template<typename First, typename...Args>
		void GWPP(const WCHAR* key, pVariant<First, Args...>& varGe, memPara para);
		template<class ptr_in_Vec>
		void GWPP_Array(std::vector<ptr_in_Vec>* vec, memPara para);
	private:
		template<class mu, bool releaseable>
		void GWPP_Passive(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para);
		inline memUnit* operator&()
		{
			return NULL;
		}
		//���pVariantָ����memManager���memUnitΪ����������ͻᱨ��										The foolish package manager in C++ forced me to write this foolish function.
		template<class mu> inline static mu* getNewClass(memManager* m) { return new mu(m); }
	};
	class memManager :public memUnit {
	public:
		template<class mu, bool releaseable> friend class memPtr;
		friend class Ingress;
		friend class Subfile;
		friend class memUnit;
		memManager();
		virtual ~memManager();
		WCHAR url[maxURL];
		BOOL download();
		BOOL upload();
		template<class mu> memPtr<Egress> makeEgress_IngressPair(const memPtr<mu>& target, const WCHAR* kw);
		memPtr<Subfile> findSubfile(const WCHAR* url);
		memPtr<Egress> findEgress(const memPtr<Subfile> subfile, const WCHAR* kw, const WCHAR* type);
		memPtr<Ingress> findIngress(const WCHAR* kw, const WCHAR* type);
		memPtr<Ingress> findIngressDangling();
		inline void setUrl(const WCHAR* wcptr) {
			wcscpy_s(url, wcptr);
		}
		// ���urlΪ���򷵻�nullptr
		inline WCHAR* getFileName()
		{
			if (url[0])
				return &wcsrchr(url, L'\\')[1];
			return nullptr;
		}
		template<SHORT para_order = ORD_DESERIALIZE>
		bool deserialize(WCHAR* Ptr, UINT StringSize);
		template<SHORT para_order = ORD_SERIALIZE>
		void serialize(std::vector<WCHAR>* bc);
	private:
		void thisCons();
		void thisDest();
		virtual void save_fetch(memPara para) = 0;
		std::set<memUnit*>* memUnits;
		memVector<Subfile>* subFiles;
		memVector<Ingress>* ingressInte;
		//
		std::set<memPtrCorr>* ptrTable = NULL;
		std::vector<memApp*>* findPtr(memUnit* ptrINI);
		std::vector<memApp*>* findPtr(memUnit* ptrINI, memPtr<memUnit>* self);
		std::vector<memApp*>* findPtr(memUnit* ptrINI, memPtrCorr** ptr);
		std::vector<memApp*>* pushPtr(memUnit* ptrINI);
		VOID deletePtrTable();
		void RawSerialize(std::vector<WCHAR>* bc);
		void RawDeserialize(WCHAR* seriStr, UINT size);
	};
	template<class mu, bool releaseable = true> using pmemVector = memPtr<memVector<mu, releaseable>>;
	template<class cast> using pmemVectorEgress = memPtr<memVectorEgress<cast>>;
	template<typename... Args> using pmemVectorVariant = memPtr<memVectorVariant<Args...>>;
	//�������ݵ�Ԫ��vector
	//��STL��vector������memVector�У���template�������memUnit������������ƣ������д���Ǵ��������memPtr
	template<class mu, bool releaseable>
	class memVector :public std::vector<memPtr<mu, releaseable>> ,public memUnit {
	protected:
		inline void save_fetch(memPara para)override {
			GWPP_Array(this, para);
		}
		friend class memUnit;
		friend class memManager;
	public:
		inline memVector(memManager* manager) :memUnit(manager) {
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base on memUnit");
		}
	};
	class memWcs {
		void save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para);
		friend class memUnit;
		std::vector<WCHAR> wc;
	public:
		memWcs() { wc = std::vector<WCHAR>(defaultWcs); }
		inline virtual ~memWcs(){}
		inline memWcs(const WCHAR* init, UINT initSize)
		{
			wc = std::vector<WCHAR>(initSize);
			wcscpy_s(&wc[0], initSize, init);
		}
		inline void operator=(const WCHAR* wcptr) {
			if (wcslen(wcptr) + 1 > wc.size())
			{
				wc.resize(wcslen(wcptr) + 1);
			}
			wcscpy_s(&wc[0], wc.capacity(), wcptr);
		}
		//��ָ�벻�ó��ڱ���
		inline WCHAR* operator*()
		{
			return &wc[0];
		}
		inline WCHAR* getWcharPtr()
		{
			return &wc[0];
		}
		inline size_t capacity()
		{
			return wc.size();
		}
		inline VOID resize(const size_t size)
		{
			wc.resize(size);
		}
	};
	struct memPtrCorr {
		memUnit* ptrINI;
		memPtr<memUnit>* ptrRUN;
		std::vector<memApp*>* appSegment;
		inline bool operator<(const memPtrCorr& ptr) const
		{
			return ptr.ptrINI < ptrINI;
		}
	};
	struct memApp
	{
		//ǧ��Ҫ�ĳ�map������
		WCHAR key[maxKey];
		std::vector<WCHAR>* value;
	};
	template<class cast> struct pEgress;
	class Egress :public memUnit {
		friend class memUnit;
		friend class memManager;
		friend class Subfile;
		friend class Ingress;
		template<class cast> friend struct pEgress;
		inline Egress(memManager* m) :memUnit(m) {};
		inline Egress(memManager* m, Subfile* upLevel) :memUnit(m) {
			ptrUpLevel = upLevel;
		};
		~Egress();
		WCHAR keywords[maxKW];
		WCHAR type[maxKW];
		void* ptrDirect = NULL;
		Ingress* ptrInge = NULL;
		Subfile* ptrUpLevel;	//��Ҫ����fetchʱδ���壬�����SubFile.save_fetch������ֵ��
		inline void save_fetch(memPara para) {
			GWPP(L"keyword", keywords, maxKW, para);
			GWPP(L"type", type, maxKW, para);
		}
		template<typename cast>
		UINT getTarget(cast*& varReturn);
	};
	class Ingress :public memUnit {
		friend class memUnit;
		friend class memManager;
		friend class Egress;
		inline Ingress(memManager* m) :memUnit(m) {};
		inline ~Ingress()
		{
			for (auto i : pointedByEgress)
			{
				i->ptrDirect = NULL;
				i->ptrInge = NULL;
			}
			memManager* m = this->mngr;
			if (this->getManager())
			{
				auto i = find(this->getManager()->ingressInte->begin(), this->getManager()->ingressInte->end(), this);
				if (*i == this)
				{
					this->mngr = NULL;
					m->ingressInte->erase(i);
				}
			}
			this->mngr = m;
		}
		WCHAR keywords[maxKW];
		WCHAR type[maxKW];
		memPtr<memUnit> mu;
		std::vector<Egress*> pointedByEgress;
		inline void save_fetch(memPara para)
		{
			//GWPP(L"keyword", keywords, maxKW, para);
			//GWPP(L"type", type, maxKW, para);
			GWPP_Passive(L"mu", mu, para);
		}
	};
	class Subfile :private memVector<Egress> {
		friend class memUnit;
		friend class memManager;
		friend class Egress;
		inline Subfile(memManager* m) :memVector<Egress>(m) {};
		inline ~Subfile()
		{
			memManager* m = this->mngr;
			if (this->getManager())
			{
				auto i = find(this->getManager()->subFiles->begin(), this->getManager()->subFiles->end(), this);
				if (*i == this)
				{
					this->mngr = NULL;
					m->subFiles->erase(i);
				}
			}
			this->mngr = m;
			for (int i = 0; i < this->size(); i++)
			{
				memVector<Egress>::operator[](i).release();
			}
		}
		WCHAR fileName[maxURL];
		memPtr<memManager> ptrManager = NULL;
		inline void save_fetch(memPara para) override{
			memVector<Egress>::save_fetch(para);
			if(para.order == ORD_FETCH)
				for (auto i : *this)
					i->ptrUpLevel = this;
			GWPP(L"fileName", fileName, maxURL, para);
		}
		//1 for success, 0 for fail
		inline int findGlobalManager()
		{
			WaitForSingleObject(global_mutex, INFINITE);
			for (auto i : global_load)
			{
				WCHAR* fn = i->getFileName();
				if (fn == nullptr)
					continue;
				if (wcscmp(fn, fileName) == 0)
				{
					ptrManager = i;
					ReleaseMutex(global_mutex);
					return 1;
				}
			}
			ReleaseMutex(global_mutex);
			return 0;
		}
	};
	//egress smart pointer wrapped by template
	template<class cast>
	struct pEgress :public memPtr<Egress> {
		inline void getFileName(WCHAR* wc, UINT length)
		{
			UINT i = length > maxURL ? maxURL : length;
			Subfile& subf = *(Egress::ptrUpLevel);
			wcscpy_s(wc, i, subf.fileName);
			return;
		}
		//getTarget��ȫ�ֲ���Ŀ��memManager����Ҫ���Ƚ�memManager upload���ڴ棬����get
		inline UINT getTarget(cast*& varReturn) {
			if (memPtr<Egress>::isFilled())
			{
				Egress& egr = *(Egress*)(memPtr<Egress>::ptr->content);
				return egr.getTarget(varReturn);
			}
			varReturn = NULL;
			return MEM_EMPTY_EGRESS;
		}
		//��ָ��memUnit��memPtr��ʽ���Σ�ʵ�ʱ�Egressָ�����memPtr��ָ��memUnit��
		inline void makeEIPair(memManager* egressMngr, const memPtr<cast>& target, const WCHAR* kw) {
			memPtr<Egress> pegr = egressMngr->makeEgress_IngressPair(target, kw);
			this->memPtr<Egress>::operator=(pegr);
		}
		inline pEgress() {}
		inline pEgress(memManager* egressMngr, const memPtr<cast>& target, const WCHAR* kw) {
			this->makeEIPair(egressMngr, target, kw);
		}
	};
	template<class cast>
	class memVectorEgress :public std::vector<pEgress<cast>>, public memUnit {
	protected:
		inline void save_fetch(memPara para) override {
			GWPP_Array(this, para);
		}
		friend class memUnit;
		friend class memManager;
	public:
		inline memVectorEgress(memManager* manager) :memUnit(manager) {
			static_assert(std::is_base_of_v<memUnit, cast>, "template is not Base on memUnit");
		}
	};
	template<typename First, typename...Args>
	class pVariant final {				//��������Ҫ�̳�pVariant�ɣ��뱻ģ�������
		UINT type;							//��1��ʼ�����Ǵ�0��ʼ
		memPtr<memUnit> ptr;
		template<typename Target, UINT i>
		constexpr inline UINT getTypeIter()
		{
			assert(!("template not match."));
			return 0;
		}
		template<typename Target, UINT i, typename IterFirst, typename... IterArgs>
		constexpr inline UINT getTypeIter()
		{
			static_assert(std::is_base_of_v<memUnit, IterFirst>, "template is not Base of memUnit");
			if constexpr (std::is_same_v<IterFirst, Target>)
			{
				return i;
			}
			else
			{
				return getTypeIter<Target, i + 1, IterArgs...>();
			}
		}
		template<typename Target>
		inline void setType()
		{
			static const UINT targetType = getTypeIter<Target, 1, First, Args...>();
			type = targetType;
		}
		template<bool _void>
		inline void createIter(UINT i, memManager* mngr)
		{
			assert(!("An error has happened when creating pVariant from an archive file. \n Matched type not found."));
		}
		template<bool _void, typename IterFirst, typename... IterArgs>
		inline void createIter(UINT i, memManager* mngr)
		{
			static_assert(std::is_base_of_v<memUnit, IterFirst>, "template is not Base of memUnit");
			if (i == 1)
			{
				ptr = memUnit::getNewClass<IterFirst>(mngr);
			}
			else
			{
				createIter<false, IterArgs...>(i - 1, mngr);
			}
			return;
		}
		inline void createClassByTypeID(memManager* mngr)
		{
			createIter<false, First, Args...>(type, mngr);
		}
	public:
		friend class memUnit;
		friend class ReflectResultKeyValue;
		inline pVariant()
		{
			type = 0;
			ptr = nullptr;
		}
		inline pVariant(std::nullptr_t) {
			type = 0;
			ptr = nullptr;
		}
		template<typename mu> inline pVariant(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			setType<mu>();
			ptr = memPtr<memUnit>(pmu);
		}
		template<typename mu, bool releaseable> inline pVariant(const memPtr<mu, releaseable>& mp)
		{
			setType<mu>();
			ptr.equalAny(mp);
		}
		//ptr will execute its destructor automatically
		template<typename mu, bool releaseable> inline void operator=(const memPtr<mu, releaseable>& mp)
		{
			setType<mu>();
			ptr.cdd();
			if (mp.isEmpty())
			{
				ptr.ptr = NULL;
			}
			else
			{
				ptr.ptr = mp.ptr;
				(ptr.ptr->count)++;
			}
		}
		template<typename mu> inline void operator=(mu* pmu)
		{
			static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
			setType<mu>();
			ptr.cdd();
			if (pmu)
			{
				if (((memUnit*)pmu)->sharedPtr)
				{
					ptr.ptr = ((memUnit*)pmu)->sharedPtr;
					(ptr.ptr->count)++;
				}
				else
				{
					ptr.ptr = new memPtrComm((memUnit*)pmu);
					((memUnit*)pmu)->sharedPtr = ptr.ptr;
				}
			}
			else
			{
				ptr.ptr = NULL;
			}
		}
		inline void operator=(const UINT i)
		{
			assert(i == 0 || !("The parameter cannot be non-zero int. This overload is used to clean the class."));
			type = 0;
			ptr = NULL;
		}
		inline bool operator==(const pVariant<First, Args...> mp)const {
			return ptr == mp.ptr;
		}
		inline bool isEmpty() const
		{
			return ptr.isEmpty();
		}
		inline bool isFilled() const
		{
			return ptr.isFilled();
		}
		template<typename Target> inline bool getPtr(Target*& target)
		{
			const UINT pointerType = getTypeIter<Target, 1, First, Args...>();
			if (type && pointerType == type)
			{
				target = (Target*)ptr.operator*();
				return 1;
			}
			else
			{
				return 0;
			}
		}
		inline void release()
		{
			type = 0;
			ptr.release();
		}
	};
	//tuple
	template<typename... Args> class memVectorVariant :public std::vector<pVariant<Args...>>, public memUnit {
	protected:
		inline void save_fetch(memPara para) override{
			GWPP_Array(this, para);
		}
		friend class memUnit;
	public:
		inline memVectorVariant(memManager* manager) :memUnit(manager) {}
	};
	//variant of function pointer
	template<typename illg, int ID = 0> class pFunction { static_assert(!std::is_same_v<illg, illg>, "not a function"); };
	template<typename Ret, typename ...Args, int ID> class pFunction<Ret(Args...), ID> {
		friend class memUnit;
		using funcPtr = Ret(*)(Args...);
		UINT type;
		inline void save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para)
		{
			GWPP_Struct(mu, key, L"f:", type, para);
		}
		inline static constexpr UINT getSizeofFunctions()
		{
			//Functions�൱�ڴ�pFunction��variant������ 
			//���û�г�ʼ������ģ��ʵ����Functions����ô���ｫ�ᱨ��
			static constexpr UINT ret = sizeof(Functions);
			return ret;
		}
	public:
		static const funcPtr Functions[];
		inline Ret operator()(Args...args)const {
			assert(type < getSizeofFunctions() / sizeof(funcPtr) || !("pFunction is empty or invaild"));
			return Functions[type](std::forward<Args>(args)...);
		}
		inline pFunction() { type = 0xffffffff; }
		inline pFunction(funcPtr newPtr) { this->operator=(newPtr); }
		inline void operator=(funcPtr newPtr) {
			//����ָ���Ȼû��constexpr������
			for (UINT i = 0; i < getSizeofFunctions() / sizeof(funcPtr); i++)
			{
				if (newPtr == Functions[i]) {
					type = i;
					return;
				}
			}
			assert(false || !("this function is not match in function list"));
		}
		inline bool operator==(const pFunction<Ret(Args...), ID> mp)const {
			if (type == mp.type)
				return true;
			return false;
		}
		inline bool operator==(funcPtr ptr)const {
			assert(type < getSizeofFunctions() / sizeof(funcPtr) || !("pFunction is empty or invaild"));
			if (ptr == Functions[type])
				return true;
			return false;
		}
		inline bool isFilled()const {
			return type < getSizeofFunctions() / sizeof(funcPtr);
		}
		inline bool isEmpty()const {
			return !this->isFilled();
		}
	};
	//���ڳ�ʼ��pFunction�ڲ���variant�ĺ�
#define INITIALIZE_PFUNCTION(Signature,ID, ...) template<> const mem::pFunction<Signature,ID>::funcPtr mem::pFunction<Signature,ID>::Functions[] = { __VA_ARGS__ };
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, DWORD nSize, LPCTSTR lpString);
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, std::vector<WCHAR>& wc);
	inline BOOL WritePrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, LPCTSTR lpString);
	inline void PushWcharVector(std::vector<WCHAR>* bc, const WCHAR* wc, UINT len);
	
	//�������ݵ�Ԫ
	//������Ҫ�洢��Ӳ�̵��࣬����̳��Դ��ࡣ
	inline memUnit::memUnit(memManager* manager) {
		//if manager is null the memUnit is a memManager
		if (manager)
		{
			manager->memUnits->insert(this);
			mngr = manager;
		}
		sharedPtr = NULL;
	}
	inline memUnit::~memUnit()
	{
		if (mngr)
		{
			auto i = mngr->memUnits->find(this);
			if (*i == this)
				mngr->memUnits->erase(i);
		}
		if (sharedPtr)
			sharedPtr->content = NULL;
	}



	//��д/����/���л�һ�庯��
	inline void memUnit::GWPP(const WCHAR* key, WCHAR* varWC, UINT size, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
			GetPrivateProfileStringW(para.appSegment, key, size, varWC);
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
			WritePrivateProfileStringW(para.appSegment, key, varWC);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varWC);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varWC, size);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, bool& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_SERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, char& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[5];
			GetPrivateProfileStringW(para.appSegment, key, 5, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[5];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, unsigned char& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT8& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (char&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, short& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[7];
			GetPrivateProfileStringW(para.appSegment, key, 7, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[7];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, unsigned short& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (short&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT32& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[12];
			GetPrivateProfileStringW(para.appSegment, key, 12, wc);
			var = _wtoi(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[12];
			_itow_s(var, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, UINT32& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, LONG& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, ULONG& var, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (INT32&)var, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, INT64& varLONG, memPara para)
	{
		switch (para.order)
		{
		case ORD_SERIALIZE:
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		case ORD_SAVE:
			GWPP(key, (UINT64&)varLONG, para);
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varLONG);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varLONG);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, UINT64& varLONG, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[22];
			GetPrivateProfileStringW(para.appSegment, key, 22, wc);
			varLONG = _wtoll(wc);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[22];
			_lltow_s(varLONG, wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varLONG);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varLONG);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, float& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[12];
			GetPrivateProfileStringW(para.appSegment, key, 12, wc);
			int a = _wtoi(wc);
			var = reinterpret_cast<float&>(a);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[12];
			_itow_s(reinterpret_cast<int&>(var), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	inline void memUnit::GWPP(const WCHAR* key, double& var, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[22];
			GetPrivateProfileStringW(para.appSegment, key, 22, wc);
			long long a = _wtoll(wc);
			var = reinterpret_cast<double&>(a);
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			WCHAR wc[22];
			_itow_s(reinterpret_cast<long long&>(var), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, var);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, var);
			break;
		}
	}
	//��дö��
	template<class _enum>
	inline std::enable_if_t<std::is_enum<_enum>::value, void>
	memUnit::GWPP(const WCHAR* key, _enum& var, memPara para) {
		GWPP(key, reinterpret_cast<std::underlying_type<_enum>::type&>(var), para);
	}
	//��д�Զ���ṹ�塣Ҫ��ýṹ��ʵ��save_fetch_struct����
	template<typename memStruct>
	//��麯���Ƿ�ΪmemPtr�����࣬��������ô�ģ�庯��
	inline std::enable_if_t<!is_based_on_memPtr<memStruct>::value && !std::is_enum<memStruct>::value, void>
	memUnit::GWPP(const WCHAR* key, memStruct& varST, memPara para)
	{
		//�Զ���ṹ����û��save_fetch_struct()����������ᱨ��
		varST.save_fetch_struct(this, key, para);
	}
	//��дָ��
	template<class mu, bool releaseable>
	inline void memUnit::GWPP(const WCHAR* key, memPtr<mu, releaseable>&varMU, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:				//��
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varMU = memPtr<mu, releaseable>();
				return;
			}
			memUnit* ptrINI = (memUnit*)_wtop(wc);
			memPtrCorr* ptr;
			para.appSegment = this->mngr->findPtr(ptrINI, &ptr);
			if (ptr->ptrRUN)
			{
				varMU = *((memPtr<mu, releaseable>*)ptr->ptrRUN);
			}
			else
			{
				varMU = new mu(this->mngr);
				ptr->ptrRUN = (memPtr<memUnit>*)varMU.getPtr();
				varMU->save_fetch(para);
			}
			if (varMU->isFetchInit)
				varMU->fetchInit();
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:				//д
		{
			if (varMU.isEmpty())return;
			assert(varMU->mngr == this->mngr || !("False: memUnit`s memManager cannot be cross."));
			WCHAR wc[20];
			_ptow_s((POINTER_L)(varMU.ptr->content), wc, 10);
			WritePrivateProfileStringW(para.appSegment, key, wc);
			if (this->mngr->findPtr(varMU.ptr->content) == NULL)
			{
				para.appSegment = this->mngr->pushPtr(varMU.ptr->content);
				varMU->save_fetch(para);
			}
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varMU);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varMU);
			break;
		}
	}
	//��д����ָ��
	template<typename First, typename...Args>
	inline void memUnit::GWPP(const WCHAR* key, pVariant<First, Args...>& varGe, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varGe = NULL;
				return;
			}
			WCHAR* wcPtr = wcschr(wc, L'g') + 1;
			memUnit* ptrINI = (memUnit*)_wtop(wcPtr);
			*(wcPtr - 1) = L'\0';
			memPtrCorr* ptr;
			para.appSegment = this->mngr->findPtr(ptrINI, &ptr);
			varGe.type = _wtol(wc);
			if (ptr->ptrRUN)
			{
				varGe.ptr = *(ptr->ptrRUN);
			}
			else
			{
				varGe.createClassByTypeID(this->mngr);
				ptr->ptrRUN = (memPtr<memUnit>*)varGe.ptr.getPtr();
				varGe.ptr->save_fetch(para);
			}
			if (varGe.ptr->isFetchInit)
				varGe.ptr->fetchInit();
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{					//д
			if (varGe.isEmpty())return;
			assert(varGe.ptr->mngr == this->mngr || !("False: memUnit`s memManager cannot be cross."));
			WCHAR wc[20];
			WCHAR typeWc[20];
			_ptow_s((POINTER_L)(varGe.ptr.ptr->content), wc, 10);
			_ltow_s((UINT)(varGe.type), typeWc, 10);
			UINT8 len = wcslen(typeWc);
			typeWc[len] = L'g';
			typeWc[len + 1] = L'\0';
			wcscat_s(typeWc, wc);
			WritePrivateProfileStringW(para.appSegment, key, typeWc);
			if (this->mngr->findPtr(varGe.ptr.ptr->content) == NULL)
			{
				para.appSegment = this->mngr->pushPtr(varGe.ptr.ptr->content);
				varGe.ptr->save_fetch(para);
			}
		}
			break;
		case ORD_REFLECTION_R:
			para.reflection->context.emplace_back(key, varGe);
			break;
		case ORD_REFLECTION_W:
			para.reflection_single->WriteMU(key, varGe);
			break;
		}
	}
	//��д����
	template<class ptr_in_Vec>
	inline void memUnit::GWPP_Array(std::vector<ptr_in_Vec>* vec, memPara para)
	{
		WCHAR wc[10];
		unsigned int k = 0;
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			this->GWPP(L"size", k, para);
			vec->resize(k);
			for (unsigned int i = 0; i < k; i++)
			{
				_itow_s(i, wc, 10);
				this->GWPP(wc, vec->at(i), para);
			}
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			for (auto i : *vec)
			{
				if (i.isFilled())
				{
					_itow_s(k, wc, 10);
					this->GWPP(wc, i, para);
					k++;
				}
			}
			this->GWPP(L"size", k, para);
		}
			break;
		case ORD_REFLECTION_R:
		{
			for (auto i : *vec)
			{
				if (i.isFilled())
				{
					_itow_s(k, wc, 10);
					para.reflection->context.emplace_back(wc, i);
					k++;
				}
			}
			para.reflection->context.emplace_back(L"size", k);
		}
			break;
		case ORD_REFLECTION_W:
		{
			for (unsigned int i = 0; i < vec->size(); i++)
			{
				_itow_s(i, wc, 10);
				para.reflection_single->WriteMU(wc, vec->at(i));
			}
		}
			break;
		}
	}
	//�������ݵ�Ԫ��wchar[]
	inline void memWcs::save_fetch_struct(memUnit* mu, const WCHAR* key, memPara para) {
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
			GetPrivateProfileStringW(para.appSegment, key, wc);
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
			mu->GWPP(key, &wc[0], wc.size(), para);
			break;
		case ORD_REFLECTION_R:
			mu->GWPP(key, &wc[0], wc.size(), para);
			break;
		case ORD_REFLECTION_W:
		{
			if (wcscmp(para.reflection_single->key, key) == 0)
			{
				UINT lenWc = wcslen(para.reflection_single->value.wchar) + 1;
				if (lenWc > wc.size())
				{
					wc.resize(lenWc);
				}
				para.reflection_single->key[0] = L'\0';
				wcscpy_s(&wc[0], wc.size(), para.reflection_single->value.wchar);
			}
		}
			break;
		}
	}
	//���ڲ�����������ʽ��дָ�룺ֻ�������������ң��Ҳ����͸���ֵ��
	template<class mu, bool releaseable>
	inline void memUnit::GWPP_Passive(const WCHAR* key, memPtr<mu, releaseable>& varMU, memPara para)
	{
		assert(wcslen(key) < maxKey || !("variable key is too long."));
		switch (para.order)
		{
		case ORD_DESERIALIZE:
		case ORD_FETCH:
		{
			WCHAR wc[20];
			if (GetPrivateProfileStringW(para.appSegment, key, 20, wc) == 0)
			{
				varMU = memPtr<mu, releaseable>();
				return;
			}
			memUnit* ptrINI = (memUnit*)_wtop(wc);
			memPtrCorr* ptr;
			para.appSegment = mngr->findPtr(ptrINI, &ptr);
			if (ptr->ptrRUN)
			{
				varMU.ptr = ptr->ptrRUN->ptr;
				varMU.ptr->count++;
				if (varMU->isFetchInit)
					varMU->fetchInit();
			}
		}
			break;
		case ORD_SERIALIZE:
		case ORD_SAVE:
		{
			if (varMU.isEmpty())return;
			if (mngr->findPtr(varMU.ptr->content))
			{
				WCHAR wc[20];
				_ptow_s((POINTER_L)(varMU.ptr->content), wc, 10);
				WritePrivateProfileStringW(para.appSegment, key, wc);
			}
		}
			break;
		case ORD_REFLECTION_R:
			break;
		case ORD_REFLECTION_W:
			break;
		}
	}



	//��ȫ�ֺ������ڸ��Ƶ���memUnit���Ҳ��ܿ�ԽmemManager
	template<class mu, bool releaseable>
	inline mu* memUnit::memCopy(const memPtr<mu, releaseable>& varMU)
	{
		static_assert(std::is_base_of_v<memUnit, mu>, "template is not Base of memUnit");
		static_assert(std::is_base_of_v<memManager, mu> == false, "cannot be Base of memManager. Please use memManager->copy() to copy whole memManager.");
		mu& varMuReference = **varMU;
		mu* ret = new mu(varMuReference);
		ret->sharedPtr = NULL;
		varMU->getManager()->memUnits->insert(ret);
		return ret;
	}

	//������release������ָ��
	//�����߳�������Ҫʱ��д
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::cdd(memPtrComm* ptrOperation)
	{
		if (ptrOperation)
			if (ptrOperation->count)
			{
				if (ptrOperation->count == 1)
				{
					if (ptrOperation->content)
						if (ptrOperation->content->mngr)
							delete(ptrOperation->content);
					delete ptrOperation;
					ptrOperation = NULL;
				}
				else
					(ptrOperation->count)--;
			}
	}
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::operator=(const memPtr<mu, releaseable>& mp)
	{
		//static_assert(&mp != this, "Equals self results undefined behavior.");
		cdd();
		if (mp.isEmpty())
		{
			ptr = NULL;
		}
		else
		{
			ptr = mp.ptr;
			(ptr->count)++;
		}
	}
	//��memUnit��ָ��
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::operator=(mu* pmu)
	{
		cdd();
		if (pmu)
		{
			if (((memUnit*)pmu)->sharedPtr)
			{
				ptr = ((memUnit*)pmu)->sharedPtr;
				(ptr->count)++;
			}
			else
			{
				ptr = new memPtrComm((memUnit*)pmu);
				((memUnit*)pmu)->sharedPtr = ptr;
			}
		}
		else
		{
			ptr = NULL;
		}
	}
	//����
	template<class mu, bool releaseable>
	inline void memPtr<mu, releaseable>::swap(memPtr& sw)
	{
		memPtrComm* i = ptr;
		ptr = sw.ptr;
		sw.ptr = i;
	}

	//�ڴ���Ӳ�̹�����
	//һ������ʵ�� == һ��Ӳ���ļ�
	//�䱾����䵱�������ݵ�Ԫ��
	inline memManager::memManager() :memUnit(NULL)
	{
		thisCons();
		memset(this->url, 0, sizeof(this->url));
		this->mngr = this;
		if(global_mutex == NULL)
			global_mutex = CreateMutex(nullptr, NULL, nullptr);
		WaitForSingleObject(global_mutex, INFINITE);
		global_load.push_back(this);
		ReleaseMutex(global_mutex);
	}
	inline memManager::~memManager()
	{
		WaitForSingleObject(global_mutex, INFINITE);
		for (auto i = global_load.begin(); i != global_load.end(); i++)
		{
			if (*i == this)
			{
				global_load.erase(i);
				break;
			}
		}
		ReleaseMutex(global_mutex);
		this->mngr = NULL;
		for (auto i : *memUnits)
		{
			i->mngr = NULL;
			if (i->sharedPtr)						//���ﲻ���Ż������ɼ�memPtr��release
				i->sharedPtr->content = NULL;
			delete i;
		}
		thisDest();
	}
	inline void memManager::thisCons()
	{
		memUnits = new std::set<memUnit*>();
		subFiles = new memVector<Subfile>(this);
		ingressInte = new memVector<Ingress>(this);
	}
	inline void memManager::thisDest()
	{
		delete memUnits;
		//subFiles and ingressInte are also memUnit that will be destructed when all of memUnit are destructed.
	}
	inline VOID memManager::deletePtrTable()
	{
		for (auto i : *ptrTable)
		{
			if (i.appSegment)
			{
				for (auto k : *i.appSegment)
				{
					if (k)
					{
						if (k->value)
							delete k->value;
						delete k;
					}
				}
				delete i.appSegment;
			}
		}
		delete ptrTable;
		ptrTable = NULL;
		return;
	}
	//save�����ص�Ӳ�̣��ɹ�����true��ʧ�ܷ���false
	inline BOOL memManager::download()
	{
		DeleteFileW(url);

		std::vector<WCHAR> bc = {};
		bc.reserve(1000);

		this->serialize<ORD_SAVE>(&bc);

		HANDLE hFile = CreateFile(url, FILE_GENERIC_READ | FILE_GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			return 0;
		}
		UINT dwErr = GetLastError();

		WORD unicode_identifier = 0xfeff;
		WriteFile(hFile, &bc[0], bc.size() * sizeof(WCHAR), NULL, NULL);

		CloseHandle(hFile);

		return 1;
	}
	//fetch���ϴ����ڴ棬�ɹ�����true��ʧ�ܷ���false����ʧ�ܲ���Ӱ��ԭ���ݡ�
	inline BOOL memManager::upload()
	{
		HANDLE hFile = CreateFile(url, FILE_GENERIC_READ | FILE_GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile)
		{
			return 0;
		}
		DWORD fileSizeH, fileSizeL, sizeRead = 0;
		fileSizeL = GetFileSize(hFile, &fileSizeH);
		UINT64 fileSize = fileSizeH;
		fileSize = (fileSize << 32);
		fileSize += fileSizeL;
		if (!fileSize)
		{
			CloseHandle(hFile);
			return 0;
		}

		WCHAR* bytes = (WCHAR*)calloc(1, fileSize + 512);
		BOOL ret = false;
		if (ReadFile(hFile, bytes, fileSize, &sizeRead, NULL))
		{
			CloseHandle(hFile);
			ret = this->deserialize<ORD_FETCH>(bytes, fileSize / sizeof(WCHAR));
		}
		free(bytes);
		return ret;
	}
	//����url����subfile
	inline memPtr<Subfile> memManager::findSubfile(const WCHAR* fileUrl)
	{
		assert(fileUrl || !("target manager's url cannot be null"));
		for (auto i : *subFiles)
		{
			if (wcscmp(fileUrl, i->fileName) == 0)
				return i;
		}
		return NULL;
	}
	//����subfile��keyword��type���ҳ���
	inline memPtr<Egress> memManager::findEgress(const memPtr<Subfile> subfile, const WCHAR* kw, const WCHAR* type)
	{
		for (auto k : **subfile)
		{
			if (wcscmp(k->keywords, kw) == 0 && wcscmp(k->type, type) == 0)
				return k;
		}
		return NULL;
	}
	//����keyword��type�������
	inline memPtr<Ingress> memManager::findIngress(const WCHAR* kw, const WCHAR* type)
	{
		for (auto i : *ingressInte)
		{
			if (wcscmp(i->keywords, kw) == 0 && wcscmp(i->type, type) == 0)
				return i;
		}
		return NULL;
	}
	//����������ڡ���release���κ��п��ܳ�Ϊ��ڵ�memUnit�󣬶�Ӧ��ִ��һ�δ˷�����
	inline memPtr<Ingress> memManager::findIngressDangling()
	{
		for (auto i : *ingressInte)
		{
			if (i->mu.isEmpty())
				return i;
			if (i->mu.ptr->count == 1)
				return i;
		}
		return NULL;
	}
	//�������-��ڶԡ��ɳ���memManagerִ�и÷�������Ŀ��memManager������һ��ָ��Ŀ��memUnit����ڣ�������һ������
	template<class mu>
	[[nodiscard]] inline memPtr<Egress> memManager::makeEgress_IngressPair(const memPtr<mu>& target, const WCHAR* kw)
	{
		const char* typeName = typeid(mu).name();
		WCHAR type[maxKW];
		MultiByteToWideChar(CP_ACP, 0, typeName, -1, type, maxKW);
		assert(target.isFilled() || !("target memUnit is Null."));
		assert(wcslen(kw) < maxKW || !("keyword is too long."));
		assert(wcslen(type) < maxKW || !("type is too long."));

		memManager* targetManager = target->getManager();

		memPtr<Subfile> sf = this->findSubfile(targetManager->getFileName());
		memPtr<Egress> egr = NULL;
		memPtr<Ingress> ingr = targetManager->findIngress(kw, type);
		if (ingr.isEmpty())
		{
			ingr = new Ingress(targetManager);
			wcscpy_s(ingr->keywords, kw);
			wcscpy_s(ingr->type, type);
			targetManager->ingressInte->push_back(ingr);
		}
		else
		{
			ingr->mu.cdd();
		}
		ingr->mu.ptr = target.ptr;
		ingr->mu.ptr->count++;
		if(sf.isEmpty())
		{
			sf = new Subfile(this);
			wcscpy_s(sf->fileName, targetManager->getFileName());
			sf->ptrManager = targetManager;
			this->subFiles->push_back(sf);
		}
		else
		{
			egr = this->findEgress(sf, kw, type);
		}
		if (egr.isEmpty())
		{
			egr = new Egress(this, *sf);
			wcscpy_s(egr->keywords, kw);
			wcscpy_s(egr->type, type);
			sf->push_back(egr);
		}
		if (egr->ptrDirect == NULL)
		{
			egr->ptrInge = *ingr;
			egr->ptrInge->pointedByEgress.push_back(*egr);
			egr->ptrDirect = (void*)*ingr->mu;
		}

		return egr;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI, memPtr<memUnit>* self)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		memPtr<memUnit>*& a = const_cast<memPtr<memUnit>*&>((*k).ptrRUN);
		a = self;
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::findPtr(memUnit* ptrINI, memPtrCorr** ptr)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		auto k = ptrTable->find(i);
		if (k == ptrTable->cend())
			return NULL;
		*ptr = (memPtrCorr*)&(*k);
		return (*k).appSegment;
	}
	inline std::vector<memApp*>* memManager::pushPtr(memUnit* ptrINI)
	{
		memPtrCorr i;
		i.ptrINI = ptrINI;
		i.ptrRUN = NULL;
		i.appSegment = new std::vector<memApp*>();
		ptrTable->insert(i);
		return i.appSegment;
		
	}

	//���Զ���ṹ���save_fetch_structʹ�õ�GWPP
	template<class T>
	inline void GWPP_Struct(memUnit* mu, const WCHAR* key1, const WCHAR* key2, T& var, memPara para)
	{
		assert(wcslen(key1) + wcslen(key2) < maxKey || !("variable key is too long."));
		WCHAR key[maxKey];
		wcscpy_s(key, key1);
		wcscat_s(key, key2);
		mu->GWPP(key, var, para);
	}

	//Ӳ�̵ײ��������
	inline bool memUnit::deserialize(WCHAR* Ptr, UINT StringSize)
	{
		this->mngr->ptrTable = new std::set<memPtrCorr>();

		//���ļ�ͷ
		memUnit* mu = (memUnit*)_wtop(Ptr);
		Ptr += wcslen(Ptr) + 1;

		if (!mu)
		{
			this->mngr->deletePtrTable();
			return false;
		}

		this->mngr->RawDeserialize(Ptr, StringSize);

		//�����д����
		memPara mp;
		mp.order = ORD_FETCH;
		memPtr<memUnit> self = this;
		mp.appSegment = this->mngr->findPtr(mu, &self);
		this->save_fetch(mp);

		//���ָ���Ӧ��������
		this->mngr->deletePtrTable();
		self.degeneracy();
		return true;
	}
	inline void memUnit::serialize(std::vector<WCHAR>* bc)
	{
		this->mngr->ptrTable = new std::set<memPtrCorr>();

		//д�ļ�ͷ
		WCHAR wc[20];
		_ptow_s((POINTER_L)this, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);

		//�����д����
		memPara mp;
		mp.order = ORD_SAVE;
		mp.appSegment = this->mngr->pushPtr(this);
		this->save_fetch(mp);

		this->mngr->RawSerialize(bc);

		//���ָ���Ӧ��������
		this->mngr->deletePtrTable();
	}
	template<SHORT para_order>
	inline bool memManager::deserialize(WCHAR* Ptr, UINT StringSize)
	{
		ptrTable = new std::set<memPtrCorr>();

		//�ֽڷָ�
		memUnit* firstmu = NULL;
		//���ļ�ͷ
		memUnit* subfile = (memUnit*)_wtop(Ptr);
		WCHAR* ptrByte = Ptr + wcslen(Ptr) + 1;
		memUnit* ingressIn = (memUnit*)_wtop(ptrByte);
		ptrByte += wcslen(ptrByte) + 1;
		firstmu = (memUnit*)_wtop(ptrByte);
		ptrByte += wcslen(ptrByte) + 1;

		this->RawDeserialize(ptrByte, StringSize);

		if (!firstmu || !subfile || !ingressIn)
		{
			deletePtrTable();
			return 0;
		}

		//��մ�ʵ���µ�����memUnit�����ô�ʵ����Ϊ������׼����
		for (auto i : *memUnits)
		{
			i->mngr = NULL;
			if (i->sharedPtr)
				i->sharedPtr->content = NULL;
			delete i;
		}
		thisDest();
		thisCons();

		//�����д����
		memPara mp;
		mp.order = para_order;
		memPtr<memUnit> self = this;
		mp.appSegment = findPtr(firstmu, &self);
		this->save_fetch(mp);

		//���س�����ڽӿڱ�
		mp.appSegment = findPtr(subfile);
		this->subFiles->save_fetch(mp);
		mp.appSegment = findPtr(ingressIn);
		this->ingressInte->save_fetch(mp);

		//���ָ���Ӧ��������
		deletePtrTable();
		self.degeneracy();
		return 1;
	}
	template<SHORT para_order>
	inline void memManager::serialize(std::vector<WCHAR>* bc)
	{
		ptrTable = new std::set<memPtrCorr>();

		//�����д����
		memPara mp;
		mp.order = para_order;
		mp.appSegment = pushPtr(this);
		this->save_fetch(mp);

		//���س�����ڽӿڱ�
		mp.appSegment = pushPtr(this->subFiles);
		this->subFiles->save_fetch(mp);
		mp.appSegment = pushPtr(this->ingressInte);
		this->ingressInte->save_fetch(mp);

		//д�ļ�ͷ
		WCHAR wc[20];
		_ptow_s((POINTER_L)this->subFiles, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);
		_ptow_s((POINTER_L)this->ingressInte, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);
		_ptow_s((POINTER_L)this, wc, 10);
		PushWcharVector(bc, wc, wcslen(wc) + 1);

		//����д��
		this->RawSerialize(bc);

		//���ָ���Ӧ��������
		deletePtrTable();
	}
	inline void memManager::RawSerialize(std::vector<WCHAR>* bc)
	{
		WCHAR wc[20];
		WCHAR null[2] = {};
		for (auto k : *this->ptrTable)
		{
			_ptow_s((POINTER_L)k.ptrINI, wc, 10);
			PushWcharVector(bc, null, 2);
			PushWcharVector(bc, wc, wcslen(wc) + 1);
			for (auto i : *(k.appSegment))
			{
				PushWcharVector(bc, i->key, wcslen(i->key) + 1);
				PushWcharVector(bc, &i->value->at(0), wcslen(&i->value->at(0)) + 1);
			}
		}
		PushWcharVector(bc, null, 2);
		PushWcharVector(bc, null, 2);
	}
	inline void memManager::RawDeserialize(WCHAR* ptrByte, UINT fileSize)
	{
		ptrByte += 2;
		memPtrCorr i;
		WCHAR* ptrByteEnd = ptrByte + fileSize;
		while (ptrByte < ptrByteEnd)
		{
			i.ptrINI = (memUnit*)_wtop(ptrByte);		//��С����
			if (i.ptrINI == 0)
				break;
			i.ptrRUN = NULL;
			ptrByte += wcslen(ptrByte) + 1;
			i.appSegment = new std::vector<memApp*>();
			if (*ptrByte == L'\0')						//С���Ƿ�Ϊ��
			{
				ptrByte += 2;
				this->ptrTable->insert(i);				//С������û���ݣ���С��(memUnit)д���ܱ���
				if (*ptrByte == L'\0')					//�ļ���ȡ����
					break;
				continue;
			}
			while (ptrByte < ptrByteEnd)
			{
				UINT len;
				memApp* ma = new memApp();
				wcscpy_s(ma->key, ptrByte);				//����
				ptrByte += wcslen(ptrByte) + 1;
				len = wcslen(ptrByte) + 1;
				if (len - 1)							//�ǿ�ֵ
				{
					ma->value = new std::vector<WCHAR>(len);
					memcpy_s(&ma->value->at(0), len * sizeof(WCHAR), (void*)ptrByte, len * sizeof(WCHAR));
					ptrByte += len;

				}
				else
				{										//��ֵ
					ma->value = NULL;
					ptrByte++;
				}
				i.appSegment->push_back(ma);
				if (*ptrByte == L'\0')
				{
					ptrByte += 2;
					break;								//С�ڶ�ȡ����
				}
			}
			this->ptrTable->insert(i);					//����С���е����ݣ���С��(memUnit)д���ܱ���
		}
	}
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, DWORD nSize, LPCTSTR lpString)
	{
		for (auto i : *lpAppSegment)
		{
			if (i->key[0])
			{
				if (wcsstr(i->key, lpKeyName))
				{
					i->key[0] = L'\0';
					if (i->value)
					{
						int a = i->value->size() < nSize ? i->value->size() : nSize;
						a *= sizeof(WCHAR);
						memcpy_s((void*)lpString, a, &i->value->at(0), a);
						return 1;
					}
					else
						return 0;
				}
			}
		}
		return 0;
	}
	inline BOOL GetPrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, std::vector<WCHAR>& wc)
	{
		for (auto i : *lpAppSegment)
		{
			if (i->key[0])
			{
				if (wcsstr(i->key, lpKeyName))
				{
					i->key[0] = L'\0';
					if (i->value)
					{
						if (i->value->size() > wc.size())
						{
							wc.resize(i->value->size());
						}
						int a = wc.size();
						a *= sizeof(WCHAR);
						memcpy_s((void*)&wc[0], a, &i->value->at(0), a);
						return 1;
					}
					else
						return 0;
				}
			}
		}
		return 0;
	}
	inline BOOL WritePrivateProfileStringW(std::vector<memApp*>* lpAppSegment, LPCTSTR lpKeyName, LPCTSTR lpString)
	{
		memApp* i = new memApp();
		wcscpy_s(i->key, lpKeyName);
		UINT len = (wcslen(lpString) + 1);
		i->value = new std::vector<WCHAR>(len);
		len *= sizeof(WCHAR);
		memcpy_s(&i->value->at(0), len, (void*)lpString, len);
		lpAppSegment->push_back(i);
		return 0;
	}
	inline void PushWcharVector(std::vector<WCHAR>* bc,const WCHAR* wc, UINT len) {
		for (UINT i = 0; i < len; i++)
		{
			bc->push_back(wc[i]);
		}
	}

	//�������غ���
	inline Egress::~Egress()
	{
		if (ptrUpLevel)
		{
			auto i = find(ptrUpLevel->begin(), ptrUpLevel->end(), this);
			if (*i == this)
			{
				auto k = ptrUpLevel;
				ptrUpLevel = NULL;
				k->erase(i);
			}
		}
		if (ptrInge)
		{
			auto k = find(ptrInge->pointedByEgress.begin(), ptrInge->pointedByEgress.end(), this);
			if (*k == this)
				ptrInge->pointedByEgress.erase(k);
		}
	}
	template<typename cast>
	inline UINT Egress::getTarget(cast*& varReturn)
	{
		if (ptrDirect == NULL)
		{
			if (ptrUpLevel->ptrManager.isEmpty())
				if (ptrUpLevel->findGlobalManager() == 0)
					return MEM_NOTFOUND_FILE;
			memPtr<Ingress> inge = ptrUpLevel->ptrManager->findIngress(keywords, type);
			if (inge.isEmpty())
				return MEM_NOTFOUND_KEYWORDS;
			else
			{
				ptrInge = *inge;
				ptrInge->pointedByEgress.push_back(this);
				ptrDirect = (void*)*ptrInge->mu;
			}
		}
		varReturn = (cast*)ptrDirect;
		if (varReturn)
			return MEM_SUCCESS;
		else
			return MEM_EMPTY_INGRESS;
	}
	


	//Trial code(demo)
#ifdef MEM_DEBUG
	inline int testFoo(int a, int b) { return a + b; }
	inline int testFoo2(int a, int b) { return a * b; }
	inline int testFoo3(int a, int b) { return a - b; }
	INITIALIZE_PFUNCTION(int(int, int), 75342, testFoo, testFoo2, testFoo3);
	class testManager;
	class testUnit2;
	class testUnit :public memUnit {
		friend class memUnit;
		void save_fetch(memPara para) override
		{
			GWPP(L"anotherone", anotherone, para);
			GWPP(L"num", num, para);
			GWPP(L"wc", wc, para);
		}
	public:
		testUnit(memManager* manager) :memUnit(manager) {};
		memPtr<testUnit> anotherone;
		double num;
		memWcs wc;
	};
	class testManager :public memManager {
	public:
		memPtr<testUnit> anothert1;
		memPtr<testUnit> anothert2;
		memPtr<testUnit2> tu2;
		pmemVector<testUnit> vec;
		UINT num;
		CHAR n1;
		BYTE n2;
		SHORT n3;
		USHORT n4;
		INT n5;
		UINT n6;
		INT64 n7;
		UINT64 n8;
		float n9;
		double n10;
		memWcs wc;
		enum { ea, eb, ec }n11;
	private:
		void save_fetch(memPara para) override{
			GWPP(L"anothert1", anothert1, para);
			GWPP(L"anothert2", anothert2, para);
			GWPP(L"tu2", tu2, para);
			GWPP(L"vec", vec, para);
			GWPP(L"num", num, para);
			GWPP(L"n1", n1, para);
			GWPP(L"n2", n2, para);
			GWPP(L"n3", n3, para);
			GWPP(L"n4", n4, para);
			GWPP(L"n5", n5, para);
			GWPP(L"n6", n6, para);
			GWPP(L"n7", n7, para);
			GWPP(L"n8", n8, para);
			GWPP(L"n9", n9, para);
			GWPP(L"n10", n10, para);
			GWPP(L"wc", wc, para);
			GWPP(L"n11", n11, para);
		}
	};
	class testUnit2 :public memUnit {
		friend class memUnit;
		void save_fetch(memPara para) override {
			GWPP(L"egressTest", egressTest, para);
			GWPP(L"genetest", genetest, para);
			GWPP(L"funcTest", funcTest, para);
		}
	public:
		pEgress<testUnit> egressTest;
		pVariant<testUnit, testUnit2> genetest;
		pFunction<int(int, int),75342> funcTest;
		testUnit2(memManager* manager) :memUnit(manager) {};
	};
	inline void testmain()
	{
		WCHAR url[100] = L"C:\\Users\\R\\Desktop\\newsave.guma";	//��������������ִ�У���Ҫ��Ϊ��һ����Ч��·��
		WCHAR url2[100] = L"C:\\Users\\R\\Desktop\\oldsave.guma";
		sizeof(memPtr<testUnit>);

		//�ڴ�й©������
		while(1){

		memPtr<testManager> testManagerA = new testManager();
		memPtr<testUnit> testUnitA = new testUnit(*testManagerA);
		memPtr<testUnit> testUnitB = new testUnit(*testManagerA);
		testUnitA->anotherone = testUnitB;
		testUnitA->num = -1000;
		testUnitA->wc = L"t1";
		testUnitB->anotherone = testUnitB;
		testUnitB->num = 2000;
		testUnitB->wc = L"t2";
		testManagerA->anothert1 = testUnitA;
		memPtr<testUnit> copyTest = memUnit::memCopy(testManagerA->anothert1);
		//testManagerA->anothert2 = memUnit::memCopy(testUnitA);
		testManagerA->anothert2 = testUnitB;
		testManagerA->num = 4000;
		testManagerA->setUrl(url);

		testManagerA->wc = L"t4";
		testManagerA->vec = new memVector<testUnit>(*testManagerA);
		testManagerA->vec->emplace_back(testUnitA);
		testManagerA->vec->emplace_back(*testUnitB);
		testManagerA->vec->emplace_back(*testUnitA);
		testManagerA->vec->emplace_back(new testUnit(*testManagerA));
		testManagerA->vec->emplace_back(new testUnit(*testManagerA));
		memPtr<testUnit> t5 = memUnit::memCopy(testUnitB);
		testManagerA->tu2 = new testUnit2(*testManagerA);
		memPtr<testUnit2> testUnitC = testManagerA->tu2;
		
		//����ָ�����
		testManagerA->tu2->genetest = testUnitA;
		testUnit* geneGet1;
		testUnit2* geneGet3;
		testManagerA->tu2->genetest.getPtr(geneGet1);
		testManagerA->tu2->genetest.getPtr(geneGet3);
		testManagerA->tu2->genetest = NULL;
		testManagerA->tu2->genetest = new testUnit2(*testManagerA);
		//����ָ���ٶȲ���
		//MSVC/C++�޷��Ա��ģ��չ�������Ƶ�constexpr���ʶ�д�ٶȶ�Ϊo(n). ����Ӧ��o(1)
		/*for (int i = 0; i < 10000000; i++)
		{
			testManagerA->tu2->genetest.getPtr(geneGet3);
		}
		for (int i = 0; i < 10000000; i++)
		{
			testManagerA->tu2->genetest.getPtr(geneGet1);
		}*/

		//�����Сֵ���ԣ��浵�ļ���ֵ�ǲ�׼ȷ�ģ����ڴ��ȡ���Ϊ׼��
		testManagerA->n1 = INT8_MAX;
		testManagerA->n2 = UINT8_MAX;
		testManagerA->n3 = INT16_MAX;
		testManagerA->n4 = UINT16_MAX;
		testManagerA->n5 = INT32_MAX;
		testManagerA->n6 = UINT32_MAX;
		testManagerA->n7 = INT64_MAX;
		testManagerA->n8 = UINT64_MAX;
		testManagerA->n9 = 3.1415926535897932f;
		testManagerA->n10 = 3.1415926535897932;
		testManagerA->n11 = testManager::ec;

		testManagerA->n1 = INT8_MIN;
		testManagerA->n3 = INT16_MIN;
		testManagerA->n5 = INT32_MIN;
		testManagerA->n7 = INT64_MIN;

		//��д�ٶȲ�����
		/*for (int i = 0; i < 25000; i++)
		{
			testManagerA->vec->push_back(new testUnit(*testManagerA));
		}
		for (auto i : **(testManagerA->vec))
		{
			i->num = 10000000000000;
			i->num *= 10000000000000;
			i->anotherone = new testUnit(*testManagerA);
			i->anotherone->anotherone = new testUnit(*testManagerA);
			i->wc = L"Hello World. ̫���ˣ�̫���ˣ�ʮ��Ԫ��Ҫ��3�룡";
		}*/

		//pFunction����
		testUnitC->funcTest = testFoo2;
		testUnitC->funcTest = testFoo3;

		int ret = testManagerA->download();

		memPtr<testManager> testManagerB = new testManager();
		testManagerB->setUrl(url);
		testManagerB->upload();


		int fooTestRet = testManagerB->tu2->funcTest(10, 5);

		//ingress/egress������
		testManagerB->setUrl(url2);
		testManagerA->tu2->egressTest.makeEIPair(*testManagerA, testManagerB->anothert1, L"ingressTest");

		//ָ�������

		//ingress/egress�ٶȲ�����
		/*for (int i = 0; i < 100000; i++)
		{
			testManagerA->tu2->egressTest.makeEIPair(*testManagerA, testManagerB->anothert1, L"ingressTest");
		}*/

		//egress vector
		pmemVectorEgress<testUnit> egressvector = new memVectorEgress<testUnit>(*testManagerB);
		egressvector->emplace_back(*testManagerA, testManagerB->anothert1, L"ingressTest");

		testManagerB->download();
		testManagerA->download();

		testManagerA.release();
		testManagerA.release();
		testManagerB.release();

		testManagerB = new testManager();			//������testManager
		testManagerB->setUrl(url);
		testManagerB->upload();
		testManagerA = new testManager();
		testManagerA->setUrl(url2);
		testManagerA->upload();
		testUnit* ingressSuccess;
		//testManagerA->anothert1.release();

		//getTarget��ȫ�ֲ���Ŀ��memManager����Ҫ���Ƚ�memManager upload���ڴ棬����get
		ret = testManagerB->tu2->egressTest.getTarget(ingressSuccess);



		//SFINAE������
		extract_template_memUnit<int>::type* sfinae1 = NULL;
		bool sfinae_isTemplate = is_template<pEgress<testUnit>>::value;
		extract_template_memUnit<memPtr<Egress>>::type* sfinae2 = NULL;
		bool sfinae3 = is_based_on_memPtr<pEgress<testUnit>>::value;
		void* sfinaeVT = memUnit::get_vtable_ptr<Egress>::ptr();



		//���������
		//��
		ReflectResult refRes;
		memUnit* refTestMu = *testManagerA;
		refTestMu->reflectionRead(&refRes);

		ReflectResult refRes2;
		memUnit* refTestMu2 = *testManagerA->tu2;
		refTestMu2->reflectionRead(&refRes2);

		//д
		ULONGLONG refNum = 123456;
		ReflectResultKeyValue refKW = ReflectResultKeyValue(L"num",(UINT&)refNum);
		refTestMu->reflectionWrite(refKW);
		refNum = 654321;
		refKW = ReflectResultKeyValue(L"n8", refNum);
		refTestMu->reflectionWrite(refKW);
		WCHAR refWC[] = L"wchar test";
		refTestMu->reflectionWrite(ReflectResultKeyValue(L"wc", refWC));
		refTestMu->reflectionWrite(ReflectResultKeyValue(L"wc", L"wchar test directly"));
		refKW = ReflectResultKeyValue(L"anothert1", testManagerA->anothert2);	//�Ϸ��ڴ浥Ԫָ��д��
		bool refSuccess = refTestMu->reflectionWrite(refKW);
		refKW = ReflectResultKeyValue(L"anothert1", testManagerA->vec);			//�Ƿ��ڴ浥Ԫָ��д��
		refSuccess = refTestMu->reflectionWrite(refKW);

		memUnit* refTestVariant = *testManagerA->anothert1;
		refKW = ReflectResultKeyValue(L"genetest", refTestVariant);
		refTestMu2->reflectionWrite(refKW);



		//���л�������(memUnit)
		std::vector<WCHAR> serializeDump = {};
		testManagerA->anothert1->num = 2.72;
		testManagerA->anothert1->serialize(&serializeDump);

		//�����л�������(memUnit)
		testManagerB = new testManager();
		testManagerB->anothert1 = new testUnit(*testManagerB);
		testManagerB->anothert1->deserialize(&serializeDump[0], serializeDump.size());
		serializeDump.clear();
		testManagerB.release();

		//���л�������(memManager)
		if (true)
		{
			testManagerA->serialize(&serializeDump);
			testManagerA.release();

			//�����л�������(memManager)
			testManagerB = new testManager();
			testManagerB->deserialize(&serializeDump[0], serializeDump.size());
		}

		//�ڴ�й©������2
		}
		return;
	}
#endif // DEBUG
}
#pragma warning(default: MEM_MANAGER_DISABLE_WARINIGS)

//1 ���ṩ������ָ�롣
//	��Ҫ������������ָ����Զ����չ��ܡ�������ָ����ܳ���ѭ������ʱ��Ӧ��ֱ��release����Щ����ָ�롣
//	���߳�ʱ���ǽ���release��ʹ�þ�ȷ�ļܹ��������֤��й©��
//	����memUnit��memManager�ڲ����м�¼���ڴ�й©�����أ����Ǻ��¡�
// 
//2 memUnit�г�ԱmemPtr��ָ�򣬲�׼��Խ���memUnit��memManager��
//  memManager֮���ͨ��ʹ��egress/ingress
//	pEgress�൱��ָ�룬ֱ��ָ��ͬmemManager��memUnit��
//	Subfile��Ingress�����ڲ��ṹ�壬���������
//	egress/ingressֻ֧�ֵ��̣߳����߳����������
//	��egress��ȡ�õ���ָ�룬���ܳ��ڳ��У������ܸ���memPtr
// 
//3 ������memManagerǰ����Ҫȷ��û���߳�����ʹ�����memManager�ڵĶ�����
// 
//4 ��memUnit�����������������ʹ��getManagerʱ����Ҫ��鷵�ص�memManager�Ƿ�Ϊ�ա�
//  ��Ϊ�գ�˵����memManager��������������
//
//5 ���̳�memVector��key��ֹȡΪ��������"size"
//
//6 ǧ����ּ���pVariant�ı��ģ�塢pFunction�ĳ�ʼ�������˳�򣬻������м�ɾ��һ������Ȼ�Ͷ������浵�ˣ���
//
//7 ����һ������̳�memPtr����ʹ��vectorʱ��������дһ�ݻ��ڴ������vector
//
//8 �浵ʱ��memVector�����ڲ��Ŀ�ָ�룬���Զ�ʡ�ԡ��ʲ�������������������memVector������memVector������ͨ��Ҳ��û�м�ֵ�ġ�
// 
// 
// 
//TODO�������Ż������л�ʱ�����С�ڲ���ͳһ�𣬶�Ҫ����save_fetchȥ�����������ڶ�ȡʱ֪���������ͣ��Ϳ�ֱ��ʹ�ý����ֽڶ��룬�����ý��������͵ı���ת����WCHAR��
//		���а�ȫ�ԸĽ���ʹ�ø��õĿ��ַ�������򣬱����쳣
//