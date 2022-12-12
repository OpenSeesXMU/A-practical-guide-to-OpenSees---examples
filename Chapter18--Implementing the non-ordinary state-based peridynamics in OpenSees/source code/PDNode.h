/* ****************************************************************** **
**    OpenSees - Open System for Earthquake Engineering Simulation    **
**          Pacific Earthquake Engineering Research Center            **
**                                                                    **
**                                                                    **
** (C) Copyright 1999, The Regents of the University of California    **
** All Rights Reserved.                                               **
**                                                                    **
** Commercial use of this program without express permission of the   **
** University of California, Berkeley, is strictly prohibited.  See   **
** file 'COPYRIGHT'  in main directory for information on usage and   **
** redistribution,  and for a DISCLAIMER OF ALL WARRANTIES.           **
**                                                                    **
** Developed by:                                                      **
**   Frank McKenna (fmckenna@ce.berkeley.edu)                         **
**   Gregory L. Fenves (fenves@ce.berkeley.edu)                       **
**   Filip C. Filippou (filippou@ce.berkeley.edu)                     **
**                                                                    **
** ****************************************************************** */
                                                                        
// Written: Lei Wang
// Created: June 2020
//
// Purpose: This file contains the class interface for PDNode.
// A Node provides the abstraction for a node in the Peridynamics.
// pdNodeNodes have original position, trial displacement, velocity and 

//
// What: "@(#) Node.h, revA"


                                                                        
#ifndef pdNode_h
#define pdNode_h
#include <DomainComponent.h>
#include <Node.h>
#include <NDMaterial.h>
class Element;
class Vector;
class Matrix;
class Channel;
class Renderer;
class DOF_Group;
class NodalThermalAction; 

class PDNode : public Node
{
  public:
	  PDNode(int tag, int ndof, double Crd1, double Crd2, NDMaterial &theNDMat, double vol, Vector *displayLoc=0 );
	  PDNode(int tag, int ndof, double Crd1, double Crd2, double Crd3, NDMaterial &theNDMat, double vol, Vector *displayLoc=0); 
	  ~PDNode();
	  double getVolumn(void);  
	  NDMaterial * getMaterial() { return theNDMaterial; };
	  int allocateSpace(int maxNumofHorizonNodes, int dimension);
	  int maxNumofHorizonNodes;
	  int neighborNum; 
	  ID *theNeiborPDNodes;   
	  ID *theNeiborPDNodesReverse; 
	  Vector *weightFun;
	  Vector **xi; 
	  Matrix *shapeK;
	  Matrix *invShapeK;
	  Matrix *shapeN;  
	  Matrix *defomationGradientF;
	  Matrix *forceState;
	  Matrix *firstPKStress;;
	  Vector *cauchyStress;
  protected:

  private:
	  double volumn; 
	  NDMaterial *theNDMaterial;
	  int pdNodeTag;
	  int dimension;	  	  
};
#endif

