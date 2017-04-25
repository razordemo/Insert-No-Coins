/* nolibc.cpp by LightShow */

#ifndef __NO_LIBC__
#define __NO_LIBC__

#include <windows.h>

#pragma comment(linker,"/OPT:NOWIN98")

void * operator new(unsigned int cb)
	{
	if (!cb) cb++;
	return (void*)GlobalAlloc(GPTR,cb);
	}

void operator delete(void * p)
	{
	if(p!=NULL) GlobalFree(p);
	}

void WinMainCRTStartup()
	{
	WinMain(0,0,NULL,0);
	ExitProcess(0);
	}

int rand()
	{
	static int lastrand=3489715357;

	__asm 
		{
		mov		eax,[lastrand]
		mov		edx,eax
		rcr		edx,11
		adc		eax,17
		xor		ax,dx
		imul	eax,edx
		xor		eax,edx
		mov		[lastrand],eax
		and		eax,0xffff
		}	

	return (int)lastrand;
	}

int _fltused;

// visual C++ _ftol
void __declspec(naked) _ftol()
	{
	_asm 
		{
    push	edx
    push	eax
    fistp qword ptr [esp]
    pop		eax
    pop		edx
    ret
		}
	}

// visual C++ _CIpow
void __declspec(naked) _CIpow()
	{
	_asm 
		{
    fxch		st(1)
    fyl2x
    fld			st(0)
    frndint
    fsubr		st(1),st(0)
    fxch		st(1)
    fchs
    f2xm1
    fld1
    faddp		st(1),st(0)
    fscale
    fstp		st(1)
    ret
		}
	}

#endif