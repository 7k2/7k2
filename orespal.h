//Filename    : ORESPAL.H
//Description : Palette resource


#ifndef __ORESPAL_H
#define __ORESPAL_H

#include <ORESDB.H>

class ResourcePal : public ResourceDb
{
public:
	short *pal_data_buf;

public:
   ResourcePal() : ResourceDb()   { pal_data_buf = 0; }
   ~ResourcePal()  { deinit(); }

   ResourcePal(char* resName,Database* dbObj,int indexOffset,int useCommonBuf=0)
		: ResourceDb ()
	{ init_flag=0; init(resName,dbObj,indexOffset,useCommonBuf); }

   void init(char*,Database*,int,int=0);
   void deinit();
   void  init_imported(char*,int,int=0);
	short* read_imported_pal(long);

private:
	void	generate_pal();
};

#endif