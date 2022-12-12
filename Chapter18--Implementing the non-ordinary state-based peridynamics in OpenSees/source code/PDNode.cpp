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
                                                                        

                                                                        
                                                                        
// Written:  Lei Wang
// Created: June 2020

// This file contains the implementation of the PDNode class

   
#include <PDNode.h>
#include <stdlib.h>
#include <Element.h>
#include <Vector.h>
#include <Matrix.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <DOF_Group.h>
#include <Renderer.h>
#include <string.h>
#include <Information.h>
#include <Parameter.h>
#include <UniaxialMaterial.h>
#include <NDMaterial.h>
#include <Domain.h>
#include <Element.h>
#include <ElementIter.h>
#include <SP_Constraint.h>
#include <SP_ConstraintIter.h>
#include <NodalLoad.h> 
#include <OPS_Globals.h>
#include <elementAPI.h>
PDNode::PDNode(int tag, int ndof, double Crd1, double Crd2, NDMaterial &theNDMat, double vol, Vector *dLoc)
	:Node(tag, ndof, Crd1,  Crd2, dLoc), volumn(vol), theNDMaterial(0), pdNodeTag(tag)
	
{
	theNDMaterial = theNDMat.getCopy("PlaneStrain");
	if (theNDMaterial == 0) {
		opserr << "FATAL PDNode::PDNode - failed to get a copy of material with tag  " << tag << endln;
		exit(-1);
	}
	dimension = 2;
	maxNumofHorizonNodes = 64;
	neighborNum = -1;
	this->allocateSpace(maxNumofHorizonNodes, dimension);
}
 PDNode::PDNode(int tag, int ndof, double Crd1, double Crd2, double Crd3, NDMaterial &theNDMat, double vol, Vector *dLoc)
	 :Node(tag, ndof, Crd1, Crd2, Crd3, dLoc), volumn(vol), theNDMaterial(0), pdNodeTag(tag)
	 
{
	 theNDMaterial = theNDMat.getCopy("ThreeDimensional");
	 if (theNDMaterial == 0) {
		 opserr << "FATAL PDNode::PDNode - failed to get a copy of material with tag  " << tag << endln;
		 exit(-1);
	 }
	 dimension = 3;
	 maxNumofHorizonNodes = 216;
	 neighborNum = -1;
	 this->allocateSpace(maxNumofHorizonNodes, dimension);
}
PDNode::~PDNode()
{
	if (theNeiborPDNodes != 0)
		delete theNeiborPDNodes;
	if (theNeiborPDNodesReverse != 0)
		delete theNeiborPDNodesReverse;
	if (shapeK != 0)
		delete shapeK;
	if (invShapeK != 0)
		delete invShapeK;
	if (shapeN != 0)
		delete shapeN;
	if (defomationGradientF != 0)
		delete defomationGradientF;
	if (weightFun != 0)
		delete weightFun;
}

int 
PDNode :: allocateSpace(int maxNumofHorizonNodes, int dimension) {
	weightFun = new Vector(maxNumofHorizonNodes);
	weightFun->Zero();
	theNeiborPDNodes = new ID(maxNumofHorizonNodes);
	theNeiborPDNodes->Zero();
	theNeiborPDNodesReverse = new ID(maxNumofHorizonNodes);
	theNeiborPDNodesReverse->Zero();
	shapeK = new Matrix(dimension, dimension);
	shapeK->Zero();
	invShapeK = new Matrix(dimension, dimension);
	invShapeK->Zero();
	shapeN = new Matrix(dimension, dimension);
	shapeN->Zero();
	defomationGradientF = new Matrix(dimension, dimension);
	defomationGradientF->Zero();
	firstPKStress = new Matrix(dimension, dimension);
	firstPKStress->Zero();
	cauchyStress = new Vector(dimension * 3 - 3);
	cauchyStress->Zero();
	forceState = new Matrix(maxNumofHorizonNodes, dimension);
	forceState->Zero();
	xi = new Vector * [maxNumofHorizonNodes];
	return 0;
}
double 
PDNode::getVolumn()
{
	return volumn;
}



