#ifdef _WIN32
#define MEM_MAX_FILE_PATH 260
#else
#define MEM_MAX_FILE_PATH 4096
#endif

namespace file {
	using handleType = FILE*;

	inline void Remove(const char* c) {
		remove(c);
	}

	inline void* Fopen_r(const char* c) {
		return fopen(c, "rb");
	}

	inline void* Fopen_w(const char* c) {
		return fopen(c, "wb");
	}

	inline size_t GetSize(void* handle) {
		fseek((handleType)handle, 0, SEEK_END);

		size_t size = ftell((handleType)handle);

		fseek((handleType)handle, 0, SEEK_SET);
		return size;
	}

	inline size_t Fwrite(void* handle, const uint8_t* content, size_t size) {
		return fwrite(content, 1, size, (handleType)handle);
	}

	inline size_t Fread(void* handle, uint8_t* buffer, size_t size) {
		return fread(buffer, 1, size, (handleType)handle);
	}

	inline void Fclose(void* h) {
		fclose((handleType)h);
	}
}