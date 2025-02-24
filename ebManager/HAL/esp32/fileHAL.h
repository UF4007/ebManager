//if failed, check your partition table setting and assign enough space for memManager.
#define MEM_MAX_FILE_PATH 15

namespace file
{
    constexpr size_t block_size = 3500; // Blob larger than 3500 bytes must be divided into another blob chunk.
    using handleType = nvs_handle_t;
    static inline bool isNvsInit = []
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            return !nvs_flash_init();
        }
        return !ret;
    }();

    inline void Remove(const char *c)
    {
        nvs_handle handle2;
        if (nvs_open(c, NVS_READWRITE, &handle2) != ESP_OK)
                return;
        nvs_erase_all(handle2);
        nvs_close(handle2);
    }

    inline void *Fopen_r(const char *c)
    {
        handleType ret;
        nvs_open(c, NVS_READONLY, &ret);
        return (void *)ret;
    }

    inline void *Fopen_w(const char *c)
    {
        handleType ret;
        nvs_open(c, NVS_READWRITE, &ret);
        return (void *)ret;
    }

    inline size_t GetSize(void *handle)
    {
        uint64_t ret = 0;
        nvs_get_u64((handleType)handle, "s", &ret);
        return ret;
    }

    inline size_t Fwrite(void *handle, const uint8_t *content, size_t size)
    {
        if (nvs_set_u64((handleType)handle, "s", size) == ESP_OK)
            if (nvs_set_blob((handleType)handle, "c", content, size) == ESP_OK)
                return size;
        return 0;
    }

    inline size_t Fread(void *handle, uint8_t *buffer, size_t size)
    {
        if (nvs_get_blob((handleType)handle, "c", buffer, &size) == ESP_OK)
        {
            return size;
        }
        return 0;
    }

    inline void Fclose(void *h)
    {
        nvs_close((handleType)h);
    }
}