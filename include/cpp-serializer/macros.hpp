#ifndef CPPSER_MACROS_DEFINED

#   define CPPSER_DEFINE_MIXTIME_STRUCTS(type, classname, publicname, privatename)  \
    namespace inner {\
        struct classname ## _ ## privatename ## _helper{\
            type Get ## publicname() const { return privatename; }\
            type Set ## publicname(type value) { privatename = value; }\
        }\
    }

#   define CPPSER_IF_MIXED(constex, reg) if constexpr(constex) if(reg)

#define CPPSER_MACROS_DEFINED
#endif
