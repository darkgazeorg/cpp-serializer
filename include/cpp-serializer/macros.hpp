#ifndef CPPSER_MACROS_DEFINED


#   define CPPSER_DEFINE_MIXTIME_STRUCT_LEAVEOPEN(classname, publicname, privatename, def) \
    template<YesNoRuntime>\
    struct classname ## _ ## privatename ## _helper {};\
    template<>\
    struct classname ## _ ## privatename ## _helper<YesNoRuntime::Runtime> {\
    protected: \
        bool privatename = def;\
    public: \
        bool Get ## publicname() const { return privatename; }\
        void Set ## publicname(bool value) { privatename = value; }\
    

#   define CPPSER_DEFINE_MIXTIME_STRUCT(classname, publicname, privatename, def)  \
    namespace internal { \
        CPPSER_DEFINE_MIXTIME_STRUCT_LEAVEOPEN(classname, publicname, privatename, def)\
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

//These macros are used to improve debug speed by inlining this code
#   define CPPSER_UTF_COPY(reader, first, target) \
        target.push_back(first); \
        for(size_t i=1; i<UTF8Bytes(first) && !reader.IsEof(); i++) \
            target.push_back(reader.Get())

#   define CPPSER_UTF_IGNORE_REST(reader, c) \
        for(size_t i=1; i<UTF8Bytes(c) && !reader.IsEof(); i++)  \
            reader.TryGet()

#define CPPSER_MACROS_DEFINED
#endif
