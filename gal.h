#include "gal_defs.h"

struct GAL
{
    GAL_TYPE type;
    W8 id0, id1;
    const SW8 *name;
    W32 fuses;
    W32 pins;
    W32 rows;
    W32 bits;
    W32 uesrow;
    W32 uesfuse;
    W32 uesbits;
    W32 eraserow;
    W32 eraseallrow;
    W32 pesrow;
    W32 pesbits;
    W32 cfgrow;
    W32 *cfg;
    W32 cfgbits;
	W32 secrow;
	W32 *pinsArr;
	W32 pinsArrSz;
	W32 opinAndsNum;
	W32 *opinArr;
	
	SW32 insideUES(W32 num_)
	{
		if (num_>=uesfuse && num_< (uesfuse + (uesbits)))
			return num_-uesfuse;
		return -1;
	}
	SW32 insideCFG(W32 num_)
	{
		for (W32 i=0; i < cfgbits; i++)
			if (cfg[i]==num_) return i ;
		return -1;
	}
};
static GAL gals[]=
{
	 {DEV_UNKNOWN,0, 0,   unknownStr,   0, 0, 0, 0, 0,     
	      0,      0,            0,      0,   0, 0, 0,
		  0,      0,            0,      0,   0, 0, 0},

		//type      //id         //name     //fuses    //pin //rows //bits  //uesrow
	 {ATF_16V8B  ,0x00, 0x00,   "ATF16V8B",   2194,      20,    32,   64,     32,     
	   //uesfuse   	//uesbytes  //eraserow  //pesrow //pesbytes   //pesbits	//cfgrow 				       
	      2056,       8*8,          63,        54,       58,          64,    60,
	    //cfg                 //cfgbits       //secfuse  //pinsArr  //pinsArrSz
		GAL_CFG(16V8AB),      GAL_CFGSZ(16V8AB),     61,      pins16V8,  16,
		//
		8, opins16V8
	 }
};

GAL_TYPE gal_select(SW8* str_)
{
	for (W32 i=0; i<strlen(str_); i++) str_[i] = toupper(str_[i]);
	for (W32 i=1; i< sizeof(GAL_TYPEStrs)/sizeof(SW8*); i++)
	{
		if (!strcmp(str_, GAL_TYPEStrs[0]))
			return (GAL_TYPE)i;
	}
	return DEV_UNKNOWN;
} 

SW32 gal_searchId(SW32 id_, const SW32 *arr_, SW32 arrSz_)
{
	for (SW32 i=0; i< arrSz_; i++)
	{
		if (arr_[i]==id_)
		{
			return i;
		}
	}
	return -1;
}