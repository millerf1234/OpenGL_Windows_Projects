//
// File:                                                 OpenGLEnumToString.cpp
//
// Description:                      Implementation file for OpenGLEnumToString
//                 
//                                             The implementation relies upon a 
//                                     std::unordered_map of GLenum/std::string
//                                    pairs that provides near-constant look-up
//                                   times. This table is initialized the first
//                                                 time this function is called.
//
//

#include "OpenGLEnumToString.h"

/////////////////////////////////////////////////////////////////
//////   TUNE THE std::unordered_map BEHAVIOR AS DESIRED   //////
/////////////////////////////////////////////////////////////////

//Controls the load factor behavior of the std::unordered_map's 
//hash table. Value Should be 1.0f or less, with 0.7f - 0.8f 
//typically being the desirable sweet-spot. The default load 
//factor of std::unordered_map is 1.0f. To be safe the code 
//will clamp any values outside of the range [0.5f, 0.95f] to
//the closest value in that range.
constexpr float TARGET_MAXIMUM_LOAD_FACTOR = 0.75f; 

//The GLEnums in 'glad.h' start at line 170 and end at line 1537
//This value is currently unused
//constexpr size_t BUCKETS_TO_RESERVE = 1537u - 170u; 



using namespace GL_ENUM_TO_STRING_INTERNAL_NAMESPACE;


std::string convertGLEnumToString(GLenum hexEnum) noexcept {
    static DictionaryOfGLEnums dictionary;

    std::string enumName = dictionary.lookup(hexEnum);
    return enumName;
}



DictionaryOfGLEnums::DictionaryOfGLEnums() noexcept {
    float loadFactor;
    if constexpr (TARGET_MAXIMUM_LOAD_FACTOR < 0.5f)
        loadFactor = 0.5f;
    else if constexpr (TARGET_MAXIMUM_LOAD_FACTOR > 0.95f)
        loadFactor = 0.95f;
    else 
        loadFactor = TARGET_MAXIMUM_LOAD_FACTOR;
    mDictionary_.max_load_factor(loadFactor);
    //mDictionary_.reserve(16384ULL); //Using the debugger it appears as though
    //it is best to not reserve space ahead of time because the resulting Hash
    //table size upon completion is quite a bit smaller. Maybe one day I can 
    //investigate why this is... I'm probably misinterpreting std::unordered_map's
    //syntax somewhere somehow. 
    buildDictionary();
    fprintf(MSGLOG, "\nGLEnum Dictionary Built With %d Mappings Total!\n",
        mDictionary_.bucket_count());
}

DictionaryOfGLEnums::~DictionaryOfGLEnums() noexcept {
    
}


std::string DictionaryOfGLEnums::lookup(GLenum hexEnum) noexcept {
    auto result = mDictionary_.find(hexEnum);
    if (result != mDictionary_.end())
        return result->second; //String is the second member of each bucket
    return UNKNOWN_GLENUM_NAME;
}

void DictionaryOfGLEnums::insert(GLenum hexEnum, std::string name) noexcept {
    const auto res = mDictionary_.insert(std::make_pair(hexEnum, name));
    assert(res.second); //This checks the second parameter of the return pair which
    //represents a boolean representing whether our insertion was unique or not.
}






void DictionaryOfGLEnums::buildDictionary() noexcept {

    //The first 2 we insert are special because there are multiple GLenums that
    //share the same value. All five enums in the set {GL_FALSE, GL_ZERO,
    //GL_POINTS, GL_NONE, GL_NO_ERROR} are defined to be '0' while likewise 
    //all three enums in the set {GL_TRUE, GL_ONE, GL_LINES} are defined as 
    //the value '1'. 
    // [The following GLenums also could be included, but it was decided by me not to]
    //     #define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
    //     #define GL_SYNC_FLUSH_COMMANDS_BIT 0x00000001
    insert(GL_FALSE,"GL_FALSE/GL_ZERO/GL_POINTS/GL_NONE/GL_NO_ERROR");//0x0000    
    insert(GL_TRUE,        "GL_TRUE/GL_ONE/GL_LINES");        //0x0001


    //That is it for the special cases. For the rest of them, this is
    //pretty much just a brute force process inserting each one at a time.
    //Here goes...

    //  [I have tried to match the order in which these appear in 'glad.h' except as noted]

    insert(GL_NEVER,                   "GL_NEVER");                   //0x0200
    insert(GL_LESS,                    "GL_LESS");                    //0x0201
    insert(GL_EQUAL,                   "GL_EQUAL");                   //0x0202
    insert(GL_LEQUAL,                  "GL_LEQUAL");                  //0x0203
    insert(GL_GREATER,                 "GL_GREATER");                 //0x0204
    insert(GL_NOTEQUAL,                "GL_NOTEQUAL");                //0x0205
    insert(GL_GEQUAL,                  "GL_GEQUAL");                  //0x0206
    insert(GL_ALWAYS,                  "GL_ALWAYS");                  //0x0207
     
    insert(GL_SRC_COLOR,               "GL_SRC_COLOR");               //0x0300
    insert(GL_ONE_MINUS_SRC_COLOR,     "GL_ONE_MINUS_SRC_COLOR");     //0x0301
    insert(GL_SRC_ALPHA,               "GL_SRC_ALPHA");               //0x0302
    insert(GL_ONE_MINUS_SRC_ALPHA,     "GL_ONE_MINUS_SRC_ALPHA");     //0x0303
    insert(GL_DST_ALPHA,               "GL_DST_ALPHA");               //0x0304
    insert(GL_ONE_MINUS_DST_ALPHA,     "GL_ONE_MINUS_DST_ALPHA");     //0x0305
    insert(GL_DST_COLOR,               "GL_DST_COLOR");               //0x0306
    insert(GL_ONE_MINUS_DST_COLOR,     "GL_ONE_MINUS_DST_COLOR");     //0x0307
    insert(GL_SRC_ALPHA_SATURATE,      "GL_SRC_ALPHA_SATURATE");      //0x0308

    insert(GL_FRONT_LEFT,              "GL_FRONT_LEFT");              //0x0400
    insert(GL_FRONT_RIGHT,             "GL_FRONT_RIGHT");             //0x0401
    insert(GL_BACK_LEFT,               "GL_BACK_LEFT");               //0x0402
    insert(GL_BACK_RIGHT,              "GL_BACK_RIGHT");              //0x0403
    insert(GL_FRONT,                   "GL_FRONT");                   //0x0404
    insert(GL_BACK,                    "GL_BACK");                    //0x0405
    insert(GL_LEFT,                    "GL_LEFT");                    //0x0406
    insert(GL_RIGHT,                   "GL_RIGHT");                   //0x0407
    insert(GL_FRONT_AND_BACK,          "GL_FRONT_AND_BACK");          //0x0408

    insert(GL_INVALID_ENUM,            "GL_INVALID_ENUM");            //0x0500
    insert(GL_INVALID_VALUE,           "GL_INVALID_VALUE");           //0x0501
    insert(GL_INVALID_OPERATION,       "GL_INVALID_OPERATION");       //0x0502
    insert(GL_OUT_OF_MEMORY,           "GL_OUT_OF_MEMORY");           //0x0505
     
    insert(GL_CW,                      "GL_CW");                      //0x0900
    insert(GL_CCW,                     "GL_CCW");                     //0x0901

    //  ...  [Skip a bunch]
    
    insert(GL_SUBPIXEL_BITS,           "GL_SUBPIXEL_BITS");           //0x0D50
    insert(GL_TEXTURE_1D,              "GL_TEXTURE_1D");              //0x0DE0
    insert(GL_TEXTURE_2D,              "GL_TEXTURE_2D");              //0x0DE1
    insert(GL_TEXTURE_WIDTH,           "GL_TEXTURE_WIDTH");           //0x1000
    insert(GL_TEXTURE_HEIGHT,          "GL_TEXTURE_HEIGHT");          //0x1001
    insert(GL_TEXTURE_BORDER_COLOR,    "GL_TEXTURE_BORDER_COLOR");    //0x1004
    insert(GL_DONT_CARE,               "GL_DONT_CARE");               //0x1100
    insert(GL_FASTEST,                 "GL_FASTEST");                 //0x1101
    insert(GL_NICEST,                  "GL_NICEST");                  //0x1102
    insert(GL_BYTE,                    "GL_BYTE");                    //0x1400
    insert(GL_UNSIGNED_BYTE,           "GL_UNSIGNED_BYTE");           //0x1401
    insert(GL_SHORT,                   "GL_SHORT");                   //0x1402
    insert(GL_UNSIGNED_SHORT,          "GL_UNSIGNED_SHORT");          //0x1403
    insert(GL_INT,                     "GL_INT");                     //0x1404
    insert(GL_UNSIGNED_INT,            "GL_UNSIGNED_INT");            //0x1405
    insert(GL_FLOAT,                   "GL_FLOAT");                   //0x1406
    insert(GL_DOUBLE,                  "GL_DOUBLE");                  //0x140A //Out of sequence 
    insert(GL_FIXED,                   "GL_FIXED");                   //0x140C //Out-Of-Sequence 
    insert(GL_CLEAR,                   "GL_CLEAR");                   //0x1500
    insert(GL_AND,                     "GL_AND");                     //0x1501
    insert(GL_AND_REVERSE,             "GL_AND_REVERSE");             //0x1502
    insert(GL_COPY,                    "GL_COPY");                    //0x1503
    insert(GL_AND_INVERTED,            "GL_AND_INVERTED");            //0x1504
    insert(GL_NOOP,                    "GL_NOOP");                    //0x1505
    insert(GL_XOR,                     "GL_XOR");                     //0x1506
    insert(GL_OR,                      "GL_OR");                      //0x1507
    insert(GL_NOR,                     "GL_NOR");                     //0x1508
    insert(GL_EQUIV,                   "GL_EQUIV");                   //0x1509
    insert(GL_INVERT,                  "GL_INVERT");                  //0x150A
    insert(GL_OR_REVERSE,              "GL_OR_REVERSE");              //0x150B
    insert(GL_COPY_INVERTED,           "GL_COPY_INVERTED");           //0x150C
    insert(GL_OR_INVERTED,             "GL_OR_INVERTED");             //0x150D
    insert(GL_NAND,                    "GL_NAND");                    //0x150E
    insert(GL_SET,                     "GL_SET");                     //0x150F
    insert(GL_TEXTURE,                 "GL_TEXTIRE");                 //0x1702
    insert(GL_COLOR,                   "GL_COLOR");                   //0x1800
    insert(GL_DEPTH,                   "GL_DEPTH");                   //0x1801
    insert(GL_STENCIL,                 "GL_STENCIL");                 //0x1802
    insert(GL_STENCIL_INDEX,           "GL_STENCIL_INDEX");           //0x1901
    insert(GL_DEPTH_COMPONENT,         "GL_DEPTH_COMPONENT");         //0x1902
    insert(GL_RED,                     "GL_RED");                     //0x1903
    insert(GL_GREEN,                   "GL_GREEN");                   //0x1904
    insert(GL_BLUE,                    "GL_BLUE");                    //0x1905
    insert(GL_ALPHA,                   "GL_ALPHA");                   //0x1906
    insert(GL_RGB,                     "GL_RGB");                     //0x1907
    insert(GL_RGBA,                    "GL_RGBA");                    //0x1908

    //  ...   [Skip some more]
    
    insert(GL_NEAREST,                 "GL_NEAREST");                 //0x2600
    insert(GL_LINEAR,                  "GL_LINEAR");                  //0x2601

    insert(GL_R3_G3_B2,                "GL_R3_G3_B2");                //0x2A10
    insert(GL_RGB4,                    "GL_RGB4");                    //0x804F
    insert(GL_RGB5,                    "GL_RGB5");                    //0x8050
    insert(GL_RGB8,                    "GL_RGB8");                    //0x8051
    insert(GL_RGB10,                   "GL_RGB10");                   //0x8052
    insert(GL_RGB12,                   "GL_RGB12");                   //0x8053
    insert(GL_RGB16,                   "GL_RGB16");                   //0x8054
    insert(GL_RGBA2,                   "GL_RGBA2");                   //0x8055
    insert(GL_RGBA4,                   "GL_RGBA4");                   //0x8056
    insert(GL_RGB5_A1,                 "GL_RGB5_A1");                 //0x8057
    insert(GL_RGBA8,                   "GL_RGBA8");                   //0x8058
    insert(GL_RGB10_A2,                "GL_RGB10_A2");                //0x8059
    insert(GL_RGBA12,                  "GL_RGBA12");                  //0x805A
    insert(GL_RGBA16,                  "GL_RGBA16");                  //0x805B
    insert(GL_UNSIGNED_BYTE_3_3_2,     "GL_UNSIGNED_BYTE_3_3_2");     //0x8032
    insert(GL_UNSIGNED_SHORT_4_4_4_4,  "GL_UNSIGNED_SHORT_4_4_4_4");  //0x8033
    insert(GL_UNSIGNED_SHORT_5_5_5_1,  "GL_UNSIGNED_SHORT_5_5_5_1");  //0x8034
    insert(GL_UNSIGNED_INT_8_8_8_8,    "GL_UNSIGNED_INT_8_8_8_8");    //0x8035
    insert(GL_UNSIGNED_INT_10_10_10_2, "GL_UNSIGNED_INT_10_10_10_2"); //0x8036

    //   ...   

    insert(GL_UNSIGNED_BYTE_2_3_3_REV, "GL_UNSIGNED_BYTE_2_3_3_REV"); //0x8362
    insert(GL_UNSIGNED_SHORT_5_6_5,    "GL_UNSIGNED_SHORT_5_6_5");    //0x8363
    insert(GL_UNSIGNED_SHORT_5_6_5_REV,"GL_UNSIGNED_SHORT_5_6_5_REV");//0x8364
    //Stupid long GLenums messing up my pattern... 
    insert(GL_UNSIGNED_SHORT_4_4_4_4_REV,"GL_UNSIGNED_SHORT_4_4_4_4_REV");//0x8365
    insert(GL_UNSIGNED_SHORT_1_5_5_5_REV,"GL_UNSIGNED_SHORT_1_5_5_5_REV");//0x8366
    insert(GL_UNSIGNED_INT_8_8_8_8_REV,"GL_UNSIGNED_INT_8_8_8_8_REV");//0x8367
    insert(GL_UNSIGNED_INT_2_10_10_10_REV,"GL_UNSIGNED_INT_2_10_10_10_REV");//0x8368
    insert(GL_BGR,                     "GL_BGR");                     //0x80E0
    insert(GL_BGRA,                    "GL_BGRA");                    //0x80E1

    //  ...

    insert(GL_TEXTURE0,                "GL_TEXTURE0");                //0x84C0
    insert(GL_TEXTURE1,                "GL_TEXTURE1");                //0x84C1
    insert(GL_TEXTURE2,                "GL_TEXTURE2");                //0x84C2
    insert(GL_TEXTURE3,                "GL_TEXTURE3");                //0x84C3
    insert(GL_TEXTURE4,                "GL_TEXTURE4");                //0x84C4
    insert(GL_TEXTURE5,                "GL_TEXTURE5");                //0x84C5
    insert(GL_TEXTURE6,                "GL_TEXTURE6");                //0x84C6
    insert(GL_TEXTURE7,                "GL_TEXTURE7");                //0x84C7
    insert(GL_TEXTURE8,                "GL_TEXTURE8");                //0x84C8
    insert(GL_TEXTURE9,                "GL_TEXTURE9");                //0x84C9
    insert(GL_TEXTURE10,               "GL_TEXTURE10");               //0x84CA
    insert(GL_TEXTURE11,               "GL_TEXTURE11");               //0x84CB
    insert(GL_TEXTURE12,               "GL_TEXTURE12");               //0x84CC
    insert(GL_TEXTURE13,               "GL_TEXTURE13");               //0x84CD
    insert(GL_TEXTURE14,               "GL_TEXTURE14");               //0x84CE
    insert(GL_TEXTURE15,               "GL_TEXTURE15");               //0x84CF
    insert(GL_TEXTURE16,               "GL_TEXTURE16");               //0x84D0
    insert(GL_TEXTURE17,               "GL_TEXTURE17");               //0x84D1
    insert(GL_TEXTURE18,               "GL_TEXTURE18");               //0x84D2
    insert(GL_TEXTURE19,               "GL_TEXTURE19");               //0x84D3
    insert(GL_TEXTURE20,               "GL_TEXTURE20");               //0x84D4
    insert(GL_TEXTURE21,               "GL_TEXTURE21");               //0x84D5
    insert(GL_TEXTURE22,               "GL_TEXTURE22");               //0x84D6
    insert(GL_TEXTURE23,               "GL_TEXTURE23");               //0x84D7
    insert(GL_TEXTURE24,               "GL_TEXTURE24");               //0x84D8
    insert(GL_TEXTURE25,               "GL_TEXTURE25");               //0x84D9
    insert(GL_TEXTURE26,               "GL_TEXTURE26");               //0x84DA
    insert(GL_TEXTURE27,               "GL_TEXTURE27");               //0x84DB
    insert(GL_TEXTURE28,               "GL_TEXTURE28");               //0x84DC
    insert(GL_TEXTURE29,               "GL_TEXTURE29");               //0x84DD
    insert(GL_TEXTURE30,               "GL_TEXTURE30");               //0x84DE
    insert(GL_TEXTURE31,               "GL_TEXTURE31");               //0x84DF

    //  ...

    insert(GL_COMPRESSED_RGB,          "GL_COMPRESSED_RGB");          //0x84ED
    insert(GL_COMPRESSED_RGBA,         "GL_COMPRESSED_RGBA");         //0x84EE

    //  ...

    insert(GL_FLOAT_VEC2,              "GL_FLOAT_VEC2");              //0x8B50
    insert(GL_FLOAT_VEC3,              "GL_FLOAT_VEC3");              //0x8B51
    insert(GL_FLOAT_VEC4,              "GL_FLOAT_VEC4");              //0x8B52
    insert(GL_INT_VEC2,                "GL_INT_VEC2");                //0x8B53
    insert(GL_INT_VEC3,                "GL_INT_VEC3");                //0x8B54
    insert(GL_INT_VEC4,                "GL_INT_VEC4");                //0x8B55
    insert(GL_BOOL,                    "GL_BOOL");                    //0x8B56
    insert(GL_BOOL_VEC2,               "GL_BOOL_VEC2");               //0x8B57
    insert(GL_BOOL_VEC3,               "GL_BOOL_VEC3");               //0x8B58
    insert(GL_BOOL_VEC4,               "GL_BOOL_VEC4");               //0x8B59
    insert(GL_FLOAT_MAT2,              "GL_FLOAT_MAT2");              //0x8B5A
    insert(GL_FLOAT_MAT3,              "GL_FLOAT_MAT3");              //0x8B5B
    insert(GL_FLOAT_MAT4,              "GL_FLOAT_MAT4");              //0x8B5C
    insert(GL_SAMPLER_1D,              "GL_SAMPLER_1D");              //0x8B5D
    insert(GL_SAMPLER_2D,              "GL_SAMPLER_2D");              //0x8B5E
    insert(GL_SAMPLER_3D,              "GL_SAMPLER_3D");              //0x8B5F
    insert(GL_SAMPLER_CUBE,            "GL_SAMPLER_CUBE");            //0x8B60
    insert(GL_SAMPLER_1D_SHADOW,       "GL_SAMPLER_1D_SHADOW");       //0x8B61
    insert(GL_SAMPLER_2D_SHADOW,       "GL_SAMPLER_2D_SHADOW");       //0x8B62
                                                   
    //   ...

    insert(GL_FLOAT_MAT2x3,            "GL_FLOAT_MAT2x3");            //0x8B65
    insert(GL_FLOAT_MAT2x4,            "GL_FLOAT_MAT2x4");            //0x8B66
    insert(GL_FLOAT_MAT3x2,            "GL_FLOAT_MAT3x2");            //0x8B67
    insert(GL_FLOAT_MAT3x4,            "GL_FLOAT_MAT3x4");            //0x8B68
    insert(GL_FLOAT_MAT4x2,            "GL_FLOAT_MAT4x2");            //0x8B69
    insert(GL_FLOAT_MAT4x3,            "GL_FLOAT_MAT4x3");            //0x8B6A
    insert(GL_SRGB,                    "GL_SRGB");                    //0x8C40
    insert(GL_SRGB8,                   "GL_SRGB8");                   //0x8C41
    insert(GL_SRGB_ALPHA,              "GL_SRGB_ALPHA");              //0x8C42
    insert(GL_SRGB8_ALPHA8,            "GL_SRGB8_ALPHA8");            //0x8C43
    insert(GL_COMPRESSED_SRGB,         "GL_COMPRESSED_SRGB");         //0x8C48
    insert(GL_COMPRESSED_SRGB_ALPHA,   "GL_COMPRESSED_SRGB_ALPHA");   //0x8C49

    //  ...

    insert(GL_COMPRESSED_RED,          "GL_COMPRESSED_RED");          //0x8225
    insert(GL_COMPRESSED_RG,           "GL_COMPRESSED_RG");           //0x8226
    /* 'GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT' would go here but is skipped*/
    insert(GL_RGBA32F,                 "GL_RGBA32F");                 //0x8814
    insert(GL_RGB32F,                  "GL_RGB32F");                  //0x8815
    insert(GL_RGBA16F,                 "GL_RGBA16F");                 //0x881A
    insert(GL_RGB16F,                  "GL_RGB16F");                  //0x881B

    //   ...

    insert(GL_R11F_G11F_B10F,          "GL_R11F_G11F_B10F");          //0x8C3A
    insert(GL_UNSIGNED_INT_10F_11F_11F_REV,"GL_UNSIGNED_INT_10F_11F_11F_REV");//0x8C3B
    insert(GL_RGB9_E5,                 "GL_RGB9_E5");                 //0x8C3D
    insert(GL_UNSIGNED_INT_5_9_9_9_REV,"GL_UNSIGNED_INT_5_9_9_9_REV");//0x8C3E

    //   ...

    insert(GL_RGBA32UI,                "GL_RGBA32UI");                //0x8D70
    insert(GL_RGB32UI,                 "GL_RGB32UI");                 //0x8D71
    insert(GL_RGBA16UI,                "GL_RGBA16UI");                //0x8D76
    insert(GL_RGB16UI,                 "GL_RGB16UI");                 //0x8D77
    insert(GL_RGBA8UI,                 "GL_RGBA8UI");                 //0x8D7C
    insert(GL_RGB8UI,                  "GL_RGB8UI");                  //0x8D7D
    insert(GL_RGBA32I,                 "GL_RGBA32I");                 //0x8D82
    insert(GL_RGB32I,                  "GL_RGB32I");                  //0x8D83
    insert(GL_RGBA16I,                 "GL_RGBA16I");                 //0x8D88
    insert(GL_RGB16I,                  "GL_RGB16I");                  //0x8D89
    insert(GL_RGBA8I,                  "GL_RGBA8I");                  //0x8D8E
    insert(GL_RGB8I,                   "GL_RGB8I");                   //0x8D8F
    insert(GL_RED_INTEGER,             "GL_RED_INTEGER");             //0x8D94
    insert(GL_GREEN_INTEGER,           "GL_GREEN_INTEGER");           //0x8D95
    insert(GL_BLUE_INTEGER,            "GL_BLUE_INTEGER");            //0x8D96
    insert(GL_RGB_INTEGER,             "GL_RGB_INTEGER");             //0x8D98
    insert(GL_RGBA_INTEGER,            "GL_RGBA_INTEGER");            //0x8D99
    insert(GL_BGR_INTEGER,             "GL_BGR_INTEGER");             //0x8D9A
    insert(GL_BGRA_INTEGER,            "GL_BGRA_INTEGER");            //0x8D9B
    insert(GL_SAMPLER_1D_ARRAY,        "GL_SAMPLER_1D_ARRAY");        //0x8DC0
    insert(GL_SAMPLER_2D_ARRAY,        "GL_SAMPLER_2D_ARRAY");        //0x8DC1
    insert(GL_SAMPLER_1D_ARRAY_SHADOW, "GL_SAMPLER_1D_ARRAY_SHADOW"); //0x8DC3
    insert(GL_SAMPLER_2D_ARRAY_SHADOW, "GL_SAMPLER_2D_ARRAY_SHADOW"); //0x8DC4
    insert(GL_SAMPLER_CUBE_SHADOW,     "GL_SAMPLER_CUBE_SHADOW");     //0x8DC5
    insert(GL_UNSIGNED_INT_VEC2,       "GL_UNSIGNED_INT_VEC2");       //0x8DC6
    insert(GL_UNSIGNED_INT_VEC3,       "GL_UNSIGNED_INT_VEC3");       //0x8DC7
    insert(GL_UNSIGNED_INT_VEC4,       "GL_UNSIGNED_INT_VEC4");       //0x8DC8
    insert(GL_INT_SAMPLER_1D,          "GL_INT_SAMPLER_1D");          //0x8DC9
    insert(GL_INT_SAMPLER_2D,          "GL_INT_SAMPLER_2D");          //0x8DCA
    insert(GL_INT_SAMPLER_3D,          "GL_INT_SAMPLER_3D");          //0x8DCB
    insert(GL_INT_SAMPLER_CUBE,        "GL_INT_SAMPLER_CUBE");        //0x8DCC
    insert(GL_INT_SAMPLER_1D_ARRAY,    "GL_INT_SAMPLER_1D_ARRAY");    //0x8DCE
    insert(GL_INT_SAMPLER_2D_ARRAY,    "GL_INT_SAMPLER_2D_ARRAY");    //0x8DCF
    insert(GL_UNSIGNED_INT_SAMPLER_1D, "GL_UNSIGNED_INT_SAMPLER_1D"); //0x8DD1
    insert(GL_UNSIGNED_INT_SAMPLER_2D, "GL_UNSIGNED_INT_SAMPLER_2D"); //0x8DD2
    insert(GL_UNSIGNED_INT_SAMPLER_3D, "GL_UNSIGNED_INT_SAMPLER_3D"); //0x8DD3
    insert(GL_UNSIGNED_INT_SAMPLER_CUBE,"GL_UNSIGNED_INT_SAMPLER_CUBE");//0x8DD4
    insert(GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,"GL_UNSIGNED_INT_SAMPLER_1D_ARRAY");//0x8DD6
    insert(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,"GL_UNSIGNED_INT_SAMPLER_2D_ARRAY");//0x8DD7
    
    // ...
    
    //(This next one is quite the beast
    insert(GL_FLOAT_32_UNSIGNED_INT_24_8_REV, "GL_FLOAT_32_UNSIGNED_INT_24_8_REV");//0x8DAD
    insert(GL_INVALID_FRAMEBUFFER_OPERATION,"GL_INVALID_FRAMEBUFFER_OPERATION");//0x0506

    //  ... 

    insert(GL_UNSIGNED_INT_24_8,       "GL_UNSIGNED_INT_24_8");       //0x84FA
    insert(GL_DEPTH24_STENCIL8,        "GL_DEPTH24_STENCIL8");        //0x88F0

    //   ...  

    insert(GL_FRAMEBUFFER_SRGB,        "GL_FRAMEBUFFER_SRGB");        //0x8DB9
    insert(GL_HALF_FLOAT,              "GL_HALF_FLOAT");              //0x140B

    //   ...

    insert(GL_COMPRESSED_RED_RGTC1,    "GL_COMPRESSED_RED_RGTC1");    //0x8DBB
    insert(GL_COMPRESSED_SIGNED_RED_RGTC1,"GL_COMPRESSED_RED_RGTC1"); //0x8DBC
    insert(GL_COMPRESSED_RG_RGTC2,     "GL_COMPRESSED_RG_RGTC1");     //0x8DBD
    insert(GL_COMPRESSED_SIGNED_RG_RGTC2,"GL_COMPRESSED_RG_RGTC1");   //0x8DBE
    insert(GL_RG,                      "GL_RG");                      //0x8227
    insert(GL_RG_INTEGER,              "GL_RG_INTEGER");              //0x8228
    insert(GL_R8,                      "GL_R8");                      //0x8229
    insert(GL_R16,                     "GL_R16");                     //0x822A
    insert(GL_RG8,                     "GL_RG8");                     //0x822B
    insert(GL_RG16,                    "GL_RG16");                    //0x822C
    insert(GL_R16F,                    "GL_R16F");                    //0x822D
    insert(GL_R32F,                    "GL_R32F");                    //0x822E
    insert(GL_RG16F,                   "GL_RG16F");                   //0x822F
    insert(GL_RG32F,                   "GL_RG32F");                   //0x8230
    insert(GL_R8I,                     "GL_R8I");                     //0x8231
    insert(GL_R8UI,                    "GL_R8UI");                    //0x8232
    insert(GL_R16I,                    "GL_R16I");                    //0x8233
    insert(GL_R16UI,                   "GL_R16UI");                   //0x8234
    insert(GL_R32I,                    "GL_R32I");                    //0x8235
    insert(GL_R32UI,                   "GL_R32UI");                   //0x8236
    insert(GL_RG8I,                    "GL_RG8I");                    //0x8237
    insert(GL_RG8UI,                   "GL_RG8UI");                   //0x8238
    insert(GL_RG16I,                   "GL_RG16I");                   //0x8239
    insert(GL_RG16UI,                  "GL_RG16UI");                  //0x823A
    insert(GL_RG32I,                   "GL_RG16I");                   //0x823B
    insert(GL_RG32UI,                  "GL_RG16UI");                  //0x823C


    //   ...


    insert(GL_R8_SNORM,                "GL_R8_SNORM");                //0x8F94
    insert(GL_RG8_SNORM,               "GL_RG8_SNORM");               //0x8F95
    insert(GL_RGB8_SNORM,              "GL_RGB8_SNORM");              //0x8F96
    insert(GL_RGBA8_SNORM,             "GL_RGBA8_SNORM");             //0x8F97
    insert(GL_R16_SNORM,               "GL_R16_SNORM");               //0x8F98
    insert(GL_RG16_SNORM,              "GL_RG16_SNORM");              //0x8F99
    insert(GL_RGB16_SNORM,             "GL_RGB16_SNORM");             //0x8F9A
    insert(GL_RGBA16_SNORM,            "GL_RGBA16_SNORM");            //0x8F9B
    insert(GL_SIGNED_NORMALIZED,       "GL_SIGNED_NORMALIZED");       //0x8F9c

    // ...

    insert(GL_SYNC_GPU_COMMANDS_COMPLETE,"GL_SYNC_GPU_COMMANDS_COMPLETE");//0x9117
    insert(GL_UNSIGNALED,              "GL_UNSIGNALED");              //0x9118
    insert(GL_SIGNALED,                "GL_SIGNALED");                //0x9119
    insert(GL_ALREADY_SIGNALED,        "GL_ALREADY_SIGNALED");        //0x911A
    insert(GL_TIMEOUT_EXPIRED,         "GL_TIMEOUT_EXPIRED");         //0x911B
    insert(GL_CONDITION_SATISFIED,     "GL_CONDITION_SATISFIED");     //0x911C
    insert(GL_WAIT_FAILED,             "GL_WAIT_FAILED");             //0x911D

    //   ...

    insert(GL_RGB10_A2UI,              "GL_RGB10_A2UI");              //0x906F

    //   ...

    insert(GL_INT_2_10_10_10_REV,      "GL_INT_2_10_10_10_REV");      //0x8D9F

    //   ...

    insert(GL_DOUBLE_VEC2,             "GL_DOUBLE_VEC2");             //0x8FFC
    insert(GL_DOUBLE_VEC3,             "GL_DOUBLE_VEC3");             //0x8FFD
    insert(GL_DOUBLE_VEC4,             "GL_DOUBLE_VEC4");             //0x8FFE
    insert(GL_DOUBLE_MAT2,             "GL_DOUBLE_MAT2");             //0x8F46
    insert(GL_DOUBLE_MAT3,             "GL_DOUBLE_MAT3");             //0x8F47
    insert(GL_DOUBLE_MAT4,             "GL_DOUBLE_MAT4");             //0x8F48

    insert(GL_DOUBLE_MAT2x3,           "GL_DOUBLE_MAT2x3");           //0x8F49
    insert(GL_DOUBLE_MAT2x4,           "GL_DOUBLE_MAT2x4");           //0x8F4A
    insert(GL_DOUBLE_MAT3x2,           "GL_DOUBLE_MAT3x2");           //0x8F4B
    insert(GL_DOUBLE_MAT3x4,           "GL_DOUBLE_MAT3x4");           //0x8F4C
    insert(GL_DOUBLE_MAT4x2,           "GL_DOUBLE_MAT4x2");           //0x8F4D
    insert(GL_DOUBLE_MAT4x3,           "GL_DOUBLE_MAT4x3");           //0x8F4E

    //    ...

    insert(GL_RGB565,                  "GL_RGB565");                  //0x8D62


    

    
    insert(GL_DEPTH_STENCIL,           "GL_DEPTH_STENCIL");           //0x84F9



    /////////////////////////////////////////////////////
    //  Requires OpenGL 4.6 or newer below this point  //  
    /////////////////////////////////////////////////////
    //The following macro should be defined in 'glad.h' if using OpenGL 4.6
#ifdef GL_VERSION_4_6
    
    //    ...

    //These appear in glad.h at around line 4516 
    insert(GL_ALPHA4_EXT,              "GL_ALPHA4_EXT");              //0x803B
    insert(GL_ALPHA8_EXT,              "GL_ALPHA8_EXT");              //0x803C
    insert(GL_ALPHA12_EXT,             "GL_ALPHA12_EXT");             //0x803D
    insert(GL_ALPHA16_EXT,             "GL_ALPHA16_EXT");             //0x803E
    insert(GL_LUMINANCE4_EXT,          "GL_LUMINANCE4_EXT");          //0x803F
    insert(GL_LUMINANCE8_EXT,          "GL_LUMINANCE8_EXT");          //0x8040
    insert(GL_LUMINANCE12_EXT,         "GL_LUMINANCE12_EXT");         //0x8041
    insert(GL_LUMINANCE16_EXT,         "GL_LUMINANCE16_EXT");         //0x8042
    insert(GL_LUMINANCE4_ALPHA4_EXT,   "GL_LUMINANCE4_ALPHA4_EXT");   //0x8043    
    insert(GL_LUMINANCE6_ALPHA2_EXT,   "GL_LUMINANCE6_ALPHA2_EXT");   //0x8044
    insert(GL_LUMINANCE8_ALPHA8_EXT,   "GL_LUMINANCE8_ALPHA8_EXT");   //0x8045
    insert(GL_LUMINANCE12_ALPHA4_EXT,  "GL_LUMINANCE12_ALPHA4_EXT");  //0x8046
    insert(GL_LUMINANCE12_ALPHA12_EXT, "GL_LUMINANCE12_ALPHA12_EXT"); //0x8047
    insert(GL_LUMINANCE16_ALPHA16_EXT, "GL_LUMINANCE16_ALPHA16_EXT"); //0x8048
    insert(GL_INTENSITY_EXT,           "GL_INTENSITY_EXT");           //0x8049
    insert(GL_INTENSITY4_EXT,          "GL_INTENSITY4_EXT");          //0x804A
    insert(GL_INTENSITY8_EXT,          "GL_INTENSITY8_EXT");          //0x804B
    insert(GL_INTENSITY12_EXT,         "GL_INTENSITY12_EXT");         //0x804C
    insert(GL_INTENSITY16_EXT,         "GL_INTENSITY16_EXT");         //0x804D
    insert(GL_RGB2_EXT,                "GL_RGB2_EXT");                //0x804E
    
    //Apparently OpenGL decided to reuse all of the following enum values as
    //they had before, the only difference being the macro adding an "_EXT" to
    //the end. Thus all of these following entries already appear in the map.
    //This happens again in several other places below within this 4.6-specific
    //section.

    //insert(GL_RGB4_EXT,              "GL_RGB4_EXT");                //0x804F
    //insert(GL_RGB5_EXT,              "GL_RGB5_EXT");                //0x8050
    //insert(GL_RGB8_EXT,              "GL_RGB8_EXT");                //0x8051
    //insert(GL_RGB10_EXT,             "GL_RGB10_EXT");               //0x8052
    //insert(GL_RGB12_EXT,             "GL_RGB12_EXT");               //0x8053
    //insert(GL_RGB16_EXT,             "GL_RGB16_EXT");               //0x8054
    //insert(GL_RGBA2_EXT,             "GL_RGBA2_EXT");               //0x8055
    //insert(GL_RGBA4_EXT,             "GL_RGBA4_EXT");               //0x8056
    //insert(GL_RGB5_A1_EXT,           "GL_RGB5_A1_EXT");             //0x8057
    //insert(GL_RGBA8_EXT,             "GL_RGBA8_EXT");               //0x8058
    //insert(GL_RGB10_A2_EXT,          "GL_RGB10_A2_EXT");            //0x8059
    //insert(GL_RGBA12_EXT,            "GL_RGBA12_EXT");              //0x805A
    //insert(GL_RGBA16_EXT,            "GL_RGBA16_EXT");              //0x805B

    //   ...

    
    insert(GL_TEXTURE_TOO_LARGE_EXT,   "GL_TEXTURE_TOO_LARGE_EXT");   //0x8065

    //   ...               /* These Names are too long for the pattern... */  

    insert(GL_COMPRESSED_LUMINANCE_LATC1_EXT,
                                       "GL_COMPRESSED_LUMINANCE_LATC1_EXT"); //0x8C70
    insert(GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT,
                                "GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT"); //0x8C71
    insert(GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,
                                 "GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT"); //0x8C72
    insert(GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT,
                          "GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT"); //0x8C73
    //insert(GL_COMPRESSED_RED_RGTC1_EXT,
    //                                   "GL_COMPRESSED_RED_RGTC1_EXT");     //0x8DBB
    //insert(GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,
    //                                 "GL_COMPRESSED_SIGNED_RED_RGTC1_EXT");//0x8DBC
    //insert(GL_COMPRESSED_RED_GREEN_RGTC2_EXT, 
    //                                  "GL_COMPRESSED_RED_GREEN_RGTC2_EXT");//0x8DBD
    //insert(GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, 
    //                           "GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT");//0x8DBE
    insert(GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
                                          "GL_COMPRESSED_RGB_S3TC_DXT1_EXT");//0x83F0
    insert(GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
                                         "GL_COMPRESSED_RGBA_S3TC_DXT1_EXT");//0x83F1
    insert(GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
                                         "GL_COMPRESSED_RGBA_S3TC_DXT3_EXT");//0x83F2
    insert(GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
                                         "GL_COMPRESSED_RGBA_S3TC_DXT5_EXT");//0x83F3

    //     ...

    //insert(GL_SRGB_EXT,                "GL_SRGB_EXT");              //0x8C40
    //insert(GL_SRGB8_EXT,               "GL_SRGB8_EXT");             //0x8C41
    //insert(GL_SRGB_ALPHA_EXT,          "GL_SRGB_ALPHA_EXT");        //0x8C42
    //insert(GL_SRGB8_ALPHA8_EXT,        "GL_SRGB8_ALPHA8_EXT");      //0x8C43
    insert(GL_SLUMINANCE_ALPHA_EXT,    "GL_SLUMINANCE_ALPHA_EXT");    //0x8C44
    insert(GL_SLUMINANCE8_ALPHA8_EXT,  "GL_SLUMINANCE8_ALPHA8_EXT");  //0x8C45
    insert(GL_SLUMINANCE_EXT,          "GL_SLUMINANCE_EXT");          //0x8C46
    insert(GL_SLUMINANCE8_EXT,         "GL_SLUMINANCE8_EXT");         //0x8C47
    //insert(GL_COMPRESSED_SRGB_EXT,     "GL_COMPRESSED_SRGB_EXT");   //0x8C48
    //insert(GL_COMPRESSED_SRGB_ALPHA_EXT,
    //                                "GL_COMPRESSED_SRGB_ALPHA_EXT");//0x8C49
    insert(GL_COMPRESSED_SLUMINANCE_EXT,
                                      "GL_COMPRESSED_SLUMINANCE_EXT");//0x8C4A
    insert(GL_COMPRESSED_SLUMINANCE_ALPHA_EXT,
                                "GL_COMPRESSED_SLUMINANCE_ALPHA_EXT");//0x8C4B
    insert(GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
                                  "GL_COMPRESSED_SRGB_S3TC_DXT1_EXT");//0x8C4C
    insert(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
                            "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT");//0x8C4D
    insert(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
                            "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT");//0x8C4E
    insert(GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
                            "GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT");//0x8C4F
#endif //GL_VERSION_4_6

}






//Old leftover Code. I thought perhaps it might once again have a usage
//since it cherry-picks special sets of values
#if 0


std::string internalFormatEnumToString(GLenum format) noexcept {
    std::string name;
    switch (format) {
    default:
        name = unknownFormatName;
        break;
    case GL_R8:
        name = "GL_R8";
        break;
    case GL_R8_SNORM:
        name = "GL_R8_SNORM";
        break;
    case GL_R16:
        name = "GL_R16";
        break;
    case GL_R16_SNORM:
        name = "GL_R16_SNORM";
        break;
    case GL_R3_G3_B2:
        name = "GL_R3_G3_B2";
        break;
    case GL_RGB4:
        name = "GL_RGB4";
        break;
    case GL_RGB5:
        name = "GL_RGB5";
        break;
    case GL_RGB565:
        name = "GL_RGB565";
        break;
    case GL_RGB8:
        name = "GL_RGB8";
        break;
    case GL_RGB8_SNORM:
        name = "GL_RGB8_SNORM";
        break;
    case GL_RGB10:
        name = "GL_RGB10";
        break;
    case GL_RGB12:
        name = "GL_RGB12";
        break;
    case GL_RGB16:
        name = "GL_RGB16";
        break;
    case GL_RGB16_SNORM:
        name = "GL_RGB16_SNORM";
        break;
    case GL_RGBA2:
        name = "GL_RGBA2";
        break;
    case GL_RGBA4:
        name = "GL_RGBA4";
        break;
    case GL_RGB5_A1:
        name = "GL_RGB5_A1";
        break;
    case GL_RGBA8:
        name = "GL_RGBA8";
        break;
    case GL_RGBA8_SNORM:
        name = "GL_RGBA8_SNORM";
        break;
    case GL_RGB10_A2:
        name = "GL_RGB10_A2";
        break;
    case GL_RGB10_A2UI:
        name = "GL_RGB10_A2UI";
        break;
    case GL_RGBA12:
        name = "GL_RGBA12";
        break;
    case GL_RGBA16:
        name = "GL_RGBA16";
        break;
    case GL_RGBA16_SNORM:
        name = "GL_RGBA16_SNORM";
        break;
    case GL_SRGB8:
        name = "GL_SRGB8";
        break;
    case GL_SRGB8_ALPHA8:
        name = "GL_SRGB8_ALPHA8";
        break;
    case GL_R16F:
        name = "GL_R16F";
        break;
    case GL_RG16F:
        name = "GL_RG16F";
        break;
    case GL_RGB16F:
        name = "GL_RGB16F";
        break;
    case GL_RGBA16F:
        name = "GL_RGBA16F";
        break;
    case GL_R32F:
        name = "GL_R32F";
        break;
    case GL_RG32F:
        name = "GL_RG32F";
        break;
    case GL_RGB32F:
        name = "GL_RGB32F";
        break;
    case GL_RGBA32F:
        name = "GL_RGBA32F";
        break;
    case GL_R11F_G11F_B10F:
        name = "GL_R11F_G11F_B10F";
        break;
    case GL_RGB9_E5:
        name = "GL_RGB9_E5";
        break;
    case GL_R8I:
        name = "GL_R8I";
        break;
    case GL_R8UI:
        name = "GL_R8UI";
        break;
    case GL_R16I:
        name = "GL_R16I";
        break;
    case GL_R16UI:
        name = "GL_R16UI";
        break;
    case GL_R32I:
        name = "GL_R32I";
        break;
    case GL_R32UI:
        name = "GL_R32UI";
        break;
    case GL_RG8I:
        name = "GL_RG8I";
        break;
    case GL_RG8UI:
        name = "GL_RG8UI";
        break;
    case GL_RG16I:
        name = "GL_RG16I";
        break;
    case GL_RG16UI:
        name = "GL_RG16UI";
        break;
    case GL_RG32I:
        name = "GL_RG32I";
        break;
    case GL_RG32UI:
        name = "GL_RG32UI";
        break;
    case GL_RGB8I:
        name = "GL_RGB8I";
        break;
    case GL_RGB8UI:
        name = "GL_RGB8UI";
        break;
    case GL_RGB16I:
        name = "GL_RGB16I";
        break;
    case GL_RGB16UI:
        name = "GL_RGB16UI";
        break;
    case GL_RGB32I:
        name = "GL_RGB32I";
        break;
    case GL_RGB32UI:
        name = "GL_RGB32UI";
        break;
    case GL_RGBA8I:
        name = "GL_RGBA8I";
        break;
    case GL_RGBA8UI:
        name = "GL_RGBA8UI";
        break;
    case GL_RGBA16I:
        name = "GL_RGBA16I";
        break;
    case GL_RGBA16UI:
        name = "GL_RGBA16UI";
        break;
    case GL_RGBA32I:
        name = "GL_RGBA32I";
        break;
    case GL_RGBA32UI:
        name = "GL_RGBA32UI";
        break;
    }
    return name;
}

#ifndef GL_PALETTE4_RGB8_OES
#define GL_PALETTE4_RGB8_OES              0x8B90
#define GL_PALETTE4_RGBA8_OES             0x8B91
#define GL_PALETTE4_R5_G6_B5_OES          0x8B92
#define GL_PALETTE4_RGBA4_OES             0x8B93
#define GL_PALETTE4_RGB5_A1_OES           0x8B94
#define GL_PALETTE8_RGB8_OES              0x8B95
#define GL_PALETTE8_RGBA8_OES             0x8B96
#define GL_PALETTE8_R5_G6_B5_OES          0x8B97
#define GL_PALETTE8_RGBA4_OES             0x8B98
#define GL_PALETTE8_RGB5_A1_OES           0x8B99
#endif 

std::string checkAgainstOtherValues(GLenum hexEnum) noexcept {
    std::string name = unknownFormatName;
    switch (hexEnum) {
    default:
        break;

        //These are older OpenGL 1.0 enums
    case(GL_PALETTE4_RGB8_OES)://           0x8B90
        name = "GL_PALETTE4_RGB8_OES";
        break;
    case(GL_PALETTE4_RGBA8_OES)://          0x8B91
        name = "GL_PALETTE4_RGBA8_OES";
        break;
    case(GL_PALETTE4_R5_G6_B5_OES)://       0x8B92
        name = "GL_PALETTE4__R5_G6_B5_OES";
        break;
    case(GL_PALETTE4_RGBA4_OES)://          0x8B93
        name = "GL_PALETTE4_RGBA4_OES";
        break;
    case (GL_PALETTE4_RGB5_A1_OES)://       0x8B94
        name = "GL_PALETTE4_RGB5_A1_OES";
        break;
    case(GL_PALETTE8_RGB8_OES)://           0x8B95
        name = "GL_PALETTE8_RGB8_OES";
        break;
    case(GL_PALETTE8_RGBA8_OES)://          0x8B96
        name = "GL_PALETTE8_RGBA8_OES";
        break;
    case(GL_PALETTE8_R5_G6_B5_OES)://       0x8B97
        name = "GL_PALETTE8__R5_G6_B5_OES";
        break;
    case(GL_PALETTE8_RGBA4_OES)://          0x8B98
        name = "GL_PALETTE8_RGBA4_OES";
        break;
    case (GL_PALETTE8_RGB5_A1_OES)://       0x8B99
        name = "GL_PALETTE8_RGB5_A1_OES";
        break;
    }

    return name;
}

#endif