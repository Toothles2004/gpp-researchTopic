#pragma once
// Here we determine which application is currently active
// Create the define here using the "ActiveApp_..." convention and set up the correct include and typedef in the #ifdef below.

//#define ActiveApp_AStar
#define ActiveApp_FlowFields

//---------- Registered Applications -----------
#ifdef ActiveApp_AStar
#include "projects/Movement/Pathfinding/PathfindingAStar/App_PathfindingAStar.h"
typedef App_PathfindingAStar CurrentApp;
#endif

#ifdef ActiveApp_FlowFields
#include "projects/Movement/Pathfinding/FlowFields/App_FlowFieldPathfinding.h"
typedef App_FlowFieldPathfinding CurrentApp;
#endif

class App_Selector {
public: 
	static IApp* CreateApp() {
		IApp* myApp = new CurrentApp();
		return myApp;
	}
};