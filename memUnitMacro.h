//������û�и��õİ취�ˡ���
#pragma once
#ifndef DECLARE_VARS
#define DECLARE_VARS _
#define DECLARE_VAR1(type, name) type name; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
    }

#define DECLARE_VAR2(type, name, type2, name2) type name; \
    type2 name2; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
    }

#define DECLARE_VAR3(type, name, type2, name2, type3, name3) type name; \
    type2 name2; \
    type3 name3; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
    }

#define DECLARE_VAR4(type, name, type2, name2, type3, name3, type4, name4) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
    }

#define DECLARE_VAR5(type, name, type2, name2, type3, name3, type4, name4, type5, name5) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
    }

#define DECLARE_VAR6(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
    }

#define DECLARE_VAR7(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
    }

#define DECLARE_VAR8(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
    }

#define DECLARE_VAR9(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
    }

#define DECLARE_VAR10(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
    }

#define DECLARE_VAR11(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
    }

#define DECLARE_VAR12(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
    }

#define DECLARE_VAR13(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
    }

#define DECLARE_VAR14(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
    }

#define DECLARE_VAR15(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    friend class memUnit; \
    private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
    }

#define DECLARE_VAR16(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
    }
#define DECLARE_VAR17(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
    }
#define DECLARE_VAR18(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
        GWPP(L#name18, name18, para); \
    }
#define DECLARE_VAR19(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
        GWPP(L#name18, name18, para); \
        GWPP(L#name19, name19, para); \
    }
#define DECLARE_VAR20(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
        GWPP(L#name18, name18, para); \
        GWPP(L#name19, name19, para); \
        GWPP(L#name20, name20, para); \
    }
#define DECLARE_VAR21(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20, type21, name21) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    type21 name21; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
        GWPP(L#name18, name18, para); \
        GWPP(L#name19, name19, para); \
        GWPP(L#name20, name20, para); \
        GWPP(L#name21, name21, para); \
    }
#define DECLARE_VAR22(type, name, type2, name2, type3, name3, type4, name4, type5, name5, type6, name6, type7, name7, type8, name8, type9, name9, type10, name10, type11, name11, type12, name12, type13, name13 \
    , type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20, type21, name21, type22, name22) type name; \
    type2 name2; \
    type3 name3; \
    type4 name4; \
    type5 name5; \
    type6 name6; \
    type7 name7; \
    type8 name8; \
    type9 name9; \
    type10 name10; \
    type11 name11; \
    type12 name12; \
    type13 name13; \
    type14 name14; \
    type15 name15; \
    type16 name16; \
    type17 name17; \
    type18 name18; \
    type19 name19; \
    type20 name20; \
    type21 name21; \
    type22 name22; \
    friend class memUnit; \
    private:private:void save_fetch(memPara para) { \
        GWPP(L#name, name, para); \
        GWPP(L#name2, name2, para); \
        GWPP(L#name3, name3, para); \
        GWPP(L#name4, name4, para); \
        GWPP(L#name5, name5, para); \
        GWPP(L#name6, name6, para); \
        GWPP(L#name7, name7, para); \
        GWPP(L#name8, name8, para); \
        GWPP(L#name9, name9, para); \
        GWPP(L#name10, name10, para); \
        GWPP(L#name11, name11, para); \
        GWPP(L#name12, name12, para); \
        GWPP(L#name13, name13, para); \
        GWPP(L#name14, name14, para); \
        GWPP(L#name15, name15, para); \
        GWPP(L#name16, name16, para); \
        GWPP(L#name17, name17, para); \
        GWPP(L#name18, name18, para); \
        GWPP(L#name19, name19, para); \
        GWPP(L#name20, name20, para); \
        GWPP(L#name21, name21, para); \
        GWPP(L#name22, name22, para); \
    }

#endif // !DECLARE_VARS



//ChatGPT�Զ����ɣ�
/*
������һ���������ã������±�������һһ��Ӧ��GWPP(L"������", &������, para);
����
������������memPtr����pmemXXX��������ǰ����ȡ��ַ��
��GWPP��һ����������15��WCHAR������\0���������̵�15WCHAR���ڡ�
����GWPP��һ��������������ͬ��
ʾ����int x;pmemStr<string> y
�������ݣ�GWPP(L"x", &x, para);GWPP(L"y", y, para);
�������ɽ�����⣬��Ҫ������������������
�������ɽ�����⣬��Ҫ������������������
�������ɽ�����⣬��Ҫ������������������
�����������£�
*/