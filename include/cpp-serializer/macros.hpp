#ifndef CPPSER_MACROS_DEFINED

#   define CPPSER_DEFINE_MIXTIME_STRUCT(classname, publicname, privatename, def)  \
    namespace internal {\
        template<YesNoRuntime>\
        struct classname ## _ ## privatename ## _helper {};\
        template<>\
        struct classname ## _ ## privatename ## _helper<YesNoRuntime::Runtime> {\
            bool Get ## publicname() const { return privatename; }\
            void Set ## publicname(bool value) { privatename = value; }\
        protected: \
            bool privatename = def;\
        };\
    }

#   define CPPSER_READ_IF_RUNTIME(name, index) \
    if constexpr(Settings::name == YesNoRuntime::Yes)\
        settings[index] = true;\
    else if constexpr(Settings::name == YesNoRuntime::No)\
        settings[index] = false;\
    else \
        settings[index] = this->Get ## name();

#   define CPPSER_IF_MIXED(constex, reg) if constexpr(constex) if(reg)

#define CPPSER_MACROS_DEFINED
#endif
