#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "resource.h"
#include "timer.h"
#include "minifmod.h"

#define DEBUG 0						// debug [0/1]
#define START DEBUG?28:0	// part to start
#define PI 3.14159265358979323846f // pi
#define PID PI/180.0f			// pi ratio
#define CR 1.0f/256.0f		// color ratio
#define SNG DEBUG?0:1			// music flag

Timer *timer;
float timer_global=0;
float timer_global_previous=0;
float timer_fps=0;
float timer_fps_total=0;
float timer_fps_average=0;
float timer_fps_min=32768;
float timer_fps_max=0;
float timer_music=0;
float timer_max=0;
int frame_counter=0;
int loop_counter=DEBUG?1:-1;
bool done=false;

FMUSIC_MODULE *mod;			// music handle
int mod_ord=-1;					// pattern order
int mod_row=-1;					// row number
int mod_prv_row=0;			// previous row number
int mod_time=0;					// time
//float mod_tempo=59.625f;// time for one row
bool mod_play=false;		// flag

HDC				hDC=NULL;			// GDI device context
HGLRC			hRC=NULL;			// rendering context
HWND			hWnd=NULL;		// window handle
HINSTANCE	hInstance;		// instance application

int keys[256];					// keyboard array
int active=true;				// window active flag
bool fullscreen=DEBUG?false:true;	// fullscreen flag
bool pause=false;				// pause flag
float nearplane=0.5f;		// nearplane
float farplane=1000.0f;	// farplane
bool polygon=true;			// polygon mode
int ratio_2d=1;					// 2d ratio
/* fov variable					*/
bool fov_flag=false;		// flag
float fov_base=60;			// base fov angle
float fov=fov_base;			// field of view angle
float fov_angle=0;			// angle
float fov_value=0;			// value
/* window variable			*/
int window_w=800;				// width
int window_h=500;				// height
int screen_w;						// width
int screen_h;						// height
int window_color=32;		// color depth
int window_depth=16;		// depth buffer
/* object variable			*/
float	p_x=0;						// position x
float	p_y=0;						// position y
float	p_z=0;						// position z
float	a_x=0;						// angle x
float	a_y=0;						// angle y
float	a_z=0;						// angle z
float	main_angle;				// main angle
float	main_angle_prv;		// previous main angle
/* color variable				*/
float color_inc=0.025f;	// color incrementation
float base_r=0.25f;			// base r
float base_g=0.35f;			// base g
float base_b=0.3f;			// base b
float bgd_base_r=base_r;// red base value
float bgd_base_g=base_g;// green base value
float bgd_base_b=base_b;// blue base value
float bgd_r=0;					// red value
float bgd_g=0;					// green value
float bgd_b=0;					// blue value
/* fog variable					*/
float fog_color[]={base_r,base_g,base_b,1.0f};	// fog color definition
/* liner variable				*/
bool liner_flag=false;	// flag
int car;								// ascii code
int liner_length;				// text length
int liner_n;						// line number
int liner_max;					// line max length
int liner_line;					// line increment
int liner_i;						// char increment
int liner_count=0;			// counter
int liner_count_start=0;// counter start
float liner_angle;			// angle
int liner_w;						// width
int liner_h;						// height
float liner_color;			// color increment
int liner_vtx[8];				// vertex array
/* text variable				*/
char *name="Razor 1911 - Insert No Coins";
char *txt_dos="\rLoading and decrunching data...\r\r/\\______  /\\______  ____/\\______     __\r\\____   \\/  \\__   \\/  _ \\____   \\ __/  \\____\r / _/  _/    \\/   /   /  / _/  _// / / / / /\r/  \\   \\  /\\ /   /\\  /  /  \\   \\/ /\\  / / /\r\\__/\\   \\/RTX______\\___/\\__/\\   \\/ / /_/_/\r=====\\___)===\\__)============\\___)=\\/=======\rRAZOR 1911 * SHARPENING THE BLADE SINCE 1985\r\r\rInsert No Coins - FINAL VERSION!\r\r\rhotkeys:\rF1: display debug infos\rF2: wireframe rendering\rF3: enable/disable scanline\rRETURN: reset debug timer\rSPACE: fullscreen/windowed mode\rxxx: hidden part *";
char *txt_info1="\r\r\r We are proud to \r   present you   \r\r\"Insert No Coins\"\r\r  A new release  \r  by Razor 1911  \r                 \r                 ";
char *txt_info2="\r\r\r   - Credits -   \r\r code:       rez \r logo:     ilkke \r music:  dubmood \r\r hugs to bp,hyde \r keops and 4mat! \r\r      - * -      \r                 \r                 ";
char *txt_info3="\r\r\r  - Greetings -  \r\r Ate Bit/Equinox \r Andromeda/Live! \r D-bug/F4CG/TRSi \r Vision  Factory \r TBL/Paradox/Orb \r FRequency/TPOLM \r Melon./Bomb!/FR \r\r      - * -      \r                 \r                 ";
char *txt_info4="\r\r\r  Be oldsk00l!  \r\r  Show us that  \rnamevoting still\r works in 2010! \r\r                \r                ";
char *txt_info5="\r\r\r      - * -      \r\r   You finally   \r   reached the   \r   end of this   \r   small intro   \r\r      - * -      \r                 \r                 ";
char *txt_info6="\r\r\r   Ho, you are   \r   still here?   \r\r   Let it loop   \r  one more time  \r                 \r                 ";
char *txt_info7=" our intro runs better in realtime than on youtube";
char *txt_hidden1="\r\r\r      - * -      \r\r Congratulations!\r\r  You just found \r the hidden part!\r\r      - * -      \r                 \r                 ";
char *txt_hidden2="\r\r\rThanks goes to:    \r\r keops: timer code \r ryg:     kkrunchy \r\r4mat,coda,bubsy for\rfor bpm/sync help! \r                   \r                   ";
char *txt_hidden3="\r\r\r   - Credits -   \r\r code:       rez \r logo:     ilkke \r music:      rez \r\r      - * -      \r                 \r                 ";
char *txt_hidden4="\r\r\r      - * -      \r\r   Lovings to    \r Maali/ShitRow!  \r\r      - * -      \r                 \r                 ";
char *txt_happycube="\r       _____\r      /    /|\r     +----+ |\r     | :D | |\r     +----+/ \rHappycube is Happy!\r                   ";
char *txt=txt_dos;
/* cube variable				*/
bool cube_flag=false;		// flag
int cube_n=16;					// number
float cube_size=2.0f;		// size
float cube_x[256];			// position x
float cube_y[256];			// position y
float cube_z[256];			// position z
float cube_a[256];			// angle
float cube_w=cube_size*0.5f;// width
float cube_h=cube_size*2.0f;// height
float cube_ratio=PID*cube_n;// ratio
float cube_angle=0;			// angle
float cube_vtx[60];			// cube vertex array
float cube_tex[]={0,0.745f,0.25f,0.745f,0.25f,0.75f,0,0.75f,0,0.745f,0,0.75f,0.25f,0.75f,0.25f,0.745f,0.25f,0.735f,0,0.735f,0,0.74f,0.25f,0.74f,0,0.735f,0.25f,0.735f,0.25f,0.74f,0,0.74f,0.25f,0.75f,0.25f,1.0f,0,1.0f,0,0.75f};
float cube_col[]={0,0,0,0,0,0,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0,0,0,0,0,0,0,0,0,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,0,0,0,0,0,0,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f};
float circuit_tex[]={0.75f,0.75f,0.75f,1.0f,0.625f,1.0f,0.625f,0.75f,0.625f,0.75f,0.625f,1.0f,0.5f,1.0f,0.5f,0.75f};
float circuit_vtx[]={cube_w,0,cube_w,cube_w,0,-cube_w,0,0,-cube_w,0,0,cube_w,0,0,cube_w,0,0,-cube_w,-cube_w,0,-cube_w,-cube_w,0,cube_w};
float chipset_vtx[60*9];// chip vertex array
float chipset_tex[40*9];// chip texture array
/* circuit variable			*/
bool circuit_flag=false;// flag
/* logo variable				*/
bool logo_flag=false;		// flag
int logo_w;							// width
int logo_h;							// height
int logo_margin;				// margin
int logo_vtx[8];				// vertex array
float logo_tex[]={0.5f,1.0f,0.4375f,1.0f,0.4375f,0.9375f,0.5f,0.9375f};
/* loop variable				*/
int loop_w;							// width
int loop_h;							// height
int loop_margin;				// margin
int loop_vtx[8];				// vertex array
float loop_tex[]={0.46484375f,0.76953125f,0.25f,0.76953125f,0.25f,0.75f,0.46484375f,0.75f};
/* razor variable				*/
bool razor_flag=false;	// flag
int razor_n=13;					// divide number
int razor_y;						// position y
int razor_w;						// width
int razor_h;						// height
int razor_margin;				// margin
int razor_radius;				// synchro radius
int razor_b_vtx[24];		// back vertex array
float razor_b_col[]={0,0,0,1.0f,0,0,0,1.0f,0.125f,0,0.25f,0.25f,0.125f,0,0.25f,0.25f,0.125f,0,0.25f,0.25f,0.125f,0,0.25f,0.25f,0.25f,0,0.125f,0.25f,0.25f,0,0.125f,0.25f,0.25f,0,0.125f,0.25f,0.25f,0,0.125f,0.25f,0,0,0,1.0f,0,0,0,1.0f};
float razor_vtx[104];		// vertex array
float razor_tex[104];		// texture array
float razor_col[156];		// color array
/* glenz variable				*/
bool glenz_flag=false;	// flag
float glenz_vtx[432];		// vertex array
float glenz_col[576];		// color array
/* intro variable				*/
bool intro_flag=false;	// flag
int intro_n=12;					// number
int intro_i=0;					// counter
float intro_radius;			// radius
float intro_angle=0;		// angle
/* tunnel variable			*/
bool tunnel_flag=false;	// flag
int tunnel_n1=64;				// depth number
int tunnel_n2=16;				// circle number
float tunnel_angle=0;		// angle
float tunnel_x[64];			// position x
float tunnel_y[64];			// position y
float tunnel_z[64];			// position z
float tunnel_depth=0.25f;// depth
float tunnel_radius=1.5f;// radius
float tunnel_path=4.0f;	// path radius
float tunnel_vtx[]={-0.125f,-0.1875f,0.125f,-0.1875f,0.125f,0.1875f,-0.125f,0.1875f};
float tunnel_tex[]={0.25f,0.78125f,0.3125f,0.78125f,0.3125f,0.875f,0.25f,0.875f};
int star_n=2560;				// star number
float star_x[2560];			// position x
float star_y[2560];			// position y
float star_z[2560];			// position z
float star_angle[2560];	// angle
float star_vtx[]={-0.0375f,-0.0375f,0.0375f,-0.0375f,0.0375f,0.0375f,-0.0375f,0.0375f};
float star_tex[]={0.40625f,0.96875f,0.375f,0.96875f,0.375f,1.0f,0.40625f,1.0f};
/* greeting variable		*/
bool greeting_flag=false;// flag
float disk_vtx[72];			// vertex array
float disk_tex[48];			// texture array
float triforce_vtx[180];// vertex array
float triforce_col[180];// color array
/* copper variable			*/
int copper_n=10;				// number
int copper_h;						// height
int copper_vtx[80];			// vertex array
float copper_col[120];	// color array
/* vote variable				*/
bool vote_flag=false;		// flag
int vote_n1=48;					// number x
int vote_n2=48;					// number y
float vote_w=0.5f;			// space between dot
float vote_vtx[]={-0.325f,-0.325f,0.325f,-0.325f,0.325f,0.325f,-0.325f,0.325f};
/* heart variable				*/
bool heart_flag=false;	// flag
int heart_n=12;					// number
float heart_vtx[]={-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f,-0.5f,-0.5f,0.5f,-0.5f,0.5f,0.5f,-0.5f,0.5f};
float heart_tex[]={0.437f,0.8755f,0.3755f,0.8755f,0.3755f,0.937f,0.437f,0.937f,0.437f,0.8755f,0.3755f,0.8755f,0.3755f,0.937f,0.437f,0.937f};
/* end variable					*/
bool end_flag=false;		// flag
float end_radius=-8.0f;	// radius
/* tekk variable				*/
bool tekk_flag=false;		// flag
int tekk_bar=48;				// bar number
int tekk_n=64;					// polygon per bar
float tekk_w=0.5f;			// space between bar
float tekk_size=0.125f;	// bar size
float tekk_radius=1.5f;	// radius
bool tekk_zoom_flag=false;// zoom flag
float tekk_zoom_angle=0;// zoom angle
float tekk_zoom_value=0;// zoom value
float tekk_vtx[147456];	// vertex array
float tekk_col[147456];	// color array
int youtube_vtx[16];		// vertex array
/* hidden variable			*/
bool hidden=false;			// flag
bool hidden_flag=false;	// flag
float hidden_radius=27.0f;// radius
float hidden_vtx[]={-0.25f,-0.25f,0.25f,-0.25f,0.25f,0.25f,-0.25f,0.25f};
float hidden_tex[]={0.437f,0.812f,0.3755f,0.812f,0.3755f,0.8745f,0.437f,0.8745f};
float gameboy_tex[]={0.25f,0.78125f,0.3125f,0.78125f,0.3125f,0.875f,0.25f,0.875f};
/* flash variable				*/
bool flash_flag=false;	// flag
float flash_angle=0;		// angle
/* dos variable					*/
bool dos_flag=DEBUG?false:true;// flag
float dos_time=DEBUG?0:1.0f;
int dos_w;							// width
int dos_h;							// height
int dos_vtx[8];					// vertex array
int shell_vtx[8];				// vertex array
float shell_tex[]={1.0f,0.9453125f,1.0f,1.0f,0.75f,1.0f,0.75f,0.9453125f};
/* decrunch variable		*/
bool decrunch_flag=false;// flag
int decrunch_h=0;				// height
int decrunch_y=0;				// top
int decrunch_split=0;		// bar split random
int decrunch_split_w;		// bar split w
float decrunch_time=DEBUG?0:dos_time+2.0f;
/* debug variable				*/
bool debug_flag=DEBUG?true:false;// flag
bool debug_test=true;		// test
int debug_w;						// width
int debug_h;						// height
int debug_vtx[8];				// vertex array
/* border variable			*/
bool border1_flag=false;// flag
bool border2_flag=false;// flag
int border_h;						// height
int border_vtx[16];			// vertex array
float border_co1[]={0,0,0,0.125f,0,0,0,0.125f,0,0,0,1.0f,0,0,0,1.0f,0,0,0,1.0f,0,0,0,1.0f,0,0,0,0.125f,0,0,0,0.125f};
float border_co2[]={0.125f,0.125f,0.375f,0.125f,0.125f,0.125f,0.375f,0.125f,0,0,0,1.0f,0,0,0,1.0f,0,0,0,1.0f,0,0,0,1.0f,0.125f,0.125f,0.375f,0.125f,0.125f,0.125f,0.375f,0.125f};
/* scanline variable		*/
bool scanline_flag=true;// flag
int scanline_vtx[8];		// vertex array
float scanline_tex[8];	// texture array
/* synchro variable			*/
bool synchro_flag=false;// flag
float synchro_angle=0;	// angle
float synchro_value=0;	// value
bool sync2_flag=false;	// flag
float sync2_angle=0;		// angle
float sync2_value=0;		// value
float sync2_mul;				// multiplicator
/* beat variable				*/
bool beat_flag=false;		// flag
float beat_angle=0;			// angle
float beat_value=0;			// value
/* fade variable				*/
bool fade_flag=false;		// flag
float fade_angle=0;			// angle
float fade_value=0;			// value
/* move variable				*/
bool move_flag=false;		// flag
float move_angle=0;			// angle
float move_value=0;			// value
/* speed variable				*/
bool speed_flag=false;	// flag
float speed_angle=0;		// angle
float speed_value=0;		// value
int i,j,k;
float x,y,z,w,h;
float r,g,b,c;
float angle,radius,scale;

LRESULT	CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);	// wndProc declaration

static PIXELFORMATDESCRIPTOR pfd=
	{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,											// version number
	PFD_DRAW_TO_WINDOW|			// format must support window
	PFD_SUPPORT_OPENGL|			// format must support openGL
	PFD_DOUBLEBUFFER,				// must support double buffering
	PFD_TYPE_RGBA,					// request an RGBA format
	window_color,						// select our color depth
	0,0,0,0,0,0,						// color bits ignored
	0,											// no alpha buffer
	0,											// shift bit ignored
	0,											// no accumulation buffer
	0,0,0,0,								// accumulation bits ignored
	window_depth,						// z-buffer (depth buffer)
	0,											// no stencil buffer
	0,											// no auxiliary buffer
	PFD_MAIN_PLANE,					// main drawing layer
	0,											// reserved
	0,0,0										// layer masks ignored
	};

#if SNG

	typedef struct 
		{
		int length,pos;
		void *data;
		} MEMFILE;

	MEMFILE *memfile;
	HRSRC	rec;	

	unsigned int memopen(char *name)
		{
		HGLOBAL	handle;
		memfile=(MEMFILE *)GlobalAlloc(GMEM_FIXED|GMEM_ZEROINIT,sizeof(MEMFILE));
		rec=FindResource(NULL,name,RT_RCDATA);
		handle=LoadResource(NULL,rec);
		memfile->data=LockResource(handle);
		memfile->length=SizeofResource(NULL,rec);
		memfile->pos=0;
		return (unsigned int)memfile;
		}

	void memclose(unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		GlobalFree(memfile);
		}

	int memread(void *buffer,int size,unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		if(memfile->pos+size>=memfile->length) size=memfile->length-memfile->pos;
		memcpy(buffer,(char *)memfile->data+memfile->pos,size);
		memfile->pos+=size;	
		return size;
		}

	void memseek(unsigned int handle,int pos,signed char mode)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		if(mode==SEEK_SET) memfile->pos=pos;
		else if(mode==SEEK_CUR) memfile->pos+=pos;
		else if(mode==SEEK_END) memfile->pos=memfile->length+pos;
		if(memfile->pos>memfile->length) memfile->pos=memfile->length;
		}

	int memtell(unsigned int handle)
		{
		MEMFILE *memfile=(MEMFILE *)handle;
		return memfile->pos;
		}

#endif

int load_tex(WORD file,GLint clamp,GLint mipmap)
	{
	HBITMAP hBMP;	// bitmap handle
	BITMAP BMP;		// bitmap structure
	hBMP=(HBITMAP)LoadImage(GetModuleHandle(NULL),MAKEINTRESOURCE(file),IMAGE_BITMAP,0,0,LR_CREATEDIBSECTION);
	if(hBMP)
		{	
		GetObject(hBMP,sizeof(BMP),&BMP);
		glPixelStorei(GL_UNPACK_ALIGNMENT,4);
		glBindTexture(GL_TEXTURE_2D,file);
		glTexImage2D(GL_TEXTURE_2D,0,3,BMP.bmWidth,BMP.bmHeight,0,GL_BGR_EXT,GL_UNSIGNED_BYTE,BMP.bmBits);
		gluBuild2DMipmaps(GL_TEXTURE_2D,3,BMP.bmWidth,BMP.bmHeight,GL_BGR_EXT,GL_UNSIGNED_BYTE,BMP.bmBits);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,mipmap);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,mipmap);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
		DeleteObject(hBMP);
		}
	return 0;
	}

void init3d(GLsizei width,GLsizei height)
	{
	glViewport(0,0,width,height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluPerspective(fov,(float)((float)width/(float)height),nearplane,farplane); // aspect ratio
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	}

void init2d(GLsizei width,GLsizei height)
	{
	glViewport(0,0,width,height);	// reset viewport
	glMatrixMode(GL_PROJECTION);	// select projection matrix
	glLoadIdentity();							// reset projection matrix
	gluOrtho2D(0,width,height,0);	// init orthographic mode
	glMatrixMode(GL_MODELVIEW);		// select modelview matrix
	glLoadIdentity();							// reset modelview matrix
	}

void calc_txt()
	{
	liner_length=strlen(txt);
	liner_count=0;
	liner_angle=main_angle;
	liner_n=0;
	liner_max=0;
	liner_i=0;
	for(i=0;i<liner_length;i++)
		{
		if((byte)txt[i]!=13)
			{
			liner_i++;
			}
		else
			{
			if(liner_i>liner_max) liner_max=liner_i;
			liner_n++;
			liner_i=0;
			}
		}
	if(liner_i>liner_max) liner_max=liner_i;
	fade_value=1.0f;
	}

void flash()
	{
	flash_flag=true;
	flash_angle=main_angle;
	}

void fov_anim()
	{
	fov_flag=true;
	fov_angle=main_angle;
	}

void synchro()
	{
	synchro_flag=true;
	synchro_angle=main_angle;
	}

void sync2(float mul)
	{
	sync2_flag=true;
	sync2_angle=main_angle;
	sync2_mul=mul;
	}

void beat()
	{
	beat_flag=true;
	beat_angle=main_angle;
	}

void fade()
	{
	fade_flag=true;
	fade_angle=main_angle;
	}

void move()
	{
	move_flag=true;
	move_angle=main_angle;
	}

void speed()
	{
	speed_flag=true;
	speed_angle=main_angle;
	}

void tekk_zoom()
	{
	tekk_zoom_flag=true;
	tekk_zoom_angle=main_angle;
	tekk_zoom_value=0;
	}

void pins(int n1,int n2,float x,float y,float z,float a,float b,bool type)
	{
	float k=0.015625f;
	a=type?a:-a;
	b=type?-b:b;
	float vertex[]={x+a,0,z+b,x,0,z+b,x,0,z-b,x+a,0,z-b,x+a,-y,z+b,x+a,0,z+b,x+a,0,z-b,x+a,-y,z-b,x+a,-y,z-b,x+a,0,z-b,x+a,0,z+b,x+a,-y,z+b,x+a,-y*3,z+b*0.5f,x+a,-y,z+b*0.5f,x+a,-y,z-b*0.5f,x+a,-y*3,z-b*0.5f,x+a,-y*3,z-b*0.5f,x+a,-y,z-b*0.5f,x+a,-y,z+b*0.5f,x+a,-y*3,z+b*0.5f};
	float texture[]={0.3125f,0.875f+k+k,0.3125f,0.875f+k,0.25f,0.875f+k,0.25f,0.875f+k+k,0.375f,0.875f+k+k,0.375f,0.875f+k+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.375f,0.875f+k+k,0.375f,0.875f+k+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k,0.3125f,0.875f+k};
	for(i=0;i<60;i++) chipset_vtx[n1+i]=vertex[i];
	for(i=0;i<40;i++) chipset_tex[n2+i]=texture[i];
	}

void chipset(float x,float y,float z,float a,float b)
	{
	float k=0.015625f;
	float vertex[]={x,y,z,-x,y,z,-x,-y,z,x,-y,z,-x,y,-z,x,y,-z,x,-y,-z,-x,-y,-z,-x,y,z,-x,y,-z,-x,-y,-z,-x,-y,z,x,y,-z,x,y,z,x,-y,z,x,-y,-z,x,y,z,x,y,-z,-x,y,-z,-x,y,z};
	float texture[]={0.375f,0.875f+k*3,0.25f,0.875f+k*3,0.25f,0.875f+k*2,0.375f,0.875f+k*2,0.25f,0.875f+k*3,0.375f,0.875f+k*3,0.375f,0.875f+k*2,0.25f,0.875f+k*2,0.375f,0.875f+k*4,0.25f,0.875f+k*4,0.25f,0.875f+k*3,0.375f,0.875f+k*3,0.25f,0.875f+k*4,0.375f,0.875f+k*4,0.375f,0.875f+k*3,0.25f,0.875f+k*3,0.375f,1.0f,0.25f,1.0f,0.25f,0.9375f,0.375f,0.9375f};
	for(i=0;i<60;i++) chipset_vtx[i]=vertex[i];
	for(i=0;i<40;i++) chipset_tex[i]=texture[i];
	pins(60  ,40  ,-x,y,-z*0.75f,a,b,false);
	pins(60*2,40*2,-x,y,-z*0.25f,a,b,false);
	pins(60*3,40*3,-x,y, z*0.25f,a,b,false);
	pins(60*4,40*4,-x,y, z*0.75f,a,b,false);
	pins(60*5,40*5, x,y,-z*0.75f,a,b,true);
	pins(60*6,40*6, x,y,-z*0.25f,a,b,true);
	pins(60*7,40*7, x,y, z*0.25f,a,b,true);
	pins(60*8,40*8, x,y, z*0.75f,a,b,true);
	}

void rectangle(int x,int y,int w,int h)
	{
	//int vertex[]={x+w,y,x,y,x,y+h,x+w,y+h};
	//glVertexPointer(2,GL_INT,0,vertex);
	//glDrawArrays(GL_QUADS,0,4);
	glLoadIdentity();
	glBegin(GL_QUADS);
		glVertex2i(x+w,y  );
		glVertex2i(x  ,y  );
		glVertex2i(x  ,y+h);
		glVertex2i(x+w,y+h);
	glEnd();
	}

void cube(float w,float h)
	{
	float vertex[]={-w,0,w,w,0,w,w,h,w,-w,h,w,-w,0,-w,-w,h,-w,w,h,-w,w,0,-w,-w,0,-w,-w,0,w,-w,h,w,-w,h,-w,w,0,w,w,0,-w,w,h,-w,w,h,w,w,h,w,w,h,-w,-w,h,-w,-w,h,w};
	for(i=0;i<60;i++) cube_vtx[i]=vertex[i];
	}

void copper()
	{
	copper_h=screen_h/copper_n;
	for(i=0;i<copper_n;i++)
		{
		copper_vtx[i*8  ]=screen_w;
		copper_vtx[i*8+1]=copper_h*i;
		copper_vtx[i*8+2]=0;
		copper_vtx[i*8+3]=copper_h*i;
		copper_vtx[i*8+4]=0;
		copper_vtx[i*8+5]=copper_h*(i+1);
		copper_vtx[i*8+6]=screen_w;
		copper_vtx[i*8+7]=copper_h*(i+1);
		}
	}

void disk(float s)
	{
	float w=s/128.0f*122.0f;
	float h=s;
	float d=s/128.0f*6.0f;
	float t=1.5f/128.0f;
	glColor3f(1.0f,1.0f,1.0f);
	float vertex[]={-w,-h,d,w,-h,d,w,h,d,-w,h,d,w,-h,-d,-w,-h,-d,-w,h,-d,w,h,-d,-w,-h,-d,-w,-h,d,-w,h,d,-w,h,-d,w,h,-d,w,h,d,w,-h,d,w,-h,-d,-w,h,d,w,h,d,w,h,-d,-w,h,-d,-w,-h,-d,w,-h,-d,w,-h,d,-w,-h,d};
	float texture[]={0.5f+t,0,1.0f-t,0,1.0f-t,0.5f,0.5f+t,0.5f,1.0f-t,0,0.5f+t,0,0.5f+t,0.5f,1.0f-t,0.5f,0.5f+t*0.5f,0,0.5f+t*0.75f,0,0.5f+t*0.75f,0.5f,0.5f+t*0.5f,0.5f,1.0f-t*0.5f,0.5f,1.0f-t*0.75f,0.5f,1.0f-t*0.75f,0,1.0f-t*0.5f,0,0.5f+t,0.5f+t*1.0f,1.0f-t,0.5f+t*1.0f,1.0f-t,0.5f+t*0.75f,0.5f+t,0.5f+t*0.75f,0.5f+t,0.5f+t*0.5f,1.0f-t,0.5f+t*0.5f,1.0f-t,0.5f+t*0.25f,0.5f+t,0.5f+t*0.25f};
	for(i=0;i<72;i++) disk_vtx[i]=vertex[i];
	for(i=0;i<48;i++) disk_tex[i]=texture[i];
	}

void glenz(float d1,float d2,float r1,float g1,float b1,float a)
	{
	float c1=(r1+g1+b1)/3.0f;
	float c2=c1*0.625f;
	float r2=r1*0.625f;
	float g2=g1*0.625f;
	float b2=b1*0.625f;
	float vertex[]={-d1,d1,d1,d1,d1,d1,0,0,d2,d1,d1,-d1,-d1,d1,-d1,0,0,-d2,d1,-d1,d1,-d1,-d1,d1,0,0,d2,-d1,-d1,-d1,d1,-d1,-d1,0,0,-d2,-d1,-d1,-d1,-d1,d1,-d1,-d2,0,0,d1,-d1,d1,d1,d1,d1,d2,0,0,-d1,-d1,-d1,-d1,-d1,d1,0,-d2,0,-d1,d1,d1,-d1,d1,-d1,0,d2,0,-d1,d1,d1,-d1,-d1,d1,-d2,0,0,d1,d1,-d1,d1,-d1,-d1,d2,0,0,d1,-d1,d1,d1,-d1,-d1,0,-d2,0,d1,d1,-d1,d1,d1,d1,0,d2,0,-d1,d1,-d1,-d1,d1,d1,-d2,0,0,d1,d1,d1,d1,d1,-d1,d2,0,0,-d1,-d1,d1,d1,-d1,d1,0,-d2,0,-d1,d1,-d1,d1,d1,-d1,0,d2,0,-d1,-d1,d1,-d1,-d1,-d1,-d2,0,0,d1,-d1,-d1,d1,-d1,d1,d2,0,0,d1,-d1,-d1,-d1,-d1,-d1,0,-d2,0,d1,d1,d1,-d1,d1,d1,0,d2,0,-d1,-d1,d1,-d1,d1,d1,0,0,d2,d1,-d1,-d1,d1,d1,-d1,0,0,-d2,d1,d1,d1,d1,-d1,d1,0,0,d2,-d1,d1,-d1,-d1,-d1,-d1,0,0,-d2,d1,d1,-d1,d1,d1,d1,d2,0,0,-d1,d1,d1,-d1,d1,-d1,-d2,0,0,d1,-d1,d1,d1,-d1,-d1,d2,0,0,-d1,-d1,-d1,-d1,-d1,d1,-d2,0,0,-d1,-d1,-d1,-d1,d1,-d1,0,0,-d2,d1,-d1,d1,d1,d1,d1,0,0,d2,-d1,-d1,-d1,d1,-d1,-d1,0,-d2,0,d1,d1,-d1,-d1,d1,-d1,0,d2,0,d1,d1,-d1,d1,-d1,-d1,0,0,-d2,-d1,d1,d1,-d1,-d1,d1,0,0,d2,d1,-d1,d1,-d1,-d1,d1,0,-d2,0,-d1,d1,d1,d1,d1,d1,0,d2,0,-d1,d1,-d1,d1,d1,-d1,0,0,-d2,d1,d1,d1,-d1,d1,d1,0,0,d2,d1,-d1,-d1,d1,-d1,d1,0,-d2,0,-d1,d1,-d1,-d1,d1,d1,0,d2,0,d1,-d1,-d1,-d1,-d1,-d1,0,0,-d2,-d1,-d1,d1,d1,-d1,d1,0,0,d2,-d1,-d1,d1,-d1,-d1,-d1,0,-d2,0,d1,d1,d1,d1,d1,-d1,0,d2,0,d1,-d1,-d1,d1,d1,-d1,d2,0,0,-d1,-d1,d1,-d1,d1,d1,-d2,0,0,d1,d1,d1,d1,-d1,d1,d2,0,0,-d1,d1,-d1,-d1,-d1,-d1,-d2,0,0};
	float color[]={c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c1,c1,c1,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,c2,c2,c2,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r1,g1,b1,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a,r2,g2,b2,a};
	for(i=0;i<432;i++) glenz_vtx[i]=vertex[i];
	for(i=0;i<576;i++) glenz_col[i]=color[i];
	}

void triforce(float radius2,float z,float r1,float g1,float b1,float r2,float g2,float b2)
	{
	float radius1=radius2*0.5f;
	float x1=radius1*cosf(PID* 30.0f);
	float y1=radius1*sinf(PID* 30.0f);
	float x2=radius1*cosf(PID*150.0f);
	float y2=radius1*sinf(PID*150.0f);
	float x3=radius1*cosf(PID*270.0f);
	float y3=radius1*sinf(PID*270.0f);
	float x4=radius2*cosf(PID* 90.0f);
	float y4=radius2*sinf(PID* 90.0f);
	float x5=radius2*cosf(PID*210.0f);
	float y5=radius2*sinf(PID*210.0f);
	float x6=radius2*cosf(PID*330.0f);
	float y6=radius2*sinf(PID*330.0f);
	glColor3f(r1,g1,b1);
	float vertex[]={x1,y1,z,x1,y1,z,x4,y4,z,x2,y2,z,x2,y2,z,x2,y2,z,x5,y5,z,x3,y3,z,x3,y3,z,x3,y3,z,x6,y6,z,x1,y1,z,x1,y1,0,x1,y1,0,x4,y4,0,x2,y2,0,x2,y2,0,x2,y2,0,x5,y5,0,x3,y3,0,x3,y3,0,x3,y3,0,x6,y6,0,x1,y1,0,x1,y1,z,x2,y2,z,x2,y2,0,x1,y1,0,x2,y2,z,x4,y4,z,x4,y4,0,x2,y2,0,x4,y4,z,x1,y1,z,x1,y1,0,x4,y4,0,x2,y2,z,x3,y3,z,x3,y3,0,x2,y2,0,x3,y3,z,x5,y5,z,x5,y5,0,x3,y3,0,x5,y5,z,x2,y2,z,x2,y2,0,x5,y5,0,x3,y3,z,x1,y1,z,x1,y1,0,x3,y3,0,x1,y1,z,x6,y6,z,x6,y6,0,x1,y1,0,x6,y6,z,x3,y3,z,x3,y3,0,x6,y6,0};
	float color[]={r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r1,g1,b1,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2,r2,g2,b2};
	for(i=0;i<180;i++)
		{
		triforce_vtx[i]=vertex[i];
		triforce_col[i]=color[i];
		}
	}

int InitGL(void)
	{
	glClearDepth(1.0f);								// set depth buffer
	glDepthMask(GL_TRUE);							// do not write z-buffer
	glEnable(GL_CULL_FACE);						// disable cull face
	glCullFace(GL_BACK);							// don't draw front face
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// fog
	glFogi(GL_FOG_MODE,GL_LINEAR);		// fog mode
	glFogfv(GL_FOG_COLOR,fog_color);	// fog color
	glFogf(GL_FOG_DENSITY,1.0f);			// fog density
	glHint(GL_FOG_HINT,GL_NICEST);		// fog hint value
	glFogf(GL_FOG_START,2.0f);				// fog start depth
	glFogf(GL_FOG_END,24.0f);					// fog end depth
	// load texture
	load_tex(IDB_MAP,GL_REPEAT,GL_NEAREST);
	//glBindTexture(GL_TEXTURE_2D,IDB_MAP);
	// initialize some variable
	timer=new Timer();
	calc_txt();
	glenz(1.0f,1.625f,0.125f,0.625f,0.75f,0.5f);
	cube(cube_w,cube_size*0.75f);
	chipset(cube_w*0.25f,cube_w*0.0625f,cube_w*0.5f,cube_w*0.1f,cube_w*0.0625f);
	disk(2.0f);
	triforce(1.0f,0.125f,0.875f,0.75f,0.25f,0.625f,0.5f,0);
	for(i=0;i<star_n;i++)
		{
		star_angle[i]=(rand()%3600)*0.1f;
		radius=((rand()%1000)*0.01f);
		radius=tunnel_radius*1.125f+((radius<0.0f)?-radius:radius);
		star_x[i]=radius*cosf(star_angle[i]);
		star_y[i]=radius*sinf(star_angle[i]);
		star_z[i]=-(rand()%(int)(tunnel_depth*tunnel_n1*1000))*0.001f;
		}
	y=0;
	k=0;
	float x1,x2,y1,y2;
	for(i=0;i<tekk_bar;i++)
		{
		x1=-tekk_w;
		x2=0;
		y+=tekk_w*2.0f;
		y1=y-tekk_size;
		y2=y+tekk_size;
		for(j=0;j<tekk_n;j++)
			{
			x1+=tekk_w;
			x2+=tekk_w;
			float angle2=1080.0f*PID/tekk_n*j+main_angle*2.0f;
			tekk_vtx[k   ]=x1;
			tekk_vtx[k+ 1]=y2;
			tekk_vtx[k+ 3]=x1;
			tekk_vtx[k+ 4]=y1;
			tekk_vtx[k+ 6]=x2;
			tekk_vtx[k+ 7]=y1;
			tekk_vtx[k+ 9]=x2;
			tekk_vtx[k+10]=y2;
			tekk_vtx[k+12]=x1;
			tekk_vtx[k+13]=y1;
			tekk_vtx[k+15]=x1;
			tekk_vtx[k+16]=y2;
			tekk_vtx[k+18]=x2;
			tekk_vtx[k+19]=y2;
			tekk_vtx[k+21]=x2;
			tekk_vtx[k+22]=y1;
			tekk_vtx[k+24]=x1;
			tekk_vtx[k+25]=y1;
			tekk_vtx[k+27]=x1;
			tekk_vtx[k+28]=y1;
			tekk_vtx[k+29]=0;
			tekk_vtx[k+30]=x2;
			tekk_vtx[k+31]=y1;
			tekk_vtx[k+32]=0;
			tekk_vtx[k+33]=x2;
			tekk_vtx[k+34]=y1;
			tekk_vtx[k+36]=x1;
			tekk_vtx[k+37]=y2;
			tekk_vtx[k+38]=0;
			tekk_vtx[k+39]=x1;
			tekk_vtx[k+40]=y2;
			tekk_vtx[k+42]=x2;
			tekk_vtx[k+43]=y2;
			tekk_vtx[k+45]=x2;
			tekk_vtx[k+46]=y2;
			tekk_vtx[k+47]=0;
			r=0.25f+0.25f*cosf(angle2);
			g=0.25f;
			b=0.25f+0.25f*sinf(angle2);
			tekk_col[k   ]=0.45f;
			tekk_col[k+ 1]=0.5f;
			tekk_col[k+ 2]=0.55f;
			tekk_col[k+ 3]=0.45f;
			tekk_col[k+ 4]=0.5f;
			tekk_col[k+ 5]=0.55f;
			tekk_col[k+ 6]=0.45f;
			tekk_col[k+ 7]=0.5f;
			tekk_col[k+ 8]=0.55f;
			tekk_col[k+ 9]=0.45f;
			tekk_col[k+10]=0.5f;
			tekk_col[k+11]=0.55f;
			tekk_col[k+12]=0.875f;
			tekk_col[k+13]=0;
			tekk_col[k+14]=0;
			tekk_col[k+15]=0.875f;
			tekk_col[k+16]=0;
			tekk_col[k+17]=0;
			tekk_col[k+18]=0.875f;
			tekk_col[k+19]=0;
			tekk_col[k+20]=0;
			tekk_col[k+21]=0.875f;
			tekk_col[k+22]=0;
			tekk_col[k+23]=0;
			tekk_col[k+24]=r;
			tekk_col[k+25]=g;
			tekk_col[k+26]=b;
			tekk_col[k+27]=r;
			tekk_col[k+28]=g;
			tekk_col[k+29]=b;
			tekk_col[k+30]=r;
			tekk_col[k+31]=g;
			tekk_col[k+32]=b;
			tekk_col[k+33]=r;
			tekk_col[k+34]=g;
			tekk_col[k+35]=b;
			tekk_col[k+36]=r;
			tekk_col[k+37]=g;
			tekk_col[k+38]=b;
			tekk_col[k+39]=r;
			tekk_col[k+40]=g;
			tekk_col[k+41]=b;
			tekk_col[k+42]=r;
			tekk_col[k+43]=g;
			tekk_col[k+44]=b;
			tekk_col[k+45]=r;
			tekk_col[k+46]=g;
			tekk_col[k+47]=b;
			k+=48;
			}
		}
	return true;
	}

int DrawGLScene(void) // draw scene
	{
	frame_counter++;
	// synchro
	timer->update();
	timer_global_previous=timer_global;
	timer_global=timer->elapsed;
	if(!pause)
		{
		// compute rotation
		main_angle_prv=main_angle;
		main_angle=timer_global*100.0f*PID;
		}
	timer_fps=1.0f/(timer_global-timer_global_previous);
	timer_fps_total+=timer_fps;
	timer_fps_average=timer_fps_total/frame_counter;
	if(timer_fps<timer_fps_min) timer_fps_min=timer_fps;
	if(timer_fps>timer_fps_max) timer_fps_max=timer_fps;
	// start decrunch
	if(!mod_play&&timer_global>dos_time)
		{
		decrunch_flag=true;
		}
	// start music
	if(!mod_play&&timer_global>decrunch_time)
		{
		dos_flag=false;
		decrunch_flag=false;
		mod_play=true;
		#if SNG
		FMUSIC_PlaySong(mod);
		#endif
		timer_music=timer_global;
		}
	if(mod_play)
		{
		//mod_time=(timer_global-timer_music)*1000.0f;
		mod_time=FMUSIC_GetTime(mod);
		mod_prv_row=mod_row;
		//mod_row=(int)(mod_time/mod_tempo)%64;
		mod_row=FMUSIC_GetRow(mod);
		if(mod_row>mod_prv_row+1) mod_row=mod_prv_row;
		if(mod_row!=mod_prv_row)
			{
			if(!hidden)
				{
				if(mod_row==0)
					{
					timer_max=(timer_global-timer_music)*1000.0f;
					timer_music=timer_global;
					#if !DEBUG
						mod_ord=FMUSIC_GetOrder(mod);
						/*
						mod_ord++;
						if((mod_ord%32==0)&&(mod_ord>0))
							{
							mod_ord=8;
							loop_counter++;
							}
						*/
					#endif
					#if DEBUG
						if(debug_test)
							{
							mod_ord=START;
							debug_test=false;
							}
						else
							{
							mod_ord++;
							}
					#endif
					}
				if(((mod_ord>7&&mod_ord<16)||(mod_ord>17&&mod_ord<24&&mod_ord!=21))&&(mod_row%16==8||mod_row==52)) synchro();
				if((mod_ord==16||mod_ord==17)&&(mod_row==52)) synchro();
				if((mod_ord==21)&&(mod_row==8||mod_row==24||mod_row==56)) synchro();
				if((mod_ord>27&&mod_ord<32)&&(mod_row%8==0)) sync2(1.75f);
				if((loop_counter>0)&&(mod_row%16==0)) beat();
				switch(mod_ord)
					{
					case 0:
						switch(mod_row)
							{
							case 0:
								decrunch_flag=false;
								intro_flag=true;
								glenz_flag=true;
								flash();
								intro_i=1;
								synchro();
								break;
							case  6: intro_i=2; synchro(); break;
							case 12: intro_i=3; synchro(); break;
							case 32: intro_i=4; synchro(); break;
							case 38: intro_i=5; synchro(); break;
							case 44: intro_i=6; synchro(); break;
							}
						break;
					case 1:
						switch(mod_row)
							{
							case  0: intro_i=7; synchro(); break;
							case  6: intro_i=8; synchro(); break;
							case 12: intro_i=9; synchro(); break;
							case 32: intro_i=10; synchro(); break;
							case 38: intro_i=11; synchro(); break;
							case 44: intro_i=12; synchro(); break;
							case 59: move(); break;
							}
						break;
					case 2:
						switch(mod_row)
							{
							case  0: intro_i=13; synchro(); break;
							case  6: intro_i=14; synchro(); break;
							case 12: intro_i=15; synchro(); break;
							case 32: intro_i=16; synchro(); break;
							case 38: intro_i=17; synchro(); break;
							case 44: intro_i=18; synchro(); break;
							}
						break;
					case 3:
						switch(mod_row)
							{
							case  0: intro_i=19; synchro(); break;
							case  6: intro_i=20; synchro(); break;
							case 12: intro_i=21; synchro(); break;
							case 32: speed(); break;
							}
						break;
					case 4:
						if(mod_row==0)
							{
							border1_flag=true;
							border2_flag=false;
							razor_flag=false;
							cube_flag=true;
							cube_angle=main_angle;
							circuit_flag=false;
							intro_flag=false;
							move_flag=false;
							speed_flag=false;
							speed_value=1.0f;
							end_flag=false;
							glenz_flag=false;
							liner_flag=false;
							flash();
							fov_anim();
							bgd_base_r=0.125f;
							bgd_base_g=0.15f;
							bgd_base_b=0.1f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 7:
						if(mod_row==54) speed();
						break;
					case 8:
						if(mod_row==0)
							{
							loop_counter++; 
							border1_flag=false;
							border2_flag=true;
							logo_flag=true;
							razor_flag=true;
							tekk_flag=false;
							cube_flag=true;
							circuit_flag=true;
							glenz_flag=true;
							intro_radius=3.0f;
							liner_flag=true;
							speed_flag=false;
							txt=(loop_counter<1)?txt_info1:txt_info6;
							calc_txt();
							flash();
							if(loop_counter>0) fov_anim();
							bgd_base_r=0;
							bgd_base_g=0.125f;
							bgd_base_b=0.25f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 11:
						if(mod_row==40)
							{
							fade();
							sync2(0.5f);
							}
						break;
					case 12:
						if(mod_row==0)
							{
							border1_flag=true;
							border2_flag=false;
							logo_flag=true;
							razor_flag=false;
							cube_flag=false;
							circuit_flag=false;
							tunnel_flag=true;
							tunnel_angle=main_angle;
							glenz_flag=false;
							liner_flag=true;
							txt=txt_info2;
							calc_txt();
							flash();
							for(i=0;i<tunnel_n1;i++)
								{
								angle=540.0f*PID;
								x=tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f);
								y=tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f);
								tunnel_x[i]=x;
								tunnel_y[i]=y;
								tunnel_z[i]=-tunnel_depth*i;
								}
							bgd_base_r=0.0f;
							bgd_base_g=0.05f;
							bgd_base_b=0.1f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 15:
						switch(mod_row)
							{
							case 40: fade(); break;
							case 48: speed(); break;
							}
						break;
					case 16:
						if(mod_row==0)
							{
							border1_flag=false;
							border2_flag=true;
							logo_flag=true;
							razor_flag=false;
							tunnel_flag=false;
							greeting_flag=true;
							glenz_flag=true;
							intro_radius=1.0f;
							liner_flag=true;
							speed_flag=false;
							txt=txt_info3;
							calc_txt();
							flash();
							bgd_base_r=0.25f;
							bgd_base_g=0.25f;
							bgd_base_b=0.375f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 19:
						if(mod_row==40) fade();
						break;
					case 20:
						if(mod_row==0)
							{
							txt=txt_info4;
							border2_flag=true;
							logo_flag=true;
							razor_flag=true;
							greeting_flag=false;
							vote_flag=true;
							heart_flag=true;
							glenz_flag=false;
							liner_flag=true;
							calc_txt();
							flash();
							sync2(0.5f);
							bgd_base_r=0.25f;
							bgd_base_g=0.125f;
							bgd_base_b=0.25f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 23:
						if(mod_row==40) fade();
						break;
					case 24:
						if(mod_row==0)
							{
							border2_flag=true;
							logo_flag=true;
							razor_flag=false;
							vote_flag=false;
							heart_flag=false;
							glenz_flag=true;
							intro_radius=1.0f;
							end_flag=true;
							liner_flag=true;
							txt=txt_info5;
							calc_txt();
							flash();
							fov_anim();
							bgd_base_r=0.3f;
							bgd_base_g=0.25f;
							bgd_base_b=0.2f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 26:
						switch(mod_row)
							{
							case 0: case 6: case 12: case 32: case 38: case 44: synchro(); break;
							}
						break;
					case 27:
						switch(mod_row)
							{
							case  0: case 6: case 12: case 38: case 44: synchro(); break;
							case 32: synchro(); speed(); break;
							case 40: fade(); break;
							}
						break;
					case 28:
						if(mod_row==0)
							{
							border1_flag=true;
							border2_flag=false;
							logo_flag=true;
							razor_flag=true;
							glenz_flag=false;
							end_flag=false;
							tekk_flag=true;
							liner_flag=false;
							txt=txt_info7;
							calc_txt();
							flash();
							bgd_base_r=0.25f;
							bgd_base_g=0.2f;
							bgd_base_b=0.0f;
							fog_color[0]=bgd_base_r;
							fog_color[1]=bgd_base_g;
							fog_color[2]=bgd_base_b;
							glFogfv(GL_FOG_COLOR,fog_color);
							}
						break;
					case 30:
						if(mod_row==32) tekk_zoom();
						break;
					case 31:
						switch(mod_row)
							{
							case 32: case 48: case 56: case 60: case 62: synchro(); break;
							}
						break;
					}
				}
			else
				{
				if(mod_row==0)
					{
					timer_max=(timer_global-timer_music)*1000.0f;
					timer_music=timer_global;
					mod_ord++; // if(mod_ord>4) mod_ord=0;
					if(mod_ord==0)
						{
						border2_flag=true;
						logo_flag=true;
						razor_flag=true;
						liner_flag=true;
						hidden_flag=true;
						flash();
						bgd_base_r=0;
						bgd_base_g=0;
						bgd_base_b=0;
						fog_color[0]=bgd_base_r;
						fog_color[1]=bgd_base_g;
						fog_color[2]=bgd_base_b;
						glFogfv(GL_FOG_COLOR,fog_color);
						}
					int ord=mod_ord%4;
					switch(ord)
						{
						case 0: txt=txt_hidden1; break;
						case 1: txt=txt_hidden2; break;
						case 2: txt=txt_hidden3; break;
						case 3: txt=txt_hidden4; break;
						}
					calc_txt();
					}
				if(mod_row%8==4||mod_row==60||mod_row==62) synchro();
				if(mod_row==56) fade();
				}
			}
		}
	if(synchro_flag)
		{
		angle=(main_angle-synchro_angle)*2.0f;
		synchro_value=1.0f-sinf(angle);
		if(angle>90.0f*PID)
			{
			synchro_value=0;
			synchro_flag=false;
			}
		}
	if(sync2_flag)
		{
		angle=(main_angle-sync2_angle)*sync2_mul;
		sync2_value=1.0f-sinf(angle);
		if(angle>90.0f*PID)
			{
			sync2_value=0;
			sync2_flag=false;
			}
		}
	if(beat_flag)
		{
		angle=(main_angle-beat_angle)*1.5f;
		beat_value=1.0f-sinf(angle);
		if(angle>90.0f*PID)
			{
			beat_value=0;
			beat_flag=false;
			}
		}
	if(fade_flag)
		{
		angle=(main_angle-fade_angle)*1.0f;
		fade_value=1.0f-sinf(angle);
		if(angle>90.0f*PID)
			{
			fade_value=0;
			fade_flag=false;
			}
		}
	if(move_flag)
		{
		angle=(main_angle-move_angle)*2.0f;
		move_value=1.0f-sinf(angle);
		if(angle>90.0f*PID)
			{
			move_value=0;
			//move_flag=false;
			}
		}
	if(speed_flag)
		{
		angle=(main_angle-speed_angle)*0.45f;
		speed_value=cosf(angle);
		if(angle>90.0f*PID)
			{
			speed_value=0;
			speed_flag=false;
			}
		}
	// clear screen and depth buffer
	fov=fov_base+15.0f*sinf((main_angle-fov_angle)*0.25f);
	if(fov_flag)
		{
		angle=(main_angle-fov_angle)*0.25f;
		fov_value=sinf(angle);
		fov+=fov_value*fov_base-fov_base;
		if(angle>90.0f*PID) fov_flag=false;
		}
	init3d(screen_w,screen_h);
	glClearColor(bgd_base_r,bgd_base_g,bgd_base_b,1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FOG);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	// draw cube
	if(cube_flag)
		{
		glEnableClientState(GL_COLOR_ARRAY);
		glDisable(GL_BLEND);
		angle=cosf((cube_angle-main_angle)*0.125f);
		radius=cube_size-cube_size*angle;
		p_x=0;//4.0f*angle;
		p_y=3.0f-2.0f*angle;
		p_z=-14.0f+2.0f*angle;
		a_x=60.0f+10.0f*angle;
		a_y=-60.0f+(cube_angle-main_angle)*8.0f+speed_value*180.0f;
		a_z=0;
		h=(float)(cube_size*0.1f+(circuit_flag?0.5f-fabs(synchro_value*0.2f*cosf((main_angle-synchro_angle)*8.0f)):0));
		k=0;
		for(i=0;i<cube_n;i++)
			{
			float a=i*cube_ratio;
			x=-(cube_n-1)*cube_size*0.5f+i*cube_size;
			for(j=0;j<cube_n;j++)
				{
				cube_x[k]=x;
				float b=j*cube_ratio;
				cube_a[k]=((cube_angle-main_angle)*0.5f+a+b);
				cube_y[k]=cube_size+radius*cosf(cube_a[k]);
				cube_z[k]=-(cube_n-1)*cube_size*0.5f+j*cube_size;
				k++;
				}
			}
		glVertexPointer(3,GL_FLOAT,0,cube_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,cube_tex);
		for(i=0;i<k;i++)
			{
			glLoadIdentity();
			glTranslatef(p_x,p_y,p_z);
			glRotatef(a_x,1.0f,0,0);
			glRotatef(a_y,0,1.0f,0);
			glTranslatef(cube_x[i],cube_y[i],cube_z[i]);
			glColorPointer(3,GL_FLOAT,0,cube_col);
			glDrawArrays(GL_QUADS,0,20);
			}
		// draw circuit
		if(circuit_flag)
			{
			glVertexPointer(3,GL_FLOAT,0,circuit_vtx);
			glTexCoordPointer(2,GL_FLOAT,0,circuit_tex);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE,GL_DST_ALPHA);
			for(i=0;i<k;i++)
				{
				glLoadIdentity();
				glTranslatef(p_x,p_y,p_z);
				glRotatef(a_x,1.0f,0,0);
				glRotatef(a_y,0,1.0f,0);
				glTranslatef(cube_x[i],cube_y[i]+cube_size*0.75f+0.01f,cube_z[i]);
				float c1=0.5f+0.5f*cosf(cube_a[i]*4.0f);
				float c2=0.5f+0.5f*sinf(cube_a[i]*4.0f);
				float circuit_col[]={c1,0,c2,c2,c1,0,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,c1,-c2,-c2,0,c2,c1,c1,0,c2};
				glColorPointer(3,GL_FLOAT,0,circuit_col);
				glDrawArrays(GL_QUADS,0,8);
				}
			glDisable(GL_BLEND);
			}
		glDisableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,chipset_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,chipset_tex);
		glColor3f(1.0f,1.0f,1.0f);
		for(i=0;i<k;i++)
			{
			glLoadIdentity();
			glTranslatef(p_x,p_y,p_z);
			glRotatef(a_x,1.0f,0,0);
			glRotatef(a_y,0,1.0f,0);
			glTranslatef(cube_x[i],cube_y[i]+cube_size*0.75f+0.01f+h,cube_z[i]);
			glDrawArrays(GL_QUADS,0,180);
			}
		glEnableClientState(GL_COLOR_ARRAY);
		glEnable(GL_BLEND);
		glDisableClientState(GL_COLOR_ARRAY);
		}
	if(intro_flag||glenz_flag||tunnel_flag||greeting_flag||vote_flag||tekk_flag||end_flag||hidden_flag)
		{
		fov=fov_base+((!greeting_flag&&!vote_flag&&!end_flag)?30.0f:10.0f);
		init3d(screen_w,screen_h);
		}
	// draw intro
	if(intro_flag)
		{
		glDisable(GL_BLEND);
		glColor3f(1.0f,1.0f,1.0f);
		intro_radius=3.0f;
		if(move_flag) intro_radius=2.25f+0.75f*move_value;
		angle=main_angle*4.0f;
		x=0.5f;
		y=0.125f*cosf(main_angle*0.5f);
		z=-5.0f;
		if(speed_flag)
			{
			angle+=-270.0f+270.0f*speed_value;
			intro_radius+=3.25f-3.25f*speed_value;
			x-=-0.25f+0.25f*speed_value;
			y+=0.75f-0.75f*speed_value;
			}
		for(i=0;i<intro_i;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,z);
			glRotatef(90.0f,x,y,-0.125f);
			glRotatef(360.0f/intro_n*i+angle,0,1.0f,0);
			glTranslatef(intro_radius,0,0);
			glRotatef(90.0f,0.5f,-0.25f,0.75f);
			glVertexPointer(3,GL_FLOAT,0,chipset_vtx);
			glTexCoordPointer(2,GL_FLOAT,0,chipset_tex);
			glDrawArrays(GL_QUADS,0,180);
			}
		for(i=intro_n;i<intro_i;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,z);
			glRotatef(90.0f,x,y,-0.125f);
			glRotatef(360.0f/(intro_n-3)*i+angle,0,1.0f,0);
			glTranslatef(intro_radius+1.0f,0,0);
			glRotatef(90.0f,0.5f,-0.25f,0.75f);
			glVertexPointer(3,GL_FLOAT,0,chipset_vtx);
			glTexCoordPointer(2,GL_FLOAT,0,chipset_tex);
			glDrawArrays(GL_QUADS,0,180);
			}
		glEnable(GL_BLEND);
		}
	glDisable(GL_DEPTH_TEST);
	// draw tunnel
	if(tunnel_flag)
		{
		glBlendFunc(GL_SRC_COLOR,GL_ONE);
		glVertexPointer(2,GL_FLOAT,0,tunnel_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,tunnel_tex);
		angle=(main_angle-tunnel_angle)+540.0f*PID;
		x=tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f);
		y=tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f);
		angle-=590.0f*PID;
		radius=synchro_value*0.1f*cosf((main_angle-synchro_angle)*16.0f);
		p_x=-(tunnel_path*sinf(angle*0.125f)-tunnel_path*cosf(angle*0.375f))-(speed_flag?(1.0f-speed_value)*tunnel_path*5.0f:0.0f);
		p_y=-(tunnel_path*sinf(angle*0.25f)-tunnel_path*cosf(angle*0.25f)+radius);
		a_z=p_x*10.0f;
		for(i=0;i<tunnel_n1;i++)
			{
			tunnel_z[i]+=(main_angle-main_angle_prv)*1.5f;
			if(tunnel_z[i]>0.0f)
				{
				tunnel_x[i]=x;
				tunnel_y[i]=y;
				tunnel_z[i]-=tunnel_n1*tunnel_depth;
				}
			angle=360.0f*PID/tunnel_n2;
			float angle2=720.0f/tunnel_n1*i;
			for(j=0;j<tunnel_n2;j++)
				{
				c=(tunnel_z[i]<-1.0f)?1.0f:-tunnel_z[i];
				glLoadIdentity();
				glColor3f((0.5f+0.5f*cosf(angle*j))*c,0.5f*c,(0.5f+0.5f*sinf(angle*j))*c);
				glRotatef(a_z,0,0,1.0f);
				glTranslatef(p_x+tunnel_x[i]+tunnel_radius*cosf(angle*j+angle2*PID),p_y+tunnel_y[i]+tunnel_radius*sinf(angle*j+angle2*PID),tunnel_z[i]);
				glRotatef(360.0f/tunnel_n2*j+angle2,0,0,1.0f);
				glDrawArrays(GL_QUADS,0,4);
				}
			}
		// draw star
		glVertexPointer(2,GL_FLOAT,0,star_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,star_tex);
		glColor3f(1.0f,1.0f,1.0f);
		for(i=0;i<star_n;i++)
			{
			star_z[i]+=(main_angle-main_angle_prv)*1.5f;
			if(star_z[i]>0.0f)
				{
				radius=((rand()%1000)*0.01f);
				radius=tunnel_radius*1.25f+((radius<0.0f)?-radius:radius);
				star_x[i]=x+radius*cosf(star_angle[i]);
				star_y[i]=y+radius*sinf(star_angle[i]);
				star_z[i]-=tunnel_n1*tunnel_depth;
				}
			glLoadIdentity();
			glRotatef(a_z,0,0,1.0f);
			glTranslatef(p_x+star_x[i],p_y+star_y[i],star_z[i]);
			glDrawArrays(GL_QUADS,0,4);
			}
		}
	glEnable(GL_BLEND);
	// draw greeting
	if(greeting_flag)
		{
		int greeting_x=14;
		int greeting_y=(int)(14.0f/screen_w*screen_h+2);
		w=4.125f;
		h=4.25f;
		x=-w*(greeting_x-1)*0.5f+0.15f*synchro_value*cosf((main_angle-synchro_angle)*16.0f);
		y=-h*(greeting_y-0)*0.5f+(float)fmod(main_angle*2.0f,h);
		z=-12.0f;
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		glColor3f(1.0f,1.0f,1.0f);
		glVertexPointer(3,GL_FLOAT,0,disk_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,disk_tex);
		for(i=0;i<greeting_x;i++)
			{
			for(j=0;j<greeting_y;j++)
				{
				glLoadIdentity();
				glRotatef(22.5f*cosf(main_angle*0.25f),1.0f,0,0);
				glTranslatef(x+i*w,y+j*h,z);
				glRotatef(10.0f,0,0,1.0f);
				glDrawArrays(GL_QUADS,0,24);
				}
			}
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,triforce_vtx);
		glColorPointer(3,GL_FLOAT,0,triforce_col);
		for(i=0;i<greeting_x;i++)
			{
			for(j=0;j<greeting_y;j++)
				{
				glLoadIdentity();
				glRotatef(22.5f*cosf(main_angle*0.25f),1.0f,0,0);
				glTranslatef(x+i*w,y+j*h,z);
				glRotatef(10.0f,0,0,1.0f);
				glTranslatef(0,0.75f,0.125f);
				glRotatef((float)fabs(120.0f*synchro_value*cosf((main_angle-synchro_angle)*5.0f)),0,0,1.0f);
				glDrawArrays(GL_QUADS,0,60);
				}
			}
		glDisableClientState(GL_COLOR_ARRAY);
		}
	// draw vote
	glEnable(GL_TEXTURE_2D);
	if(vote_flag)
		{
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		w=0.325f;
		h=0.325f;
		angle=sync2_value*cosf((main_angle-sync2_angle)*1.25f);
		a_x=50.0f;//+180.0f*cosf(main_angle*0.125f);
		a_y=-main_angle*16.0f-120.0f*angle;
		p_x=-vote_n1*vote_w*0.25f;
		p_y=-32.0f*angle;
		p_z=-16.0f;
		z=-vote_n2*vote_w*0.5f;
		radius=1.0f;
		glVertexPointer(2,GL_FLOAT,0,vote_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,star_tex);
		for(i=0;i<vote_n1;i++)
			{
			x=p_x+i*vote_w;
			angle=720.0f*PID/vote_n1*i+cosf(i*0.375f)+main_angle*1.625f;
			for(j=0;j<vote_n2;j++)
				{
				y=-0.5f-radius*1.5f*cosf(main_angle*0.25f)+sinf((i+j)*0.25f)+radius*cosf(angle)+radius*sinf(720.0f*1.5f*PID/vote_n2*j+main_angle);
				glColor3f(0.5f+0.5f*cosf(90.0f*PID*y),1.0f,0.5f+0.5f*sinf(90.0f*PID*y));
				if(y<-2.0f&&y>-2.5f) glColor3f(0.625f,1.0f,0.75f);
				if(y<-1.0f) y=-y-1.35f;
				if(y<-0.625f) y=-0.5f;
				glLoadIdentity();
				glTranslatef(p_y,0,p_z);
				glRotatef(a_x,1.0f,0,0);
				glRotatef(a_y,0,1.0f,0);
				glTranslatef(p_x+x,y,z+j*vote_w);
				glRotatef(-a_y,0,1.0f,0);
				glRotatef(-a_x,1.0f,0,0);
				glDrawArrays(GL_QUADS,0,4);
				}
			}
		}
	// draw end
	if(end_flag)
		{
		float angle1=cosf(main_angle*0.25f);
		float angle2=sinf(main_angle*0.25f);
		if(speed_flag) intro_radius=1.0f+7.0f-7.0f*speed_value;
		radius=-(float)fabs(0.625f*synchro_value*cosf((main_angle-synchro_angle)*8.0f));
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		glLoadIdentity();
		glTranslatef(0,0,end_radius+intro_radius);
		glRotatef(main_angle*16.0f,angle1,0,angle2);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,glenz_vtx);
		glColorPointer(4,GL_FLOAT,0,glenz_col);
		glScalef(10.0f,11.0f,11.0f);
		glDrawArrays(GL_TRIANGLES,0,144);
		glScalef(0.875f,0.875f,0.875f);
		glDrawArrays(GL_TRIANGLES,0,144);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		glColor3f(1.0f,1.0f,1.0f);
		glVertexPointer(3,GL_FLOAT,0,disk_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,disk_tex);
		for(i=0;i<8;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(22.5f+45.0f*i,0,1.0f,0);
			glTranslatef(0,0,end_radius+radius);
			glDrawArrays(GL_QUADS,0,24);
			}
		for(i=0;i<4;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(90.0f*i,0,1.0f,0);
			glRotatef(45.0f,1.0f,0,0);
			glTranslatef(0,0,end_radius+radius);
			glDrawArrays(GL_QUADS,0,24);
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(90.0f*i,0,1.0f,0);
			glRotatef(-45.0f,1.0f,0,0);
			glTranslatef(0,0,end_radius+radius);
			glDrawArrays(GL_QUADS,0,24);
			}
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,triforce_vtx);
		glColorPointer(3,GL_FLOAT,0,triforce_col);
		for(i=0;i<8;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(22.5f+45.0f*i,0,1.0f,0);
			glTranslatef(0,0,end_radius+radius);
			glTranslatef(0,0.75f,0.125f);
			glDrawArrays(GL_QUADS,0,60);
			}
		for(i=0;i<4;i++)
			{
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(90.0f*i,0,1.0f,0);
			glRotatef(45.0f,1.0f,0,0);
			glTranslatef(0,0,end_radius+radius);
			glTranslatef(0,0.75f,0.125f);
			glDrawArrays(GL_QUADS,0,60);
			glLoadIdentity();
			glTranslatef(0,0,end_radius+intro_radius);
			glRotatef(main_angle*16.0f,angle1,0,angle2);
			glRotatef(90.0f*i,0,1.0f,0);
			glRotatef(-45.0f,1.0f,0,0);
			glTranslatef(0,0,end_radius+radius);
			glTranslatef(0,0.75f,0.125f);
			glDrawArrays(GL_QUADS,0,60);
			}
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		}
	// draw tekk
	if(tekk_flag)
		{
		glDisable(GL_TEXTURE_2D);
		if(tekk_zoom_flag)
			{
			angle=(main_angle-tekk_zoom_angle)*0.15f;
			tekk_zoom_value=1.0f-cosf(angle);
			if(angle>90.0f*PID)
				{
				tekk_zoom_value=0;
				tekk_zoom_flag=false;
				}
			bgd_base_r=0.25f+tekk_zoom_value*0.25f;
			bgd_base_g=0.2f-tekk_zoom_value*0.125f;
			bgd_base_b=0.0f+tekk_zoom_value*0.125f;
			fog_color[0]=bgd_base_r;
			fog_color[1]=bgd_base_g;
			fog_color[2]=bgd_base_b;
			glFogfv(GL_FOG_COLOR,fog_color);
			}
		p_x=-2.0f;
		p_y=-tekk_bar*tekk_w+0.5f*cosf(main_angle*0.5f);
		p_z=-7.0f;
		a_x=-36.0f;
		a_y=90.0f;//+tekk_zoom_value*4.0f*sinf(main_angle*2.0f);
		a_z=0;//tekk_zoom_value*4.0f*sinf(main_angle*2.0f);
		y=0;
		k=0;
		float z1,z2;
		float a1=main_angle*0.5f+tekk_zoom_value*4.0f;
		for(i=0;i<tekk_bar;i++)
			{
			angle=tekk_radius*cosf(1080.0f*PID/tekk_bar*i+cosf(i*1.5f)+a1);
			float a2=main_angle*2.0f+tekk_zoom_value*24.0f;
			for(j=0;j<tekk_n;j++)
				{
				float angle2=1080.0f*PID/tekk_n*j+a2;
				z=sync2_value*2.0f*(rand()%100)/200.0f*cosf((main_angle-sync2_angle)*16.0f);
				z1=z2;
				z2=(float)fabs(angle+tekk_radius*sinf(angle2))+z;
				if(z2<0.5f) z2=0.5f;
				tekk_vtx[k+ 2]=z1;
				tekk_vtx[k+ 5]=z1;
				tekk_vtx[k+ 8]=z2;
				tekk_vtx[k+11]=z2;
				tekk_vtx[k+14]=z1;
				tekk_vtx[k+17]=z1;
				tekk_vtx[k+20]=z2;
				tekk_vtx[k+23]=z2;
				tekk_vtx[k+26]=z1;
				tekk_vtx[k+35]=z2;
				tekk_vtx[k+41]=z1;
				tekk_vtx[k+44]=z2;
				k+=48;
				}
			}
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,tekk_vtx);
		glColorPointer(3,GL_FLOAT,0,tekk_col);
		glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
		glLoadIdentity();
		glRotatef(a_x,1.0f,0,0);
		glRotatef(a_z,0,1.0f,0);
		glRotatef(a_y,0,0,1.0f);
		glTranslatef(p_x,p_y,p_z);
		glDrawArrays(GL_QUADS,0,tekk_bar*tekk_n*16);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		}
	// draw hidden
	if(hidden_flag)
		{
		w=0.25f;
		h=0.25f;
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		a_x=20.0f*cosf(main_angle*0.375f);
		a_y=30.0f*sinf(main_angle*0.25f);
		a_z=main_angle*8.0f;
		glVertexPointer(2,GL_FLOAT,0,hidden_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,hidden_tex);
		for(i=0;i<star_n;i++)
			{
			star_z[i]+=(main_angle-main_angle_prv)*2.0f;
			if(star_z[i]>0)
				{
				radius=hidden_radius*0.025f+((rand()%(int)(hidden_radius*75))*0.01f);
				radius=1.75f+((radius<0.0f)?-radius:radius);
				star_x[i]=radius*cosf(star_angle[i]);
				star_y[i]=radius*sinf(star_angle[i]);
				star_z[i]-=hidden_radius;
				}
			c=(star_z[i]<-2.0f)?1.0f:-star_z[i]*0.5f;
			glLoadIdentity();
			glColor3f(c,c,c);
			glRotatef(a_x,1.0f,0,0);
			glRotatef(a_y,0,1.0f,0);
			glRotatef(a_z,0,0,1.0f);
			glTranslatef(star_x[i],star_y[i],star_z[i]);
			glRotatef(-a_z,0,0,1.0f);
			glRotatef(-a_y,0,1.0f,0);
			glRotatef(-a_x,1.0f,0,0);
			glRotatef(360.0f,0,0,(1.0f-c));
			glDrawArrays(GL_QUADS,0,4);
			}
		w=1.0f+synchro_value*0.25f*cosf((main_angle-synchro_angle)*16.0f);
		h=1.5f+synchro_value*0.25f*sinf((main_angle-synchro_angle)*16.0f);
		glBlendFunc(GL_ONE,GL_ZERO);
		glLoadIdentity();
		glColor3f(1.0f,1.0f,1.0f);
		glRotatef(a_x,1.0f,0,0);
		glRotatef(a_y,0,1.0f,0);
		glTranslatef(0,0,-10.0f);
		float vertex[]={-w,-h,w,-h,w,h,-w,h};
		glVertexPointer(2,GL_FLOAT,0,vertex);
		glTexCoordPointer(2,GL_FLOAT,0,gameboy_tex);
		glDrawArrays(GL_QUADS,0,4);
		}
	glDisable(GL_FOG);
	// draw heart
	if(heart_flag)
		{
		glVertexPointer(2,GL_FLOAT,0,heart_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,heart_tex);
		glBlendFunc(GL_SRC_COLOR,GL_DST_ALPHA);
		a_x=30.0f+180.0f*cosf(main_angle*0.125f);
		a_y=main_angle*24.0f;
		radius=2.25f+synchro_value*0.25f*cosf((main_angle-synchro_angle)*12.0f);
		glColor3f(1.0f,1.0f,1.0f);
		for(i=0;i<heart_n;i++)
			{
			angle=a_y+360.0f/heart_n*i;
			glLoadIdentity();
			glTranslatef(0,0,-8.0f);
			glRotatef(a_x,1.0f,0,0);
			glRotatef(angle,0,1.0f,0);
			glTranslatef(0,0,radius);
			glRotatef(-angle,0,1.0f,0);
			glRotatef(-a_x,1.0f,0,0);
			glDrawArrays(GL_QUADS,0,4);
			}
		}
	// draw glenz
	if(glenz_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		glLoadIdentity();
		glTranslatef(0,0,-8.0f+intro_radius+(float)fabs(synchro_value*0.375f*cosf((main_angle-synchro_angle)*12.0f)));
		glRotatef(main_angle*16.0f,cosf(main_angle*0.25f),0,sinf(main_angle*0.25f));
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(3,GL_FLOAT,0,glenz_vtx);
		glColorPointer(4,GL_FLOAT,0,glenz_col);
		glDrawArrays(GL_TRIANGLES,0,144);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		}
	init2d(screen_w,screen_h);
	// draw copper
	if(greeting_flag||end_flag||hidden_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glEnableClientState(GL_COLOR_ARRAY);
		glBlendFunc(GL_SRC_COLOR,GL_DST_COLOR);
		angle=180.0f/copper_n*PID;
		for(i=0;i<copper_n;i++)
			{
			copper_col[i*12   ]=0.625f-0.125f*cosf(main_angle*0.5f+angle*i);
			copper_col[i*12+1 ]=0.625f;
			copper_col[i*12+2 ]=0.625f+0.125f*sinf(main_angle+angle*i);
			copper_col[i*12+3 ]=copper_col[i*12];
			copper_col[i*12+4 ]=copper_col[i*12+1];
			copper_col[i*12+5 ]=copper_col[i*12+2];
			copper_col[i*12+6 ]=0.625f-0.125f*cosf(main_angle*0.5f+angle*(i+1));
			copper_col[i*12+7 ]=0.625f;
			copper_col[i*12+8 ]=0.625f+0.125f*sinf(main_angle+angle*(i+1));
			copper_col[i*12+9 ]=copper_col[i*12+6];
			copper_col[i*12+10]=copper_col[i*12+7];
			copper_col[i*12+11]=copper_col[i*12+8];
			}
		glLoadIdentity();
		glVertexPointer(2,GL_INT,0,copper_vtx);
		glColorPointer(3,GL_FLOAT,0,copper_col);
		glDrawArrays(GL_QUADS,0,copper_n*4);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		}
	// draw border
	if(border1_flag||border2_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glEnableClientState(GL_COLOR_ARRAY);
		glLoadIdentity();
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		glVertexPointer(2,GL_INT,0,border_vtx);
		glColorPointer(4,GL_FLOAT,0,border1_flag?border_co1:border_co2);
		glDrawArrays(GL_QUADS,0,8);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnable(GL_TEXTURE_2D);
		}
	// draw liner
	if(liner_flag)
		{
		glVertexPointer(2,GL_INT,0,liner_vtx);
		glBlendFunc(GL_ONE,GL_ONE);
		j=0;
		liner_line=-1;
		liner_count=(int)((main_angle-liner_angle)*20.0f)-5;
		if(liner_count>liner_length) liner_count=liner_length;
		for(i=0;i<liner_count;i++)
			{
			j++;
			car=(byte)txt[i];
			if(car>32)
				{
				liner_color=(liner_count-i)*(0.05f); if(liner_color>1.0f) liner_color=1.0f;
				angle=main_angle*2.0f+j*PID*12.0f;
				radius=1.0f+(1.0f-liner_color)*6.0f;
				glLoadIdentity();
				glColor3f(liner_color*fade_value,liner_color*fade_value,liner_color*fade_value);
				glRotatef(6.0f*cosf(angle)*(1.0f-liner_color),0,0,1.0f);
				glTranslatef(x+j*liner_w*2.0f+liner_w*1.5f*cosf(angle),y+liner_h*2.0f*cosf(angle),0);
				glRotatef(-12.5f*sinf(angle),0,0,1.0f);
				glScalef(radius,radius,0);
				float l_w=(car%16)*0.03125f;
				float l_h=(car-car%16)*0.001953125f;
				float liner_tex[]={l_w,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.00001f,l_w,0.75f-l_h-0.00001f};
				glTexCoordPointer(2,GL_FLOAT,0,liner_tex);
				glDrawArrays(GL_QUADS,0,4);
				}
			if(car==13)
				{
				j=0;
				liner_line++;
				x=screen_w*0.5f-liner_max*liner_w;
				y=screen_h*0.5f-(liner_n-1)*liner_h*1.25f+liner_line*liner_h*2.5f;
				}
			}
		}
	if(tekk_flag)
		{
		glEnableClientState(GL_COLOR_ARRAY);
		glVertexPointer(2,GL_INT,0,youtube_vtx);
		x=(screen_w-liner_max*liner_w*1.5f)*0.5f;
		y=screen_h*0.5f+liner_h*7.0f;
		j=0;
		for(i=0;i<liner_length;i++)
			{
			j++;
			car=(byte)txt[i];
			if(car>32)
				{
				angle=main_angle*2.0f+j*PID*12.0f;
				r=0.5f+0.5f*cosf(j*0.125f+main_angle*2.0f);
				b=0.5f+0.5f*sinf(j*0.125f+main_angle*2.0f);
				float l_w=(car%16)*0.03125f;
				float l_h=(car-car%16)*0.001953125f;
				float texture[]={l_w,0.75f-l_h-0.03125f,l_w+0.015625f,0.75f-l_h-0.03125f,l_w+0.015625f,0.75f-l_h-0.00001f,l_w,0.75f-l_h-0.00001f,l_w+0.015625f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h-0.00001f,l_w+0.015625f,0.75f-l_h-0.00001f};
				float color[]={r*0.375f,0.1875f,b*0.375f,r,0.5f,b,r,0.5f,b,r*0.375f,0.1875f,b*0.375f,r,0.5f,b,r,0.5f,b,r,0.5f,b,r,0.5f,b};
				glTexCoordPointer(2,GL_FLOAT,0,texture);
				glColorPointer(3,GL_FLOAT,0,color);
				glLoadIdentity();
				glTranslatef(x+i*liner_w*1.5f,y+(float)fabs(liner_h*synchro_value*cosf((main_angle-synchro_angle)*12.0f)),0);
				glDrawArrays(GL_QUADS,0,8);
				}
			}
		glDisableClientState(GL_COLOR_ARRAY);
		}
	// draw razor
	if(razor_flag)
		{
		glEnableClientState(GL_COLOR_ARRAY);
		glBlendFunc(GL_SRC_COLOR,GL_SRC_ALPHA);
		if(!tekk_flag&&!hidden_flag)
			{
			glLoadIdentity();
			glVertexPointer(2,GL_INT,0,razor_b_vtx);
			glColorPointer(4,GL_FLOAT,0,razor_b_col);
			glDrawArrays(GL_QUADS,0,8);
			}
		glLoadIdentity();
		glTranslated(screen_w/2-(razor_w-razor_w/razor_n/2)/2-ratio_2d,(!tekk_flag?razor_y:screen_h/2),0);
		c=0.25f*cosf(main_angle);
		float x1=0;
		float x2=0;
		float x3=0;
		float x4=0;
		for(i=0;i<razor_n;i++)
			{
			if(i>0) x1+=(i!=razor_n/2+1)?razor_w/razor_n:razor_w/razor_n/3*2;
			x2+=(i!=razor_n/2)?razor_w/razor_n:razor_w/razor_n/3*2;
			x3=0;
			x4=0;
			if(synchro_flag)
				{
				angle=360.0f/razor_n*PID*3.375f;
				x3=synchro_value*razor_radius*cosf((main_angle-synchro_angle)*12.0f+angle*i);
				x4=synchro_value*razor_radius*cosf((main_angle-synchro_angle)*12.0f+angle*(i+1));
				}
			x=1.0f/razor_n;
			float t_x1=(i<razor_n/2+1)?x*i:x*(i-razor_n/2-1);
			float t_x2=(i<razor_n/2+1)?x*(i+1):x*(i+1-razor_n/2-1);
			float t_y1=(i<razor_n/2+1)?0.5f:0.25f;
			float t_y2=(i<razor_n/2+1)?0.25f:0.0f;
			if(i==razor_n/2)
				{
				t_x1=0.475f;
				t_x2=0.5f;
				}
			float c1=0.375f/razor_radius*x3;
			float c2=0.375f/razor_radius*x4;
			k=i*12;
			razor_col[k   ]=0.75f+0.25f/razor_n*i-c1;
			razor_col[k+ 1]=0.875f-c1;
			razor_col[k+ 2]=1.0f-0.25f/razor_n*i-c1;
			razor_col[k+ 3]=0.625f+0.375f/razor_n*i-c1;
			razor_col[k+ 4]=0.875f-c1;
			razor_col[k+ 5]=1.0f-0.375f/razor_n*i-c1;
			razor_col[k+ 6]=0.625f+0.375f/razor_n*(i+1)-c2;
			razor_col[k+ 7]=0.875f-c2;
			razor_col[k+ 8]=1.0f-0.375f/razor_n*(i+1)-c2;
			razor_col[k+ 9]=0.75f+0.25f/razor_n*(i+1)-c2;
			razor_col[k+10]=0.875f-c2;
			razor_col[k+11]=1.0f-0.25f/razor_n*(i+1)-c2;
			k=i*8;
			razor_tex[k  ]=t_x1;
			razor_tex[k+1]=t_y1;
			razor_tex[k+2]=t_x1;
			razor_tex[k+3]=t_y2;
			razor_tex[k+4]=t_x2;
			razor_tex[k+5]=t_y2;
			razor_tex[k+6]=t_x2;
			razor_tex[k+7]=t_y1;
			razor_vtx[k  ]=x1+x3;
			razor_vtx[k+1]=-razor_h-x3*0.5f;
			razor_vtx[k+2]=x1+x3;
			razor_vtx[k+3]=razor_h+x3*0.5f;
			razor_vtx[k+4]=x2+x4;
			razor_vtx[k+5]=razor_h+x4*0.5f;
			razor_vtx[k+6]=x2+x4;
			razor_vtx[k+7]=-razor_h-x4*0.5f;
			}
		glVertexPointer(2,GL_FLOAT,0,razor_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,razor_tex);
		glColorPointer(3,GL_FLOAT,0,razor_col);
		glDrawArrays(GL_QUADS,0,razor_n*4);
		glDisableClientState(GL_COLOR_ARRAY);
		}
	// draw decrunch
	if(decrunch_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glColor3f((float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)));
		for(i=0;i<screen_h;i++)
			{
			decrunch_y=i*decrunch_h;
			decrunch_split=rand()%4;
			if(decrunch_split==0)
				{
				if(rand()%4==0) glColor3f((float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)));
				decrunch_split_w=rand()%(screen_w/16)*16;
				rectangle(0,decrunch_y-decrunch_h,decrunch_split_w,decrunch_h);
				if(rand()%8==0) glColor3f((float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)),(float)(CR*(128+rand()%128)));
				rectangle(decrunch_split_w,decrunch_y-decrunch_h,screen_w-decrunch_split_w,decrunch_h);
				}
			else
				{
				rectangle(0,decrunch_y-decrunch_h,screen_w,decrunch_h);
				}
			}
		glEnable(GL_BLEND);
		glEnable(GL_TEXTURE_2D);
		}
	// draw dos
	if(dos_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		if(!decrunch_flag)
			{
			glColor3f((float)(CR*0),(float)(CR*85),(float)(CR*170));
			rectangle(0,0,screen_w,screen_h);
			}
		glColor3f(1.0f,1.0f,1.0f);
		rectangle(0,0,screen_w-(int)(26.5f*ratio_2d),10*ratio_2d);
		rectangle(screen_w-(int)(25.5f*ratio_2d),0,11*ratio_2d,10*ratio_2d);
		rectangle(screen_w-(int)(13.5f*ratio_2d),0,11*ratio_2d,10*ratio_2d);
		rectangle(screen_w-(int)(1.5f*ratio_2d),0,(int)(1.5f*ratio_2d),10*ratio_2d);
		rectangle(0,10*ratio_2d,1*ratio_2d,screen_h-10*ratio_2d);
		rectangle(screen_w-1*ratio_2d,10*ratio_2d,1*ratio_2d,screen_h-10*ratio_2d);
		rectangle(1*ratio_2d,screen_h-1*ratio_2d,screen_w-2*ratio_2d,1*ratio_2d);
		glColor3f((float)(CR*0),(float)(CR*85),(float)(CR*170));
		rectangle((int)(34.5f*ratio_2d),2*ratio_2d,screen_w-(int)(62.5f*ratio_2d),2*ratio_2d);
		rectangle((int)(34.5f*ratio_2d),6*ratio_2d,screen_w-(int)(62.5f*ratio_2d),2*ratio_2d);
		rectangle(screen_w-(int)(24.5f*ratio_2d),1*ratio_2d,7*ratio_2d,6*ratio_2d);
		glColor3f(1.0f,1.0f,1.0f);
		rectangle(screen_w-(int)(23.5f*ratio_2d),2*ratio_2d,5*ratio_2d,4*ratio_2d);
		glColor3f(0,0,0);
		rectangle(screen_w-(int)(22.5f*ratio_2d),3*ratio_2d,7*ratio_2d,6*ratio_2d);
		rectangle(screen_w-(int)(12.5f*ratio_2d),1*ratio_2d,7*ratio_2d,6*ratio_2d);
		glColor3f((float)(CR*0),(float)(CR*85),(float)(CR*170));
		rectangle(screen_w-(int)(10.5f*ratio_2d),2*ratio_2d,7*ratio_2d,7*ratio_2d);
		glColor3f(1.0f,1.0f,1.0f);
		rectangle(screen_w-(int)(9.5f*ratio_2d),3*ratio_2d,5*ratio_2d,5*ratio_2d);
		rectangle(screen_w-8*ratio_2d,screen_h-9*ratio_2d,7*ratio_2d,8*ratio_2d);
		glColor3f((float)(CR*0),(float)(CR*85),(float)(CR*170));
		rectangle(screen_w-7*ratio_2d,screen_h-8*ratio_2d,6*ratio_2d,7*ratio_2d);
		glColor3f(1.0f,1.0f,1.0f);
		rectangle(screen_w-4*ratio_2d,screen_h-8*ratio_2d,4*ratio_2d,2*ratio_2d);
		rectangle(screen_w-7*ratio_2d,screen_h-5*ratio_2d,2*ratio_2d,5*ratio_2d);
		rectangle(screen_w-6*ratio_2d,screen_h-7*ratio_2d,1*ratio_2d,1*ratio_2d);
		rectangle(screen_w-4*ratio_2d,screen_h-5*ratio_2d,2*ratio_2d,3*ratio_2d);
		glEnable(GL_TEXTURE_2D);
		glVertexPointer(2,GL_INT,0,shell_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,shell_tex);
		glDrawArrays(GL_QUADS,0,4);
		// text
		glVertexPointer(2,GL_INT,0,dos_vtx);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE,GL_ONE);
		j=0;
		liner_line=-1;
		liner_count=liner_length;
		for(i=0;i<liner_count;i++)
			{
			j++;
			car=(byte)txt[i];
			if(car>32)
				{
				glLoadIdentity();
				glTranslated(x+j*(dos_w+2*ratio_2d),y,0);
				if(car>32)
					{
					float l_w=(car%16)*0.03125f;
					float l_h=(car-car%16)*0.001953125f;
					float dos_tex[]={l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h,l_w,0.75f-l_h,l_w,0.75f-l_h-0.03125f};
					glTexCoordPointer(2,GL_FLOAT,0,dos_tex);
					glDrawArrays(GL_QUADS,0,4);
					}
				}
			if(car==13)
				{
				j=0;
				liner_line++;
				x=(float)(2*ratio_2d);
				y=(float)(15*ratio_2d+liner_line*(dos_h+4*ratio_2d));
				}
			}
		}
	// draw flash
	if(flash_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE,GL_ONE);
		angle=(main_angle-flash_angle)*1.0f;
		if(angle>90.0f*PID) flash_flag=false;
		c=1.0f-sinf(angle);
		glColor3f(c,c,c);
		glLoadIdentity();
		glVertexPointer(2,GL_INT,0,scanline_vtx);
		glDrawArrays(GL_QUADS,0,4);
		glEnable(GL_TEXTURE_2D);
		}
	if(speed_flag)
		{
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE,GL_ONE);
		c=1.0f-speed_value;
		glColor3f(c,c,c);
		glLoadIdentity();
		glVertexPointer(2,GL_INT,0,scanline_vtx);
		glDrawArrays(GL_QUADS,0,4);
		glEnable(GL_TEXTURE_2D);
		}
	// draw logo
	if(logo_flag)
		{
		glLoadIdentity();
		glBlendFunc(GL_SRC_COLOR,GL_ONE);
		glTranslatef((float)(screen_w-logo_w-logo_margin),(float)(screen_h-logo_h-logo_margin-fabs(ratio_2d*4*synchro_value*cosf((main_angle-synchro_angle)*10.0f))),0);
		glColor3f(1.0f,1.0f,1.0f);
		glVertexPointer(2,GL_INT,0,logo_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,logo_tex);
		glDrawArrays(GL_QUADS,0,4);
		}
	// draw loop
	if(loop_counter>0)
		{
		c=beat_value;
		glLoadIdentity();
		glBlendFunc(GL_SRC_COLOR,GL_ONE);
		glTranslated(loop_margin,screen_h-loop_h-loop_margin,0);
		glColor3f(0.25f+c*0.5f,0.25f+c*0.125f,0.25f-c*0.25f);
		glVertexPointer(2,GL_INT,0,loop_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,loop_tex);
		glDrawArrays(GL_QUADS,0,4);
		}
	// draw debug
	if(debug_flag)
		{
		char debug[512];
		sprintf(debug,"--+ DEBUG +--\nscreen=%dx%d\n2d ratio=%dx\nfps=%3.1f\naverage=%3.1f\nfps min=%3.1f\nfps max=%3.1f\nmusic=%02d-%02d\ntimer=%3.3fs\npattern=%1.3fs\nloop=%dx",screen_w,screen_h,ratio_2d,timer_fps,timer_fps_average,timer_fps_min,timer_fps_max,mod_ord,mod_row,mod_time*0.001f,timer_max*0.001f,loop_counter);
		glVertexPointer(2,GL_INT,0,debug_vtx);
		glBlendFunc(GL_ONE,GL_ONE);
		glLoadIdentity();
		glColor3f(0.25f,0.25f,0.25f);
		glTranslated(-debug_w+2*ratio_2d,debug_h+2*ratio_2d,0);
		j=0;
		for(i=0;i<(int)strlen(debug);i++)
			{
			j++;
			car=(byte)debug[i];
			if(car==10)
				{
				glTranslated(-j*debug_w*2,debug_h*2,0);
				j=0;
				}
			glTranslated(debug_w*2,0,0);
			if(car>32)
				{
				float l_w=(car%16)*0.03125f;
				float l_h=(car-car%16)*0.001953125f;
				float debug_tex[]={l_w+0.03125f,0.75f-l_h-0.03125f,l_w+0.03125f,0.75f-l_h,l_w,0.75f-l_h,l_w,0.75f-l_h-0.03125f};
				glTexCoordPointer(2,GL_FLOAT,0,debug_tex);
				glDrawArrays(GL_QUADS,0,4);
				}
			}
		}
	// draw scanline
	if(scanline_flag)
		{
		glLoadIdentity();
		glBlendFunc(GL_DST_COLOR,GL_SRC_ALPHA);
		glColor4f(1.0f,1.0f,1.0f,0.625f);
		glVertexPointer(2,GL_INT,0,scanline_vtx);
		glTexCoordPointer(2,GL_FLOAT,0,scanline_tex);
		glDrawArrays(GL_QUADS,0,4);
		}
	return true;
	}

void KillGLWindow(void)							// kill window
	{
	if(fullscreen)
		{
		ChangeDisplaySettings(NULL,0);	// switch back to desktop
		ShowCursor(false);							// show mouse pointer
		}
	if(hRC)
		{
		if(!wglMakeCurrent(NULL,NULL)) MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION);
		if(!wglDeleteContext(hRC)) MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION);
		hRC=NULL;
		}
	if(hDC&&!ReleaseDC(hWnd,hDC)) { MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hDC=NULL; }
	if(hWnd&&!DestroyWindow(hWnd)) { MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hWnd=NULL; }
	if(!UnregisterClass("razor1911",hInstance)) { MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK|MB_ICONINFORMATION); hInstance=NULL; }
	delete timer;
	}

int CreateGLWindow(char* title)
	{
	GLuint PixelFormat;												// pixel format result
	WNDCLASS wc;															// windows class structure
	DWORD dwExStyle;													// window extended style
	DWORD dwStyle;														// window style
	RECT WindowRect;													// upper_left/lower_right values
	int w=GetSystemMetrics(SM_CXSCREEN);
	int h=GetSystemMetrics(SM_CYSCREEN);
	screen_w=fullscreen?w:window_w;
	screen_h=fullscreen?h:window_h;
	timer_fps_min=32768;
	timer_fps_max=0;
	border_h=screen_h/4;
	border_vtx[ 0]=screen_w;
	border_vtx[ 1]=0;
	border_vtx[ 2]=0;
	border_vtx[ 3]=0;
	border_vtx[ 4]=0;
	border_vtx[ 5]=border_h;
	border_vtx[ 6]=screen_w;
	border_vtx[ 7]=border_h;
	border_vtx[ 8]=screen_w;
	border_vtx[ 9]=screen_h-border_h;
	border_vtx[10]=0;
	border_vtx[11]=screen_h-border_h;
	border_vtx[12]=0;
	border_vtx[13]=screen_h;
	border_vtx[14]=screen_w;
	border_vtx[15]=screen_h;
	decrunch_h=(int)(screen_h*0.01f);
	ratio_2d=(int)(screen_w/400);
	logo_w=16*ratio_2d;
	logo_h=16*ratio_2d;
	logo_margin=2*ratio_2d;
	logo_vtx[0]=logo_w;
	logo_vtx[1]=0;
	logo_vtx[2]=0;
	logo_vtx[3]=0;
	logo_vtx[4]=0;
	logo_vtx[5]=logo_h;
	logo_vtx[6]=logo_w;
	logo_vtx[7]=logo_h;
	loop_w=110*ratio_2d;
	loop_h=10*ratio_2d;
	loop_margin=3*ratio_2d;
	loop_vtx[0]=loop_w;
	loop_vtx[1]=0;
	loop_vtx[2]=0;
	loop_vtx[3]=0;
	loop_vtx[4]=0;
	loop_vtx[5]=loop_h;
	loop_vtx[6]=loop_w;
	loop_vtx[7]=loop_h;
	liner_w=4*ratio_2d;
	liner_h=4*ratio_2d;
	liner_vtx[0]=-liner_w;
	liner_vtx[1]=(int)(liner_h*1.5f);
	liner_vtx[2]=liner_w;
	liner_vtx[3]=liner_h;
	liner_vtx[4]=liner_w;
	liner_vtx[5]=(int)(-liner_h*1.5f);
	liner_vtx[6]=-liner_w;
	liner_vtx[7]=-liner_h;
	razor_margin=8*ratio_2d;
	razor_radius=8*ratio_2d;
	razor_w=256*ratio_2d;
	razor_h=32*ratio_2d;
	razor_y=razor_h-razor_margin;
	int y1=razor_y-razor_margin+razor_h;
	int y2=razor_y+razor_margin-razor_h;
	razor_b_vtx[ 0]=0;
	razor_b_vtx[ 1]=y1+liner_w;
	razor_b_vtx[ 2]=screen_w;
	razor_b_vtx[ 3]=y1+liner_w;
	razor_b_vtx[ 4]=screen_w;
	razor_b_vtx[ 5]=y1;
	razor_b_vtx[ 6]=0;
	razor_b_vtx[ 7]=y1;
	razor_b_vtx[ 8]=0;
	razor_b_vtx[ 9]=y1;
	razor_b_vtx[10]=screen_w;
	razor_b_vtx[11]=y1;
	razor_b_vtx[12]=screen_w;
	razor_b_vtx[13]=y2;
	razor_b_vtx[14]=0;
	razor_b_vtx[15]=y2;
	razor_b_vtx[16]=0;
	razor_b_vtx[17]=y2;
	razor_b_vtx[18]=screen_w;
	razor_b_vtx[19]=y2;
	razor_b_vtx[20]=screen_w;
	razor_b_vtx[21]=y2-liner_w;
	razor_b_vtx[22]=0;
	razor_b_vtx[23]=y2-liner_w;
	copper();
	youtube_vtx[ 0]=-liner_w;
	youtube_vtx[ 1]=(int)(liner_h*1.5f);
	youtube_vtx[ 2]=0;
	youtube_vtx[ 3]=(int)(liner_h*1.25f);
	youtube_vtx[ 4]=0;
	youtube_vtx[ 5]=-liner_h;
	youtube_vtx[ 6]=-liner_w;
	youtube_vtx[ 7]=-liner_h;
	youtube_vtx[ 8]=0;
	youtube_vtx[ 9]=(int)(liner_h*1.25f);
	youtube_vtx[10]=liner_w;
	youtube_vtx[11]=liner_h;
	youtube_vtx[12]=liner_w;
	youtube_vtx[13]=-liner_h;
	youtube_vtx[14]=0;
	youtube_vtx[15]=-liner_h;
	dos_w=2*ratio_2d;
	dos_h=4*ratio_2d;
	dos_vtx[0]=dos_w;
	dos_vtx[1]=dos_h;
	dos_vtx[2]=dos_w;
	dos_vtx[3]=-dos_h;
	dos_vtx[4]=-dos_w;
	dos_vtx[5]=-dos_h;
	dos_vtx[6]=-dos_w;
	dos_vtx[7]=dos_h;
	shell_vtx[0]=33*ratio_2d;
	shell_vtx[1]=8*ratio_2d;
	shell_vtx[2]=33*ratio_2d;
	shell_vtx[3]=1*ratio_2d;
	shell_vtx[4]=1*ratio_2d;
	shell_vtx[5]=1*ratio_2d;
	shell_vtx[6]=1*ratio_2d;
	shell_vtx[7]=8*ratio_2d;
	debug_w=2*ratio_2d;
	debug_h=4*ratio_2d;
	debug_vtx[0]=debug_w;
	debug_vtx[1]=debug_h;
	debug_vtx[2]=debug_w;
	debug_vtx[3]=-debug_h;
	debug_vtx[4]=-debug_w;
	debug_vtx[5]=-debug_h;
	debug_vtx[6]=-debug_w;
	debug_vtx[7]=debug_h;
	scanline_tex[0]=0;
	scanline_tex[1]=0.7325f;
	scanline_tex[2]=0;
	scanline_tex[3]=0.7275f;
	scanline_tex[4]=screen_h/256.0f;
	scanline_tex[5]=0.7275f;
	scanline_tex[6]=screen_h/256.0f;
	scanline_tex[7]=0.7325f;
	scanline_vtx[0]=screen_w;
	scanline_vtx[1]=0;
	scanline_vtx[2]=0;
	scanline_vtx[3]=0;
	scanline_vtx[4]=0;
	scanline_vtx[5]=screen_h;
	scanline_vtx[6]=screen_w;
	scanline_vtx[7]=screen_h;
	main_angle_prv=0;
	WindowRect.left=(long)(fullscreen?0:2);		// set left value
	WindowRect.right=(long)screen_w;					// set right value
	WindowRect.top=(long)(fullscreen?0:2);		// set top value
	WindowRect.bottom=(long)screen_h;					// set bottom value
	pfd.cColorBits=window_color;							// set color depth
	hInstance=GetModuleHandle(NULL);					// window instance
	wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;	// redraw on size, own DC for window
	wc.lpfnWndProc=(WNDPROC) WndProc;					// WndProc handles messages
	wc.cbClsExtra=0;													// no extra window data
	wc.cbWndExtra=0;													// no extra window data
	wc.hInstance=hInstance;										// set the instance
	wc.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(ICON_32));	// load default icon
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);		// load arrow pointer
	wc.hbrBackground=NULL;										// no background
	wc.lpszMenuName=NULL;											// no menu
	wc.lpszClassName="razor1911";							// set class name
	if(!RegisterClass(&wc))										// register window class
		{
		MessageBox(NULL,"windows FAIL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(fullscreen)
		{
		DEVMODE dmScreenSettings;															// device mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// is memory cleared?
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);			// devmode structure size
		dmScreenSettings.dmPelsWidth=screen_w;								// screen width
		dmScreenSettings.dmPelsHeight=screen_h;								// screen height
		dmScreenSettings.dmBitsPerPel=window_color;						// bits per pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// set selected mode
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
			{
			if(MessageBox(NULL,"your video card sucks.\nuse windowed mode instead?","ERROR",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				{
				fullscreen=false;
				}
			else
				{
				MessageBox(NULL,"the cake is a lie","ERROR",MB_OK|MB_ICONSTOP);
				return false;
				} 
			}
		}
	if(fullscreen)
		{
		dwExStyle=WS_EX_APPWINDOW;									// window extended style
		dwStyle=WS_POPUP;														// windows style
		ShowCursor(false);													// hide cursor
		}
	else
		{
		dwExStyle=WS_EX_APPWINDOW|WS_EX_CLIENTEDGE;	// window extended style
		dwStyle=WS_OVERLAPPEDWINDOW;								// windows style
		}
	AdjustWindowRectEx(&WindowRect,dwStyle,false,dwExStyle);	// adjust window to requested size
	// create window
	if(!(hWnd=CreateWindowEx(dwExStyle,		// extended style for window
		"razor1911",												// class name
		title,															// window title
		(dwStyle|														// defined window style
		WS_CLIPSIBLINGS|										// required window style
		WS_CLIPCHILDREN)										// required window style
		&~WS_THICKFRAME 										// window style (no-resize)
		//&~WS_SYSMENU												// window style (system menu)
		&~WS_MAXIMIZEBOX 										// window style (maximize)
		&~WS_MINIMIZEBOX,										// window style (minimize)
		(int)((w-screen_w)/2),							// window position x
		(int)((h-screen_h)/2),							// window position y
		(WindowRect.right-WindowRect.left),	// window width
		(WindowRect.bottom-WindowRect.top),	// window height
		NULL,																// no parent window
		NULL,																// no menu
		hInstance,													// instance
		NULL)))															// don't pass anything to WM_CREATE!
		{
		KillGLWindow();
		MessageBox(NULL,"Window creation FAIL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(!(hDC=GetDC(hWnd)))	// Did We Get A Device Context?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))	// Are We Able To Set The Pixel Format?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(!(hRC=wglCreateContext(hDC)))	// Are We Able To Get A Rendering Context?
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	if(!wglMakeCurrent(hDC,hRC))	// Try To Activate The Rendering Context
		{
		KillGLWindow();
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	ShowWindow(hWnd,SW_SHOW);		// show window
	SetForegroundWindow(hWnd);	// set higher priority
	SetFocus(hWnd);							// set keyboard focus to window
	init3d(screen_w,screen_h);	// set up perspective of GL screen
	if(!InitGL())								// initialize GL window
		{
		KillGLWindow();
		MessageBox(NULL,"init FAIL","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return false;
		}
	return true;
	}

// window handle,window message,additional message,additional message
LRESULT CALLBACK WndProc(HWND	hWnd,UINT	uMsg,WPARAM	wParam,LPARAM	lParam)
	{
	switch(uMsg)								// check windows messages
		{
		case WM_ACTIVATE:					// watch for window activate message
			{
			if(!HIWORD(wParam))			// check minimization state
				{
				active=true;					// program is active
				}
			else
				{
				active=false;					// program is no longer active
				}
			return 0;								// return to the message loop
			}
		case WM_SYSCOMMAND:				// intercept system commands
			{
			switch(wParam)					// check system calls
				{
				case SC_SCREENSAVE:		// screensaver trying to start?
				case SC_MONITORPOWER:	// monitor trying to enter powersave?
				return 0;							// prevent from happening
				}
			break;									// exit
			}
		case WM_CLOSE:						// close message?
			{
			PostQuitMessage(0);			// post quit message
			return 0;
			}
		case WM_KEYDOWN:					// key down?
			{
			keys[wParam]=true;			// mark key as true
			return 0;
			}
		case WM_KEYUP:						// key released?
			{
			keys[wParam]=false;			// mark key as false
			return 0;
			}
		/*
	  case WM_SIZE:							// resize openGL window
			{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));	// loword=width,hiword=height
			return 0;
			}
		*/
		}
	return DefWindowProc(hWnd,uMsg,wParam,lParam); // pass all unhandled messages to DefWindowProc
	}

// instance,previous instance,command line parameters,window show state
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
	{
	MSG msg;																		// windows message structure
	done=false;																	// exit loop
	// ask for fullscreen mode
	#if !DEBUG
	fullscreen=(MessageBox(NULL,"fullscreen mode?",name,MB_YESNO|MB_ICONQUESTION)==IDYES)?true:false;
	#endif
	// create openGL window
	if(!CreateGLWindow(name)) return 0;					// quit if window not created
	// load and play music
	#if SNG
	FSOUND_File_SetCallbacks(memopen,memclose,memread,memseek,memtell);
	mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_DATA2),NULL);
	#endif
	// main loop
	while(!done)
		{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// a message is waiting?
			{
			if(msg.message==WM_QUIT)								// a quit message?
				{
				done=true;														// quit window
				}
			else																		// a window message?
				{
				TranslateMessage(&msg);								// translate message
				DispatchMessage(&msg);								// dispatch message
				}
			}
		else
			{
			// draw the scene, watch for escape key and quit message from DrawGLScene()
			if((active&&!DrawGLScene())||keys[VK_ESCAPE]) done=true; else SwapBuffers(hDC);	// exit or swap buffers
			if(keys[VK_SPACE])
				{
				KillGLWindow();
				fullscreen=!fullscreen;
				if(!CreateGLWindow(name)) return 0;		// quit if window not created
				keys[VK_SPACE]=false;
				}
			if(keys[VK_F1])
				{
				debug_flag=!debug_flag;
				keys[VK_F1]=false;
				}
			if(keys[VK_F2])
				{
				polygon=!polygon;
				glPolygonMode(GL_FRONT,polygon?GL_FILL:GL_LINE);
				keys[VK_F2]=false;
				}
			if(keys[VK_F3])
				{
				scanline_flag=!scanline_flag;
				keys[VK_F3]=false;
				}
			if(keys[VK_F12])
				{
				border1_flag=false;
				intro_flag=false;
				cube_flag=false;
				circuit_flag=false;
				move_flag=false;
				speed_flag=false;
				tunnel_flag=false;
				greeting_flag=false;
				vote_flag=false;
				heart_flag=false;
				end_flag=false;
				tekk_flag=false;
				glenz_flag=false;
				//
				hidden=true;
				for(i=0;i<star_n;i++)
					{
					radius=hidden_radius*0.025f+((rand()%(int)(hidden_radius*75))*0.01f);
					radius=1.75f+((radius<0.0f)?-radius:radius);
					star_x[i]=radius*cosf(star_angle[i]);
					star_y[i]=radius*sinf(star_angle[i]);
					star_z[i]=-(rand()%(int)(hidden_radius*100))*0.01f;
					}
				loop_counter=0;
				decrunch_time=timer_global+0.5f;
				#if SNG
				mod_play=false;
				mod_ord=-1;
				mod_row=-1;
				mod_prv_row=0;
				//mod_tempo=mod_tempo*2;
				FMUSIC_StopSong(mod);
				FMUSIC_FreeSong(mod);
				mod=FMUSIC_LoadSong(MAKEINTRESOURCE(IDR_DATA4),NULL);
				#endif
				keys[VK_F12]=false;
				}
			#if DEBUG
				if(keys[VK_F5])
					{
					synchro();
					keys[VK_F5]=false;
					}
				if(keys[VK_F6])
					{
					sync2(1.75f);
					keys[VK_F6]=false;
					}
				if(keys[VK_F7])
					{
					beat();
					keys[VK_F7]=false;
					}
				if(keys[VK_TAB])
					{
					tekk_zoom();
					keys[VK_TAB]=false;
					}
				if(keys[VK_BACK])
					{
					pause=!pause;
					keys[VK_BACK]=false;
					}
			#endif
			if(keys[VK_RETURN])
				{
				#if DEBUG
					flash();
					calc_txt();
				#endif
				timer_fps_total=0;
				timer_fps_min=32768;
				timer_fps_max=0;
				frame_counter=0;
				keys[VK_RETURN]=false;
				}
			}
		}
	// shutdown
	#if SNG
	FMUSIC_FreeSong(mod);					// stop and kill music
	#endif
	KillGLWindow();								// kill the window
	return (msg.wParam);					// exit the program
	}