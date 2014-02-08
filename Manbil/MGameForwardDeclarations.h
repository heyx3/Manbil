#pragma once

//Holds all the world's game objects. Also holds all components in a global map.
class MWorld;
//A single game entity. Holds a bunch of individual "MComponent"s in a map.
class MObject;
//A single facet of an MObject's behavior.
class MComponent;
//Creates some kind of MComponent.
class MComponentFactory;
//Camera plus some extra data like transformation matrices.
struct MCameraInfo;