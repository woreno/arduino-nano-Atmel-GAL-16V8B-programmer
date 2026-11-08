#ifndef VL_TYPES_H
#define VL_TYPES_H
//////////////////////////////////////////////////////////////////////////////////
// Velociraptor TYPES
// f

// 1. Define os tipos primários W8...SW64 segundo arquitetura
#if ((VL_SYSTEM_BITS==VL_SYSTEM_BITS_32) || (VL_SYSTEM_BITS==VL_SYSTEM_BITS_64))
	// nunca usar o tipo "long" apenas porque
	// i64/i32-windows e i32-linux: 4bytes   
	// i64-linux: 8bytes 
	#define W0		void
	#define W1		bool
	#define W8		unsigned char
	#define SW8		char
	#define W16		unsigned short
	#define SW16	short
	#define W32		unsigned int
	#define SW32	int
	#define W64		unsigned long long 
	#define SW64	long long
#else
	#define W0    void
	#define W1	  bool
	#define SW8   char 
	#define SW16  int 
	#define SW32  long int
	#define W8    unsigned char
	#define W16   unsigned int
	#define W32   unsigned long int
	#define W64   unsigned long long int
#endif

	#define FP32  float
	#define FP64  double

//2. inteiro e floating point padrão
#if (VL_SYSTEM_BITS == VL_SYSTEM_BITS_16)
	#define VL_UINT unsigned short
	#define VL_INT  short
	#define VL_FP   float
	#define VL_SZ	W16
#else
#if (VL_SYSTEM_BITS == VL_SYSTEM_BITS_64)
	#define VL_UINT unsigned long long int
	#define VL_INT  long long int
	#define VL_FP   double
	#define VL_SZ    size_t
	#define VL_PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#else
	#define VL_UINT unsigned int 
	#define VL_INT  int
	#define VL_FP   float
	#define VL_SZ   size_t
#endif
#endif

enum  VL_TYPE
{ 
	VL_TYPE_W0,
	VL_TYPE_W1,
	VL_TYPE_W8,
	VL_TYPE_SW8,
	VL_TYPE_W16,
	VL_TYPE_SW16,
	VL_TYPE_W32,
	VL_TYPE_SW32,
	VL_TYPE_W64,
	VL_TYPE_SW64,
	VL_TYPE_FP32,
	VL_TYPE_FP64
};

#define UNIT_1K 1024
#define UNIT_2K 2048
#define UNIT_4K 4096
#define UNIT_8K 8192
#define UNIT_16K 16384
#define UNIT_32K 32768
#define UNIT_64K 65536
#define UNIT_128K 131072

#define VL_INT_BITS ((sizeof(VL_INT))<<3)
#define VL_UINT_BITS ((sizeof(VL_UINT))<<3)

/*
// virgula fixa
#define VL_FIXED(F_,D_,V_)
					((D_)<<F_)|
					((V_&1)<<(F_-1))|
					(((V_>>1)&1)<<(F_-1))|
					(((V_>>2)&1)<<(F_-2))|
					(((V_>>3)&1)<<(F_-3))|
					
*/

#define VL_TYPE_IS_UNSIGNED(DT_) ((DT_(0)-1)>0) 

//3. virgula flutuante fracao e expoente
	#define FP32_EXP_RAW(X_)   (((X_)>>23) & 7F)    // 8bits
	#define FP32_EXP(X_)       (FP32_EXP_RAW(X_)-(127))
	#define FP32_FRAC(X_)      ((X_) & (0x7FFFFFF)) // 23bits
	//
	#define FP64_EXP_RAW(X_)   (((X_)>>23) & 7FF)	 // 11bits
	#define FP64_EXP(X_)       (FP64_EXP_RAW(X_)-(1023))
	#define FP64_FRAC(X_)      ((X_) & (0xFFFFFFFFFFFFF)) // 52bits

//4. nº de bits e sinal
	#define VL_DT_NBITS(DT)			((sizeof(DT))<<3)
	#define VL_DT_NBITS_1(DT)		(((sizeof(DT))<<3)-1)
	#define VL_SIGN_DT(SC_,V_)		((V_>>VL_DT_NBITS_1(SC_))&1)
	#define VL_SIGN8(X)				((((X))>>7)&1)
	#define VL_SIGN16(X)			((((X))>>15)&1)
	#define VL_SIGN32(X)			((((X))>>31)&1)
	#define VL_SIGN64(X)			(((X)>>63)&1)

//5. restringir valores a um tipo
	#define VL_SAT_W8(A)			MIN(0xFF,MAX(A,0))
	#define VL_SAT_SW8(A)			MIN(127,MAX(A,-128))
	#define VL_SAT_W16(A)			MIN(0xFFFF,MAX(A,0))
	#define VL_SAT_SW16(A)			MIN(32767,MAX(A,-32768))
	#define VL_SAT_W32(A)			MIN(0xFFFFFFFF,MAX(A,0))
	#define VL_SAT_SW32(A)			MIN(2147483647,MAX(A,(-2147483647 - 1)))
	#define VL_SAT_(X_,A)			VL_SAT_##X(A)

//6. manipulação de bits
	#define W8_GETH2L(A7,A6,A5,A4,A3,A2,A1,A0)		 ((A0)|((A1)<<1)|((A2)<<2)|((A3)<<3)|((A4)<<4)|((A5)<<5)|((A6)<<6)|((A7)<<7))    
	#define W8_GETL2H(A0,A1,A2,A3,A4,A5,A6,A7)		 W8_GETH2L((A7),(A6),(A5),(A4),(A3),(A2),(A1),(A0))
	#define W8_GET(A7,A6,A5,A4,A3,A2,A1,A0)		     W8_GETH2L((A7),(A6),(A5),(A4),(A3),(A2),(A1),(A0))
	#define VL_BIT_SET(W, N, V)						 (((W) & (~(1<<(N)))) | ((V)<<(N)) )
	#define VL_BIT_GET(B, N)						 ( ((B)>>(N)) & 1 )
	#define VL_BIT_FLIP(B, N)						 VL_BIT_SET( (B), (N), !VL_BIT_GET((B),(N)) ) 
	#define VL_BITS_FLIP(W, N)						 VL_BIT_SET( (W),(N),(~VL_BIT_GET((W),(N))) )
	#define VL_BITS_GET_REPLACED(V,START,N,NV)		 (((~(((1<<N)-1)<<START)) & V) |    ( NV & ((1<<N)-1) )<<START  )
	#define VL_BITS_REPLACE(V,START,N,NV)			 { V = VL_BIT_GET_REPLACE(V,START,N,NV);}
	//

//7. manipulação por tipo 
	#define W8_BIT(V,START)					(((V)>>(START))&1)			
	#define W8_BIT_SET(W, N, V)				VL_BIT_SET(W, N, V)	
	#define W8_REVERSE(W)					(W8_BIT(W,7)|(W8_BIT(W,6)<<1)|(W8_BIT(W,5)<<2)|(W8_BIT(W,4)<<3)|(W8_BIT(W,3)<<4)|(W8_BIT(W,2)<<5)|(W8_BIT(W,1)<<6)|(W8_BIT(W,0)<<7))
	#define W8_TAKE(V,START,N)				(  (~(0xFF<<N))   & ((V)>>START) )
	#define W8_NIBBLE_PUTINTO_LO(W,N)	    ( ((W)  & 0xF0) | ((N) & 0xF) )
	#define W8_NIBBLE_PUTINTO_HI(W,N)	    ( ((W)  & 0x0F) | ((N) & 0xF) )
	#define W8_INT2BCD(R_)                  (( ((BYTE)(floor((R_)/10.0f)))<<4) + ((BYTE)(R_)%10) )
	#define W8_BCD2INT(R_)                  (  ((R_)&0xF) + (10*(((R_)>>4)&0xF)) )

	#define W8_NIBBLE_GETHI(W)			   ( ((W)>>4) & 0xF )
	#define W8_NIBBLE_GETLO(W)			   ( (W) & 0xF)
	#define W8_NIBBLE_SWAP(W)			   ( (W8_NIBBLE_GETHI(W)) | (W8_NIBBLE_GETLO(W)<<4) )
	#define W8_MAX						   (0xFF)
												   
	#define W16_TAKE(V,START,N)		    (  (~(0xFFFF<<N))   & ((V)>>START) )
	#define W16_GET(L,H)				( ((H)<<(8)) | (L)   )
	#define W16_GETHI(W)				((0xFF00 & (W)) >> 8  )
	#define W16PTR_GETHI_PTR(W)			(((unsigned char*)W)+1) 
	
	#define W16_SWAP_BYTES(X_)			(((X_)<<8)|((X_)>>8))
	#define W16_SETHI(W,H)				{ W = ((0xFF & (W)) | (H<<8) );  }
	#define W16_GETLOW(W)				( 0x00FF & (W))
	#define W16PTR_GETLOW_PTR(W)		((unsigned char*)W) 

	#define W16_NIBBLES_L2H(L0,L1,L2,L3) (((L3)<<12)|((L2)<<8)|((L1)<<4)|(L0))
	#define W16_NIBBLES_H2L(L3,L2,L1,L0) W16_NIBBLES_L2H(L0,L1,L2,L3)
	#define W16_SETLOW(W,L)              { W = ((0xFF00 & (W)) | (L) ); }
	#define W16_LOWHI_EX(W)              { W = ( ((0xFF00 & (W))>>8) | ((0xFF00 & W) << 8) ) }
	#define W16_GET_FLIPBYTES(W)         (((W & 0xFF)<<8) | ((W & 0xFF00)>>8))
	#define W16_MAX						 (0xFFFF)
	#define W16_REVERSE(W)		\
							((W8_BIT(W,15)    )\
							|(W8_BIT(W,14)<< 1)\
							|(W8_BIT(W,13)<< 2)\
							|(W8_BIT(W,12)<< 3)\
							|(W8_BIT(W,11)<< 4)\
							|(W8_BIT(W,10)<< 5)\
							|(W8_BIT(W, 9)<< 6)\
							|(W8_BIT(W, 8)<< 7)\
							|(W8_BIT(W, 7)<< 8)\
							|(W8_BIT(W, 6)<< 9)\
							|(W8_BIT(W, 5)<<10)\
							|(W8_BIT(W, 4)<<11)\
							|(W8_BIT(W, 3)<<12)\
							|(W8_BIT(W, 2)<<13)\
							|(W8_BIT(W, 1)<<14)\
							|(W8_BIT(W, 0)<<15))


	#define W32_TAKE(V,START,N)      ((~(0xFFFFFFFF<<N))   & ((V)>>START) )
	#define W32_GET_B0(W)            ((W) & 0xFF )
	#define W32_GET_B1(W)            (((W)>>8) & 0xFF )
	#define W32_GET_B2(W)            (((W)>>16) & 0xFF )
	#define W32_GET_B3(W)            (((W)>>24) & 0xFF )
	#define W32_GET(L0,L1,L2,L3)     (((L3)<<24) | ((L2)<<16) | ((L1)<<8) | (L0)  )
	#define W32_GET_FLIPBYTES(W)     ((((W) & 0xFF) << 24) | (((W) & 0xFF00) << 8) | (((W) & 0xFF0000) >> 8) | (((W) & 0xFF000000)>>24) )
	#define W32_GETHI(W)             (((W)>>16) & 0xFFFF) 
	#define W32_GETLOW(W)            (0xFFFF & (W))
	#define W32_MAX					 (0xFFFFFFFF)

	#define W32_SWAP_BYTES(X_)\
			((X_)>>24)| \
			(((X_)&0xFF)>>8)|\
			(((X_)&0xFF)<<8)|\
			((X_)<<24)


	#define W64_TAKE(V,START,N)      ((~(0xFFFFFFFFFFFFFFFF<<N))   & ((V)>>START) )
	#define W64_GET_FLIPBYTES(W)                        \
				  (										\
				  (((W) & 0xFF)                << 56) | \
				  (((W) & 0xFF00)              << 40) | \
				  (((W) & 0xFF0000)            << 24) | \
				  (((W) & 0xFF000000)           << 8) | \
				  ((W) >> 56)                         | \
				  (((W) & 0x00FF000000000000 ) >> 40) | \
				  (((W) & 0x0000FF0000000000 ) >> 24) | \
				  (((W) & 0x000000FF00000000 ) >> 8)  	\
				  )										


//8. Caracteres e rastreio
	#define            VL_IS_01(C_)		    ((C_)=='0'||(C_)=='1')
	// 09
	#define            VL_IS_DIGIT(C_)		((C_)>47 && (C_)<58)
	// 09AFaf
	#define			VL_IS_HEXADECIMAL(C_)	(VL_IS_DIGIT(C_) || (C_>=65&&C_<=70) || (C_>=97&&C_<=102) )
	// AZ
	#define	       VL_IS_UPPERALPHA(c)		(((c)>64)&&((c)<91))
	// az
	#define	       VL_IS_LOWERALPHA(c)		(((c)>96)&&((c)<123))
	// azAZ
	#define            VL_IS_ALPHA(C_)		(((C_)>64 && (C_)<91) || ((C_)>96 && (C_)<123))
	// AZaz09	
	#define          VL_IS_ALPHANUM(C_)		(((C_)>47 && (C_)<58) || ((C_)>64 && (C_)<91) || ((C_)>96 && (C_)<123))
	// AZaz09_
	#define     VL_IS_ALFANUMUSCORE(C_)		(((C_)>47 && (C_)<58) || ((C_)>64 && (C_)<91) || ((C_)>96 && (C_)<123) || (C_==95))
	// abaixo de ESPAÇO (32) é considerado WS
	#define            VL_IS_WHITE(C_)		( (C_)<33 )
	#define      VL_IS_STRICTWHITE(C_)		( (C_)==10 || (C_)==13 || (C_)==32 || (C_)==9 )
	#define               VL_IS_WS(CH_)	    VL_IS_WHITE(CH_)
	#define        VL_IS_PRINTABLE(C_)      ((C_)>31 && (C_)<127)
	#define            VL_TO_LOWER(C_)		(VL_IS_ALPHA(C_)?(C_|(0x20)):(C_))
	#define            VL_TO_UPPER(C_)		(VL_IS_ALPHA(C_)?(C_&(0xDF)):(C_))

// VL_SKIP, pára sempre em 0, e

	// fazer undef e definir com as variaveis de L,C, PTR
	#define VL_SKIP_COL
	#define VL_SKIP_LINE
	#define VL_SKIP_PTR
    //
	#define VL_SKIP_LCCNT          { VL_SKIP_COL++; if (*(VL_SKIP_PTR)==10) { VL_SKIP_COL=0; VL_SKIP_LINE++;} }
	// se for necessário undef e definir estas:
	#define VL_SKIP_ISWHITE		   VL_IS_STRICTWHITE(*(VL_SKIP_PTR))
	#define VL_SKIP_OK             ((*(VL_SKIP_PTR))!=0)
	#define VL_SKIP_FAIL
	#define VL_SKIP_FBEFORE
	#define VL_SKIP_FAFTER
	#define VL_SKIP_LOAD           if(!VL_SKIP_OK) { VL_SKIP_FAIL }

	//
	#define        VL_SKIP_FUNCTION { VL_SKIP_FBEFORE VL_SKIP_LOAD VL_SKIP_LCCNT (VL_SKIP_PTR)++; VL_SKIP_FAFTER }
	#define           VL_SKIP_UNTIL(CH_)	VL_SKIP_LOAD while (VL_SKIP_OK && ((*(VL_SKIP_PTR))!=CH_))    VL_SKIP_FUNCTION
	#define          VL_SKIP_DIGITS while (VL_SKIP_OK && VL_IS_DIGIT(*(VL_SKIP_PTR)))        VL_SKIP_FUNCTION
	#define              VL_SKIP_01 while (VL_SKIP_OK && VL_IS_01(*(VL_SKIP_PTR)))        VL_SKIP_FUNCTION
	#define         VL_SKIP_ALFANUM while (VL_SKIP_OK && VL_IS_ALPHANUM(*(VL_SKIP_PTR)))      VL_SKIP_FUNCTION
	#define    VL_SKIP_EXCEPTDQUOTE while (VL_SKIP_OK && ((*(VL_SKIP_PTR))!=34))             VL_SKIP_FUNCTION
	#define   VL_SKIP_ALFANUMUSCORE while (VL_SKIP_OK && VL_IS_ALFANUMUSCORE(*(VL_SKIP_PTR)) VL_SKIP_FUNCTION
	#define           VL_SKIP_WHITE while (VL_SKIP_OK && VL_SKIP_ISWHITE) VL_SKIP_FUNCTION
	#define			      VL_SKIP_1 VL_SKIP_FUNCTION
	#define			   VL_SKIP_NEXT	VL_SKIP_1 VL_SKIP_WHITE
	#define			VL_SKIP_NUM(N_)	{ for (SW32 i=0; i< N_; i++) { if (VL_SKIP_OK) { VL_SKIP_FUNCTION }}}

	
	#define	  VL_HEX_TO_NUM(C_)    (VL_IS_DIGIT(C_)?(C_-'0'):(10+(VL_IS_UPPERALPHA(C_)?C_-'A':C_-'a')))

	// conta as linhas/colunas (windows) para variavel cnt; a usar em "..." em VL_SKIP
	#define            VL_CNTROWCOL(d,cnt)	if ((*d)==13) {cnt[0]++; cnt[1]=-2;}else{ cnt[1]++;}
	#define         VL_CNTROWCOLTAB(d,cnt)	if ((*d)==13) {cnt[0]++; cnt[1]=-2; cnt[2]=0; }else{ cnt[1]++; if ((*d)==9) { cnt[2]++; cnt[1]+=8; } }
	#define        VL_BREAK_IF_ZERO(d)		if ((*(d))==0) break;
	#define        VL_RET_IF_ZERO(d,VAL)	if ((*(d))==0) return(VAL);

	// utf16 (PTR = 16 bits)
	#define VL_UTF16_CHAR_IS_CODE(CH)     (((CH)>=0xD800) && ((CH)<0xE000))
	#define VL_UTF16_CHAR_IS_LOCODE(CH)   ((CH)&0xD800)
	#define VL_UTF16_CHAR_IS_HICODE(CH)   ((CH)&0xDC00)

		// READNUMBER_: nº de bytes a avançar ponteiro PTR_ (W16)
		//              se zero NÃO RECONHECIDO  
	#define VL_UTF16_TO_UNICODE(PTR_, VALUE_, READNUMBER_)						 \
			if (!VL_UTF16_CHAR_IS_CODE(PTR_[0]))								 \
			{ VALUE_ = PTR_[0]; READNUMBER_=1; }								 \
			else if ((VL_UTF16_CHAR_IS_LOCODE(PTR_[0]))&&						 \
				     (VL_UTF16_CHAR_IS_CODE(PTR_[1])))							 \
			{ VALUE_ = ((PTR_[0]&(0x3FFF))<<10)+(PTR_[1]&(0x3FFF)); READNUMBER_=2;}		 \
			else																 \
			{ READNUMBER_ = 0; }

	#define VL_UTF16_STRIP_NUMBERBYTES(PTR_,READNUMBER_)							\
			if (!VL_UTF16_CHAR_IS_CODE(PTR_[0]))									\
			{ READNUMBER_=1; }														\
			else (VL_UTF16_CHAR_IS_LOCODE(PTR_[0])&&VL_UTF16_CHAR_IS_CODE(PTR_[1]))	\
			{ READNUMBER_=2;}else{ READNUMBER_=0; }

	// utf8 (PTR = 8 bits)
	#define VL_UTF8_CHAR_IS_CODE(CH)   ((CH)>127)
	#define VL_UTF8_CHAR_IS_ASCII(CH)  ((CH)<128)
	#define VL_UTF8_IS_EXTRA_BYTE(CH)  (((CH)&0xC0)==0x80)
	#define VL_UTF8_STRIP_NUMBERBYTES(CH)		\
			if (CH<0x80){CH=1;}					\
			else if ((CH>>=3)==0x1E){CH=4;}		\
			else if ((CH>>=1)==0xE){CH=3;}		\
			else if ((CH>>=1)==6){CH=2;}		\
			else { CH=0;}
		// BYTES_: nº de bytes a avançar ponteiro PTR_ (W8)
		//         se zero NÃO RECONHECIDO
	#define VL_UTF8_TO_UNICODE(PTR_, VALUE_, BYTES_)	\
			if (PTR_[0]<0x80)							\
			{											\
				VALUE_ = PTR_[0];						\
				BYTES_ = 1;								\
			}											\
			else										\
			{											\
				W8 CH = PTR_[0];						\
				if ((CH>>=3)==0x1E)						\
				{										\
					BYTES_ = 4;							\
					VALUE_ = ((PTR_[0] & 7)  <<18) | 	\
							 ((PTR_[1] & 63) <<12) | 	\
							 ((PTR_[2] & 63) <<6)  |	\
							 (PTR_[3] & 63);			\
				}else if ((CH>>=1)=0xE)					\
				{										\
					BYTES_ = 3;							\
					VALUE_ = ((PTR_[0] & 15)<<12) | 	\
							 ((PTR_[1] & 63)<<6)  | 	\
							 (PTR_[2] & 63);			\
				}else if ((CH>>=1)==6)					\
				{										\
					BYTES_ = 2;							\
					VALUE_ =  ((PTR_[0]&31)<<6) |		\
							  (PTR_[1]&63);				\
				}else									\
					BYTES_=0;							\
			}



	// BYTES_: nº de bytes a avançar ponteiro PTR_ (UTF8)
	//         se zero NÃO RECONHECIDO
	#define VL_UTF8_FROM_UNICODE(UCHAR_, PTR_, BYTES_, ERR_)\
		if (UCHAR_<0x80)							\
		{											\
			BYTES_=1;								\
			PTR_[0]=UCHAR_; 						\
		}											\
		else if(UCHAR_<0x800)						\
		{											\
			BYTES_=2;								\
			UCHAR_-=0x80;							\
			PTR_[1]= 0x80|(UCHAR_&63);				\
			PTR_[0]= 0xC0|(UCHAR_>>6)&31			\
		}else if(UCHAR_<0x10000)					\
		{											\
			BYTES_=3;								\
			UCHAR_-=0x800;							\
			PTR_[2]= 0x80|(UCHAR_&63);				\
			PTR_[1]= 0x80|((UCHAR_>>6)&63);			\
			PTR_[0]= 0xE0|(UCHAR_>>12)&15;			\
		}else if (UCHAR_<0x101000)					\
		{											\
			BYTES_ = 4;								\
			UCHAR_-=0x10000;						\
			PTR_[3]= 0x80|(UCHAR_&63);				\
			PTR_[2]= 0x80|((UCHAR_>>6)&63);			\
			PTR_[1]= 0x80|((UCHAR_>>12)&63);		\
			PTR_[0]= 0xE0|(UCHAR_>>18)&7;			\
		}											\
			
		

	/*
	#define VL_UTF8_2BYTES_(CSTR_)\
		 (CSTR_[0] & 15)<<12 | (CSTR_[1] & 63)<<6
	#define VL_UTF8_3BYTES_(CODE) \
		 (CSTR_[0] & 15)<<12 | (CSTR_[1] & 63)<<6 | (CSTR_[2] & 63)

	#define VL_VAL_TO

	#define VL_UTF8_STARTCHAR(CH, DSTCH,BYTES_REMAINING) \
					BYTES_REMAINING = (((CH)>>5)==6)? 1: \
									  (((CH)>>4)==0xE)? 2: \
 									  (((CH)>>3)==0x1E)? 3: 4; \
					  DSTCH = (CH) & ((1<<(6-BYTES_REMAINING))-1);
	*/

	
inline W16 UTF8_3bytes_to_UNICODE(SW8* cstr)
{
	return (cstr[0] & 15)<<12 | (cstr[1] & 63)<<6 | (cstr[2] & 63) ;
}
inline W0 UNICODE_to_UTF8_3bytes(W16 n, SW8* utf8str)
{
	unsigned char L = W16_GETLOW(n), H = W16_GETHI(n);
	utf8str[2] = L & 63 | 128;
	utf8str[1] = 128 | (H&15)<<2 | L>>6;
	utf8str[0] = 224| H>>4;
}

// 9. Alinhamento

// dado um numero, alinha-o à régua de M (próxima divisão se sobrar, mesma posição senão)
#define VL_ALIGN_NUMBER(N, M)  ( (((W64)(N))%M)? ((((((W64)(N)))/M ) +1 )*M): N)

// dado um apontador, alinha-o à régua de M,
// com cast ao tipo de dados T
#define VL_ALIGN_TPTR(ptr, T, M)  (( (((W64)(ptr))%M)? (T*)(( ((((W64)(ptr)))/M ) +1 )*M): (T*)ptr))

// dado um tipo de dados T e o seu tamanho M nestas unidades, 
// uma macro se step==-1, então usa o tamanho em bytes, senão
// usa o que está em step
#define VL_ROWBYTESTEP(step_,T,M) ((step_==-1)? (sizeof(T)*M): step_)

// dado um tipo de dados T
// dado um tamanho M no tipo de dados T,
// e N o nº bytes a alinhar,
// devolve o tamanho alinhado ao nº de bytes, em bytes
// pode ser usado em tempo de compilação
#define VL_ALIGNED_STEP(T,M,N) \
		((((M)*sizeof(T))%(N))? ((((M*sizeof(T))/(N))+1)*(N)): ((M)*sizeof(T)) ) 

// dado um tamanho N dado nas unidades em T, 
// alinha-o à regua M e devolve o em bytes
#define VL_ALIGN_SIZE(T,M,N)   VL_ALIGNED_STEP(T,M,N)

// dado um tipo de dados T
// dado um tamanho M no tipo de dados T,
// e N o nº bytes a alinhar,
// devolve o tamanho alinhado ao nº de bytes, mas na unidade
// do tamanho do tipo T (arredondado ao próximo) 
// pode ser usado em tempo de compilação
#define VL_ALIGNED_TYPESTEP(T,M,N) \
		((((M)*sizeof(T))%(N))? (((((M)*sizeof(T))/(N))+1)*(N))/sizeof(T): (M) )  

// dado um tamanho N dado nas unidades em T, 
// alinha-o à regua M e devolve o nas unidades de T
#define VL_ALIGN_TSIZE(T,M,N)   VL_ALIGNED_TYPESTEP(T,M,N)

#define VL_MIN(a_,b_)		((a_) > (b_) ? (b_) : (a_))
#define VL_MIN3(a_,b_,c_)  VL_MIN(VL_MIN(a_,b_),c_)
#define VL_MAX(a_,b_)	((a_) < (b_) ? (b_) : (a_))
#define VL_MAX3(a_,b_,c_)  VL_MAX2(VL_MAX2(a_,b_),c_)
#define	VL_M_ROUND(A)   ((((A)-floor(A))<0.5f)? floor(A):ceil(A))
#define	VL_M_CEIL(A)    ceil(A)
#define	VL_M_FLOOR(A)   floor(A)

#if (VL_SYSTEM_CC == VL_SYSTEM_CC_BORLAND)
	#define  VL_FAR_SPEC	      far
	#define  VL_NEAR_SPEC         near
	#define  VL_OPCODE_EMIT(X)    __emit__(X);
	#define  MK_FP( seg,ofs )    ( (W0 _seg * )( seg ) +( W0 near * )( ofs ))
	#define  MK_DSFAR(OFF_)		  MK_FP(_DS, OFF_)
#else
	#define  VL_FAR_SPEC	 
	#define  VL_NEAR_SPEC  
	#define  VL_OPCODE_EMIT(X)
	#define  MK_FP( seg,ofs )  0
	#define  MK_DSFAR(OFF_)	 0
#endif
#define  FARSEG(X)  ((W16)(((W32)(X))>>16))
#define  FAROFF(X)  ((W16)(((W32)(X))&0xFFFF))
 
#define VL_ISHEX(CH_)					((CH_>='0' && CH_<='9') || (CH_>='A' && CH_<='F')|| (CH_>='a' && CH_<='f'))
#define VL_HEXNIBBLE2DEC(CH_)			((CH_<='9')?(CH_-'0'):10+((CH_&0xDF)-'A'))
#define VL_NIBBLEDEC2HEX(N_)			(((N_)>9)? ('A'+((N_)-10)):('0'+(N_) ))
#define VL_EAT_WS_LR(L_, R_)\
	while(L_!=R_ && *L_<33) { L_++; }\
	while(L_!=R_ && *R_<33) { R_--; }

// "tabuada"
// ...pode dar jeito para optimização
#define VL_MUL1x2  2
#define VL_MUL1x4  4
#define VL_MUL1x8  8
#define VL_MUL1x16 16
#define VL_MUL2x2  4
#define VL_MUL2x4  8
#define VL_MUL2x8  16
#define VL_MUL2x16 32
#define VL_MUL4x1  4
#define VL_MUL4x2  8
#define VL_MUL4x4  16
#define VL_MUL4x8  32
#define VL_MUL4x16 64
#define VL_MUL6x1  6
#define VL_MUL6x2  12
#define VL_MUL6x4  18
#define VL_MUL6x8  32
#define VL_MUL8x1  8
#define VL_MUL8x2  16
#define VL_MUL8x4  32
#define VL_MUL8x8  64
#define VL_MUL8x16 128
#define VL_MUL_(A_,B_)			VL_MUL##A_##x##B_
#define VL_MUL(A_,B_)			VL_MUL_(A_,B_)



// fazendo uma lista:
// #define Lista(X) X(x1,y1) X(x2,y2) X##END(x3,y3)

#define VL_TUPLE_XY(X_,Y_)		X_ Y_,
#define VL_TUPLE_XYEND(X_,Y_)	X_ Y_
#define VL_TUPLE_X(X_,Y_)		X_,
#define VL_TUPLE_XEND(X_,Y_)	X_
#define VL_TUPLE_Y(X_,Y_)		Y_,
#define VL_TUPLE_YEND(X_,Y_)	Y_
	
// Lista(VL_TUPLE_XY) 
// devolve x1 y1, x2 y2, x3 y3
// Lista(VL_TUPLE_X) 
// devolve x1, x2, x3
// Lista(VL_TUPLE_Y) 
// devolve y1, y2, y3

#define VL_ASCII_NUL 0
#define VL_ASCII_SOH 1
#define VL_ASCII_STX 2
#define VL_ASCII_ETX 3
#define VL_ASCII_EOT 4
#define VL_ASCII_ENQ 5
#define VL_ASCII_ACK 6
#define VL_ASCII_BEL 7
#define VL_ASCII_BS  8
#define VL_ASCII_HT 9
#define VL_ASCII_TAB 9
#define VL_ASCII_LF  10
#define VL_ASCII_VT  11
#define VL_ASCII_FF  12
#define VL_ASCII_CR  13
#define VL_ASCII_SO  14
#define VL_ASCII_SI  15
#define VL_ASCII_DLE 16
#define VL_ASCII_DC1 17
#define VL_ASCII_DC2 18
#define VL_ASCII_DC3 19
#define VL_ASCII_DC4 20
#define VL_ASCII_NAK 21
#define VL_ASCII_SYN 22
#define VL_ASCII_ETB 23
#define VL_ASCII_CAN 24
#define VL_ASCII_EM  25
#define VL_ASCII_SUB 26
#define VL_ASCII_ESC 27
#define VL_ASCII_FS  28
#define VL_ASCII_GS  29
#define VL_ASCII_RS  30
#define VL_ASCII_US  31
#define VL_ASCII_SP  32


#ifdef VL_TYPES_FOR_SYSEMU
	#pragma message("- Defines para emulacao: ON -")
#endif

#endif