/* En bloc data structure manager and serialize lib
 * ------Head-Only------
 * 
 * implements a more flexible single-thread smart pointer, cross-file reference, static reflection, binary serialization, and JSON serialization in the eb::base/eb::manager class.
 * 
 * using rapidJson for json support; utfcpp for multi-string support.
 * 
 * C++ standard: 17 or higher
 * 
 * Licensed under the MIT License.
 * Looking forward to visiting https://github.com/UF4007/memManager to propose issues and making eb::manager available.
*/

#pragma once
#ifdef _MSC_VER
#define __MEM_MANAGER_DISABLE_WARINIGS 4244 4267 4996
#pragma warning(disable: __MEM_MANAGER_DISABLE_WARINIGS)
#endif

#include "internal/config.h"
#include "internal/includes.h"

namespace eb {	// en bloc
#include "HAL/fileHAL.h"

	//global const
	constexpr size_t maxKey = 50;
	constexpr size_t maxURL = MEM_MAX_FILE_PATH;
	constexpr size_t likelyBytesPerUnit = 100;			//reserve vector capacity we assumed for per eb::base.

	//headonly version distinguish, prevent the linker from mixing differental versions when multi-reference.
	inline namespace v3 {

#include "internal/dataStructure.h"
#include "internal/forwardDeclarations.h"
#include "internal/sfinae.h"
#include "internal/lowlevel.h"

		// data structure meta unit
		// UB: Create an eb::base with a manager-managed on the stack or global section.
		class base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
			lowlevel::memPtrComm* sharedPtr = nullptr;
			manager* mngr;
		public:
			template<typename _T>
			static constexpr bool isGWPPValid = std::is_arithmetic<_T>::value ||
				std::is_array<_T>::value ||
				eb::is_stl_container<_T>::value ||
				eb::is_atomic<_T>::value ||
				eb::is_string<_T>::value ||
				(std::is_enum<_T>::value && std::is_arithmetic<typename eb::enum_type<_T>::type>::value) ||
				eb::is_variant<_T>::value ||
				eb::is_pair<_T>::value ||
				eb::is_optional<_T>::value ||
				eb::has_save_fetch_struct<_T>::value ||
				eb::is_chrono<_T>::value ||
				eb::is_impPtr<_T>::value || eb::is_memPtr<_T>::value;
			void operator=(const base& munit) = delete;		//use smart pointers to do with RAII please.
			void operator=(base&& munit) = delete;
			inline manager* getManager() const { return mngr; }
#if MEM_REFLECTION_ON
			void reflectionRead(ReflectResult* output);
			//write a single variable using reflection, true for success, false for fail
			bool reflectionWrite(ReflectResultKeyValue inputKW);
#endif
			//binary serialize a eb::base to raw data format, ignore all pointers
			bool deserialize(uint8_t* Ptr, uint32_t StringSize);
			//binary deserialize a raw data format to a eb::base, ignore all pointers
			void serialize(std::vector<uint8_t>* bc);
#if MEM_RJSON_ON
			//serialize a eb::base to json. for recurring objects, ignore then
			void serializeJson(std::string* bc);
			//deserialize a eb::base from a json string
			bool deserializeJson(const std::string& bc);
			//deserialize a eb::base from a json string
			bool deserializeJson(const char* Ptr, uint32_t StringSize);
#endif
#if MEM_MYSQL_ON
            template <typename _T>
            static constexpr bool isMySQLValid = std::is_arithmetic<_T>::value ||
                                                 (std::is_array<_T>::value && std::is_same<typename std::remove_extent<_T>::type, char>::value) ||
                                                 std::is_same<MYSQL_TIME, _T>::value ||
                                                 eb::is_string<_T>::value;
            struct mysql_meta{
                const char* key;
                bool readonly;
            };
			// create a temporary object of T, save_fetch then
            template <typename T>
            static std::enable_if_t<std::is_base_of<base, T>::value, std::vector<mysql_meta>> get_SQL_metadata();
            static std::chrono::system_clock::time_point SQL_TIME_to_tp(MYSQL_TIME time);
            static MYSQL_TIME tp_to_SQL_TIME(std::chrono::system_clock::time_point tp);
            void SQL_bind(std::vector<mysql_meta> &metadata, MYSQL_BIND *read_bind_out, MYSQL_BIND *write_bind_out, size_t *psize);
            void SQL_bind(std::vector<mysql_meta> &metadata, MYSQL_BIND *read_bind_out, size_t *psize);
            void SQL_bind(MYSQL_BIND *read_bind_out);
            size_t SQL_checkstr(MYSQL_BIND* bind_in);
#endif
        protected:
			base(manager* manager);
			base(const base& munit);
			base(base&& munit) noexcept;
			virtual ~base();
			virtual void save_fetch(para para) = 0;
			//read or write any struct bluntly (via memcpy)
			template<class _any>
			void GWPP_Any(const char* key, _any& var, para& para);
			template<class _T>
			std::enable_if_t<eb::base::isGWPPValid<_T>, void>
				GWPP(const char* key, _T& var, para& para);
			//for any custom struct that fulfilled member function save_fetch_sub()
			template<typename _memSub>
			std::enable_if_t<eb::has_save_fetch_sub<_memSub>::value, void>
				GWPP(const char* key, _memSub& varST, para& para);
#if MEM_MYSQL_ON
			// designate this key as read-only, never archive.
            template <class _T>
            std::enable_if_t<eb::base::isMySQLValid<_T> || eb::has_save_fetch_sub<_T>::value, void>
            GWPP_SQL_READ(const char *key, _T &var, para &para);
            void GWPP_SQL_TIME_READ(const char *key, MYSQL_TIME &var, para &para);
			// MYSQL_TIME cannot be serialized/deserialized in non-MySQL format e.g. json.
            void GWPP_SQL_TIME(const char *key, MYSQL_TIME &var, para &para);
#endif
		private:
			//this constructor is for managers. never use it manually.
			inline base() { mngr = nullptr; }

			template<class _subType>
			static constexpr size_t getArrayValueTypeSize();
			//read or write arithmetic type
			template<class _arit>
			std::enable_if_t<std::is_arithmetic<_arit>::value, void>
				GWPP_Base(void* pValue, _arit& var, para& para);
			//read or write arithmetic type or their original array
			template<class _array>
			std::enable_if_t<std::is_array<_array>::value&& eb::base::isGWPPValid<typename std::remove_extent<_array>::type>, void>
				GWPP_Base(void* pValue, _array& var, para& para);
			//read or write stl container
			template<class _stlCont>
			std::enable_if_t<eb::is_stl_container<_stlCont>::value, void>
				GWPP_Base(void* pValue, _stlCont& var, para& para);
			//read or write atomic
			template<class _atom>
			std::enable_if_t<eb::is_atomic<_atom>::value, void>
				GWPP_Base(void* pValue, _atom& var, para& para);
			//read or write chrono, system clock timepoint has a special json expression than common chrono
			template<class _chrono>
			std::enable_if_t<eb::is_chrono<_chrono>::value, void>
				GWPP_Base(void* pValue, _chrono& var, para& para);
			//read or write string, always memcpy no matter what the content saved by container
			template<class _string>
			std::enable_if_t<eb::is_string<_string>::value, void>
				GWPP_Base(void* pValue, _string& var, para& para);
			//read or write enum
			template<class _enum>
			std::enable_if_t<std::is_enum<_enum>::value, void>
				GWPP_Base(void* pValue, _enum& var, para& para);

			//for unions, it is more safety to designate them manually. no GWPP was offered.

			//read or write variants
			template<class...Args>
			void GWPP_Base(void* pValue, std::variant<Args...>& var, para& para);
			//read or write pair
			template<class T1, class T2>
			void GWPP_Base(void* pValue, std::pair<T1, T2>& var, para& para);
			//read or write optional
			template<class T>
			void GWPP_Base(void* pValue, std::optional<T>& var, para& para);

			//for any custom struct that fulfilled member function save_fetch_struct() and fixed memory size save_fetch_size
			template<typename _memStruct>
			std::enable_if_t<eb::has_save_fetch_struct<_memStruct>::value, void>
				GWPP_Base(void* pValue, _memStruct& varST, para& para);
			//read or write implicit pointer
			template<typename _mu, bool _r>
			void GWPP_Base(void* pValue, impPtr<_mu, _r>& var, para& para);
			//read or write memory pointer
			template<typename _mu, bool _r>
			void GWPP_Base(void* pValue, memPtr<_mu, _r>& var, para& para);
		};

		// smart pointer of eb::base. original dumb pointer. It only counts, points stuff and can never be save-fetch.
		template<typename mu, bool releaseable>
		class dumbPtr {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				lowlevel::memPtrComm* ptr;
			void cdd();																			//smart pointer self reducing
			void degeneracy();																	//degeneracy will self-reduction the counter, but when reduced to 0 it makes eb::base return to the status without a smart pointer instead of destruction.
			template<class _any, bool _r> inline void equalAny(const dumbPtr<_any, _r>& mp);	//danger: equal any, whether their type
		public:
			dumbPtr();
			dumbPtr(mu* pmu);
			~dumbPtr();
            dumbPtr(const dumbPtr<mu, releaseable> &mp) noexcept;
			//copy
            void operator=(const dumbPtr<mu, releaseable>& mp) noexcept;
			dumbPtr(dumbPtr<mu, releaseable>&& mp) noexcept;
			//move
			void operator=(dumbPtr<mu, releaseable>&& mp) noexcept;
			//polymorphic construct (derived to eb::base)
			template<typename _any, bool _r>
			dumbPtr(const dumbPtr<_any, _r>& mp);
			//polymorphic copy (derived to eb::base)
			template<typename _any, bool _r>
			typename std::enable_if<
				(!std::is_same_v<mu, _any> &&
					(std::is_base_of_v<mu, _any> //|| std::is_base_of_v<_any, mu>
						) && _r == releaseable), void>::type
				operator=(const dumbPtr<_any, _r>& mp);
			//set a memPtr a raw pointer
			void operator=(mu* pmu);
			//cast in. 0 for fail, 1 for success.
			template<typename _anotherMu>
			std::enable_if_t<std::is_base_of<base, _anotherMu>::value, bool>
				precision_cast(_anotherMu* pamu);
			//cast out. nullptr for fail.
			template<typename _anotherMuPtr>
			std::enable_if_t<std::is_base_of<base, typename std::remove_pointer<_anotherMuPtr>::type>::value, _anotherMuPtr>
				precision_cast();
			//exchange
			void swap(dumbPtr& sw);
			void swap(impPtr<mu, releaseable>& right) = delete;
			void swap(memPtr<mu, releaseable>& right) = delete;
			bool isFilled() const;
			bool isEmpty() const;
			bool operator==(const dumbPtr& right) const;
			bool operator==(const mu* right) const;
			inline bool operator!=(const dumbPtr& right) const { return !(this->operator==(right)); };
			mu* operator*() const;
			mu* operator->() const;
			void release();
			//move a smart pointer in
			void moveIn(dumbPtr& right);
			//move a smart pointer in
			inline void operator<<(dumbPtr& right) { this->moveIn(right); }
			//use a new eb::base to supplant, all memPtr who points the old eb::base will point the new. free the old (if have).
			void supplantIn(mu* newOne);
		};

		// implicit pointer, which points and counts the same as dumbPtr, but when fetch it will never create a new class, only try to match an existing class from the 'corresponding table'.
		template<typename mu, bool releaseable>
		class impPtr :public dumbPtr<mu, releaseable> {
		public:
			inline impPtr() {}
			template <typename... Args> impPtr(Args&&... args) : dumbPtr<mu, releaseable>(std::forward<Args>(args)...) {}
			impPtr(const dumbPtr<mu, releaseable>) = delete;	//prohibits to use dumbPtr to construct impPtr
			impPtr(const dumbPtr<mu, releaseable>&) = delete;
			impPtr(dumbPtr<mu, releaseable>&&) = delete;
			void operator<<(dumbPtr<mu, releaseable>& right) = delete;
			inline void operator<<(impPtr& right) { this->moveIn(right); }
			void swap(dumbPtr<mu, releaseable>& right) = delete;
			void swap(memPtr<mu, releaseable>& right) = delete;
			inline void swap(impPtr<mu, releaseable>& right) { this->dumbPtr<mu, releaseable>::swap((dumbPtr<mu, releaseable>&)right); }
		};

		// memory pointer, which points and counts the same as dumbPtr, and it can save and fetch in GWPP like other variables.
		template<typename mu, bool releaseable>
		class memPtr :public impPtr<mu, releaseable> {
		public:
			inline memPtr() {}
			template <typename... Args> memPtr(Args&&... args) : impPtr<mu, releaseable>(std::forward<Args>(args)...) {}
			memPtr(const dumbPtr<mu, releaseable>) = delete;	//prohibits to use dumbPtr to construct memPtr
			memPtr(const dumbPtr<mu, releaseable>&) = delete;
			memPtr(dumbPtr<mu, releaseable>&&) = delete;
			memPtr(const impPtr<mu, releaseable>) = delete;		//prohibits to use impPtr to construct memPtr
			memPtr(const impPtr<mu, releaseable>&) = delete;
			memPtr(impPtr<mu, releaseable>&&) = delete;
			void operator<<(dumbPtr<mu, releaseable>& right) = delete;
			void operator<<(impPtr<mu, releaseable>& right) = delete;
			inline void operator<<(memPtr& right) { this->moveIn(right); }
			void swap(dumbPtr<mu, releaseable>& right) = delete;
			void swap(impPtr<mu, releaseable>& right) = delete;
			inline void swap(memPtr<mu, releaseable>& right) { this->dumbPtr<mu, releaseable>::swap((dumbPtr<mu, releaseable>&)right); }
		};

		// special param in save_fetch function
		//do not do any relative with this structure till you know how this lib works completely.
		struct para {
			union {
				struct {
					uint8_t* startPointer;				//where the section start pointed by ptrTable
					uint8_t* likelyPointer;				//it asserts the position of the next key value
				} section;									//r
				std::vector<uint8_t>* sectionVector;		//w
				ReflectResult* reflection;					//r
				ReflectResultKeyValue* reflection_single;	//w
#if MEM_RJSON_ON
				rapidjson::Value* rapidJson_section;		//r w
#endif
#if MEM_MYSQL_ON
                struct {
                    MYSQL_BIND *bind;                       //bind
                    size_t *resized;                        // resized string count
                } mysql;
                std::vector<base::mysql_meta> *mysql_metadata_v; //get metadata
#endif
			};
            enum : uint8_t
            {
                binary_serialize_manager,   // serialize whole manager
                binary_deserialize_manager, // deserialize whole manager
                reflection_read,               // read variable using reflection
                reflection_write,              // write variable using reflection (once for single variable)
                binary_serialize_base,		   // serialize single eb::base, ignore pointers
                binary_deserialize_base,       // deserialize single eb::base, ignore pointers
                rjson_seriazlize,              // rapid json stringify
                rjson_deseriazlize,            // rapid json parse
                mysql_bind,                    // bind variables within MYSQL_BIND, forced to use STMT.
                mysql_checksize,               // Check if the string length <= MYSQL_BIND[].length (the size of the data is), and resize which is short.
                mysql_metadata,                // get metadata, only valid on mysql supported data type
                mysql_bind_w,                  // bind MySQL without string length.
            } order;
            inline bool isConstruct()
			{
				switch (order)
				{
				case binary_deserialize_manager:
				case reflection_write:
				case binary_deserialize_base:
				case rjson_deseriazlize:
					return true;
				}
				return false;
            }
            inline bool isArchive()
            {
                switch (order)
                {
                case binary_serialize_manager:
                case reflection_read:
                case binary_serialize_base:
                case rjson_seriazlize:
                    return true;
                }
                return false;
            }
        };

#if MEM_REFLECTION_ON
		// key-value result in a base class of the static reflection
		class ReflectResultKeyValue {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				template<bool _void>
			inline void MatchVariant(void* vtptr, uint32_t& type, memPtr<base>& ptr);
			template<bool _void, typename IterFirst, typename... IterArgs>
			void MatchVariant(void* vtptr, uint32_t& type, memPtr<base>& ptr);
			template<bool isSet, class _T, class e_T>
			inline void SetOrGetType(_T& thisSide, _T& baseSide, e_T et);
			template <bool isSet> inline void TypeFunc(bool& va) { SetOrGetType<isSet>(value._bool, va, bool_); };
			template <bool isSet> inline void TypeFunc(char& va) { SetOrGetType<isSet>(value._char, va, char_); };
			template <bool isSet> inline void TypeFunc(unsigned char& va) { SetOrGetType<isSet>(value.u8, va, u8); };
			template <bool isSet> inline void TypeFunc(signed char& va) { SetOrGetType<isSet>(value.i8, va, i8); };
			template <bool isSet> inline void TypeFunc(unsigned short& va) { SetOrGetType<isSet>(value.u16, va, u16); };
			template <bool isSet> inline void TypeFunc(short& va) { SetOrGetType<isSet>(value.i16, va, i16); };
			template <bool isSet> inline void TypeFunc(unsigned int& va) { SetOrGetType<isSet>(value.u32, va, u32); };
			template <bool isSet> inline void TypeFunc(int& va) { SetOrGetType<isSet>(value.i32, va, i32); };
			template <bool isSet> inline void TypeFunc(unsigned long& va) { SetOrGetType<isSet, unsigned int>(value.u32, reinterpret_cast<unsigned int&>(va), u32); };
			template <bool isSet> inline void TypeFunc(long& va) { SetOrGetType<isSet, int>(value.i32, reinterpret_cast<int&>(va), i32); };
			template <bool isSet> inline void TypeFunc(unsigned long long& va) { SetOrGetType<isSet>(value.u64, va, u64); };
			template <bool isSet> inline void TypeFunc(long long& va) { SetOrGetType<isSet>(value.i64, va, i64); };
			template <bool isSet> inline void TypeFunc(float& va) { SetOrGetType<isSet>(value.f32, va, f32); };
			template <bool isSet> inline void TypeFunc(double& va) { SetOrGetType<isSet>(value.f64, va, f64); };
			template <bool isSet, class _MU, bool re> inline void TypeFunc(memPtr<_MU, re>& va);
			//template<bool isSet, class First, class ...Args>inline void TypeFunc(pVariant<First, Args...>& va);
			template<class _T>
			inline void WriteMU(const char* muProperty_key, _T& muProperty);
			inline void WriteMU(const char* muProperty_key, wchar_t* muProperty, uint32_t size);
			inline void WriteMU(const char* muProperty_key, char* muProperty, uint32_t size);
		public:
			std::string key;
			enum {
				void_,	//never use
				bool_,
				char_,
				wchar_,
				char32_,
				char64_,
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
				mUnit_p
			}type;
			enum {
				single,
				array,				//continuous address and cannot be resized
				vector,				//continuous address and able to resize
				other,				//discontinuity address and able to resize
				variant
			}container;
			uint32_t size;			//size of the container.
			uint32_t at;			//the current position of the container.
			bool isConst;			//is read only data
			union {
				bool bool_;
				char char_;
				wchar_t wchar_;
				char32_t char32_;
				uint64_t char64_;
				uint8_t u8;
				int8_t i8;
				uint16_t u16;
				int16_t i16;
				uint32_t u32;
				int32_t i32;
				uint64_t u64;
				int64_t i64;
				float f32;
				double f64;
				base* mUnit_p;
			}value;
			inline ReflectResultKeyValue();
			template<class _T, typename std::enable_if<
				!std::is_convertible<_T, const wchar_t*>::value &&
				!std::is_convertible<_T, const char*>::value &&
				!std::is_convertible<_T, base*>::value
				, int>::type = 0>
			inline ReflectResultKeyValue(const char* _key, _T& va);
			inline ReflectResultKeyValue(const char* _key, const wchar_t* va);
			inline ReflectResultKeyValue(const char* _key, const char* va);
			inline ReflectResultKeyValue(const char* _key, base* va);
		};

		// static reflection on a base class which results were formatted as a key-value array
		struct ReflectResult {
			std::vector<ReflectResultKeyValue> context;
		};
#endif

		// only put two keys together, one is written in eb::base and another is written in sub-struct.
        template <typename _T>
        static void GWPP_sub(base *mem, const char *key1, const char *key2, _T &var, para &para);

        // it copys binary memory and moves the content pointer to the next
		template<typename _T>
		inline static void GWPP_memcpy(uint8_t*& content, _T& var, para& para)
		{
			if (para.isConstruct())
			{
				::memcpy(&var, content, sizeof(_T));
			}
			else
			{
				::memcpy(content, &var, sizeof(_T));
			}
			content += sizeof(_T);
		}

		// ingress class, any class required to be linked by egress must inherit this class
		class Ingress :public base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
			Ingress(const Ingress&) = default;
			Ingress(Ingress&&) = default;
			Ingress& operator=(const Ingress& right) = default;
			//fulfill it, return a const char type name
			virtual const char* getConstTypeName() = 0;
		protected:
			inline Ingress(manager* m) :base(m) {};
			inline ~Ingress();
		};

		// internal ingress reference to save, never use
		class refIngress final :public base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				inline refIngress(manager* m) :base(m) {}
			impPtr<Ingress> ptrIng;
			std::string keyword;
			std::string type;
			inline void save_fetch(para para) override;
		};

		enum egressFindErr :uint8_t {
			ok,						//get Ingress successfully
			empty_egress,			//pEgress is empty
			file_notfound,			//the file name bind with this Egress not found in global file list
			keyword_notfound,		//cannot found corresponding keyword in target file
			cast_failed				//failed cast ingerss to egress type
		};
		// egress class that only records within manager
		class Egress final :public base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				inline Egress(manager* m) :base(m) {}
			inline Egress(manager* m, memPtr<Subfile> upLevel) : base(m) { ptrUpLevel = upLevel; }
			Egress(const Egress& right) = delete;
			Egress(const Egress&& right) = delete;
			void operator=(const Egress& right) = delete;
			~Egress();
			inline void save_fetch(para para) override;
			template<typename cast>
			egressFindErr getTarget(cast*& varReturn);
			dumbPtr<Ingress> ptrDirect;
		public:
			std::string keyword;
			std::string type;
			dumbPtr<Subfile> ptrUpLevel;						//don't worry ptrUpLevel will be undefined, it will load in SubFile.save_fetch.
		};

		// egress smart pointer wrapped by template
		// RTTI must be enabled.
		template<class cast>
		struct pEgress :public memPtr<Egress> {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				static_assert(std::is_base_of<Ingress, cast>::value, "pEgress template ERROR: cast target must derived from Ingress class");
			void getFileName(char* strOut, uint32_t length);
			//getTarget will find target manager in global file list. remember upload target manager before use this function.
			egressFindErr getTarget(cast*& varReturn);
			void makeEIPair(manager* egressMngr, const memPtr<cast>& target, const char* kw);
			inline pEgress() {}
			pEgress(manager* egressMngr, const memPtr<cast>& target, const char* kw);
		};

		// subfile class that only records within manager denotes another sub-manager that this manager is using.
		class Subfile :public base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
				inline Subfile(manager* m) : base(m) {};
			Subfile(const Subfile& right) = delete;
			Subfile(const Subfile&& right) = delete;
			void operator=(const Subfile& right) = delete;
			~Subfile();
			void save_fetch(para para) override;
			vector_memPtr<Egress> egresses;
		public:
			//1 for success, 0 for fail
			inline int findGlobalManager();
			std::string fileName;
			//never save-fetch
			dumbPtr<manager> ptrManager;
			//try to load the relative manager with specified directory before the file name. 0 for failed, 1 for successed.
			template<typename _type>
			bool tryLoadSubfile(const char* directory = nullptr);
		};

		// the manager of bulk structure
		//	an instance of this class == a file in the disk
		//	also derived from meta structure
		class manager :public base {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
		public:
			manager();
			manager(const manager& m) = delete;			//if we just copy every eb::base, it will causes the disorder of the pointers within every eb::base.
			manager(manager&& m) = delete;
			void operator=(const manager& m) = delete;		//the only correct way to copy managers is to binary serialize and deserialize.
			void operator=(manager&& m) = delete;
			manager(const char* path);
			virtual ~manager();
            std::string url;
			//save, download things from memory to disk. returns false if failed.
			bool download();
			//fetch, upload things from disk to memory. returns false if failed.
			bool upload();
			//find specific subfile in this manager by path. subfile is the file be used for Egress
			memPtr<Subfile> findSubfile(const char* fileName);
			//find Egress by keyword and type
			memPtr<Egress> findEgress(const memPtr<Subfile> subfile, const char* kw, const char* type);
			//find Ingress by keyword and type
			impPtr<Ingress> findIngress(const char* kw, const char* type);
			//set the path of the manager
			inline void setUrl(const std::string& scptr) { url = scptr; }
			//if url is vacancy returns nullptr
			char* getFileName();
			//binary deserialize the whole manager
			bool deserialize(uint8_t* Ptr, uint32_t StringSize);
			//binary serialize the whole manager
			void serialize(std::vector<uint8_t>* bc);
			//binary deserialize the specific sub object
			template<typename T>
			bool deserializeSub(T& sub, uint8_t* Ptr, uint32_t StringSize);
			//binary serialize the specific sub object
			template<typename T>
			bool serializeSub(T& sub, const std::string&);
#if MEM_RJSON_ON
			//binary Json deserialize the specific sub object
			template<typename T>
			bool deserializeSubJson(T& sub, uint8_t* Ptr, uint32_t StringSize);
			//binary Json serialize the specific sub object
			template<typename T>
			bool serializeSubJson(T& sub, const std::string&);
#endif
			uint32_t statusBadValue = 0;																					//the sum of bad value in the last deserialize call
		private:
			//make a pair of Ingress-Egress. This function execute by egress manager, and it will make an Ingress to point target eb::base in target manager, and returns an Egress.
			[[nodiscard]] memPtr<Egress> makeEgress_IngressPair(const impPtr<Ingress>& target, const char* kw);
			void thisCons();
			void thisDest();
			virtual void save_fetch(para para) = 0;
			memPtr<refIngress> findIngressRef(const char* kw, const char* type);
			std::set<base*> bases;
			vector_memPtr<Subfile> subFiles;
			vector_memPtr<refIngress> ingressInte;

			// save_fetch() function intermediate stuff
			std::set<lowlevel::memPtrCorr> ptrTable;
#if MEM_RJSON_ON
			struct {
				rapidjson::Document::AllocatorType* allocator = nullptr;
				rapidjson::Document* doc = nullptr;
			}rjson;
#endif
			union {
				std::vector<uint8_t>* bytes;	//for write
				struct {
					uint8_t* start;
					uint8_t* end;
				};								//for read
			}binSeri;

			//global file list (files loaded)
			inline static std::mutex global_mutex;
			inline static std::vector<manager*> global_load;
		};

		template<typename..._Args, typename _Out>
		inline static bool pVariantGet(std::variant<_Args...>& vari, _Out*& out) {
			memPtr<_Out>* get = std::get_if<memPtr<_Out>>(&vari);
            if (get)
            {
                out = **get;
                if (out)
                    return true;
            }
            else
            {
                out = nullptr;
            }
            return false;
        };

		//variant of function pointer
		template<typename illg, int ID> class pFunction { static_assert(!std::is_same_v<illg, illg>, "not a function"); };
		template<typename Ret, typename ...Args, int ID> class pFunction<Ret(Args...), ID> {
			__MEMMNGR_INTERNAL_HEADER_PERMISSION
			using funcPtr = Ret(*)(Args...);
			inline void save_fetch_struct(uint8_t* content, para& para) { GWPP_memcpy(content, type, para); }
			inline static constexpr uint32_t getSizeofFunctions()
			{
				//static const 'Functions' run in same logic as variant templates
				//this will occurance an error when the 'Functions' variable don't got correct init
				constexpr uint32_t ret = sizeof(Functions);
				return ret;
			}
		public:
			uint32_t type;
			constexpr static size_t save_fetch_size = sizeof(type);
			static const funcPtr Functions[];
			inline Ret operator()(Args&&...args)const {
				assert((type < getSizeofFunctions() / sizeof(funcPtr)) || !("pFunction is empty or invaild"));
				return Functions[type](std::forward<Args>(args)...);
			}
			inline pFunction() { type = 0xffffffff; }
			inline pFunction(funcPtr newPtr) { this->operator=(newPtr); }
			inline void operator=(funcPtr newPtr) {
				//damn the function pointer cannot use constexpr
				for (uint32_t i = 0; i < getSizeofFunctions() / sizeof(funcPtr); i++)
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
				assert((type < getSizeofFunctions() / sizeof(funcPtr)) || !("pFunction is empty or invaild"));
				if (ptr == Functions[type])
					return true;
				return false;
			}
			inline bool isFilled() const{
                return type < getSizeofFunctions() / sizeof(funcPtr);
            }
            inline bool isEmpty() const{
                return !this->isFilled();
            }
            inline funcPtr operator*() const  {
                if (this->isFilled())
                    return Functions[type];
                return nullptr;
            }
        };
		//MACRO: use to initialize variant (Functions) in pFunction 
#define INITIALIZE_PFUNCTION(___Signature___,___ID___, ...) template<> const eb::pFunction<___Signature___,___ID___>::funcPtr eb::pFunction<___Signature___,___ID___>::Functions[] = { __VA_ARGS__ };

#include "internal/definitions.h"
	}
}
#ifdef _MSC_VER
#pragma warning(default: __MEM_MANAGER_DISABLE_WARINIGS)
#endif