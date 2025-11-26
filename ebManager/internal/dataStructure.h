//archive file structure definition
constexpr uint8_t magicOfFile[] = { 0x17, 0x3a, 0x56 };
constexpr uint8_t magicOfFile32[] = { 0x17, 0x3a, 0x56, 0x32 };
constexpr uint8_t magicOfFile64[] = { 0x17, 0x3a, 0x56, 0x64 };

#if defined(_MSC_VER)  // MSVC
#define MEM_PACKED_STRUCT(___name___) \
        __pragma(pack(push, 1)) struct ___name___ 
#define MEM_PACKED_STRUCT_END \
        __pragma(pack(pop))
#else
#define MEM_PACKED_STRUCT(___name___) \
        struct __attribute__((packed, aligned(1))) ___name___
#define MEM_PACKED_STRUCT_END
#endif

MEM_PACKED_STRUCT(headerOfFile) {
	uint8_t magic[4];
	uint64_t pointerOfFirst;
	uint32_t offsetOfSubfile;
	uint32_t offsetOfIngress;
	uint32_t offsetOfPtrTable;
	uint32_t sizeOfPtrTable;
	enum :uint8_t {
		Big_Endian = 0,
		Little_Endian = 1,
	} endian;
	uint8_t reserve[31];
};
MEM_PACKED_STRUCT_END
// that means a single file cannot be larger than 4GB. that is enough. who needs a 4GB large file?


MEM_PACKED_STRUCT(variantOfFile) {
	uint32_t offset;
	uint32_t type;
};
MEM_PACKED_STRUCT_END
//how the variant save in the file


MEM_PACKED_STRUCT(pairOfFile) {
	uint32_t offset;
	uint32_t sizeOfFirst;
};
MEM_PACKED_STRUCT_END
//how the pair save in the file


MEM_PACKED_STRUCT(optionalOfFile) {
	uint32_t offset;
};
MEM_PACKED_STRUCT_END
//how the optional save in the file

inline bool is_big_endian() {
	static const union endian {
		int i;
		char c[4];
	}u = {1};
	return (u.c[0] == 0);
}
//judge the endian of the intend machine

namespace json_const {
	inline static const char json_type[] = "Variant Type";
	inline static const char json_value[] = "Value";
	inline static const char json_recurring[] = "Recurring Object";
	inline static const char json_first[] = "First";
	inline static const char json_second[] = "Second";
	inline static const char json_year[] = "Year";
	inline static const char json_month[] = "Month";
	inline static const char json_day[] = "Day";
	inline static const char json_date[] = "Date";
	inline static const char json_hour[] = "Hour";
	inline static const char json_minute[] = "Minute";
	inline static const char json_secondT[] = "Second";
	enum class json_time_mode_t
	{
		object_second, // precise to second
		object_day, // precise to day
		unix_s,
		unix_ms,
		built_in_value,
		string_Y_M_D
	};
	inline static json_time_mode_t json_time_mode = json_time_mode_t::object_second;
};