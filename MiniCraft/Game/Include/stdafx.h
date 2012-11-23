// stdafx.h

#include <assert.h>

//Ogre
#include <Ogre.h>


//STL
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <Windows.h>


#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete []p; p=NULL; } }
#define SAFE_RELEASE(p) { if(p) { p->Release(); p=NULL; } }

