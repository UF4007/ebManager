//definitions of member functions



//dumbPtr
template<typename mu, bool releaseable>
template<class _any, bool _r>
inline void dumbPtr<mu, releaseable>::equalAny(const dumbPtr<_any, _r>& mp)
{
	cdd();
	if (mp.isEmpty())
	{
		ptr = nullptr;
	}
	else
	{
		ptr = mp.ptr;
		(ptr->count)++;
	}
}
template<typename mu, bool releaseable>
inline dumbPtr<mu, releaseable>::dumbPtr()
{
	static_assert(std::is_base_of_v<base, mu>, "smart pointer template ERROR: template is not Base on base");
	ptr = nullptr;
}
template<typename mu, bool releaseable>
inline dumbPtr<mu, releaseable>::dumbPtr(mu* pmu)
{
	static_assert(std::is_base_of_v<base, mu>, "smart pointer template ERROR: template is not Base on base");
	if (pmu)
	{
		if (((base*)pmu)->sharedPtr)
		{
			ptr = ((base*)pmu)->sharedPtr;
			(ptr->count)++;
		}
		else
		{
			ptr = new lowlevel::memPtrComm((base*)pmu);
			((base*)pmu)->sharedPtr = ptr;
		}
	}
	else
	{
		ptr = nullptr;
	}
}
template<typename mu, bool releaseable>
inline dumbPtr<mu, releaseable>::dumbPtr(const dumbPtr<mu, releaseable>& mp) noexcept
{
	if (mp.isEmpty())
	{
		ptr = nullptr;
	}
	else
	{
		ptr = mp.ptr;
		(ptr->count)++;
	}
}
template <class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::operator=(const dumbPtr<mu, releaseable> &mp) noexcept
{
    // assert(&mp != this || !"Equals self results undefined behavior.");
    if (&mp == this)
        return;
    cdd();
    if (mp.isEmpty())
    {
        ptr = nullptr;
    }
    else
    {
        ptr = mp.ptr;
        (ptr->count)++;
    }
}
template <typename mu, bool releaseable>
inline dumbPtr<mu, releaseable>::dumbPtr(dumbPtr<mu, releaseable> &&mp)  noexcept : ptr(mp.ptr)
{
    mp.ptr = nullptr;
}
template <class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::operator=(dumbPtr<mu, releaseable> &&mp) noexcept
{
    cdd();
    this->ptr = mp.ptr;
    mp.ptr = nullptr;
}
template<typename mu, bool releaseable>
inline dumbPtr<mu, releaseable>::~dumbPtr() {
	cdd();
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::cdd() {
	if (ptr)
		if (ptr->count)
		{
			if (ptr->count == 1)
			{
				if (ptr->content)
					if (ptr->content->mngr)
						delete(ptr->content);
				delete(ptr);
				ptr = nullptr;
			}
			else
				(ptr->count)--;
		}
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::degeneracy()
{
	if (ptr)
		if (ptr->count)
		{
			if (ptr->count == 1)
			{
				if (ptr->content)
					ptr->content->sharedPtr = nullptr;
				delete ptr;
				ptr = nullptr;
			}
			else
				(ptr->count)--;
		}
}
template<class mu, bool releaseable>
template<typename _anotherMu>
inline std::enable_if_t<std::is_base_of<base, _anotherMu>::value, bool>
dumbPtr<mu, releaseable>::precision_cast(_anotherMu* pamu) {
	if (pamu == nullptr)
		return false;
	if constexpr (std::is_base_of<mu, _anotherMu>::value)
	{
		this->operator=(reinterpret_cast<mu*>(pamu));
		return true;
	}
	else
	{
#if MEM_RTTI_ON
		this->operator=(dynamic_cast<mu*>(pamu));
#else
		return false;
#endif
	}
	return false;
}
template<class mu, bool releaseable>
template<typename _anotherMuPtr>
inline std::enable_if_t<std::is_base_of<base, typename std::remove_pointer<_anotherMuPtr>::type>::value, _anotherMuPtr>
dumbPtr<mu, releaseable>::precision_cast() {
	using _anotherMu = typename std::remove_pointer<_anotherMuPtr>::type;
	if (this->isEmpty())
		return nullptr;
	if constexpr (std::is_base_of<_anotherMu, mu>::value)
	{
		return reinterpret_cast<_anotherMu*>(ptr->content);
	}
	else
	{
#if MEM_RTTI_ON
		this->operator=(dynamic_cast<_anotherMu*>(ptr->content));
#else
		return nullptr;
#endif
	}
	return nullptr;
}
template<typename mu, bool releaseable>
template<typename _any, bool _r>
inline dumbPtr<mu, releaseable>::dumbPtr(const dumbPtr<_any, _r>& mp) {
	static_assert(std::is_base_of_v<base, mu>, "smart pointer template ERROR: template is not Base on base");
	static_assert((!std::is_same_v<mu, _any> &&
		(std::is_base_of_v<mu, _any> //|| std::is_base_of_v<_any, mu>						//no idea to transfer base into derive without RTTI. consider precision_cast
			) && _r == releaseable), "smart pointer polymorphic ERROR: param pointer has nothing polymorphic to do with the new pointer");
	ptr = nullptr;
	this->equalAny(mp);
}
template<typename mu, bool releaseable>
template<typename _any, bool _r>
inline typename std::enable_if<
	(!std::is_same_v<mu, _any> &&
		(std::is_base_of_v<mu, _any> //|| std::is_base_of_v<_any, mu>
			) &&_r == releaseable), void>::type
dumbPtr<mu, releaseable>::operator=(const dumbPtr<_any, _r>& mp) {
	this->equalAny(mp);
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::operator=(mu* pmu)
{
	cdd();
	if (pmu)
	{
		if (((base*)pmu)->sharedPtr)
		{
			ptr = ((base*)pmu)->sharedPtr;
			(ptr->count)++;
		}
		else
		{
			ptr = new lowlevel::memPtrComm((base*)pmu);
			((base*)pmu)->sharedPtr = ptr;
		}
	}
	else
	{
		ptr = nullptr;
	}
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::swap(dumbPtr& sw)
{
	lowlevel::memPtrComm* i = ptr;
	ptr = sw.ptr;
	sw.ptr = i;
}
template<class mu, bool releaseable>
inline bool dumbPtr<mu, releaseable>::isFilled() const
{
	if (ptr)
	{
		return ptr->content;
	}
	return 0;
}
template<class mu, bool releaseable>
inline bool dumbPtr<mu, releaseable>::isEmpty() const
{
	return !this->isFilled();
}
template<class mu, bool releaseable>
inline bool dumbPtr<mu, releaseable>::operator==(const dumbPtr& right) const
{
	if (isFilled())
		return ptr == right.ptr;
	else if (right.isEmpty())
		return true;
	else
		return false;
}
template<class mu, bool releaseable>
inline bool dumbPtr<mu, releaseable>::operator==(const mu* right) const {
	if (isFilled())
		return ptr->content == (base*)right;
	else if (right == nullptr)
		return true;
	else
		return false;
}
template<class mu, bool releaseable>
inline mu* dumbPtr<mu, releaseable>::operator*() const
{
	if (isFilled())
		return (mu*)(ptr->content);
	else
		return nullptr;
}
template<class mu, bool releaseable>
inline mu* dumbPtr<mu, releaseable>::operator->() const
{
	if (isFilled())
		return (mu*)(ptr->content);
	else
		return nullptr;
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::release()
{
	static_assert(releaseable == true, "smart pointer release() ERROR: this memPtr was defined that cannot be released");
	if (this->isFilled())
	{
		base* i = ptr->content;	//here cannot be optimized. although the program will execute the same operation in the Base class base destructor C++ executes the destructor in the derived class first, must erase memPtr of base before the derived class destructor runs.
		ptr->content = nullptr;
		delete i;
	}
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::moveIn(dumbPtr& right) {
	cdd();
	ptr = right.ptr;
	right.ptr = nullptr;
}
template<class mu, bool releaseable>
inline void dumbPtr<mu, releaseable>::supplantIn(mu* newOne) {
	static_assert(releaseable == true, "smart pointer release() ERROR: this memPtr was defined that cannot be released");
	if (this->isFilled())
	{
		base* i = ptr->content;
		ptr->content = (base*)newOne;
		delete i;
	}
	else
	{
		this->operator=(newOne);
	}
}



#if MEM_REFLECTION_ON
//ReflectResultKeyValue
template<bool _void>
inline void ReflectResultKeyValue::MatchVariant(void* vtptr, uint32_t& type, memPtr<base>& ptr)
{
	type = 0;
	ptr = nullptr;
}
template<bool _void, typename IterFirst, typename... IterArgs>
inline void ReflectResultKeyValue::MatchVariant(void* vtptr, uint32_t& type, memPtr<base>& ptr) {
	if (base::get_vtable_ptr<IterFirst>::ptr() == vtptr)
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
inline void ReflectResultKeyValue::SetOrGetType(_T& thisSide, _T& baseSide, e_T et) {
	if constexpr (isSet)
	{
		thisSide = baseSide;
		type = et;
	}
	else
	{
		key[0] = L'\0';
		baseSide = thisSide;
	}
}
//template<bool isSet, class First, class ...Args>
//inline void ReflectResultKeyValue::TypeFunc(pVariant<First, Args...>& va)
//{
//	if constexpr (isSet)
//	{
//		type = p_variant;
//		value.ptr = reinterpret_cast<base*>(*(va.ptr));
//	}
//	else
//	{
//		va.type = 1;
//		MatchVariant<false, First, Args...>(*(void**)std::addressof(*value.ptr), va.type, va.ptr);
//	}
//}
template <bool isSet, class _MU, bool re>
inline void ReflectResultKeyValue::TypeFunc(memPtr<_MU, re>& va) {
	if constexpr (isSet)
	{
		type = p_base;
		value.ptr = reinterpret_cast<base*>(*va);
	}
	else
	{
		//if (typeid(_MU) == typeid(*value.ptr))
		if (base::get_vtable_ptr<_MU>::ptr() == *(void**)std::addressof(*value.ptr))		//if RTTI was banned use the vTable pointer to judge
		{
			key[0] = L'\0';
			va = reinterpret_cast<_MU*>(value.ptr);
		}
	}
};
template<class _T>
inline void ReflectResultKeyValue::WriteMU(const char* muProperty_key, _T& muProperty) {
	if (strcmp(muProperty_key, key.data()) == 0)
	{
		TypeFunc<false>(muProperty);
	}
}
inline void ReflectResultKeyValue::WriteMU(const char* muProperty_key, wchar_t* muProperty, uint32_t size) {
	if (strcmp(muProperty_key, key.data()) == 0)
	{
		key[0] = L'\0';
		lowlevel::wcscpy_mem(muProperty, value.wchar_p, size);
	}
}
inline void ReflectResultKeyValue::WriteMU(const char* muProperty_key, char* muProperty, uint32_t size) {
	if (strcmp(muProperty_key, key.data()) == 0)
	{
		key[0] = L'\0';
		memcpy(muProperty, value.char_p, size);
	}
}
inline ReflectResultKeyValue::ReflectResultKeyValue() {
	key[0] = L'\0';
	type = void_;
	value.u64 = 0;
}
template<class _T, typename std::enable_if<
	!std::is_convertible<_T, const wchar_t*>::value &&
	!std::is_convertible<_T, const char*>::value &&
	!std::is_convertible<_T, base*>::value
	, int>::type>
inline ReflectResultKeyValue::ReflectResultKeyValue(const char* _key, _T& va) {
	key = _key;
	TypeFunc<true>(va);
}
inline ReflectResultKeyValue::ReflectResultKeyValue(const char* _key, const wchar_t* va) {
	key = _key;
	value.wchar_p = (wchar_t*)va;
	type = wchar_;
}
inline ReflectResultKeyValue::ReflectResultKeyValue(const char* _key, const char* va) {
	key = _key;
	value.char_p = (char*)va;
	type = char_;
}
inline ReflectResultKeyValue::ReflectResultKeyValue(const char* _key, base* va) {
	key = _key;
	value.ptr = va;
	type = p_base;
}
inline void base::reflectionRead(ReflectResult* output)
{
	para para;
	para.reflection = output;
	para.order = para::reflection_read;
	this->save_fetch(para);
}
inline bool base::reflectionWrite(ReflectResultKeyValue inputKW)
{
	para para;
	para.reflection_single = &inputKW;
	para.order = para::reflection_write;
	this->save_fetch(para);
	return !(inputKW.key[0]);		//put first char to '\0' if write success
}
#endif



//base
inline base::base(manager* manager) {
	this->mngr = manager;
	if (manager)
		manager->bases.insert(this);
	//else assert(!"a manager must be designated when creating a new base.");					//technically we allow this behavior
	sharedPtr = nullptr;
}
inline base::base(const base& munit) {
	mngr = munit.mngr;
	mngr->bases.insert(this);
	sharedPtr = nullptr;
}
inline base::base(base&& munit) noexcept {
	mngr = munit.mngr;
	mngr->bases.insert(this);
	if (sharedPtr)
	{
		sharedPtr->content = this;
		munit.sharedPtr = nullptr;
	}
}
inline base::~base()
{
	if (mngr)
	{
		auto i = mngr->bases.find(this);
		if (*i == this)
			mngr->bases.erase(i);
	}
	if (sharedPtr)
		sharedPtr->content = nullptr;
}



//manager
inline manager::manager() :base() {
	thisCons();
}
inline manager::manager(const char* path) {
	thisCons();
    url = path;
}
inline manager::~manager()
{
	thisDest();
	if (this->rjson.doc != nullptr)
	{
		delete this->rjson.doc;
	}
}
inline char* manager::getFileName()
{
	if (url[0]) {
		char* ret = strrchr(url.data(), '\\');
		if (ret)
			return &ret[1];
		else
			return url.data();
	}
	return nullptr;
}
inline void manager::thisCons()
{
	this->mngr = this;
    std::lock_guard<std::mutex> lck (manager::global_mutex);
	global_load.push_back(this);
}
inline void manager::thisDest()
{
    {
        std::lock_guard<std::mutex> lck(manager::global_mutex);
        for (auto i = global_load.begin(); i != global_load.end(); i++)
        {
            if (*i == this)
            {
                global_load.erase(i);
                break;
            }
        }
    }
    this->mngr = nullptr;				//it is a sign to protect against repeat destruction
	auto iter = bases.begin();				//other bases will be erased in the destructor of the specific base
	for (; iter != bases.end();)
	{
		base* mu = *iter;
		bases.erase(iter);
		mu->mngr = nullptr;
		if (mu->sharedPtr)						//here cannot be optimized, see memPtr::release()
			mu->sharedPtr->content = nullptr;
		delete mu;
		iter = bases.begin();
	}
	//subFiles and ingressInte are also base that will be destructed when all of base are destructed.
}
inline bool manager::download()
{
	char* fpath = url.data();

	eb::file::Remove(fpath);

	std::vector<uint8_t> bc = {};
	bc.reserve(1000);

	this->serialize(&bc);

	void* handle = eb::file::Fopen_w(fpath);
	if (!handle)
	{
		return 0;
	}

	if (eb::file::Fwrite(handle, &bc[0], bc.size()))
	{
		eb::file::Fclose(handle);
		return 0;
	}

	eb::file::Fclose(handle);

	return 1;
}
inline bool manager::upload()
{
	char* fpath = url.data();

	void* handle = eb::file::Fopen_r(fpath);
	if (!handle)
	{
		return 0;
	}
	uint64_t fileSize = eb::file::GetSize(handle);
	if (!fileSize)
	{
		eb::file::Fclose(handle);
		return 0;
	}

	uint8_t* bytes = (uint8_t*)::operator new(fileSize + 10);
	memset(bytes, 0, fileSize + 10);
	bool ret = false;
	if (fileSize == eb::file::Fread(handle, bytes, fileSize))
	{
		ret = this->deserialize(bytes, fileSize);
	}
	eb::file::Fclose(handle);
	::operator delete(bytes);
	return ret;
}
inline memPtr<Subfile> manager::findSubfile(const char* fileName)
{
	assert(fileName || !("target manager's fileName cannot be null"));
	for (auto iter = subFiles.begin(); iter != subFiles.end(); iter++)
	{
		auto& i = *iter;
		if (i.isFilled())
		{
			if (strcmp(fileName, i->fileName.c_str()) == 0)
				return i;
		}
		else
		{
			iter = subFiles.erase(iter);
		}
	}
	return nullptr;
}
inline memPtr<Egress> manager::findEgress(const memPtr<Subfile> subfile, const char* kw, const char* type)
{
	for (auto iter = subfile->egresses.begin(); iter != subfile->egresses.end(); iter++)
	{
		auto& k = *iter;
		if (k.isFilled())
		{
			if (strcmp(k->keyword.c_str(), kw) == 0 && strcmp(k->type.c_str(), type) == 0)
				return k;
		}
		else
		{
			iter = subfile->egresses.erase(iter);
		}
	}
	return nullptr;
}
inline memPtr<refIngress> manager::findIngressRef(const char* kw, const char* type) {
	for (auto iter = ingressInte.begin(); iter != ingressInte.end(); iter++)
	{
		auto& i = *iter;
		if (i->ptrIng.isFilled())
		{
			if (strcmp(i->keyword.c_str(), kw) == 0 && strcmp(i->type.c_str(), type) == 0)
				return i;
		}
		else
		{
			iter = ingressInte.erase(iter);
		}
	}
	return nullptr;
}
inline impPtr<Ingress> manager::findIngress(const char* kw, const char* type)
{
	auto found = findIngressRef(kw, type);
	if (found.isFilled())
		return found->ptrIng;
	return nullptr;
}
[[nodiscard]] inline memPtr<Egress> manager::makeEgress_IngressPair(const impPtr<Ingress>& target, const char* kw)
{
	if (target.isEmpty())
		return nullptr;
	const char* typeName = target->getConstTypeName();

	manager* targetManager = target->getManager();

	memPtr<refIngress> ingrP = targetManager->findIngressRef(kw, typeName);
	if (ingrP == nullptr)
	{
		ingrP = new refIngress(targetManager);
		ingrP->ptrIng = target;
		ingrP->keyword = kw;
		ingrP->type = typeName;
		targetManager->ingressInte.push_back(ingrP);
	}
	else
	{
		impPtr<Ingress>& ingr = ingrP->ptrIng;
		ingr = target;
	}

	memPtr<Egress> egr;
	memPtr<Subfile> sf = this->findSubfile(targetManager->getFileName());
	if (sf.isEmpty())
	{
		sf = new Subfile(this);
		sf->fileName = targetManager->getFileName();
		sf->ptrManager = targetManager;
		this->subFiles.push_back(sf);
	}
	else
	{
		egr = this->findEgress(sf, kw, typeName);
	}
	if (egr.isEmpty())
	{
		egr = new Egress(this, *sf);
		egr->keyword = kw;
		egr->type = typeName;
		sf->egresses.push_back(egr);
	}
	egr->ptrDirect = target;

	return egr;
}



//read/write/reflection/serialize functions

template<class _any>
inline void base::GWPP_Any(const char* key, _any& var, para& para) {
	static constexpr size_t _subSize = base::getArrayValueTypeSize<_any>();
	static constexpr size_t _subSizeMin = _subSize > 8 ? 8 : _subSize;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
		if (lowlevel::findKeyFrom_section(key, para.section.likelyPointer, para.section.startPointer) == true)
		{
			uint8_t* holder = para.section.likelyPointer;
			uint8_t* data = holder + 1;
			para.section.likelyPointer++;
			lowlevel::turnToNextKey(para.section.likelyPointer);
			lowlevel::SpecialCharsTo_mem<_subSizeMin>(holder, data);
			if (lowlevel::BytesTo_mem(var, data, this->mngr->binSeri.start, this->mngr->binSeri.end) == false)
				this->mngr->statusBadValue++;
		}
		else
		{
			this->mngr->statusBadValue++;
		}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
		lowlevel::appendKeyTo_section(key, para.sectionVector);
		uint32_t offset = para.sectionVector->size();
		para.sectionVector->emplace_back(0);	//holder
		lowlevel::mem_toBytes(var, para.sectionVector, this->mngr->binSeri.bytes);
		uint8_t* holder = &para.sectionVector->at(offset);
		uint8_t* data = holder + 1;
		lowlevel::mem_toSpecialChars<_subSizeMin>(holder, data);
		para.sectionVector->emplace_back(0);
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		std::string base64;
		base64_mem::Encode((const char*)&var, sizeof(_any), &base64);
		rapidjson::Value vlValue;
		vlValue.SetString(base64.c_str(), *this->mngr->rjson.allocator);
		rapidjson::GenericStringRef<char> jsonKey(key);
		para.rapidJson_section->AddMember(jsonKey, vlValue, *this->mngr->rjson.allocator);
	}
	break;
	case para::rjson_deseriazlize:
		if (para.rapidJson_section->HasMember(key) &&
			para.rapidJson_section->operator[](key).IsString())
		{
			auto& content = para.rapidJson_section->operator[](key);
			size_t lenValue = content.GetStringLength();
			const char* src = content.GetString();
			if (base64_mem::Decode(src, lenValue, (char*)&var, sizeof(_any)) == false)
				this->mngr->statusBadValue++;
		}
		else
		{
			this->mngr->statusBadValue++;
		}
		break;
#endif
	}
};
template<typename _T>
inline void GWPP_sub(base* mem, const char* key1, const char* key2, _T& var, para& para)
{
	char buffer[maxKey];
	std::strcpy(buffer, key1);
	std::strcat(buffer, key2);
#if MEM_RJSON_ON
	if (para.order == para::rjson_seriazlize)
	{
		char* s = static_cast<char*>(mem->mngr->rjson.allocator->Malloc(strlen(buffer) + 1));
		strcpy(s, buffer);
		mem->GWPP(s, var, para);
		return;
	}
#endif
	mem->GWPP(buffer, var, para);
}
template<class _T>
inline std::enable_if_t<eb::base::isGWPPValid<_T>, void>
base::GWPP(const char* key, _T& var, para& para) {
	static constexpr size_t _subSize = base::getArrayValueTypeSize<_T>();
	static constexpr size_t _subSizeMin = _subSize > 8 ? 8 : _subSize;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
		if (lowlevel::findKeyFrom_section(key, para.section.likelyPointer, para.section.startPointer) == true)
		{
			uint8_t* holder = para.section.likelyPointer;
			uint8_t* data = holder + 1;
			para.section.likelyPointer++;
			lowlevel::turnToNextKey(para.section.likelyPointer);
			lowlevel::SpecialCharsTo_mem<_subSizeMin>(holder, data);
			GWPP_Base(data, var, para);
		}
		else
		{
			this->mngr->statusBadValue++;
		}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		lowlevel::appendKeyTo_section(key, para.sectionVector);
		uint32_t offset = para.sectionVector->size();
		para.sectionVector->emplace_back(0);	//holder
		GWPP_Base(para.sectionVector, var, para);
		uint8_t* holder = &para.sectionVector->at(offset);
		uint8_t* data = holder + 1;
		lowlevel::mem_toSpecialChars<_subSizeMin>(holder, data);
		para.sectionVector->emplace_back(0);
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value vlValue;
		GWPP_Base(&vlValue, var, para);
		rapidjson::GenericStringRef<char> jsonKey(key);
		para.rapidJson_section->AddMember(jsonKey, vlValue, *this->mngr->rjson.allocator);
	}
		break;
	case para::rjson_deseriazlize:
		if (para.rapidJson_section->HasMember(key))
		{
			GWPP_Base(&para.rapidJson_section->operator[](key), var, para);
		}
		else
		{
			this->mngr->statusBadValue++;
		}
        break;
#if MEM_MYSQL_ON
    case para::mysql_metadata:
        if constexpr (eb::base::isMySQLValid<_T>)
            para.mysql_metadata_v->push_back({key, false});
        break;
    case para::mysql_bind:
    case para::mysql_bind_w:
        if constexpr (eb::base::isMySQLValid<_T>)
            GWPP_Base(nullptr, var, para);
        break;
    case para::mysql_checksize:
        if constexpr (eb::base::isMySQLValid<_T>)
            GWPP_Base(nullptr, var, para);
        break;
#endif
#endif
    default:
        assert(!"unhandled para.order");
        break;
    }
}
template<typename _memSub>
inline std::enable_if_t<eb::has_save_fetch_sub<_memSub>::value, void>
base::GWPP(const char* key, _memSub& varST, para& para)
{
	varST.save_fetch_sub(this, key, para);
}
#if MEM_MYSQL_ON
inline void base::SQL_bind(std::vector<base::mysql_meta> &metadata, MYSQL_BIND *read_bind_out, MYSQL_BIND *write_bind_out, size_t *psize)
{
    eb::para para;
    para.order = para::mysql_bind;
    para.mysql.bind = read_bind_out;
    this->save_fetch(para);
    for (int k = 0; k < metadata.size(); k++)
    {
        if (read_bind_out[k].length != nullptr)
        {
            psize[k] = (size_t)read_bind_out[k].length;
            read_bind_out[k].length = &psize[k];
        }
    }
    int i = 0;
    int ii = 0;
    for (auto &meta : metadata)
    {
        if (meta.readonly == false)
        {
            write_bind_out[ii] = read_bind_out[i];
            ii++;
        }
        i++;
    }
}
inline void base::SQL_bind(std::vector<base::mysql_meta> &metadata, MYSQL_BIND *read_bind_out, size_t *psize)
{
    eb::para para;
    para.order = para::mysql_bind;
    para.mysql.bind = read_bind_out;
    this->save_fetch(para);
    for (int k = 0; k < metadata.size(); k++)
    {
        if (read_bind_out[k].length != nullptr)
        {
            psize[k] = (size_t)read_bind_out[k].length;
            read_bind_out[k].length = &psize[k];
        }
    }
}
inline void base::SQL_bind(MYSQL_BIND *read_bind_out)
{
    eb::para para;
    para.order = para::mysql_bind_w;
    para.mysql.bind = read_bind_out;
    this->save_fetch(para);
}
inline size_t base::SQL_checkstr(MYSQL_BIND *bind_in)
{
    if (bind_in == nullptr)
    {
        throw std::invalid_argument("bind_in parameter cannot be null");
    }
    size_t ret = 0;
    eb::para para;
    para.order = para::mysql_checksize;
    para.mysql.bind = bind_in;
    para.mysql.resized = &ret;
    this->save_fetch(para);
    return ret;
}
template <class _T>
inline std::enable_if_t<eb::base::isMySQLValid<_T> || eb::has_save_fetch_sub<_T>::value, void>
base::GWPP_SQL_READ(const char *key, _T &var, para &para)
{
    if constexpr (eb::base::isMySQLValid<_T> == false)
    {
        size_t size = para.mysql_metadata_v->size();
        var.save_fetch_sub(this, key, para);
        while (size != para.mysql_metadata_v->size())
        {
            para.mysql_metadata_v->operator[](size).readonly = true;
            size++;
        }
        return;
    }
    switch (para.order)
    {
    case para::mysql_metadata:
        para.mysql_metadata_v->push_back({key, true});
        break;
    default:
        GWPP(key, var, para);
        break;
    }
}
inline void base::GWPP_SQL_TIME_READ(const char *key, MYSQL_TIME &var, para &para)
{
    switch (para.order)
    {
    case para::mysql_metadata:
        para.mysql_metadata_v->push_back({key, true});
        break;
    default:
        GWPP_SQL_TIME(key, var, para);
        break;
    }
}
inline void base::GWPP_SQL_TIME(const char* key, MYSQL_TIME& var, para& para)
{
	switch (para.order)
    {
    case para::mysql_bind:
    case para::mysql_bind_w:
    {
        para.mysql.bind->buffer = &var;
        para.mysql.bind->buffer_type = MYSQL_TYPE_DATETIME;
        para.mysql.bind->buffer_length = sizeof(var);
        para.mysql.bind++;
    }
        break;
    case para::mysql_metadata:
        para.mysql_metadata_v->push_back({key, false});
        break;
    case para::mysql_checksize:
        para.mysql.bind++;
        break;
    default:
        return;
    }
}
template <typename T>
inline std::enable_if_t<std::is_base_of<base, T>::value, std::vector<base::mysql_meta>>
base::get_SQL_metadata()
{
    T temp = T(nullptr);
    eb::para para;
    para.order = para::mysql_metadata;
    std::vector<mysql_meta> metadata;
    para.mysql_metadata_v = &metadata;
    temp.save_fetch(para);
    return metadata;
}
inline std::chrono::system_clock::time_point base::SQL_TIME_to_tp(MYSQL_TIME time)
{
    std::tm tm = {};
    tm.tm_year = time.year - 1900;
    tm.tm_mon = time.month - 1;
    tm.tm_mday = time.day;
    tm.tm_hour = time.hour;
    tm.tm_min = time.minute;
    tm.tm_sec = time.second;

    std::time_t timeSinceEpoch = lowlevel::mktime(&tm);

    auto timePoint = std::chrono::system_clock::from_time_t(timeSinceEpoch);
    timePoint += std::chrono::microseconds(time.second_part);

    return timePoint;
}
inline MYSQL_TIME base::tp_to_SQL_TIME(std::chrono::system_clock::time_point tp)
{
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count() % 1'000'000;
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
    lowlevel::gmtime(&tt, tm);
    std::tm tm_time = {};

    MYSQL_TIME ret{};
    ret.year = tm.tm_year + 1900;
    ret.month = tm.tm_mon + 1;
    ret.day = tm.tm_mday;
    ret.hour = tm.tm_hour;
    ret.minute = tm.tm_min;
    ret.second = tm.tm_sec;
    ret.second_part = static_cast<unsigned long>(micros);

    return ret;
}
#endif
template<class _subType>
inline constexpr size_t base::getArrayValueTypeSize() {
	//assert: the size must be less than 8 bytes
	if constexpr (std::is_arithmetic<_subType>::value || std::is_enum<_subType>::value)
	{
		return sizeof(_subType) > 8 ? sizeof(uint32_t) : sizeof(_subType); //min
	}
	else if constexpr (std::is_array<_subType>::value || eb::is_stl_container<_subType>::value || eb::is_string<_subType>::value)
	{
		return sizeof(uint32_t);
	}
	else if constexpr (eb::is_variant<_subType>::value)
	{
		return sizeof(variantOfFile);
	}
	else if constexpr (eb::is_pair<_subType>::value)
	{
		return sizeof(pairOfFile);
	}
	else if constexpr (eb::is_optional<_subType>::value)
	{
		return sizeof(optionalOfFile);
	}
	else if constexpr (eb::has_save_fetch_struct<_subType>::value)
	{
		return _subType::save_fetch_size > 8 ? sizeof(uint32_t) : _subType::save_fetch_size; //min
	}
	else if constexpr (eb::is_impPtr<_subType>::value || eb::is_memPtr<_subType>::value)
	{
		return sizeof(void*);
	}
	else if constexpr (eb::is_atomic<_subType>::value)
	{
		using underType = typename eb::atomic_under_type<_subType>::type;
		return getArrayValueTypeSize<underType>();
	}
	else if constexpr (eb::is_chrono<_subType>::value)
	{
		using underType = typename eb::chrono_under_type<_subType>::type;
		return getArrayValueTypeSize<underType>();
	}
	else
	{
		static_assert(!std::is_same_v<_subType, _subType>, "not a vaild size for array!");
		return 0;
	}
}
template<class _arit>
inline std::enable_if_t<std::is_arithmetic<_arit>::value, void>
base::GWPP_Base(void* pValue, _arit& var, para& para) {
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		if (lowlevel::BytesTo_mem(var, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == false)
		{
			this->mngr->statusBadValue++;
		}
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		lowlevel::mem_toBytes(var, dataVector, this->mngr->binSeri.bytes);
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if constexpr (std::is_same_v<_arit, bool>) {
			vlValue->SetBool(var);
		}
		else if constexpr (std::is_same_v<_arit, float>) {
			vlValue->SetFloat(var);
		}
		else if constexpr (std::is_same_v<_arit, double>) {
			vlValue->SetDouble(var);
		}
		else {				//ints
			if constexpr (std::is_signed_v<_arit>) {
				if constexpr (sizeof(_arit) > 4)
				{
					vlValue->SetInt64(var);
				}
				else
				{
					vlValue->SetInt(var);
				}
			}
			else
			{
				if constexpr (sizeof(_arit) > 4)
				{
					vlValue->SetUint64(var);
				}
				else
				{
					vlValue->SetUint(var);
				}
			}
		}
	}
		break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if constexpr (std::is_same_v<_arit, bool>) {
			if (vlValue->IsBool()) {
				var = vlValue->GetBool();
				break;
			}
		}
		else if constexpr (std::is_same_v<_arit, float>) {
			if (vlValue->IsFloat()) {
				var = vlValue->GetFloat();
				break;
			}
		}
		else if constexpr (std::is_same_v<_arit, double>) {
			if (vlValue->IsDouble()) {
				var = vlValue->GetDouble();
				break;
			}
		}
		else { // ints
			if constexpr (std::is_signed_v<_arit>) {
				if (vlValue->IsInt64()) {
					var = static_cast<_arit>(vlValue->GetInt64());
					break;
				}
				else if (vlValue->IsInt()) {
					var = static_cast<_arit>(vlValue->GetInt());
					break;
				}
			}
			else {
				if (vlValue->IsUint64()) {
					var = static_cast<_arit>(vlValue->GetUint64());
					break;
				}
				else if (vlValue->IsUint()) {
					var = static_cast<_arit>(vlValue->GetUint());
					break;
				}
			}
		}
		this->mngr->statusBadValue++;
	}
	break;
#endif
#if MEM_MYSQL_ON
    case para::mysql_checksize:
        para.mysql.bind++;
        break;
    case para::mysql_bind:
    case para::mysql_bind_w:
        para.mysql.bind->buffer = &var;
        if constexpr (std::is_same_v<_arit, bool>)
        {
            para.mysql.bind->buffer_type = MYSQL_TYPE_TINY;
        }
        else if constexpr (std::is_same_v<_arit, float>)
        {
            para.mysql.bind->buffer_type = MYSQL_TYPE_FLOAT;
        }
        else if constexpr (std::is_same_v<_arit, double>)
        {
            para.mysql.bind->buffer_type = MYSQL_TYPE_DOUBLE;
        }
        else
        { // ints
            constexpr size_t size = sizeof(var);
            if constexpr(size == 1)
            {
                para.mysql.bind->buffer_type = MYSQL_TYPE_TINY;
            }
            else if constexpr (size == 2)
            {
                para.mysql.bind->buffer_type = MYSQL_TYPE_SHORT;
            }
            else if constexpr (size == 4)
            {
                para.mysql.bind->buffer_type = MYSQL_TYPE_LONG;
            }
            else if constexpr (size == 8)
            {
                para.mysql.bind->buffer_type = MYSQL_TYPE_LONGLONG;
            }
            if constexpr (std::is_signed_v<_arit>)
            {
                para.mysql.bind->is_unsigned = false;
            }
            else
            {
                para.mysql.bind->is_unsigned = true;
            }
        }
        para.mysql.bind->buffer_length = sizeof(var);
        para.mysql.bind++;
        break;
#endif
	}
}
template<class _array>
inline std::enable_if_t <std::is_array<_array>::value && eb::base::isGWPPValid<typename std::remove_extent<_array>::type>, void>
base::GWPP_Base(void* pValue, _array& var, para& para) {
	using _subType = typename std::remove_extent<_array>::type;
	static constexpr size_t _subSize = base::getArrayValueTypeSize<_subType>();
	static constexpr size_t _arrLength = std::extent<_array>::value;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		uint8_t* vecData;
		uint32_t vecSize;
		if (lowlevel::BytesTo_mem(vecData, vecSize, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
		{
			size_t len = std::min((size_t)vecSize / _subSize, _arrLength);
			for (int iter = 0; iter < len; iter++)
			{
				this->GWPP_Base(vecData + iter * _subSize, var[iter], para);
			}
		}
		else
		{
			this->mngr->statusBadValue++;
		}
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t> vecData;
		for (int iter = 0; iter < _arrLength; iter++)
		{
			this->GWPP_Base(&vecData, var[iter], para);
		}
		assert(vecData.size() == _subSize * _arrLength || !"somehow cause the array error, length mismatch.");
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		uint32_t offset;
		lowlevel::mem_toBytes(offset, vecData.size(), dataVector, this->mngr->binSeri.bytes);
		memcpy(&this->mngr->binSeri.bytes->at(offset), vecData.data(), vecData.size());
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		*vlValue = rapidjson::kArrayType;
		for (int iter = 0; iter < _arrLength; iter++)
		{
			rapidjson::Value vlArr;
			this->GWPP_Base(&vlArr, var[iter], para);
			vlValue->PushBack(vlArr, *this->mngr->rjson.allocator);
		}
	}
		break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsArray() == true)
		{
			size_t len = std::min((size_t)vlValue->Size(), _arrLength);
			for (int iter = 0; iter < len; iter++)
			{
				this->GWPP_Base(&vlValue->operator[](iter), var[iter], para);
			}
		}
		else
		{
			this->mngr->statusBadValue++;
		}
	}
		break;
#endif
#if MEM_MYSQL_ON
    case para::mysql_bind:
    case para::mysql_bind_w:
        if constexpr (std::is_same_v<_subType, char>)
        {
            para.mysql.bind->buffer = var;
            para.mysql.bind->buffer_type = MYSQL_TYPE_STRING;
            para.mysql.bind->buffer_length = _arrLength;
            para.mysql.bind++;
        }
        break;
    case para::mysql_checksize:
        if constexpr (std::is_same_v<_subType, char>)
            para.mysql.bind++;
        break;
#endif
	}
}
template<class _stlCont>
inline std::enable_if_t<eb::is_stl_container<_stlCont>::value, void>
base::GWPP_Base(void* pValue, _stlCont& var, para& para) {
	using _subType = typename _stlCont::value_type;
	static constexpr size_t _subSize = base::getArrayValueTypeSize<_subType>();
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		var = _stlCont();
		uint8_t* dataPointer = (uint8_t*)pValue;
		uint8_t* vecData;
		uint32_t vecSize;
		if (lowlevel::BytesTo_mem(vecData, vecSize, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
		{
			if constexpr (eb::is_array<_stlCont>::value)
			{
				size_t len = std::min((size_t)vecSize / _subSize, var.size());
				for (int iter = 0; iter < len; iter++)
				{
					_subType& subVar = var[iter];
					this->GWPP_Base(vecData + iter * _subSize, subVar, para);
				}
			}
			else
			{
				size_t&& len = vecSize / _subSize;
				if constexpr (eb::is_forward_list<_stlCont>::value)
				{
					for (int iter = len - 1; iter >= 0; iter--)
					{
						_subType& subVar = *var.insert_after(var.before_begin(), _subType{});
						this->GWPP_Base(vecData + iter * _subSize, subVar, para);
					}
				}
				else if constexpr (eb::has_emplace_back<_stlCont>::value)
				{
					for (int iter = 0; iter < len; iter++)
					{
						var.emplace_back();
						_subType& subVar = *std::prev(var.end());
						this->GWPP_Base(vecData + iter * _subSize, subVar, para);
					}
				}
				else if constexpr (eb::has_emplace<_stlCont>::value)
				{
					for (int iter = 0; iter < len; iter++)
					{
						if constexpr (eb::is_pair<_subType>::value)
						{
							std::pair<typename std::decay_t<typename _subType::first_type>, typename _subType::second_type> subPair;
							this->GWPP_Base(vecData + iter * _subSize, subPair, para);
							var.emplace(subPair);
						}
						else
						{
							_subType subVar;
							this->GWPP_Base(vecData + iter * _subSize, subVar, para);
							var.emplace(subVar);
						}
					}
				}
				else
				{
					static_assert(!std::is_same_v<_stlCont, _stlCont>, "TYPE ERROR: Unsupported container type.");
				}
			}
		}
		else
		{
			this->mngr->statusBadValue++;
		}
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t> vecData;
		for (const _subType& element : var)
		{
			if constexpr (eb::is_pair<_subType>::value)
				this->GWPP_Base(&vecData, (std::pair<typename std::decay_t<typename _subType::first_type>, typename _subType::second_type>&)element, para);
			else
				this->GWPP_Base(&vecData, (_subType&)element, para);
		}
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		uint32_t offset;
		lowlevel::mem_toBytes(offset, vecData.size(), dataVector, this->mngr->binSeri.bytes);
		memcpy(&this->mngr->binSeri.bytes->at(offset), vecData.data(), vecData.size());
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		*vlValue = rapidjson::kArrayType;
		for (const _subType& element : var)
		{
			rapidjson::Value vlArr;
			if constexpr (eb::is_pair<_subType>::value)
				this->GWPP_Base(&vlArr, (std::pair<typename std::decay_t<typename _subType::first_type>, typename _subType::second_type>&)element, para);
			else
				this->GWPP_Base(&vlArr, (_subType&)element, para);
			vlValue->PushBack(vlArr, *this->mngr->rjson.allocator);
		}
	}
	break;
	case para::rjson_deseriazlize:
	{
		var = _stlCont();
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsArray() == true)
		{
			if constexpr (eb::is_array<_stlCont>::value)
			{
				size_t len = std::min((size_t)vlValue->Size(), var.size());
				for (int iter = 0; iter < len; iter++)
				{
					_subType& subVar = var[iter];
					this->GWPP_Base(&vlValue->operator[](iter), subVar, para);
				}
			}
			else
			{
				size_t&& len = (size_t)vlValue->Size();
				if constexpr (eb::is_forward_list<_stlCont>::value)
				{
					for (int iter = len - 1; iter >= 0; iter--)
					{
						_subType& subVar = *var.insert_after(var.before_begin(), _subType{});
						this->GWPP_Base(&vlValue->operator[](iter), subVar, para);
					}
				}
				else if constexpr (eb::has_emplace_back<_stlCont>::value)
				{
					for (int iter = 0; iter < len; iter++)
					{
						var.emplace_back();
						_subType& subVar = *std::prev(var.end());
						this->GWPP_Base(&vlValue->operator[](iter), subVar, para);
					}
				}
				else if constexpr (eb::has_emplace<_stlCont>::value)
				{
					for (int iter = 0; iter < len; iter++)
					{
						if constexpr (eb::is_pair<_subType>::value)
						{
							std::pair<typename std::decay_t<typename _subType::first_type>, typename _subType::second_type> subPair;
							this->GWPP_Base(&vlValue->operator[](iter), subPair, para);
							var.emplace(subPair);
						}
						else
						{
							_subType subVar;
							this->GWPP_Base(&vlValue->operator[](iter), subVar, para);
							var.emplace(subVar);
						}
					}
				}
				else
				{
					static_assert(!std::is_same_v<_stlCont, _stlCont>, "TYPE ERROR: Unsupported container type.");
				}
			}
		}
		else
		{
			this->mngr->statusBadValue++;
		}
	}
	break;
#endif
	}
}
template<class _atom>
inline std::enable_if_t<eb::is_atomic<_atom>::value, void>
base::GWPP_Base(void* pValue, _atom& var, para& para) {
	using underType = typename eb::atomic_under_type<_atom>::type;
	static_assert(eb::base::isGWPPValid<underType>, "TYPE ERROR: std::atomic<> template typename unsupported.");
	static_assert(!std::is_void<underType>::value, "TYPE ERROR: std::atomic<> template unexpected void.");
	switch (para.order)
	{
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
#endif
	{
		underType buffer = var.load();
		GWPP_Base(pValue, buffer, para);
	}
		break;
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
#if MEM_RJSON_ON
	case para::rjson_deseriazlize:
#endif
	{
		underType buffer;
		GWPP_Base(pValue, buffer, para);
		var.store(buffer);
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
	}
}
template<class _chrono>
inline std::enable_if_t<eb::is_chrono<_chrono>::value, void>
base::GWPP_Base(void* pValue, _chrono& var, para& para)
{
	using json_const::json_time_mode;
	using json_const::json_time_mode_t;
	using json_const::json_year;
	using json_const::json_month;
	using json_const::json_day;
	using json_const::json_date;
	using json_const::json_hour;
	using json_const::json_minute;
	using json_const::json_secondT;
	using underType = typename eb::chrono_under_type<_chrono>::type;
	using isSpecial = typename eb::is_system_clock_time_point<_chrono>;	//system clock timepoint has a special json expression than common chrono
	using isDuration = typename eb::is_chrono_duration<_chrono>;
	using isTimepoint = typename eb::is_chrono_timepoint<_chrono>;
	static_assert(!std::is_void<underType>::value, "TYPE ERROR: std::chrono<> template unexpected void.");
	switch (para.order)
	{
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
#endif
	{
#if MEM_RJSON_ON
		if constexpr (isSpecial::value)
		{
			if (para.order == para::rjson_seriazlize)
			{
				rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
				switch (json_time_mode)
				{
				case json_time_mode_t::object_second:
				{
					vlValue->SetObject();
					std::time_t tt = std::chrono::system_clock::to_time_t(var);
					//std::mktime std::gmtime are not thread safe. do not use.

					std::tm tm;
					lowlevel::gmtime(&tt, tm);
					vlValue->AddMember(json_year, rapidjson::Value().SetInt(tm.tm_year + 1900), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_month, rapidjson::Value().SetInt(tm.tm_mon + 1), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_day, rapidjson::Value().SetInt(tm.tm_mday), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_date, rapidjson::Value().SetInt(tm.tm_wday), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_hour, rapidjson::Value().SetInt(tm.tm_hour), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_minute, rapidjson::Value().SetInt(tm.tm_min), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_secondT, rapidjson::Value().SetInt(tm.tm_sec), *this->mngr->rjson.allocator);
				}
					break;
				case json_time_mode_t::object_day:
				{
					vlValue->SetObject();
					std::time_t tt = std::chrono::system_clock::to_time_t(var);
					std::tm tm;
					lowlevel::gmtime(&tt, tm);
					vlValue->AddMember(json_year, rapidjson::Value().SetInt(tm.tm_year + 1900), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_month, rapidjson::Value().SetInt(tm.tm_mon + 1), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_day, rapidjson::Value().SetInt(tm.tm_mday), *this->mngr->rjson.allocator);
					vlValue->AddMember(json_date, rapidjson::Value().SetInt(tm.tm_wday), *this->mngr->rjson.allocator);
				}
					break;
				case json_time_mode_t::unix_s:
				{
					uint64_t s = std::chrono::duration_cast<std::chrono::seconds>(var.time_since_epoch()).count();
					vlValue->SetUint64(s);
				}
					break;
				case json_time_mode_t::unix_ms:
				{
					uint64_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(var.time_since_epoch()).count();
					vlValue->SetUint64(ms);
				}
					break;
				case json_time_mode_t::built_in_value:
				{
					underType count = var.time_since_epoch().count();
					vlValue->SetUint64(count);
				}
					break;
				case json_time_mode_t::string_Y_M_D:
				{
					std::time_t tt = std::chrono::system_clock::to_time_t(var);
					std::tm tm;
					lowlevel::gmtime(&tt, tm);
					std::ostringstream oss;
					oss << std::put_time(&tm, "%Y-%m-%d");
					vlValue->SetString(oss.str().c_str(), *this->mngr->rjson.allocator);
				}
					break;
				}
				break;
			}
		}
#endif
		underType buffer;
		if constexpr (isDuration::value)
		{
			buffer = var.count();
		}
		else if constexpr (isTimepoint::value)
		{
			buffer = var.time_since_epoch().count();
		}
		GWPP_Base(pValue, buffer, para);
	}
	break;
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
#if MEM_RJSON_ON
	case para::rjson_deseriazlize:
#endif
	{
#if MEM_RJSON_ON
		if constexpr (isSpecial::value)
		{
			if (para.order == para::rjson_deseriazlize)
			{
				rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
				switch (json_time_mode)
				{
				case json_time_mode_t::object_second:
				{
					if (vlValue->IsObject())
					{

						std::tm tm = {};
						tm.tm_year = (vlValue->HasMember(json_year) && vlValue->FindMember(json_year)->value.IsInt())
							? vlValue->FindMember(json_year)->value.GetInt() - 1900
							: 0;
						tm.tm_mon = (vlValue->HasMember(json_month) && vlValue->FindMember(json_month)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_month)->value.GetInt() - 1, 0, 11)
							: 0;
						tm.tm_mday = (vlValue->HasMember(json_day) && vlValue->FindMember(json_day)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_day)->value.GetInt(), 1, 31)
							: 1;
						tm.tm_hour = (vlValue->HasMember(json_hour) && vlValue->FindMember(json_hour)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_hour)->value.GetInt(), 0, 23)
							: 0;
						tm.tm_min = (vlValue->HasMember(json_minute) && vlValue->FindMember(json_minute)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_minute)->value.GetInt(), 0, 59)
							: 0;
						tm.tm_sec = (vlValue->HasMember(json_secondT) && vlValue->FindMember(json_secondT)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_secondT)->value.GetInt(), 0, 59)
							: 0;

						//std::mktime std::gmtime are not thread safe. never use.
						time_t time = lowlevel::mktime(&tm);
						if (time == (time_t)-1)
							this->mngr->statusBadValue++;
						else
							var = std::chrono::system_clock::from_time_t(time);
					}
				}
				break;
				case json_time_mode_t::object_day:
				{
					if (vlValue->IsObject())
					{
						std::tm tm = {};
						tm.tm_year = (vlValue->HasMember(json_year) && vlValue->FindMember(json_year)->value.IsInt())
							? vlValue->FindMember(json_year)->value.GetInt() - 1900
							: 0;
						tm.tm_mon = (vlValue->HasMember(json_month) && vlValue->FindMember(json_month)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_month)->value.GetInt() - 1, 0, 11)
							: 0;
						tm.tm_mday = (vlValue->HasMember(json_day) && vlValue->FindMember(json_day)->value.IsInt())
							? std::clamp(vlValue->FindMember(json_day)->value.GetInt(), 1, 31)
							: 1;

						time_t time = lowlevel::mktime(&tm);
						if (time == (time_t)-1)
							this->mngr->statusBadValue++;
						else
							var = std::chrono::system_clock::from_time_t(time);
					}
				}
				break;
				case json_time_mode_t::unix_s:
				{
					if (vlValue->IsUint64())
					{
						var = std::chrono::system_clock::time_point(std::chrono::seconds(vlValue->GetUint64()));
					}
				}
				break;
				case json_time_mode_t::unix_ms:
				{
					if (vlValue->IsUint64())
					{
						var = std::chrono::system_clock::time_point(std::chrono::milliseconds(vlValue->GetUint64()));
					}
				}
				break;
				case json_time_mode_t::built_in_value:
				{
					if (vlValue->IsUint64())
					{
						var = std::chrono::time_point<typename isTimepoint::clock>(
							typename std::chrono::time_point<typename isTimepoint::clock>::duration(vlValue->GetUint64())
						);
					}
				}
				break;
				case json_time_mode_t::string_Y_M_D:
				{
					if (vlValue->IsString())
					{
						std::istringstream iss(vlValue->GetString());
						std::tm tm = {};
						iss >> std::get_time(&tm, "%Y-%m-%d");
						if (!iss.fail())
							var = std::chrono::system_clock::from_time_t(lowlevel::mktime(&tm));
						else
							this->mngr->statusBadValue++;
					}
				}
				break;
			}
			break;
			}
		}
#endif
		underType buffer;
		GWPP_Base(pValue, buffer, para);
		if constexpr (isDuration::value)
		{
			var = std::chrono::duration<underType>(buffer);
		}
		else if constexpr (isTimepoint::value)
		{
			var = std::chrono::time_point<typename isTimepoint::clock>(
				typename std::chrono::time_point<typename isTimepoint::clock>::duration(buffer)
			);
		}
	}
	break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
	}
}
template<class _string>
inline std::enable_if_t<eb::is_string<_string>::value, void>
base::GWPP_Base(void* pValue, _string& var, para& para) {
	using _subType = typename eb::is_string<_string>::base_type;
	static constexpr size_t _subSize = eb::is_string<_string>::byteSize;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		uint8_t* vecData;
		uint32_t vecSize;
		if (lowlevel::BytesTo_mem(vecData, vecSize, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
		{
			var = _string((const _subType*)vecData, vecSize / _subSize);
		}
		else
		{
			var = _string();
			this->mngr->statusBadValue++;
		}
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		uint32_t offset;
		lowlevel::mem_toBytes(offset, var.size() * _subSize, dataVector, this->mngr->binSeri.bytes);
		memcpy(&this->mngr->binSeri.bytes->at(offset), var.c_str(), var.size() * _subSize);
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		std::string csValue;
		if constexpr (_subSize == 1)
		{
			csValue = var;
		}
		else if constexpr (_subSize == 2)
		{
			utf8::utf16to8(var.begin(), var.end(), std::back_inserter(csValue));
		}
		else if constexpr (_subSize == 4)
		{
			utf8::utf32to8(var.begin(), var.end(), std::back_inserter(csValue));
		}
		else
		{
			static_assert(!std::is_same_v< _string, _string>, "Unknow string type.");
		}
		vlValue->SetString(csValue.c_str(), *this->mngr->rjson.allocator);
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsString()) {
			std::string csValue = vlValue->GetString();
			if constexpr (_subSize == 1)
			{
				var = csValue;
			}
			else if constexpr (_subSize == 2)
			{
				utf8::utf8to16(csValue.begin(), csValue.end(), std::back_inserter(var));
			}
			else if constexpr (_subSize == 4)
			{
				utf8::utf8to32(csValue.begin(), csValue.end(), std::back_inserter(var));
			}
			else
			{
				static_assert(!std::is_same_v< _string, _string>, "Unknow string type.");
			}
		}
		else
		{
			this->mngr->statusBadValue++;
		}
	}
	break;
#endif
#if MEM_MYSQL_ON
    case para::mysql_bind:
    case para::mysql_bind_w:
        if (var.size() == 0)
            var.resize(1);
        para.mysql.bind->buffer = (void *)var.data();
        para.mysql.bind->buffer_type = MYSQL_TYPE_MEDIUM_BLOB;
        para.mysql.bind->buffer_length = (var.size() * sizeof(_subType));
        if (para.order == para::mysql_bind)
            para.mysql.bind->length = (size_t *)(var.size() * sizeof(_subType));
        para.mysql.bind++;
        break;
    case para::mysql_checksize:
        if ((*para.mysql.bind->length) / sizeof(_subType) > var.size())
        {
            para.mysql.bind->buffer_length = *para.mysql.bind->length;
            var.resize((*para.mysql.bind->length) / sizeof(_subType));
            para.mysql.bind->buffer = (void *)var.data();
            (*para.mysql.resized)++;
        }
        para.mysql.bind++;
        break;
#endif
    }
}
template<class _enum>
inline std::enable_if_t<std::is_enum<_enum>::value, void>
base::GWPP_Base(void* pValue, _enum& var, para& para) {
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	case para::rjson_deseriazlize:
#endif
		GWPP_Base(pValue, reinterpret_cast<typename std::underlying_type<_enum>::type&>(var), para);
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
	}
}
template<class T>
inline void base::GWPP_Base(void* pValue, std::optional<T>& var, para& para) {
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		optionalOfFile* dataPointer = (optionalOfFile*)pValue;
		if (dataPointer->offset != 0)
		{
			uint8_t* valuePos;
			uint32_t length;
			if (lowlevel::BytesTo_mem(valuePos, length, (uint8_t*)&dataPointer->offset, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
			{
				var.emplace();
				this->GWPP_Base(valuePos, var.value(), para);
			}
			else
				this->mngr->statusBadValue++;
		}
		else
		{
			var.reset();
		}
	}
	break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		if (var.has_value())
		{
			std::vector<uint8_t> vecData;
			this->GWPP_Base(&vecData, var.value(), para);
			uint32_t offset;
			lowlevel::mem_toBytes(offset, vecData.size(), dataVector, this->mngr->binSeri.bytes);
			memcpy(&this->mngr->binSeri.bytes->at(offset), vecData.data(), vecData.size());
		}
		else
		{
			lowlevel::mem_toBytes<uint32_t>(0, dataVector, nullptr);
		}
	}
	break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (var.has_value())
		{
			this->GWPP_Base(vlValue, var.value(), para);
		}
		else
		{
			vlValue->SetNull();
		}
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsNull() == false)
		{
			var.emplace();
			this->GWPP_Base(vlValue, var.value(), para);
			break;
		}
		else
		{
			var.reset();
		}
		this->mngr->statusBadValue++;
	}
	break;
#endif
	}
}
template<class...Args>
inline void base::GWPP_Base(void* pValue, std::variant<Args...>& var, para& para) {
	using json_const::json_type;
	using json_const::json_value;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		variantOfFile* dataPointer = (variantOfFile*)pValue;
		uint8_t* valuePos;
		uint32_t length;
		if (lowlevel::BytesTo_mem(valuePos, length, (uint8_t*)&dataPointer->offset, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
		{
			lowlevel::pushVariantHelper<Args...> helper;
            if (helper.push(dataPointer->type, var, this, valuePos, para) == true)
                break;
		}
		this->mngr->statusBadValue++;
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t> vecData;
		std::visit([&](auto&& arg) {
			this->GWPP_Base(&vecData, arg, para);
			}, var);
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		uint32_t offset;
		lowlevel::mem_toBytes(offset, vecData.size(), dataVector, this->mngr->binSeri.bytes);
		memcpy(&this->mngr->binSeri.bytes->at(offset), vecData.data(), vecData.size());
		lowlevel::mem_toBytes<uint32_t>(var.index(), dataVector, nullptr);
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		rapidjson::Value type, value;
		type.SetUint(var.index());
		std::visit([&](auto&& arg) {
			this->GWPP_Base(&value, arg, para);
			}, var);
		vlValue->SetObject();
		rapidjson::GenericStringRef<char> jsonKeyType(json_type);
		rapidjson::GenericStringRef<char> jsonKeyValue(json_value);
		vlValue->AddMember(jsonKeyType, type, *this->mngr->rjson.allocator);
		vlValue->AddMember(jsonKeyValue, value, *this->mngr->rjson.allocator);
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsObject())
		{
			if (vlValue->HasMember(json_type) && vlValue->HasMember(json_value))
			{
				rapidjson::Value &type = vlValue->operator[](json_type),
								&value = vlValue->operator[](json_value);

				if (type.IsUint())
				{
					uint32_t index = type.GetUint();
					lowlevel::pushVariantHelper<Args...> helper;
					if (helper.push(index, var, this, &value, para) == true)
						break;
				}
			}
		}
		this->mngr->statusBadValue++;
	}
	break;
#endif
	}
}
template<class T1, class T2>
inline void base::GWPP_Base(void* pValue, std::pair<T1, T2>& var, para& para) {
	using json_const::json_first;
	using json_const::json_second;
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		pairOfFile* dataPointer = (pairOfFile*)pValue;
		//uint32_t& index = dataPointer->sizeOfFirst;
		uint8_t* valuePos;
		uint32_t length;
		if (lowlevel::BytesTo_mem(valuePos, length, (uint8_t*)&dataPointer->offset, this->mngr->binSeri.start, this->mngr->binSeri.end) == true)
		{
			this->GWPP_Base(valuePos, var.first, para);
			this->GWPP_Base(valuePos + dataPointer->sizeOfFirst, var.second, para);
			break;
		}
		this->mngr->statusBadValue++;
	}
	break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t> vecData;
		this->GWPP_Base(&vecData, var.first, para);
		uint32_t sizeOfFirst = vecData.size();
		this->GWPP_Base(&vecData, var.second, para);
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		uint32_t offset;
		lowlevel::mem_toBytes(offset, vecData.size(), dataVector, this->mngr->binSeri.bytes);
		memcpy(&this->mngr->binSeri.bytes->at(offset), vecData.data(), vecData.size());
		lowlevel::mem_toBytes<uint32_t>(sizeOfFirst, dataVector, nullptr);
	}
	break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		rapidjson::Value first, second;
		this->GWPP_Base(&first, var.first, para);
		this->GWPP_Base(&second, var.second, para);
		rapidjson::GenericStringRef<char> jsonKeyFirst(json_first);
		rapidjson::GenericStringRef<char> jsonKeySecond(json_second);
		vlValue->SetObject();
		vlValue->AddMember(jsonKeyFirst, first, *this->mngr->rjson.allocator);
		vlValue->AddMember(jsonKeySecond, second, *this->mngr->rjson.allocator);
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsObject())
		{
			if (vlValue->HasMember(json_first) && vlValue->HasMember(json_second))
			{
				rapidjson::Value& first = vlValue->operator[](json_first),
					& second = vlValue->operator[](json_second);

				this->GWPP_Base(&first, var.first, para);
				this->GWPP_Base(&second, var.second, para);
				break;
			}
		}
		this->mngr->statusBadValue++;
	}
	break;
#endif
}
}
template<typename _memStruct>
inline std::enable_if_t<eb::has_save_fetch_struct<_memStruct>::value, void>
base::GWPP_Base(void* pValue, _memStruct& varST, para& para) {
	switch (para.order)
	{
	case para::binary_deserialize_base:
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		if constexpr (_memStruct::save_fetch_size > 8)	//long value
		{
			uint8_t* pos;
			uint32_t len;
			lowlevel::BytesTo_mem(pos, len, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end);
			if (len != _memStruct::save_fetch_size)	//size mismatch
			{
				this->mngr->statusBadValue++;
				return;
			}
			varST.save_fetch_struct(pos, para);
		}
		else											//short value
		{
			uint8_t buffer[_memStruct::save_fetch_size];
			lowlevel::BytesTo_mem(buffer, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end);
			varST.save_fetch_struct(buffer, para);
		}
	}
		break;
	case para::binary_serialize_base:
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		if constexpr (_memStruct::save_fetch_size > 8)	//long value
		{
			uint32_t offsetOut;
			lowlevel::mem_toBytes(offsetOut, _memStruct::save_fetch_size, dataVector, this->mngr->binSeri.bytes);
			varST.save_fetch_struct(&(this->mngr->binSeri.bytes->at(offsetOut)), para);
		}
		else											//short value
		{
			uint8_t buffer[_memStruct::save_fetch_size];
			varST.save_fetch_struct(buffer, para);
			lowlevel::mem_toBytes(buffer, dataVector, this->mngr->binSeri.bytes);
		}
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		std::string base64;
		uint8_t buffer[_memStruct::save_fetch_size];
		varST.save_fetch_struct(buffer, para);
		base64_mem::Encode((const char*)&buffer, _memStruct::save_fetch_size, &base64);
		vlValue->SetString(base64.c_str(), *this->mngr->rjson.allocator);
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsString())
		{
			std::string src = vlValue->GetString();
			std::string buffer;
			if (base64_mem::Decode(src, &buffer) == true)
			{
				varST.save_fetch_struct((uint8_t*)buffer.data(), para);
				break;
			}
		}
		this->mngr->statusBadValue++;
	}
	break;
#endif
	}
}
template<typename _mu, bool _r>
inline void base::GWPP_Base(void* pValue, impPtr<_mu, _r>& var, para& para) {
	switch (para.order)
	{
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	case para::rjson_deseriazlize:
#endif
	case para::binary_deserialize_base:
	case para::binary_serialize_base:			//do nothing
		break;
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		base* varMU;
		if (lowlevel::BytesTo_mem(varMU, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == false)
		{
			this->mngr->statusBadValue++;
			var = memPtr<_mu, _r>();
			return;
		}
		if (varMU == nullptr)
			return;
		auto iter = this->mngr->ptrTable.find(lowlevel::memPtrCorr(varMU, 0));
		if (this->mngr->ptrTable.cend() == iter)
		{
			this->mngr->statusBadValue++;
			var = memPtr<_mu, _r>();
			return;
		}
		if (iter->isConstructed == true)
		{
			var = (_mu*)iter->ptrRUN2;
		}
	}
		break;
	case para::binary_serialize_manager:
	{
		if (var.isEmpty())return;
		if (var->base::mngr != this->base::mngr)return;

		if (this->mngr->ptrTable.cend() != this->mngr->ptrTable.find(lowlevel::memPtrCorr(var.ptr->content, 0)))
		{
			std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
			lowlevel::mem_toBytes(var.ptr->content, dataVector, this->mngr->binSeri.bytes);
		}
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
	}
}
template<typename _mu, bool _r>
inline void base::GWPP_Base(void* pValue, memPtr<_mu, _r>& var, para& param) {
	using json_const::json_recurring;
	switch (param.order)
	{
	case para::binary_deserialize_base:
	case para::binary_serialize_base:			//do nothing
		break;
	case para::binary_deserialize_manager:
	{
		uint8_t* dataPointer = (uint8_t*)pValue;
		base* varMU;
		if (lowlevel::BytesTo_mem(varMU, dataPointer, this->mngr->binSeri.start, this->mngr->binSeri.end) == false)
		{
			this->mngr->statusBadValue++;
			var = memPtr<_mu, _r>();
			return;
		}
		if (varMU == nullptr)
		{
			var = nullptr;
			break;
		}
		auto iter = this->mngr->ptrTable.find(lowlevel::memPtrCorr(varMU, 0));
		if (this->mngr->ptrTable.cend() == iter)
		{
			this->mngr->statusBadValue++;
			var = memPtr<_mu, _r>();
			return;
		}
		if (iter->isConstructed == true)
		{
			var = (_mu*)iter->ptrRUN2;
		}
		else
		{
			para mp;
			mp.order = para::binary_deserialize_manager;
			mp.section.startPointer = mp.section.likelyPointer = this->mngr->binSeri.start + iter->sectionStartOffset;

			if constexpr (std::is_base_of_v<manager, _mu>)
			{
				assert(false || !"Serialize Assert Error: somehow causes the empty manager.");
			}
			else
			{
				var = new _mu(this->mngr);
			}
			bool& isConstructed = (bool&)iter->isConstructed;
			isConstructed = true;
			base*& ptrRUN2 = (base*&)iter->ptrRUN2;
			ptrRUN2 = var.ptr->content;

			var->save_fetch(mp);
		}
	}
		break;
	case para::binary_serialize_manager:
	{
		std::vector<uint8_t>* dataVector = (std::vector<uint8_t>*)pValue;
		if (var.isEmpty())
		{
			const void* ref = 0;
			lowlevel::mem_toBytes(ref, dataVector, this->mngr->binSeri.bytes);
			break;
		}
		if (var->base::mngr != this->base::mngr)return;

		lowlevel::mem_toBytes(var.ptr->content, dataVector, this->mngr->binSeri.bytes);

		if (this->mngr->ptrTable.cend() == this->mngr->ptrTable.find(lowlevel::memPtrCorr(var.ptr->content, 0)))
		{
			std::vector<uint8_t> sectionVector = std::vector<uint8_t>();
			para mp;
			mp.order = para::binary_serialize_manager;
			mp.sectionVector = &sectionVector;

			auto emplaceResult = this->mngr->ptrTable.emplace(var.ptr->content, 0);
			var->save_fetch(mp);
			uint32_t& sectionOffset = (uint32_t&)emplaceResult.first->sectionStartOffset;
			sectionOffset = &*(this->mngr->binSeri.bytes->end() - 1) - this->mngr->binSeri.bytes->data() + 1;

			this->mngr->binSeri.bytes->insert(this->mngr->binSeri.bytes->end(), sectionVector.begin(), sectionVector.end());
		}
	}
		break;
#if MEM_REFLECTION_ON
	case para::reflection_read:
		//para.reflection->context.emplace_back(key, var);
		break;
	case para::reflection_write:
		//para.reflection_single->WriteMU(key, var, size);
		break;
#endif
#if MEM_RJSON_ON
	case para::rjson_seriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (var.isEmpty())
		{
			vlValue->SetNull();
			break;
		}

		if (this->mngr->ptrTable.cend() == this->mngr->ptrTable.find(lowlevel::memPtrCorr(var.ptr->content, 0)))
		{
			vlValue->SetObject();
			para mp;
			mp.order = para::rjson_seriazlize;
			mp.rapidJson_section = vlValue;

			auto emplaceResult = this->mngr->ptrTable.emplace(var.ptr->content, 0);
			var->save_fetch(mp);
			this->mngr->ptrTable.erase(lowlevel::memPtrCorr(var.ptr->content, 0));
		}
		else
		{
			rapidjson::GenericStringRef<char> recur(json_recurring);
			vlValue->SetString(recur);
		}
	}
	break;
	case para::rjson_deseriazlize:
	{
		rapidjson::Value* vlValue = (rapidjson::Value*)pValue;
		if (vlValue->IsObject())
		{
			para mp;
			mp.order = para::rjson_deseriazlize;
			mp.rapidJson_section = vlValue;

			if constexpr (std::is_base_of_v<manager, _mu>)
			{
				assert(false || !"Serialize Assert Error: somehow causes the empty manager.");
			}
			else
			{
				var = new _mu(this->mngr);
			}

			var->save_fetch(mp);
		}
		else if (vlValue->IsNull())
		{
			var = nullptr;
		}
		else
		{
			this->mngr->statusBadValue++;
		}

	}
	break;
#endif
	}
}



//Egress
inline Egress::~Egress() {}
template<typename cast>
inline egressFindErr Egress::getTarget(cast*& varReturn)
{
	if (ptrDirect == nullptr)
	{
		if (ptrUpLevel->ptrManager.isEmpty())
			if (ptrUpLevel->findGlobalManager() == 0)
				return egressFindErr::file_notfound;
		impPtr<Ingress> inge = ptrUpLevel->ptrManager->findIngress(keyword.c_str(), type.c_str());
		if (inge.isEmpty())
			return egressFindErr::keyword_notfound;
		else
			this->ptrDirect = inge;
	}
	varReturn = ptrDirect.precision_cast<cast*>();
	if (varReturn)
		return egressFindErr::ok;
	else
		return egressFindErr::cast_failed;
}
inline void Egress::save_fetch(para para) {
	GWPP("keyword", keyword, para);
	GWPP("type", type, para);
}



//Ingress
inline Ingress::~Ingress() {}
inline void refIngress::save_fetch(para para) {
	GWPP("p", ptrIng, para);
	GWPP("k", keyword, para);
	GWPP("t", type, para);
}



//Subfile
inline Subfile::~Subfile() {}
inline void Subfile::save_fetch(para para) {
	GWPP("vec", egresses, para);
	if (para.isConstruct())
		for (auto i : egresses)
			if (i.isFilled())
				i->ptrUpLevel = this;
	GWPP("fileName", fileName, para);
}
inline int Subfile::findGlobalManager()
{
    std::lock_guard<std::mutex> lck (manager::global_mutex);
	for (auto i : manager::global_load)
	{
		char* fn = i->getFileName();
		if (fn == nullptr)
			continue;
		if (strcmp(fn, fileName.c_str()) == 0)
		{
			ptrManager = i;
			return 1;
		}
	}
	return 0;
}
template<typename _type>
inline bool Subfile::tryLoadSubfile(const char* directory) {
	ptrManager = new _type(directory);
	if (ptrManager->url[ptrManager->url.size() - 1] != '\\')
	{
		ptrManager->url += "\\";
	}
	ptrManager->url += this->fileName;
	if (ptrManager->upload())
	{
		return true;
	}
	ptrManager = nullptr;
	return false;
}



//pEgress
template<class cast>
inline void pEgress<cast>::getFileName(char* strOut, uint32_t length)
{
	uint32_t i = std::min((size_t)length, maxURL);
	memcpy(strOut, memPtr<Egress>::operator*()->ptrUpLevel->fileName.c_str(), i);
	return;
}
template<class cast>
inline egressFindErr pEgress<cast>::getTarget(cast*& varReturn) {
	if (memPtr<Egress>::isFilled())
	{
		Egress& egr = *(Egress*)(memPtr<Egress>::ptr->content);
		return egr.getTarget(varReturn);
	}
	varReturn = nullptr;
	return egressFindErr::empty_egress;
}
template<class cast>
inline void pEgress<cast>::makeEIPair(manager* egressMngr, const memPtr<cast>& target, const char* kw) {
	memPtr<Egress> pegr = egressMngr->makeEgress_IngressPair(target, kw);
	this->memPtr<Egress>::operator=(pegr);
}
template<class cast>
inline pEgress<cast>::pEgress(manager* egressMngr, const memPtr<cast>& target, const char* kw) {
	this->makeEIPair(egressMngr, target, kw);
}



//serialize functions

inline bool base::deserialize(uint8_t* Ptr, uint32_t StringSize)
{
	para mp;
	mp.order = para::binary_deserialize_base;
	mp.section.startPointer = mp.section.likelyPointer = Ptr;

	//find long variable zone
	uint8_t* end = Ptr + StringSize;
	for (int zeroCount = 0;Ptr < end;Ptr++)
	{
		if (zeroCount == 3)
		{
			if (*Ptr)
				break;
		}
		else if (*Ptr == 0)
		{
			zeroCount++;
		}
		else
		{
			zeroCount = 0;
		}
	}

	this->mngr->binSeri.start = Ptr - 1;
	this->mngr->binSeri.end = end;
	this->mngr->statusBadValue = 0;
	this->save_fetch(mp);
	return true;
}
inline void base::serialize(std::vector<uint8_t>* bc)
{
	bc->clear();
	std::vector<uint8_t>& sectionVector = *bc;
	para mp;
	mp.order = para::binary_serialize_base;
	mp.sectionVector = &sectionVector;

	std::vector<uint8_t> longVars;
	longVars.emplace_back(0);
	this->mngr->binSeri.bytes = &longVars;

	this->save_fetch(mp);

	//merge section and long variable zone
	if (longVars.size() > 1)
	{
		sectionVector.emplace_back(0);
		sectionVector.emplace_back(0);
		if (longVars.size())
		{
			sectionVector.insert(sectionVector.end(), longVars.begin(), longVars.end());
		}
	}
}
#if MEM_RJSON_ON
inline void base::serializeJson(std::string* bc) {
	if (this->mngr->rjson.doc == nullptr)
	{
		this->mngr->rjson.doc = new rapidjson::Document;
		this->mngr->rjson.doc->SetObject();
		this->mngr->rjson.allocator = &this->mngr->rjson.doc->GetAllocator();
	}

	bc->clear();
	this->mngr->ptrTable.clear();


	//entry the iteration of write
	para mp;
	mp.order = para::rjson_seriazlize;
	mp.rapidJson_section = this->mngr->rjson.doc;
	this->mngr->ptrTable.emplace(this, 0);
	this->save_fetch(mp);

	// json write finish
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	this->mngr->rjson.doc->Accept(writer);

	// json out
	*bc = buffer.GetString();

	//clean the table of pointer and return
	this->mngr->ptrTable.clear();
	this->mngr->rjson.doc->SetObject();
}
inline bool base::deserializeJson(const std::string& bc){
    return deserializeJson(bc.c_str(), bc.size());
}
inline bool base::deserializeJson(const char* Ptr, uint32_t StringSize) {
	if (this->mngr->rjson.doc == nullptr)
	{
		this->mngr->rjson.doc = new rapidjson::Document;
		this->mngr->rjson.doc->SetObject();
		this->mngr->rjson.allocator = &this->mngr->rjson.doc->GetAllocator();
	}

	this->mngr->statusBadValue = 0;
	//json deserialize don't need that
	//this->mngr->ptrTable.clear();

	rapidjson::Document &doc = *this->mngr->rjson.doc;
	doc.Parse(Ptr, StringSize);

	//check error
	if (doc.IsObject() == false) {
		this->mngr->rjson.doc->SetObject();
		return false;
	}

	//entry the iteration of read
	para mp;
	mp.order = para::rjson_deseriazlize;
	memPtr<base> self = this;
	mp.rapidJson_section = &doc;
	this->save_fetch(mp);

	//this->mngr->ptrTable.clear();
	this->mngr->rjson.doc->SetObject();
	return true;
}
#endif
inline bool manager::deserialize(uint8_t* Ptr, uint32_t StringSize)
{
	this->statusBadValue = 0;
	headerOfFile* hof = (headerOfFile*)Ptr;
	if constexpr (sizeof(void*) == 4)
	{
		if (memcmp(&hof->magic, &magicOfFile32, sizeof(hof->magic)) != 0)
		{
			return false;	//todo: think about 64 bits to 32 bits compatibility strategy?
		}
	}
	else if (sizeof(void*) == 8)
	{
		if (memcmp(&hof->magic, &magicOfFile64, sizeof(hof->magic)) != 0)
		{
			return false;	//todo: think about 32 bits to 64 bits compatibility strategy?
		}
	}
	if (is_big_endian())
	{
		if (hof->endian == hof->Little_Endian)
			return false;	//todo: think about endian convert program
	}
	else
	{
		if (hof->endian == hof->Big_Endian)
			return false;
	}

	//retrieval the pointer table 
	this->ptrTable.clear();
	this->mngr->binSeri.start = Ptr;
	this->mngr->binSeri.end = Ptr + StringSize;
	if (hof->offsetOfPtrTable + hof->sizeOfPtrTable > StringSize)
		return false;
	uint8_t* ptrTableBegin = Ptr + hof->offsetOfPtrTable;
	uint32_t ptrTableSize = hof->sizeOfPtrTable;
	for (; ptrTableSize > 0; ptrTableSize--)
	{
		base** mu = (base**)ptrTableBegin;
		uint32_t* offset = (uint32_t*)(ptrTableBegin + sizeof(base*));
		this->ptrTable.emplace(*mu, *offset);
		ptrTableBegin += sizeof(base*) + sizeof(uint32_t);
	}

	para mp;
	mp.order = para::binary_deserialize_manager;

	auto findRes = ptrTable.find(lowlevel::memPtrCorr((base*)hof->pointerOfFirst, 0));
	if (findRes == ptrTable.end())
		return false;
	mp.section.startPointer = mp.section.likelyPointer = Ptr + findRes->sectionStartOffset;
	bool& isConstructed = (bool&)findRes->isConstructed;
	isConstructed = true;
	base*& ptrRUN2 = (base*&)findRes->ptrRUN2;
	ptrRUN2 = this;
	this->save_fetch(mp);

	mp.section.startPointer = mp.section.likelyPointer = Ptr + hof->offsetOfSubfile;
	this->GWPP("s", this->subFiles, mp);

	mp.section.startPointer = mp.section.likelyPointer = Ptr + hof->offsetOfIngress;
	this->GWPP("i", this->ingressInte, mp);

	this->ptrTable.clear();
	return true;
}
inline void manager::serialize(std::vector<uint8_t>* bc)
{
	// head of file
	bc->clear();
	bc->reserve(this->bases.size() * likelyBytesPerUnit);	//let's assume every base will cost how many bytes
	bc->resize(sizeof(headerOfFile));
	headerOfFile* hof = (headerOfFile*)bc->data();
	if constexpr (sizeof(void*) == 4)
	{
		memcpy(&hof->magic, &magicOfFile32, sizeof(hof->magic));
	}
	else if (sizeof(void*) == 8)
	{
		memcpy(&hof->magic, &magicOfFile64, sizeof(hof->magic));
	}
	if (is_big_endian())
	{
		hof->endian = hof->Big_Endian;
	}
	else
	{
		hof->endian = hof->Little_Endian;
	}
	hof->pointerOfFirst = (uint64_t)this;

	// begin serialize
	this->ptrTable.clear();
	this->mngr->binSeri.bytes = bc;
	std::vector<uint8_t> sectionVector = std::vector<uint8_t>();
	para mp;
	mp.order = para::binary_serialize_manager;
	mp.sectionVector = &sectionVector;

	auto emplaceResult = this->ptrTable.emplace(this, 0);
	this->save_fetch(mp);
	uint32_t& sectionOffset = (uint32_t&)emplaceResult.first->sectionStartOffset;
	sectionOffset = &*(bc->end() - 1) - bc->data() + 1;

	bc->insert(bc->end(), sectionVector.begin(), sectionVector.end());

	sectionVector.clear();
	this->GWPP("s", this->subFiles, mp);

	hof = (headerOfFile*)bc->data();
	hof->offsetOfSubfile = bc->size();
	bc->insert(bc->end(), sectionVector.begin(), sectionVector.end());

	sectionVector.clear();
	this->GWPP("i", this->ingressInte, mp);

	hof = (headerOfFile*)bc->data();
	hof->offsetOfIngress = bc->size();
	bc->insert(bc->end(), sectionVector.begin(), sectionVector.end());

	hof = (headerOfFile*)bc->data();
	hof->sizeOfPtrTable = this->ptrTable.size();
	hof->offsetOfPtrTable = bc->size();

	int32_t perreserve = (sizeof(base*) + sizeof(uint32_t)) * this->ptrTable.size() - (bc->capacity() - bc->size());
	if (perreserve > 0)
		bc->reserve(perreserve);
	for (auto& iter : ptrTable)
	{
		bc->resize(bc->size() + sizeof(base*));
		memcpy(&*(bc->end() - 1) - sizeof(base*) + 1, &iter.ptrRUN, sizeof(base*));
		bc->resize(bc->size() + sizeof(uint32_t));
		memcpy(&*(bc->end() - 1) - sizeof(uint32_t) + 1, &iter.sectionStartOffset, sizeof(uint32_t));
	}
	this->ptrTable.clear();
}
template<typename T>
inline std::enable_if_t<eb::has_save_fetch_sub<T>::value, bool>
manager::deserializeSub(T& sub, uint8_t* Ptr, uint32_t StringSize)
{
	para mp;
	mp.order = para::binary_deserialize_base;
	mp.section.startPointer = mp.section.likelyPointer = Ptr;

	//find long variable zone
	uint8_t* end = Ptr + StringSize;
	for (int zeroCount = 0; Ptr < end; Ptr++)
	{
		if (zeroCount == 3)
		{
			if (*Ptr)
				break;
		}
		else if (*Ptr == 0)
		{
			zeroCount++;
		}
		else
		{
			zeroCount = 0;
		}
	}

	this->binSeri.start = Ptr - 1;
	this->binSeri.end = end;
	this->statusBadValue = 0;
	sub.save_fetch_sub(this, "", mp);
	return true;
}
template<typename T>
inline std::enable_if_t<eb::has_save_fetch_sub<T>::value, void>
manager::serializeSub(T& sub, std::vector<uint8_t>* bc)
{
	bc->clear();
	std::vector<uint8_t>& sectionVector = *bc;
	para mp;
	mp.order = para::binary_serialize_base;
	mp.sectionVector = &sectionVector;

	std::vector<uint8_t> longVars;
	longVars.emplace_back(0);
	this->binSeri.bytes = &longVars;

	sub.save_fetch_sub(this, "", mp);

	//merge section and long variable zone
	if (longVars.size() > 1)
	{
		sectionVector.emplace_back(0);
		sectionVector.emplace_back(0);
		if (longVars.size())
		{
			sectionVector.insert(sectionVector.end(), longVars.begin(), longVars.end());
		}
	}
}



//variant helper
template <typename First, typename ...Args>
template<bool _void, typename IterFirst, typename... IterArgs>
inline bool eb::lowlevel::pushVariantHelper<First, Args...>::createIter(uint32_t i, std::variant<First, Args...>& variant, base* mu, para& para) noexcept
{
	if (i == 0)
	{
        variant = IterFirst();
		IterFirst& ref = *std::get_if<IterFirst>(&variant);
		mu->GWPP_Base(pv, ref, para);
	}
	return createIter<false, IterArgs...>(i - 1, variant, mu, para);
}
