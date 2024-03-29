#pragma once

// Generic info:
#define LIB_NAME      "CG Engine v0.1a"  ///< Library credits
#define LIB_VERSION   10                 ///< Library version (divide by 10)

#ifdef _WINDOWS 	
   // Export API:
   // Specifies i/o linkage (VC++ spec):
   #ifdef CG_ENGINE_EXPORTS
		#define LIB_API __declspec(dllexport)
   #else
		#define LIB_API __declspec(dllimport)
   #endif      	
#else // Under Linux
   #define LIB_API  // Dummy declaration
#endif

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/packing.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

// Engine
#include "object.h"
#include "node.h"
#include "mesh.h"
#include "perspectiveCamera.h"
#include "orthographicCamera.h"
#include "pointLight.h"
#include "directionalLight.h"


/////////////
// CLASSES //
/////////////


class LIB_API CgEngine
{
	private:
		bool initFlag;

		// Singleton
		static CgEngine* istance;
		CgEngine();
	public:   
		static CgEngine* getIstance();

		bool init(int argc, char* argv[]);
		Node* load(char* filePath);
		void parse(Node* scene);
		void run();
		bool free();     

		void setKeyboardCallback(void (*func)(unsigned char, int, int));
		void setSpecialCallback(void (*func)(int, int, int));
		void setIdleCallback(void (*func)());
		unsigned int getElapsedTime();

		void cameraRotation();
		void toggleWireframe();
		void changeFilter();
		void addGuiText(char* text);
};
