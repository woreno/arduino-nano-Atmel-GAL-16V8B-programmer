
#define GAL_CFG(X_)  (W32*)(cfg##X_)
#define GAL_CFGSZ(X_) (sizeof(cfg##X_)/sizeof(W32))

SW8 *unknownStr = "<desconhecido>";

W32 pins16V8[]  = {2,1,3,19,4,18,5,17,6,14,7,13,8,12,9,11};
W32 opins16V8[] = {19,18,17,16,15,14,13,12};

W32 cfg16V8AB[]=
{
    2048,2049,2050,2051,
    2193,
    2120,2121,2122,2123,
    2128,2129,2130,2131,2132,2133,2134,2135,2136,2137,2138,2139,2140,2141,2142,2143,2144,2145,2146,2147,2148,2149,2150,2151,2152,2153,2154,2155,2156,2157,2158,2159,2160,2161,2162,2163,2164,2165,2166,2167,2168,2169,2170,2171,2172,2173,2174,2175,2176,2177,2178,2179,2180,2181,2182,2183,2184,2185,2186,2187,2188,2189,2190,2191,
    2124,2125,2126,2127,
    2192,
    2052,2053,2054,2055
};


//////////////////////////////////////////////////////
// tipos suportados

#define DEF_GAL_TYPE(X_) \
	X_(DEV_UNKNOWN) \
	X_(ATF_16V8B)

VL_DEFS1_MKENUM(GAL_TYPE, DEF_GAL_TYPE)
VL_DEFS1_MKSTRS(GAL_TYPEStrs, DEF_GAL_TYPE)

//////////////////////////////////////////////////////
// tipo generico 

#define DEF_PES_TYPE(X_) \
	X_(PES_TYPE_16V8              , 0x0)  \
	X_(PES_TYPE_16V8A_B_C_D_Z_ZD  , 0x1A) \
	X_(PES_TYPE_20V8              , 0x20) \
	X_(PES_TYPE_20V8_A_B_Z        , 0x3A) \
	X_(PES_TYPE_6001              , 0x40) \
	X_(PES_TYPE_6001B             , 0x41) \
	X_(PES_TYPE_6002B             , 0x42) \
	X_(PES_TYPE_22V10             , 0x48) \
	X_(PES_TYPE_22V10_C_Z         , 0x49) \
	X_(PES_TYPE_18V10             , 0x50) \
	X_(PES_TYPE_18V10B            , 0x51) 

VL_DEFS2_MKENUM(PES_TYPE          , DEF_PES_TYPE)
VL_DEFS2_MKENUMARR(PES_TYPEIds    , DEF_PES_TYPE)
VL_DEFS2_MKSTRS(PES_TYPEStrs      , DEF_PES_TYPE)

//////////////////////////////////////////////////////
// fabricante 

#define DEF_PES_MANUF(X_)\
	X_(PES_MANUF_LATTICE,    0xA1) \
	X_(PES_MANUF_NATIONAL,   0x8F) \
	X_(PES_MANUF_SGSTHOMSON, 0x20) \
	X_(PES_MANUF_ATMEL16,    0x86) \
	X_(PES_MANUF_ATMEL22,    0x51)   

VL_DEFS2_MKENUM(PES_MANUFACTER,       DEF_PES_MANUF)
VL_DEFS2_MKENUMARR(PES_MANUFACTERIds, DEF_PES_MANUF)
VL_DEFS2_MKSTRS(PES_MANUFACTERStrs,   DEF_PES_MANUF)

FP32 PES_WRITEPULSESArr[] = { 10.0, 25.0, 50.0, 100.0, 200.0, 400.0 };
SW32 PES_WRITEPULSESArrSz = sizeof(PES_WRITEPULSESArr)/sizeof(FP32);

FP32 PES_ERASEPULSESArr[] = { 1.0,2.0,5.0,10.0,20.0,30.0,40.0,50.0,60.0,70.0,80.0,90.0,100.0,200.0 };
SW32 PES_ERASEPULSESArrSz = sizeof(PES_ERASEPULSESArr)/sizeof(FP32);

FP32 PES_VPPsArr[] = {    5,  5.5,    6,  6.5,    7,   7.5,    8,  8.5,    9,  9.5,
	                     10, 10.5,   11, 11.5,   12,  12.5,   13, 13.5,   14, 14.5, 
						 15, 15.5,   16, 16.5,   17,  17.5,   18, 18.5,   19, 19.5, 
						 20, 20.5};

SW32 PES_VPPsArrSz = sizeof(PES_VPPsArr)/sizeof(FP32);











