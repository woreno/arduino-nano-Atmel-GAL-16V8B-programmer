#include <conio.h>
#include "vl_conf.hpp"
#include "vl_types.hpp"
#define VL_ARRAY_STR2NUM_TEMPLATES
#define VL_ARRAY_INCLUDES_STR2NUM_CVT
#include "vl_array.hpp"
#include "vl_con.hpp"
#include "vl_util.hpp"
#include "vl_cmdline.hpp"
#include "gal.h"
#include "jed.h"
SW8 programTitle[] = "Programador de GALs";
SW8 client_str[] = "gal-moreno-client";
SW8 server_str[] = "gal-moreno-server";
#include "arduino.hpp"
 

//#define WRITE_TEST

#define DEF_MODE(X_) \
	X_(MODE_UNKNOWN,    "desconhecido")\
	X_(MODE_TEST,       "teste entradas/saidas")\
	X_(MODE_PARSE,      "parsar jdec")\
	X_(MODE_ERASE,      "eliminar mapa de fusos AND")\
	X_(MODE_ERASEALL,   "eliminar tudo")\
	X_(MODE_PROTECT,    "proteger")\
	X_(MODE_UNPROTECT,  "desproteger")\
	X_(MODE_READ_FUSES, "ler mapa de fusos AND")\
	X_(MODE_READ_CNF,   "ler CNF")\
	X_(MODE_READ_UES,   "ler UES")\
	X_(MODE_READ_PES,   "ler PES")\
	X_(MODE_READ_SEC,   "ler SEC")\
	X_(MODE_READ_ALL,   "ler tudo")\
	X_(MODE_WRITE,      "escrever jdec")
	
VL_DEFS2S_MKENUM(MODE,     DEF_MODE)
VL_DEFS2S_MKSTRS(MODEStrs, DEF_MODE)
MODE mode = MODE_UNKNOWN;

W16 writeDelay=100, eraseDelay=100;

GAL_TYPE device = ATF_16V8B;
GAL* gal;

SW8* filenameIn, *filenameOut;
JDECstruct jdec, jdecRd;
W1   doTest = false;

#define SEND_READ(ADDR_, SIZE_, BUFF_) \
		{					        \
			TX('R')			        \
			TX16(ADDR_)		        \
			TX16(SIZE_)		        \
			RXN(BUFF_, SIZE_)       \
			tx_ack();               \
			if (rx_ack()) return 1; \
		}
#define SEND_WRITE(ADDR_, SIZE_, DL_, BUFF_) \
		{					        \
			TX('W')			        \
			TX16(DL_)		        \
			TX16(ADDR_)		        \
			TX16(SIZE_)		        \
			TXN(BUFF_, SIZE_)       \
			if (rx_ack()) return 1; \
			tx_ack();               \
		}

W1  turnedOn = false;
W0    gal_reset()
{
	TX('Z')
}
SW32  gal_test_sort(const W0* a_, const W0* b_)
{
	return ((W32*)a_)[0]>((W32*)b_)[0];
}
W0    gal_test()
{

	W8 key = 0;
	printf("\n\n------------- modo de teste ATF16V8B -----------------------");
	printf("\n MUDAR botao PARA CIMA e premir uma tecla para continuar...");
	key = getch();
	TX('T')
	W32 sorted_inPins[] = {3,4,5,6,7,8,9};
	W32 sorted_outPins[] = {12,13,14,15,16,17,18,19};
	W8  inPinsNum = sizeof(sorted_inPins)/sizeof(W32);
	W8  outPinsNum = sizeof(sorted_outPins)/sizeof(W32);
	W8 *values_inPins = new W8[inPinsNum+2];
	W8 *values_outPins = new W8[outPinsNum];
	memset(values_inPins, 0, inPinsNum+2);
	memset(values_outPins, 0, outPinsNum);
	W8 *OE_=values_inPins+inPinsNum+1, *CLK=values_inPins+inPinsNum;
	W8 inPinsNum_, outPinsNum_;
	RX8(inPinsNum_)
	RX8(outPinsNum_)

	while(key!=27)
	{
		VL_SCR_CLEAR
		printf("\n [C 1 CLK:%1u] [O 11 ~OE:%1u]\n", *CLK, *OE_); 
		printf("\n Entradas:\n"); 
		for (W32 i=0; i< inPinsNum; i++) printf(" [%02u: %1u]", sorted_inPins[i], values_inPins[i]); 
		printf("\n Saidas:\n"); 
		for (W32 i=0; i< outPinsNum; i++) printf(" [%02u: %1u]", sorted_outPins[i], values_outPins[i]); 

		key = getch();
		printf("\n ...");
		if (key==27){ TX('E') break; }
		if (key=='O'||key=='o'){ *OE_=!*OE_; goto sendPins;}
		if (key=='C'||key=='c'){ *CLK=!*CLK; goto sendPins;}
		for (W32 i=0; i< inPinsNum; i++)
		{
			if (key == sorted_inPins[i]+'0')
			{	
				values_inPins[i]=!values_inPins[i];
				goto sendPins;
			}
		}	
		continue;
		sendPins:
		TX('T')
		Sleep(100);
		TXN(values_inPins, (inPinsNum+2))
		RXN(values_outPins, outPinsNum)
	}
	delete[] values_inPins;
	delete[] values_outPins; 
}
SW32  gal_protect(MODE mode_)
{
	LOG("a %s...", MODEStrs[mode_]);
	W8 on_ = mode_==MODE_PROTECT?1:0;
	SEND_WRITE(gal->secrow, 1, eraseDelay, &on_)
	return 0;
}
SW32  gal_turn(W32 on_)
{
	if (on_!=0)
	{
		if (turnedOn) return 0;
		LOG("sequencia de inicializacao...");
		TX(on_==1?'1':'2')	
		VL_SLEEP_MS(100);
		if (rx_ack()) return 1;
		tx_ack();
		turnedOn=true;
		return 0;
	}
	if (!turnedOn) return 0;
	LOG("sequencia de termino...");
	TX('0')	
	VL_SLEEP_MS(100);
	if (rx_ack()) return 1;
	tx_ack();       
	VL_SLEEP_MS(400);
	turnedOn=false;
	return 0;
}
SW32  gal_erase(MODE mode_)
{
	LOG("a %s...", MODEStrs[mode_]);
	W8 byte = 1;
	SEND_WRITE(mode_==MODE_ERASE?gal->eraserow:gal->eraseallrow, 1, eraseDelay, &byte)
	return 0;
}
SW32  gal_read(MODE mode_)
{
	W8 buffer[512];
	LOG("a %s...", MODEStrs[mode_]);
	if (mode_==MODE_READ_FUSES)
	{
		for (W16 r=0; r< gal->rows; r++)
		{
			SEND_READ(r,  gal->bits, buffer)
			for (W32 j=0; j< gal->bits; j++)
			{
				W32 fuseNum = j*(gal->rows) + r;
				jdecRd.fuseMap[fuseNum] = buffer[j];
			}
			conProgressCallback(r,gal->rows);
		}
		conProgressCallback(gal->rows,gal->rows);
		jdecRd.fuseMapReady = true;
		return 0;
	}
	switch(mode_)
	{
		case MODE_READ_UES:
			SEND_READ(gal->uesrow, gal->uesbits, jdecRd.ues)
			jdecRd.uesReady = true;
		break;
		case MODE_READ_CNF:
			SEND_READ(gal->cfgrow, gal->cfgbits, jdecRd.cfg)
			jdecRd.cfgReady = true;
		break;
		case MODE_READ_PES:
			SEND_READ(gal->pesrow,gal->pesbits,jdecRd.pes)
			jdecRd.pesReady = true;
		break;
		case MODE_READ_SEC:
			SEND_READ(gal->secrow, 1, jdecRd.sec)
			jdecRd.secReady = true;
		break;	
	}
	return 0;
}
SW32  gal_read_all()
{
	if (gal_turn(1)) return 1;
	for (W32 i=MODE_READ_FUSES; i<= MODE_READ_SEC; i++)
	{
		if (gal_read((MODE)i)) return 1;
	}
	if (gal_turn(0)) return 0;
	return 0;
}
SW32  gal_write()
{
	W8 buffer[512];
	// verificação
	if (gal_read_all()) return 1;

	// escrever report
	if (filenameOut) 
		jdecRd.report("Anterior Leitura", filenameOut); 

	PES pes; 
	jdec.getPESParams(&pes);
	eraseDelay = (SW32)pes.erasePulseDelay; 
	writeDelay = (SW32)pes.writePulseDelay;
	
	if (gal_turn(2)) return 1;
	
	gal_erase(MODE_ERASE);

	#ifdef WRITE_TEST
		LOG("a escrever TESTE...");
		for (W16 r=0; r< gal->rows; r++)
		{
			memset(buffer, 0, 512);
			for (W32 c=r; c< gal->bits; c+=6)
			{
				buffer[c]= 1;
			}
			SEND_WRITE(r, gal->bits, writeDelay, buffer)
			conProgressCallback(r,gal->rows);
		}
		conProgressCallback(gal->rows,gal->rows);
	#else
		LOG("a escrever mapa de fusos AND...");
		for (W16 r=0; r< gal->rows; r++)
		{
			for (W32 j=0; j< gal->bits; j++)
			{
				W32 fuseNum = j*(gal->rows) + r;
				buffer[j] = jdec.fuseMap[fuseNum];
			}
			SEND_WRITE(r, gal->bits, writeDelay, buffer)
			conProgressCallback(r,gal->rows);
		}
		conProgressCallback(gal->rows,gal->rows);
	#endif

	//LOG("a escrever PES lido...");
	//SEND_WRITE(gal->pesrow, gal->pesbits, writeDelay, jdecRd.pes)

	LOG("a escrever CONF...");
	SEND_WRITE(gal->cfgrow, gal->cfgbits, writeDelay, jdec.cfg)

	LOG("a escrever UES...");
	SEND_WRITE(gal->uesrow, gal->uesbits, writeDelay, jdec.ues)
		
	if (gal_turn(0)) return 1;

	// verificação
	jdecRd.clean();
	if (gal_read_all()) return 1;

	SW32 error = 0;
	LOG("a verificar mapa de fusos AND...");
	{ 
		Stream f1 = Stream::asBuffer(jdec.fuseMap, jdec.fuseMapSz),
			   f2 = Stream::asBuffer(jdecRd.fuseMap, jdec.fuseMapSz);
		error|=verifyStreams(f1,f2,jdec.fuseMapSz);
	}
	//LOG("a verificar PES...");
	//{ 
	//	Stream f1 = Stream::asBuffer(jdec.pes, gal->pesbits),
	//		   f2 = Stream::asBuffer(jdecRd.pes, gal->pesbits);
	//	error|=verifyStreams(f1,f2,gal->pesbits);
	//}
	LOG("a verificar UES...");
	{ 
		Stream f1 = Stream::asBuffer(jdec.ues, gal->uesbits),
			   f2 = Stream::asBuffer(jdecRd.ues, gal->uesbits);
		error|=verifyStreams(f1,f2,gal->uesbits);
	}
	LOG("a verificar CFG...");
	{ 
		Stream f1 = Stream::asBuffer(jdec.cfg, gal->cfgbits),
			   f2 = Stream::asBuffer(jdecRd.cfg, gal->cfgbits);
		error|=verifyStreams(f1,f2,gal->cfgbits);
	}
	// escrever report
	if (filenameOut)
	{
		jdec.report("Ficheiro parsado", filenameOut);
		jdecRd.report("Apos Escrita", filenameOut);
	}
	if(error) 
	{
	  LOG("erro na escrita (ESTA LIGADO?)"); 
	  return 1;
	}
	else
	{ LOG("verificao OK"); }


	return 0;
}

///////////////////////////////
W0   help()
{
	printf("\n %s \
		   \n *** LIGAR OS CABOS da PLACA ao CONETOR PEQUENO DA FONTE ATX           *** \
		   \n *** +12V +5V e GROUND                                                 *** \
		   \n *** ANTES DE USAR, confirmar que ARDUINO esta' a piscar continuamente *** \
		   \n opcoes: \
  		    \n  r ................................ le toda a GAL (igual a r=ALL) \
			\n  r={UES|PES|CNF|SEC|ALL} [-fout=X]. le algo da GAL\
			\n  e  ............................... apaga apenas o mapa de fusos \
			\n  ea ............................... apaga toda a GAL \
			\n  unprotect ........................ desproteger \
			\n  protect .......................... protege \
			\n  w -f=X.jed [-fout=X] [-t]..........gravar a GAL \
			\n  p -f=X.jed [-fout=X].............. escreve relatorio do parsing\
			\n  t ................................ modo interativo, testa algumas \
			\n                                     entrada/saidas da (ATF16V8B) \
			\n argumentos: \
			\n   -t .............. no modo de escrita, entra no modo interativo apos escrita \
			\n   -f=X.jed ........ ficheiro JEDEC com o mapa de fusos \
			\n   -fout=X.txt ..... escreve relatorio com conteudo da memoria antes e depois \
			\n opcional a todos: \
 			\n   -com=X ....... porta COM (def. %u)\
			\n   -dev=X ....... um destes dispositivos (def. ATF16V8B):\
			\n", programTitle, comPort);
			for (W32 i=0; i< sizeof(GAL_TYPEStrs)/sizeof(SW8*); i++)
			{
				printf("\n     %s", GAL_TYPEStrs[i]);
			}
} 
SW32 getArgs(SW32 nargs, const SW8 **argVec)
{
	nargs--;
	argVec++;
	FAIL_IF_AND_HELP ((nargs==0), "sem argumentos")
	
	if (getArduinoOps(nargs,argVec)) return 1;
		
	SW8* arg = (SW8*)argVec[0];
	tolower(arg[0]);
	if (!strcmp(arg,"w"))
		mode = MODE_WRITE;
	else if (!strcmp(arg,"r"))
		mode = MODE_TEST;
	else if (!strcmp(arg,"r"))
		mode = MODE_READ_ALL;
	else if (!strcmp(arg,"p"))
		mode = MODE_PARSE;
	else if (!strcmp(arg,"e"))
		mode = MODE_ERASE;
	else if (!strcmp(arg,"ea"))
		mode = MODE_ERASEALL;
	else if (!strcmp(arg,"protect"))
		mode = MODE_PROTECT;
	else if (!strcmp(arg,"unprotect"))
		mode = MODE_UNPROTECT;
	else if (!strncmp(arg,"r=", 2))
	{
		arg+=2;
		if (strcmp(arg,"UES"))        mode = MODE_READ_UES; 
		else if (strcmp(arg,"FUSES")) mode = MODE_READ_FUSES; 
		else if (strcmp(arg,"PES"))   mode = MODE_READ_PES; 
		else if (strcmp(arg,"CNF"))   mode = MODE_READ_CNF; 
		else if (strcmp(arg,"SEC"))   mode = MODE_READ_SEC; 
		else if (strcmp(arg,"ALL"))   mode = MODE_READ_ALL;
		else FAIL("modo leitura invalido")
	}else
		FAIL("modo invalido")

	nargs--;
	argVec++;

	if (getArduinoArgs(nargs,argVec)) return 1;

	for (SW32 i=0; i< nargs; i++)
	{
		arg = (SW8*) argVec[i];
		if (!arg[0]) continue;
		if (!strncmp(arg, "-dev=",5))
		{
			device = gal_select(arg+5);
			FAIL_IF(device=DEV_UNKNOWN, "dispositivo invalido")
		}
		else if ((mode==MODE_WRITE||mode==MODE_PARSE) && !strncmp(arg,"-f=",3))
			filenameIn = arg+=3;
		else if ((mode!=MODE_ERASE && mode!=MODE_ERASEALL) && !strncmp(arg,"-fout=",6))
			filenameOut = arg+=6;
		else if ((mode==MODE_WRITE) && !strcmp(arg,"-t"))
			doTest = true;
		else
			FAIL("argumento invalido")
	}
	return 0;
}
SW32 main(VL_INT nargs, const SW8 **argVec)
{ 
	if (getArgs(nargs, argVec)) 
	{
		if (comDetect) return comPort; 
	}
	printf("\n %s", programTitle);
	gal = gals + device;

	LOG("dispositivo: %s", gal->name);
	jdec.init(gal);
	jdecRd.init(gal);

	if (mode!=MODE_PARSE)
		if (open_port()) goto fail;
	
	//gal_test();

	// criar novo
	if (filenameOut) { FILE* f = fopen(filenameOut,"wb"); fclose(f); }

	/*
	gal_turn(1);
	gal_read_all();
	jdecRd.report("Leitura 1", filenameOut);
	jdecRd.clean();
	gal_turn(0);
	gal_turn(1);
	gal_read_all();
	jdecRd.report("Leitura 2", filenameOut);
	jdecRd.clean();
	gal_turn(0);
	goto success;
	*/

	switch(mode)
	{
		case MODE_PROTECT:
		case MODE_UNPROTECT:
			gal_protect(mode);
		break;
		case MODE_ERASE:
			gal_erase(mode);
		break;
		case MODE_PARSE:
			if (jdec.parse(filenameIn)) goto fail;
			if (jdec.report("Ficheiro parsado", filenameOut)) goto fail;
		break;
		case MODE_WRITE:
			if (jdec.parse(filenameIn)) goto fail;
			if (gal_write()) goto fail;
			if (doTest) gal_test();
		break;
		case MODE_TEST:
			gal_test();
		break;		
		case MODE_READ_FUSES:
		case MODE_READ_UES:
		case MODE_READ_PES:
		case MODE_READ_CNF:
		case MODE_READ_SEC:
			if (gal_turn(1)) goto fail;
			if (gal_read(mode)) goto fail;
			if (gal_turn(1)) goto fail;
			if (filenameOut)
				if (jdecRd.report("Leitura",filenameOut)) goto fail;
		break;
		case MODE_READ_ALL:
			if (gal_read_all()) goto fail;
			if (filenameOut)
				if (jdecRd.report("Leitura", filenameOut)) goto fail;
		break;	
	}
	goto success;
fail:
	LOG("algo falhou");
	return 1;
success:
	LOG("OK");
	gal_turn(0);
	gal_reset();
	return 0;
}



