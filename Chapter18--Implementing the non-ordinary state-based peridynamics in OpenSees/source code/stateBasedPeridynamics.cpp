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


// Written: Lei Wang, June 2020
// command: element statedBasedPeridynamic    int tag, int dimension, int ndFirst, int ndLast, double horizonSize, double dx <, double thickness>
//no material tag for this element,since each PDNode has its material character
#include <NDMaterial.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <Renderer.h>
#include <Domain.h>
#include <string.h>
#include <Information.h>
#include <Parameter.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <ElementResponse.h>
#include <ElementalLoad.h>
#include <elementAPI.h>
#include <time.h>
#include <windows.h>
#include <stateBasedPeridynamics.h>
#define OPS_Export 
OPS_Export void *
OPS_stateBasedPeridynamics()
{
	Element *theElement = 0;
	int numRemainingArgs = OPS_GetNumRemainingInputArgs();
	if (numRemainingArgs < 5) {
		opserr << "WARNING incorrect arguments\n";
		opserr << "Want: element basedBasedPeridynamic int tag,  int ndFirst, int ndLast, double horizonSize, double dx; \n";
		return 0;
	}
	double h = 0;
	double r = 0;
	double dx = 0.0;
	int iData[4];
	int numData = 4;
	if (OPS_GetInt(&numData, iData) != 0) {
		opserr << "WARNING invalid integer (tag, ndm,iNode, jNode) in the pd element" << endln;
		return 0;
	}
	numData = 1;
	if (OPS_GetDouble(&numData, &h) != 0) {
		opserr << "WARNING: Invalid horizonSize for the pd element" << iData[0] <<" $iNode $jNode $horizonSize  $r\n";
		return 0;
	}
	numData = 1;
	if (OPS_GetDouble(&numData, &dx) != 0) {
		opserr << "WARNING: Invalid horizonSize for the pd element" << iData[0] <<" $iNode $jNode $horizonSize  $r\n";
		return 0;
	}
	double thickness = dx;
	while (OPS_GetNumRemainingInputArgs() > 0) {
		const char* type = OPS_GetString();
		if (strcmp(type, "-thickness") == 0) { //for 2d case
			if (OPS_GetNumRemainingInputArgs() < 1) {
				opserr << "WARNING invalid parameter -thickness $thickness\n";
				return 0;
			}
			else {
				if (OPS_GetDoubleInput(&numData, &thickness) < 0) {  
					opserr << "WARNING invalid parameter :: no thickness info\n";
					return 0;
				}
			}
		}
	}
	theElement = new stateBasedPeridynamics(iData[0], iData[1], iData[2], iData[3], h, dx,  thickness);
	if (theElement == 0) {
		opserr << "WARNING: out of memory: state pd element " << iData[0] << "\n";
	}
	return theElement;
}
stateBasedPeridynamics::stateBasedPeridynamics(int tag, int ndm, int nd1, int nd2, double h, double dr, double thickness
)
	:Element(tag, ELE_TAG_stateBasedPeridynamic),
	connectedExternalNodes(nd2 - nd1 + 1), dimension(ndm),dx(dr) ,t(thickness)
{
	for (int i = 0; i <= nd2 - nd1; i++) {
		connectedExternalNodes(i) = nd1 + i;
	}
	horizonSize = h;  
	numExternalNodes = connectedExternalNodes.Size();
	P = 0;
	theTangent = 0;
	Ki = 0;
	perturb = new Vector*[numExternalNodes];
	eta = new Vector(dimension);
	temp = new Matrix(dimension, dimension);
	tempTrialDispI = Vector(dimension);
	tempTrialDispI.Zero();
	tempTrialDispJ = Vector(dimension);
	tempTrialDispJ.Zero();
	dispIJ = Vector(dimension);
	dispIJ.Zero();
	tempCrdJ = Vector(dimension);
	for (int i = 0; i < numExternalNodes; i++) {
		perturb[i] = new Vector(dimension);
		perturb[i]->Zero();
	}
	ones = new Matrix(dimension, dimension);
	ones->Zero();
	for (int i = 0; i < dimension; i++) {
		(*ones)(i, i) = 1.0;
	}
	thePDNodes = 0;  ///all
	s = 0.0;
}
stateBasedPeridynamics::~stateBasedPeridynamics()
{
	if (perturb != 0) {
		for (int i = 0; i < numExternalNodes; i++)
			delete perturb[i];
		delete[] perturb;
	}
	if (theTangent != 0)
		delete theTangent;
	if (ones != 0)
		delete ones;
	if (P != 0)
		delete P;
	if (eta != 0)  
		delete eta;
	if (temp != 0)
		delete temp;
}
int stateBasedPeridynamics::getNumExternalNodes(void) const
{
	return connectedExternalNodes.Size();
}
const ID & stateBasedPeridynamics::getExternalNodes(void)
{
	return connectedExternalNodes;
}
Node ** 
stateBasedPeridynamics::getNodePtrs(void)
{
	Node ** theNodes =  (Node**)thePDNodes;
    return  theNodes;
}
int
stateBasedPeridynamics::getNumDOF()

{
    return dimension *connectedExternalNodes.Size();
}
void
stateBasedPeridynamics::setDomain(Domain *theDomain)
{
	if (thePDNodes ==0) 	thePDNodes = new PDNode *[numExternalNodes];
	for (int i = 0; i < numExternalNodes; i++) {
		thePDNodes[i] = theDomain->getPDNode(connectedExternalNodes(i));  //notice it's getPDNode...
		if (thePDNodes[i] == 0) {
			opserr << "WARNING:stateBasedPeridynamics( ) - node: ";
			opserr << connectedExternalNodes(i) << " does not exist ! " << endln;
			exit(-1);
		} 
	}
	this->DomainComponent::setDomain(theDomain);
	this->updatedHorizon();
	this->calculateWeight();
	this->calculateShapeFunK();	
}
int
stateBasedPeridynamics::commitState()
{
    int retVal = 0;
    if ((retVal = this->Element::commitState()) != 0) {
      opserr << "stateBasedPeridynamics::commitState () - failed in base class";
    }   
	for (int i = 0; i < numExternalNodes; i++) {
		retVal = thePDNodes[i]->getMaterial()->commitState();
	}	
    return retVal;
}
int
stateBasedPeridynamics::revertToLastCommit()
{
    int retVal = 0;	
	for (int i = 0; i < numExternalNodes; i++) {
		retVal = thePDNodes[i]->getMaterial()->revertToLastCommit();
	}
    return retVal;
}
int
stateBasedPeridynamics::revertToStart()
{
    int retVal = 0;	
	for (int i = 0; i < numExternalNodes; i++) {
		thePDNodes[i]->theNeiborPDNodes->Zero();
		thePDNodes[i]->theNeiborPDNodesReverse->Zero();
		thePDNodes[i]->weightFun->Zero();
		thePDNodes[i]->shapeK->Zero();
		thePDNodes[i]->invShapeK->Zero();
		thePDNodes[i]->shapeN->Zero();
		thePDNodes[i]->defomationGradientF->Zero();
		thePDNodes[i]->forceState->Zero();
		retVal = thePDNodes[i]->getMaterial()->revertToStart();
	}
    return retVal;
}
int
stateBasedPeridynamics::update()
{
	int err = 0;	
	static Matrix tempFTranspose(dimension, dimension);
	static Vector vectorE(dimension * 3 - 3);
	static Matrix strainE(dimension, dimension);
	eta->Zero();
	dispIJ.Zero();
	for (int i = 0; i < numExternalNodes; i++) {
		thePDNodes[i]->shapeN->Zero();
		temp->Zero();
		thePDNodes[i]->defomationGradientF->Zero();
		strainE.Zero();
		vectorE.Zero();
		tempTrialDispI.Zero();
		tempTrialDispJ.Zero();
		for (int j = 0; j <= thePDNodes[i]->neighborNum; j++) {
			const Vector &trialDispI = thePDNodes[i]->getTrialDisp();// trial disp. of i
			const Vector &trialDispJ = thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->getTrialDisp();// trial disp. of jth neibor of i
			tempTrialDispI(0) = trialDispI(0) + (*perturb[i])(0);//due to calculating element tangent using perturb method
			tempTrialDispI(1) = trialDispI(1) + (*perturb[i])(1);
			tempTrialDispJ(0) = trialDispJ(0) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(0);
			tempTrialDispJ(1) = trialDispJ(1) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(1);
			if (dimension == 3) {
				tempTrialDispI(2) = trialDispI(2) + (*perturb[i])(2);
				tempTrialDispJ(2) = trialDispJ(2) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(2);
			}
			dispIJ = tempTrialDispJ - tempTrialDispI;//to keep the accuracy of eta-xi
			*eta = *(thePDNodes[i]->xi)[j] + dispIJ;
			(*temp)(0, 0) = dispIJ(0)*(*(thePDNodes[i]->xi)[j])(0);
			(*temp)(0, 1) = dispIJ(0)*(*(thePDNodes[i]->xi)[j])(1);
			(*temp)(1, 0) = dispIJ(1)*(*(thePDNodes[i]->xi)[j])(0);
			(*temp)(1, 1) = dispIJ(1)*(*(thePDNodes[i]->xi)[j])(1);
			if (dimension == 3) {
				(*temp)(0, 2) = dispIJ(0)*(*(thePDNodes[i]->xi)[j])(2);
				(*temp)(1, 2) = dispIJ(1)*(*(thePDNodes[i]->xi)[j])(2);
				(*temp)(2, 0) = dispIJ(2)*(*(thePDNodes[i]->xi)[j])(0);
				(*temp)(2, 1) = dispIJ(2)*(*(thePDNodes[i]->xi)[j])(1);
				(*temp)(2, 2) = dispIJ(2)*(*(thePDNodes[i]->xi)[j])(2);
			}
			thePDNodes[i]->shapeN->addMatrix(1.0, *temp, thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->getVolumn()*(*(thePDNodes[i]->weightFun))(j));
		}
		thePDNodes[i]->defomationGradientF->addMatrixProduct(0.0, *(thePDNodes[i]->shapeN), *(thePDNodes[i]->invShapeK), 1.0);//this is part of N * K-1
		thePDNodes[i]->defomationGradientF->addMatrix(1.0, *ones, 1.0);//F, + another part of N * K-1		
		tempFTranspose.Zero();
		tempFTranspose.addMatrixTranspose(0.0, *(thePDNodes[i]->defomationGradientF), 1.0);
		tempFTranspose.addMatrix(0.5, *(thePDNodes[i]->defomationGradientF), 0.5);
		strainE.addMatrix(0.0, tempFTranspose, 1.0);
		strainE.addMatrix(1.0, *ones, -1.0);
		if (dimension == 2) {
			vectorE(0) = strainE(0, 0);
			vectorE(1) = strainE(1, 1);
			vectorE(2) = strainE(0, 1) + strainE(1, 0);  //engineering strain
		}
		else if (dimension == 3) {
			vectorE(0) = strainE(0, 0);
			vectorE(1) = strainE(1, 1);
			vectorE(2) = strainE(2, 2);
			vectorE(3) = strainE(0, 1) + strainE(1, 0);
			vectorE(4) = strainE(0, 2) + strainE(2, 0);   
			vectorE(5) = strainE(1, 2) + strainE(2, 1);
		}
		thePDNodes[i]->getMaterial()->setTrialStrain(vectorE);   //save the strain in mat	
		err = 0;
	}
    if (err!=0) {
			opserr << "error in setting strain at the PD node" << endln;
	}
	return 0;
}
const Matrix & stateBasedPeridynamics::getTangentStiff(void)
{
		if (theTangent == 0) {
			theTangent = new Matrix(numExternalNodes*dimension, numExternalNodes*dimension);
		}
		theTangent->Zero();
		static Vector tempResistingForce(numExternalNodes*dimension);
		for (int i = 0; i < numExternalNodes*dimension; i++) {
			tempResistingForce(i) = (*P)(i);
		}
		double perturbValue = 1e-11;
		for (int i = 0; i < numExternalNodes; i++) {
			perturb[i]->Zero();
			const Vector &trialDispI = thePDNodes[i]->getTrialDisp();
			for (int m = 0; m < dimension; m++) {
				if (abs(trialDispI(m)) > 1e-6) {					
					perturbValue = trialDispI(m)*1e-3;
				}				
				(*perturb[i])(m) += perturbValue;
				this->update();
				this->getResistingForce();
				for (int j = 0; j < numExternalNodes*dimension; j++) {
					(*theTangent)(j, dimension*i + m) = ((*P)(j) - tempResistingForce(j)) / perturbValue;
				}
				perturb[i]->Zero();
			}
		this->update();
		this->getResistingForce();
		Ki = theTangent;   
		}		  
	return *theTangent;
}
const Matrix & stateBasedPeridynamics::getInitialStiff(void)
{
	if (Ki != 0) {
		return *Ki;
	}
	else {
		Ki = new Matrix(numExternalNodes*dimension, numExternalNodes*dimension);    // matrix to return stiff
		Ki->Zero();
		*Ki = this->getTangentStiff();
		return *Ki;
	}
}
const Vector & stateBasedPeridynamics::getResistingForce(void)
{	
	this->calculateForceStateT();
	if (P == 0) {
		P = new Vector(numExternalNodes*dimension);		
	}
	P->Zero();
	static Vector tempP(dimension);	
	static Vector tempForceStateTij(dimension);
	static Vector tempForceStateTji(dimension);
	static Vector tempRealP(dimension);
	for (int i = 0; i < numExternalNodes; i++) {
		tempForceStateTij.Zero();
		tempForceStateTji.Zero();
		tempP.Zero();
		tempRealP.Zero();
		for (int j = 0; j <= thePDNodes[i]->neighborNum; j++) {			
			tempForceStateTij(0) = (*(thePDNodes[i]->forceState))(j, 0);
			tempForceStateTij(1) = (*(thePDNodes[i]->forceState))(j, 1);
			tempForceStateTji(0) = (*(thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->forceState))((*(thePDNodes[i]->theNeiborPDNodesReverse))(j), 0);
			tempForceStateTji(1) = (*(thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->forceState))((*(thePDNodes[i]->theNeiborPDNodesReverse))(j), 1);
			if (dimension == 3) {
				tempForceStateTij(2) = (*(thePDNodes[i]->forceState))(j, 2);
				tempForceStateTji(2) = (*(thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->forceState))((*(thePDNodes[i]->theNeiborPDNodesReverse))(j), 2);
			}
			tempRealP.addVector(1.0, tempForceStateTji - tempForceStateTij, 1.0);
		}
			(*P)(i*dimension) = tempRealP(0);
			(*P)(i*dimension + 1) = tempRealP(1);
			if (dimension == 3) {
				(*P)(i*dimension + 2) = tempRealP(2);
			}
	}
	return *P;
}
int
stateBasedPeridynamics::sendSelf(int commitTag, Channel &theChannel)
{
	return 0;
}
int
stateBasedPeridynamics::recvSelf(int commitTag,Channel &theChannel,FEM_ObjectBroker &theBroker)
{
  return 0;
}
void
stateBasedPeridynamics::Print(OPS_Stream &s, int flag)
{
  
}
int
stateBasedPeridynamics::displaySelf(Renderer &theViewer, int displayMode, float fact, const char **modes, int numMode)
{
    return 0;
}
void
stateBasedPeridynamics::updatedHorizon() {
	for (int i = 0; i < numExternalNodes-1; i++){		
		for (int j = i+1; j < numExternalNodes; j++){
			tempCrdJ.Zero();
			const Vector &crdsI = thePDNodes[i]->getCrds(); 
			const Vector &crdsJ = thePDNodes[j]->getCrds(); 
			if ((crdsI - crdsJ).Norm() < horizonSize ) {
				thePDNodes[i]->neighborNum = thePDNodes[i]->neighborNum + 1;
				thePDNodes[j]->neighborNum = thePDNodes[j]->neighborNum + 1;
				(thePDNodes[i]->xi)[thePDNodes[i]->neighborNum] = new Vector(dimension);
				*(thePDNodes[i]->xi)[thePDNodes[i]->neighborNum] = crdsJ - crdsI;
				(thePDNodes[j]->xi)[thePDNodes[j]->neighborNum] = new Vector(dimension);
				*(thePDNodes[j]->xi)[thePDNodes[j]->neighborNum] = crdsI - crdsJ;
				(*(thePDNodes[i]->theNeiborPDNodes))(thePDNodes[i]->neighborNum) = j;				
				(*(thePDNodes[j]->theNeiborPDNodes))(thePDNodes[j]->neighborNum) = i;
				(*(thePDNodes[i]->theNeiborPDNodesReverse))(thePDNodes[i]->neighborNum) = thePDNodes[j]->neighborNum;		
				(*(thePDNodes[j]->theNeiborPDNodesReverse))(thePDNodes[j]->neighborNum) = thePDNodes[i]->neighborNum;
			}
		}		
	}
}
void
stateBasedPeridynamics::calculateWeight() {
			double coef;
			double pi = 3.141592653589793238462643383279502884197169399;
			if (dimension == 2) {			
				coef = 15.0 / 7.0 / pi / pow(horizonSize, 2);
				//coef = 1.0;
			}
			else if (dimension == 3) {				
				coef = 3.0 / 2.0 / pi / pow(horizonSize, 3);
				//coef = 1.0;
			}
			opserr.setPrecision(16);
			double win = 0.0;
			for (int i = 0; i < numExternalNodes; i++) {
				thePDNodes[i]->weightFun->resize(thePDNodes[i]->neighborNum);
				thePDNodes[i]->weightFun->Zero();
				for (int j = 0; j <= thePDNodes[i]->neighborNum; j++) {
					win = (thePDNodes[i]->xi)[j]->Norm();					  
					win = win / horizonSize;
					if (win <= 0.5) {
						(*(thePDNodes[i]->weightFun))(j) = coef * (2.0 / 3.0 - 4 * pow(win, 2) + 4 * pow(win, 3));
					}
					else if (win <= 1.0) {
						(*(thePDNodes[i]->weightFun))(j) = coef * 4.0 / 3.0 * pow(1.0 - win, 3);
					}  
					else if (win > 1.0) {
						(*(thePDNodes[i]->weightFun))(j) = 0.0;
					}
				}
			}
}
void 
stateBasedPeridynamics::calculateShapeFunK() {
	eta->Zero();
	for (int i = 0; i < numExternalNodes; i++) {
		thePDNodes[i]->shapeK->Zero();		
		temp->Zero();	
		thePDNodes[i]->invShapeK->Zero();
		for (int j = 0; j <= thePDNodes[i]->neighborNum; j++) {		
			(*temp)(0, 0) = (*(thePDNodes[i]->xi)[j])(0)*(*(thePDNodes[i]->xi)[j])(0);
			(*temp)(0, 1) = (*(thePDNodes[i]->xi)[j])(0)*(*(thePDNodes[i]->xi)[j])(1);
			(*temp)(1, 0) = (*(thePDNodes[i]->xi)[j])(1)*(*(thePDNodes[i]->xi)[j])(0);
			(*temp)(1, 1) = (*(thePDNodes[i]->xi)[j])(1)*(*(thePDNodes[i]->xi)[j])(1);			
			if (dimension == 3) {
				(*temp)(0, 2) = (*(thePDNodes[i]->xi)[j])(0)*(*(thePDNodes[i]->xi)[j])(2);
				(*temp)(1, 2) = (*(thePDNodes[i]->xi)[j])(1)*(*(thePDNodes[i]->xi)[j])(2);
				(*temp)(2, 0) = (*(thePDNodes[i]->xi)[j])(2)*(*(thePDNodes[i]->xi)[j])(0);
				(*temp)(2, 1) = (*(thePDNodes[i]->xi)[j])(2)*(*(thePDNodes[i]->xi)[j])(1);
				(*temp)(2, 2) = (*(thePDNodes[i]->xi)[j])(2)*(*(thePDNodes[i]->xi)[j])(2);
			}			
			thePDNodes[i]->shapeK->addMatrix(1.0, *temp, thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->getVolumn()*(*(thePDNodes[i]->weightFun))(j));
		}
		thePDNodes[i]->shapeK->Invert(*(thePDNodes[i]->invShapeK));
	}
}
void 
stateBasedPeridynamics::calculateForceStateT() {
	static Vector tempStress(3 * dimension - 3);
	static Vector tempForceState(dimension);
	static Vector tempForceState2(dimension);  
	static Matrix firstPKRealStress(dimension, dimension);
	double pi = 3.141592653589793238462643383279502884197169399;
	double detF;
	Matrix Ft(dimension, dimension);
	Matrix invFt(dimension, dimension);
	eta->Zero();
	dispIJ.Zero();
	for (int i = 0; i < numExternalNodes; i++) {
		tempStress = thePDNodes[i]->getMaterial()->getStress();
		if (dimension == 2) {
			(*thePDNodes[i]->cauchyStress)(0) = tempStress(0);   //try other method(*thePDNodes[i]->cauchyStress)-> thePDNodes[i]->getMaterial()->getStress();
			(*thePDNodes[i]->cauchyStress)(1) = tempStress(1);
			(*thePDNodes[i]->cauchyStress)(2) = tempStress(2);
		}
		else if (dimension == 3) {
			(*thePDNodes[i]->cauchyStress)(0) = tempStress(0);
			(*thePDNodes[i]->cauchyStress)(1) = tempStress(1);
			(*thePDNodes[i]->cauchyStress)(2) = tempStress(2);
			(*thePDNodes[i]->cauchyStress)(3) = tempStress(3);
			(*thePDNodes[i]->cauchyStress)(4) = tempStress(4);
			(*thePDNodes[i]->cauchyStress)(5) = tempStress(5);
		}
	}
	for (int i = 0; i < numExternalNodes; i++) {
		tempTrialDispI.Zero();
		tempTrialDispJ.Zero();
		firstPKRealStress.Zero();
		thePDNodes[i]->forceState->resize(thePDNodes[i]->maxNumofHorizonNodes, dimension);
		thePDNodes[i]->forceState->Zero();
		tempForceState.Zero();
		tempForceState2.Zero();
		temp->Zero();
		if (dimension == 2) {
			(*temp)(0, 0) = (*thePDNodes[i]->cauchyStress)(0);
			(*temp)(1, 1) = (*thePDNodes[i]->cauchyStress)(1);
			(*temp)(0, 1) = (*thePDNodes[i]->cauchyStress)(2);
			(*temp)(1, 0) = (*temp)(0, 1);
			Ft.addMatrixTranspose(0.0, *(thePDNodes[i]->defomationGradientF), 1.0);  //FT
			Ft.Invert(invFt);
			detF = (*(thePDNodes[i]->defomationGradientF))(0, 0)*(*(thePDNodes[i]->defomationGradientF))(1, 1) - (*(thePDNodes[i]->defomationGradientF))(0, 1)*(*(thePDNodes[i]->defomationGradientF))(1, 0);
			if (abs(detF) < 1e-14) {
				opserr << " detF is zero, something wrong!" << endln;
				exit(-1);
			}
			firstPKRealStress.addMatrixProduct(0.0, *temp, invFt, detF);  //firstPK stress, not symmetry, so it need to be converted to matrix form		
		}
		else if (dimension == 3) {
			(*temp)(0, 0) = (*thePDNodes[i]->cauchyStress)(0);
			(*temp)(1, 1) = (*thePDNodes[i]->cauchyStress)(1);
			(*temp)(2, 2) = (*thePDNodes[i]->cauchyStress)(2);
			(*temp)(0, 1) = (*thePDNodes[i]->cauchyStress)(3);
			(*temp)(0, 2) = (*thePDNodes[i]->cauchyStress)(4);
			(*temp)(1, 2) = (*thePDNodes[i]->cauchyStress)(5);
			(*temp)(1, 0) = (*temp)(0, 1);
			(*temp)(2, 0) = (*temp)(0, 2);
			(*temp)(2, 1) = (*temp)(1, 2);
			Ft.addMatrixTranspose(0.0, *(thePDNodes[i]->defomationGradientF), 1.0);  //FT
			Ft.Invert(invFt);
			detF = (*(thePDNodes[i]->defomationGradientF))(0, 0)*(*(thePDNodes[i]->defomationGradientF))(1, 1)*(*(thePDNodes[i]->defomationGradientF))(2, 2) + (*(thePDNodes[i]->defomationGradientF))(0, 1)*(*(thePDNodes[i]->defomationGradientF))(1, 2)*(*(thePDNodes[i]->defomationGradientF))(2, 0)
				+ (*(thePDNodes[i]->defomationGradientF))(0, 2)*(*(thePDNodes[i]->defomationGradientF))(1, 0)*(*(thePDNodes[i]->defomationGradientF))(2, 1) - ((*(thePDNodes[i]->defomationGradientF))(0, 0)*(*(thePDNodes[i]->defomationGradientF))(1, 2)*(*(thePDNodes[i]->defomationGradientF))(2, 1))
				- ((*(thePDNodes[i]->defomationGradientF))(0, 1)*(*(thePDNodes[i]->defomationGradientF))(1, 0)*(*(thePDNodes[i]->defomationGradientF))(2, 2)) - ((*(thePDNodes[i]->defomationGradientF))(0, 2)*(*(thePDNodes[i]->defomationGradientF))(1, 1)*(*(thePDNodes[i]->defomationGradientF))(2, 0));		
			if (abs(detF) < 1e-14) {
				opserr << " detF is zero, there must be something wrong!!" << endln;
			}
			firstPKRealStress.addMatrixProduct(0.0, *temp, invFt, detF);
		}
		for (int j = 0; j <= thePDNodes[i]->neighborNum; j++) {
			const Vector &trialDispI = thePDNodes[i]->getTrialDisp();// trial disp. of i
			const Vector &trialDispJ = thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->getTrialDisp();// trial disp. of jth neibor of i
			tempTrialDispI(0) = trialDispI(0) + (*perturb[i])(0);//due to calculating element tangent using perturb method
			tempTrialDispI(1) = trialDispI(1) + (*perturb[i])(1);
			tempTrialDispJ(0) = trialDispJ(0) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(0);
			tempTrialDispJ(1) = trialDispJ(1) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(1);
			if (dimension == 3) {
				tempTrialDispI(2) = trialDispI(2) + (*perturb[i])(2);
				tempTrialDispJ(2) = trialDispJ(2) + (*perturb[(*(thePDNodes[i]->theNeiborPDNodes))(j)])(2);
			}
			tempForceState2.addMatrixVector(0.0, *(thePDNodes[i]->invShapeK), *(thePDNodes[i]->xi)[j], (*(thePDNodes[i]->weightFun))(j)*thePDNodes[(*(thePDNodes[i]->theNeiborPDNodes))(j)]->getVolumn()*thePDNodes[i]->getVolumn());  //Tab=Tabsilling*Vb*Va
			tempForceState.addMatrixVector(0.0, firstPKRealStress, tempForceState2, 1.0);
			(*(thePDNodes[i]->forceState))(j, 0) = tempForceState(0);
			(*(thePDNodes[i]->forceState))(j, 1) = tempForceState(1);
			if (dimension == 3) {
				(*(thePDNodes[i]->forceState))(j, 2) = tempForceState(2);
			}
		}
	}	
}
Response*
stateBasedPeridynamics::setResponse(const char **argv, int argc,
	OPS_Stream &output)
{
	Response *theResponse = 0;
	if (strcmp(argv[0], "force") == 0 || strcmp(argv[0], "forces") == 0) { 
		theResponse = new ElementResponse(this, 1, *P);
	}
	else if (strcmp(argv[0], "strain") == 0 || strcmp(argv[0], "strains") == 0) {
		theResponse = new ElementResponse(this, 2, Vector(numExternalNodes*(3 * dimension - 3)));  
	}
	else if (strcmp(argv[0], "stress") == 0 || strcmp(argv[0], "stresses") == 0) {
		theResponse = new ElementResponse(this, 3, Vector(numExternalNodes*(3 * dimension - 3))); 
	}
	else if (strcmp(argv[0], "material") == 0 || strcmp(argv[0], "integrPoint") == 0) {
		int pointNum = atoi(argv[1]);		
		output.tag("PDNode");
        output.attr("number", pointNum);
		theResponse = thePDNodes[pointNum-1]->getMaterial()->setResponse(&argv[2], argc - 2, output);
		output.endTag(); 
	}
	return theResponse;
}
int
stateBasedPeridynamics::getResponse(int responseID, Information &eleInfo)
{
	if (responseID == 1) {
		return eleInfo.setVector(*P);
	}
	else if (responseID == 2) {
		static Vector strains(numExternalNodes*(3 * dimension - 3));
		int cnt = 0;
		for (int i = 0; i < numExternalNodes; i++) {
			const Vector &epsilon = thePDNodes[i]->getMaterial()->getStrain();  //2d,return 3*1; 3d, return 6*1
			if (dimension == 2) {
				strains(cnt) = epsilon(0);
				strains(cnt + 1) = epsilon(1);
				strains(cnt + 2) = epsilon(2);
				cnt += 3;			   
			}
			else if (dimension == 3) {
				strains(cnt) = epsilon(0);
				strains(cnt + 1) = epsilon(1);
				strains(cnt + 2) = epsilon(2);
				strains(cnt + 3) = epsilon(3);
				strains(cnt + 4) = epsilon(4);
				strains(cnt + 5) = epsilon(5);
				cnt += 6;
			}
		}
		return eleInfo.setVector(strains);
	}
	else if (responseID == 3) {
		static Vector stresses(numExternalNodes*(3 * dimension - 3));
		int cnt = 0;
		for (int i = 0; i < numExternalNodes; i++) {
			const Vector &tempStress = *thePDNodes[i]->cauchyStress;
			if (dimension == 2) {
				stresses(cnt) = tempStress(0);
				stresses(cnt + 1) = tempStress(1);
				stresses(cnt + 2) = tempStress(2);
				cnt += 3;
			}
			else if (dimension == 3) {
				stresses(cnt) = tempStress(0);
				stresses(cnt + 1) = tempStress(1);
				stresses(cnt + 2) = tempStress(2);
				stresses(cnt + 3) = tempStress(3);
				stresses(cnt + 4) = tempStress(4);
				stresses(cnt + 5) = tempStress(5);
				cnt += 6;
			}
		}
		return eleInfo.setVector(stresses);
	}
	else
		return 0;
}
const Matrix&
stateBasedPeridynamics::getMass()
{
	massK = Matrix(numExternalNodes*dimension, numExternalNodes*dimension);
	massK.Zero();
	return massK;
}
int
stateBasedPeridynamics::setParameter(const char **argv, int argc, Parameter &param)
{
		return -1;
}

int
stateBasedPeridynamics::updateParameter(int parameterID, Information &info)
{
		return -1;
}
const Vector&
stateBasedPeridynamics::getResistingForceIncInertia()
{
	double sum = 0.0;
	Vector rhoi(numExternalNodes);
	Vector tempP = this->getResistingForce();
	for (int i = 0; i < numExternalNodes; i++) {
		rhoi(i) = thePDNodes[i]->getMaterial()->getRho();
		sum += rhoi(i);
	}
	if (sum == 0.0) {
		this->getResistingForce();
		if (betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
			tempP += this->getRayleighDampingForces();
		return tempP;
	}
	Vector udotdot(numExternalNodes*dimension); 
	Vector tempAccel(dimension);
	for (int i = 0; i < numExternalNodes; i++) {
		tempAccel = thePDNodes[i]->getTrialAccel();
		if (dimension == 3) {
			udotdot(i*dimension) = tempAccel(0);
			udotdot(i*dimension + 1) = tempAccel(1);
			udotdot(i*dimension + 2) = tempAccel(2);
		}
		else {
			udotdot(i*dimension) = tempAccel(0);
			udotdot(i*dimension + 1) = tempAccel(1);
		}
	}
	this->getResistingForce();
	Matrix tempMass = this->getMass();
	tempP.addMatrixVector(1.0, tempMass, udotdot, 1.0);
	if (alphaM != 0.0 || betaK != 0.0 || betaK0 != 0.0 || betaKc != 0.0)
		tempP += this->getRayleighDampingForces();
	return tempP;
}
