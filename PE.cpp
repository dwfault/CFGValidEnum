#include "stdafx.h"
#include <locale.h>
#include <vector>

using namespace std;

INT32 g_bitmapBase = 0;
BOOL g_bitmapBaseSet = FALSE;

class CAddress {
public:
	INT32 realAddress;
	BOOL valid;
};

class CSegment {
private:
	INT32 addressBase;
	INT32 bitmapAddress;
	DWORD bitmap;
	vector<CAddress> vectorAddress;
	vector<CAddress>::iterator pd;
	CAddress tmpAddress;
	
	CHAR outputBuffer[256];
	ULONG64 displacement;
	ULONG cb;
public:
	CSegment(INT32 argAddressBase):displacement(0),cb(0)
	{
		if (g_bitmapBaseSet == FALSE)
		{
			return;
		}
		addressBase = argAddressBase;
		bitmapAddress = g_bitmapBase+addressBase*4;				//edx + eax *4
		ReadMemory(bitmapAddress, &bitmap, 4, &cb);				//mov edx,[ ]
		if(bitmap!=0)
			dprintf("Bitmap:0x%p\n", bitmap);
		for (int i = 0; i < 16; i++)
		{
			tmpAddress.realAddress = addressBase * 0x100 + 0x10 * i; 
			tmpAddress.valid = FALSE;
			
			if(i!=0)
				bitmap = bitmap >> 2;
			if((bitmap & 1) == 1)
			{	
				tmpAddress.valid = true;
			}
			vectorAddress.push_back(tmpAddress);
		}//end of for input
		for (pd = vectorAddress.begin(); pd != vectorAddress.end(); pd++)
		{
			if ((*pd).valid == 1)
			{
				GetSymbol((*pd).realAddress, outputBuffer, &displacement);
				dprintf("Valid Target:%p",(*pd).realAddress);
				for(int i=0;i<256;i++)
					dprintf("%c", outputBuffer[i]);
				dprintf("\n");
				//dprintf("+0x%p\n", displacement);
			}
		}//end of for output

	}
};

INT32 parseHexFromString(PCSTR args)
{
	PCSTR pointer = args;
	INT32 result;
	if ((args[0] == '0') && (args[1] == 'x'))
	{
		pointer = &(args[3]);
	}
	sscanf(pointer, "%x", &result);
	return result;
}


HRESULT CALLBACK setbitmapbase(PDEBUG_CLIENT4 Client, PCSTR args)
{
	if (!args || !*args) 
	{
		dprintf("No argument found. A base for bitmap should be given.\n");
		return S_FALSE;
	}

	INIT_API();

	g_bitmapBase = parseHexFromString(args);
	g_bitmapBaseSet = TRUE;

	dprintf("bitmapBase:%p\n", g_bitmapBase);
	EXIT_API();
	return S_OK;
}

HRESULT CALLBACK getvalidenum(PDEBUG_CLIENT4 Client, PCSTR args)
{
	INT32 addressBase = 0;
	INT32 tryAddress = 0;
	INT32 tmpMemory[1] = {0,};
	ULONG cb = 0;
	INIT_API();

	for (addressBase = 0x000001; addressBase <= 0x7FFFFF; addressBase++)
	{
		
		try 
		{	
			tryAddress = addressBase * 0x100;
			//dprintf("tryAddress:%p\n", tryAddress);
			ReadMemory(tryAddress,&tmpMemory,4,&cb);

			CSegment * validateSegment = new CSegment(addressBase);
			delete validateSegment;
		}
		catch (exception ex)
		{
			dprintf("exception\n");
		}
	}
	EXIT_API();
	return S_OK;
}


HRESULT CALLBACK help(PDEBUG_CLIENT4 Client, PCSTR args)
{
	INIT_API();

	dprintf("Help for CFGValidEnum.dll\n"
		">help							= Shows this help.\n"
		">setbitmapbase			= Set base of bitmap.Necessary before getvalidenum.\n"
		"	i.e:	setbitmapbase		04000010\n"
		">getvalidenum			= Get all of the valid call targets.\n"
	);

	EXIT_API();
	return S_OK;
}







