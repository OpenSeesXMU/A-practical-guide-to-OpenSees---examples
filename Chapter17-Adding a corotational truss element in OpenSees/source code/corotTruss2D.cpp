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

// $Revision$
// $Date$
// $URL$


// Written: Lei Wang 
// Created: Aug 30, 2019
// Revision: A
//
// Description: This file contains the implementation for the corotTruss2D class.
//
// What: "@(#) corotTruss2D.C, revA"

#include <corotTruss2D.h>
#include <Information.h>
#include <Parameter.h>

#include <Domain.h>
#include <Node.h>
#include <Channel.h>
#include <FEM_ObjectBroker.h>
#include <UniaxialMaterial.h>
#include <Renderer.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <ElementResponse.h>

//#include <fstream>

// initialise the class wide variables

Matrix corotTruss2D::corotTruss2DM4(4, 4);
Vector corotTruss2D::corotTruss2DV4(4);
// constructor:
//  responsible for allocating the necessary space needed by each object
//  and storing the tags of the corotTruss2D end nodes.

#include <elementAPI.h>
#define OPS_Export 

OPS_Export void *
OPS_corotTruss2DElement()
{
	Element *theElement = 0;

	int numRemainingArgs = OPS_GetNumRemainingInputArgs();

	if (numRemainingArgs < 4) {
		opserr << "Invalid Args want: element corotTruss2D $tag $iNode $jNode $sectTag \n";
		opserr << " or: element corotTruss2D $tag $iNode $jNode $A $matTag\n";
		return 0;
	}


	int iData[3];
	double A = 0.0;
	int matTag = 0;
	int ndm = OPS_GetNDM();

	int numData = 3;
	if (OPS_GetInt(&numData, iData) != 0) {
		opserr << "WARNING invalid integer (tag, iNode, jNode) in element corotTruss2D " << endln;
		return 0;
	}

	numData = 1;
	if (OPS_GetDouble(&numData, &A) != 0) {
		opserr << "WARNING: Invalid A: element corotTruss2D " << iData[0] <<
			" $iNode $jNode $A $matTag\n";
		return 0;
	}

	numData = 1;
	if (OPS_GetInt(&numData, &matTag) != 0) {
		opserr << "WARNING: Invalid matTag: element corotTruss2D " << iData[0] <<
			" $iNode $jNode $A $matTag\n";
		return 0;
	}

	UniaxialMaterial *theUniaxialMaterial = OPS_GetUniaxialMaterial(matTag);

	if (theUniaxialMaterial == 0) {
		opserr << "WARNING: Invalid material not found element corotTruss2D " << iData[0] << " $iNode $jNode $A " <<
			matTag << "\n";
		return 0;
	}

	// now create the corotTruss2D
	theElement = new corotTruss2D(iData[0], ndm, iData[1], iData[2], *theUniaxialMaterial, A);

	if (theElement == 0) {
		opserr << "WARNING: out of memory: element corotTruss2D " << iData[0] <<
			" $iNode $jNode $A $matTag\n";
	}

	return theElement;
}

corotTruss2D::corotTruss2D(int tag, int dim,
	int Nd1, int Nd2,
	UniaxialMaterial &theMat,
	double a)
	:Element(tag, ELE_TAG_corotTruss2D),
	theMaterial(0), connectedExternalNodes(2),
	dimension(dim), numDOF(0), A(a),
	theMatrix(0), theVector(0),
	Lo(0.0)
{
	// get a copy of the material and check we obtained a valid copy
	theMaterial = theMat.getCopy();
	if (theMaterial == 0) {
		opserr << "FATAL corotTruss2D::corotTruss2D - " << tag <<
			"failed to get a copy of material with tag " << theMat.getTag() << endln;
		exit(-1);
	}

	// ensure the connectedExternalNode ID is of correct size & set values
	if (connectedExternalNodes.Size() != 2) {
		opserr << "FATAL corotTruss2D::corotTruss2D - " << tag << "failed to create an ID of size 2\n";
		exit(-1);
	}

	connectedExternalNodes(0) = Nd1;
	connectedExternalNodes(1) = Nd2;

	// set node pointers to NULL
	for (int i = 0; i < 2; i++)
		theNodes[i] = 0;

	cosX[0] = 0.0;
	cosX[1] = 0.0;
	cosX[2] = 0.0;

	cosBeta = 0.0;
	sinBeta = 0.0;
}

// constructor:
//   invoked by a FEM_ObjectBroker - blank object that recvSelf needs
//   to be invoked upon
corotTruss2D::corotTruss2D()
	:Element(0, ELE_TAG_corotTruss2D),
	theMaterial(0), connectedExternalNodes(2),
	dimension(0), numDOF(0),
	theMatrix(0), theVector(0),
	Lo(0.0), A(0.0)
{
	// ensure the connectedExternalNode ID is of correct size 
	if (connectedExternalNodes.Size() != 2) {
		opserr << "FATAL corotTruss2D::corotTruss2D - failed to create an ID of size 2\n";
		exit(-1);
	}

	for (int i = 0; i < 2; i++)
		theNodes[i] = 0;

	cosX[0] = 0.0;
	cosX[1] = 0.0;
	cosX[2] = 0.0;


}

//  destructor
//     delete must be invoked on any objects created by the object
//     and on the matertial object.
corotTruss2D::~corotTruss2D()
{
	// invoke the destructor on any objects created by the object
	// that the object still holds a pointer to
	if (theMaterial != 0)
		delete theMaterial;


}


int
corotTruss2D::getNumExternalNodes(void) const
{
	return 2;
}

const ID &
corotTruss2D::getExternalNodes(void)
{
	return connectedExternalNodes;
}

Node **
corotTruss2D::getNodePtrs(void)
{
	return theNodes;
}

int
corotTruss2D::getNumDOF(void)
{
	return numDOF;
}


// method: setDomain()
//    to set a link to the enclosing Domain and to set the node pointers.
//    also determines the number of dof associated
//    with the corotTruss2D element, we set matrix and vector pointers,
//    allocate space for t matrix, determine the length
//    and set the transformation matrix.
void
corotTruss2D::setDomain(Domain *theDomain)
{
	// check Domain is not null - invoked when object removed from a domain
	if (theDomain == 0) {
		theNodes[0] = 0;
		theNodes[1] = 0;
		Lo = 0;
		return;
	}

	// first set the node pointers
	int Nd1 = connectedExternalNodes(0);
	int Nd2 = connectedExternalNodes(1);
	theNodes[0] = theDomain->getNode(Nd1);
	theNodes[1] = theDomain->getNode(Nd2);

	// if can't find both - send a warning message
	if ((theNodes[0] == 0) || (theNodes[1] == 0)) {
		if (theNodes[0] == 0)
			opserr << "corotTruss2D::setDomain() - corotTruss2D" << this->getTag() << " node " << Nd1 <<
			"does not exist in the model\n";
		else
			opserr << "corotTruss2D::setDomain() - corotTruss2D" << this->getTag() << " node " << Nd2 <<
			"does not exist in the model\n";
		return;
	}


	// call the base class method
	this->DomainComponent::setDomain(theDomain);
	// now set the number of dof for element and set matrix and vector pointer

	numDOF = 4;
	theMatrix = &corotTruss2DM4;
	theVector = &corotTruss2DV4;

	// now determine the length, cosines and fill in the transformation
	// NOTE t = -t(every one else uses for residual calc)
	const Vector &end1Crd = theNodes[0]->getCrds();
	const Vector &end2Crd = theNodes[1]->getCrds();
	const Vector &end1Disp = theNodes[0]->getDisp();
	const Vector &end2Disp = theNodes[1]->getDisp();

	double dx = end2Crd(0) - end1Crd(0);
	double dy = end2Crd(1) - end1Crd(1);


	double iDispX = end2Disp(0) - end1Disp(0);
	double iDispY = end2Disp(1) - end1Disp(1);
	Lo = sqrt(dx*dx + dy * dy);
	Ln = Lo;
	if (Lo == 0.0) {
		opserr << "WARNING corotTruss2D::setDomain() - corotTruss2D " << this->getTag() << " has zero length\n";
		return;
	}
	cosX[0] = dx / Lo;
	cosX[1] = dy / Lo;
}




int
corotTruss2D::commitState()
{
	int retVal = 0;
	// call element commitState to do any base class stuff
	if ((retVal = this->Element::commitState()) != 0) {
		opserr << "corotTruss2D::commitState () - failed in base class";
	}
	retVal = theMaterial->commitState();
	return retVal;
}

int
corotTruss2D::revertToLastCommit()
{
	return theMaterial->revertToLastCommit();
}

int
corotTruss2D::revertToStart()
{
	return theMaterial->revertToStart();
}

int
corotTruss2D::update(void)
{
	// Nodal displacements
	const Vector &end1Disp = theNodes[0]->getTrialDisp();
	const Vector &end2Disp = theNodes[1]->getTrialDisp();
	const Vector &crdNode1 = theNodes[0]->getCrds();
	const Vector &crdNode2 = theNodes[1]->getCrds();
	Vector currentCrd1 = end1Disp + crdNode1;
	Vector currentCrd2 = end2Disp + crdNode2;

	Ln = sqrt(pow(currentCrd1(0) - currentCrd2(0), 2) + pow(currentCrd1(1) - currentCrd2(1), 2));
	double strain = (Ln - Lo) / Lo;
	double cosab = (currentCrd2(0) - currentCrd1(0)) / Ln;
	double sinab = (currentCrd2(1) - currentCrd1(1)) / Ln;
	Vector cossinab(2);
	cossinab(0) = cosab;
	cossinab(1) = sinab;
	Matrix trans(2, 2);
	trans.Zero();
	trans(0, 0) = cosX[0];
	trans(0, 1) = -cosX[1];
	trans(1, 0) = cosX[1];
	trans(1, 1) = cosX[0];
	Matrix transInv(2, 2);
	trans.Invert(transInv);
	Vector cosb(2);	
	cosb.Zero();
	cosb.addMatrixVector(1.0, transInv, cossinab,1.0);
	cosBeta = cosb(0);
	sinBeta = cosb(1);
	// Set material trial strain
	return theMaterial->setTrialStrain(strain);
}


const Matrix &
corotTruss2D::getTangentStiff(void)
{
	Matrix &kg = *theMatrix;
	kg.Zero();

	static Matrix kgm(4, 4);
	static Matrix kgt(4, 4);

	kgt.Zero();
	kgm.Zero();

	// Material stiffness
	//
	// Get material tangent
	double EA = A * theMaterial->getTangent();
	EA /= Lo;
	Matrix RBM = this->getRBM();
	kgm.addMatrixTransposeProduct(1.0, RBM, RBM, EA);  //kgm=RBM'*EA*RBM


	// Geometric stiffness
	//
	// Get material stress
	double q = A * theMaterial->getStress();
	double SL = q / Ln;
	Matrix RBMpartialU = this->getRBMTpartialU();

	kgt.addMatrix(0.0, RBMpartialU, SL);

	//local stiffness
	kg.addMatrix(0.0, kgm, 1.0);
	kg.addMatrix(1.0, kgt, 1.0);

	Matrix R = this->getR();
	//global one
	kg.addMatrixTripleProduct(0.0, R, kg, 1.0);
	return *theMatrix;
}

const Matrix &
corotTruss2D::getRBM(void)
{
	static Matrix rbm(1, 4);
	rbm.Zero();

	rbm(0, 0) = -cosBeta;
	rbm(0, 1) = -sinBeta;
	rbm(0, 2) = cosBeta;
	rbm(0, 3) = sinBeta;

	return rbm;
}

const Matrix &
corotTruss2D::getRBMTpartialU(void)
{
	static Matrix rbmm(4, 4);  //(4,4)
	rbmm.Zero();
	double s2 = pow(sinBeta, 2);
	double sc = sinBeta * cosBeta;
	double c2 = pow(cosBeta, 2);
	rbmm(0, 0) = s2;     rbmm(0, 1) = -sc;   rbmm(0, 2) = -s2;   rbmm(0, 3) = sc;
	rbmm(1, 0) = -sc; rbmm(1, 1) = c2;	rbmm(1, 2) = sc;	 rbmm(1, 3) = -c2;
	rbmm(2, 0) = -s2; rbmm(2, 1) = sc;	rbmm(2, 2) = s2;	 rbmm(2, 3) = -sc;
	rbmm(3, 0) = sc;  rbmm(3, 1) = -c2;	rbmm(3, 2) = -sc;	 rbmm(3, 3) = c2;

	return rbmm;
}
const Matrix &
corotTruss2D::getR(void)
{
	static Matrix r(4, 4);
	r.Zero();
	double cosx = cosX[0];
	double sinx = cosX[1];
	r(0, 0) = cosx;  r(1, 0) = -sinx;
	r(0, 1) = sinx;  r(1, 1) = cosx;
	r(2, 2) = cosx;  r(3, 2) = -sinx;
	r(2, 3) = sinx;  r(3, 3) = cosx;
	return r;
}



const Vector &
corotTruss2D::getResistingForce()
{

	double force = A * theMaterial->getStress();  //basic Q

	Vector transRBM(4);
	transRBM(0) = -cosBeta;
	transRBM(1) = -sinBeta;
	transRBM(2) = cosBeta;
	transRBM(3) = sinBeta;

	Matrix R = this->getR();
	//R = this->transT(R);
	Vector &F = *theVector;
	F.Zero();
	F.addMatrixTransposeVector(0.0, R, transRBM, force);

	return *theVector;
}



int
corotTruss2D::sendSelf(int commitTag, Channel &theChannel)
{
	int res;

	return 0;
}

int
corotTruss2D::recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker)
{

	return 0;
}

int
corotTruss2D::displaySelf(Renderer &theViewer, int displayMode, float fact,
	const char **displayModes, int numModes)
{
	int res = 0;

	return res;
}



void
corotTruss2D::Print(OPS_Stream &s, int flag)
{

}



Response*
corotTruss2D::setResponse(const char **argv, int argc, OPS_Stream &output)
{

	Response *theResponse = 0;

	output.tag("ElementOutput");
	output.attr("eleType", "corotTruss2D");
	output.attr("eleTag", this->getTag());
	output.attr("node1", connectedExternalNodes[0]);
	output.attr("node2", connectedExternalNodes[1]);

	//
	// we compare argv[0] for known response types for the corotTruss2D
	//


	if ((strcmp(argv[0], "force") == 0) || (strcmp(argv[0], "forces") == 0)
		|| (strcmp(argv[0], "globalForce") == 0) || (strcmp(argv[0], "globalForces") == 0)) {
		char outputData[10];
		int numDOFperNode = numDOF / 2;
		for (int i = 0; i < numDOFperNode; i++) {
			sprintf(outputData, "P1_%d", i + 1);
			output.tag("ResponseType", outputData);
		}
		for (int j = 0; j < numDOFperNode; j++) {
			sprintf(outputData, "P2_%d", j + 1);
			output.tag("ResponseType", outputData);
		}
		theResponse = new ElementResponse(this, 1, Vector(numDOF));

	}
	else if ((strcmp(argv[0], "axialForce") == 0) ||
		(strcmp(argv[0], "basicForce") == 0) ||
		(strcmp(argv[0], "localForce") == 0) ||
		(strcmp(argv[0], "basicForces") == 0)) {
		output.tag("ResponseType", "N");
		theResponse = new ElementResponse(this, 2, Vector(1));

	}
	else if (strcmp(argv[0], "defo") == 0 || strcmp(argv[0], "deformation") == 0 ||
		strcmp(argv[0], "deformations") == 0 || strcmp(argv[0], "basicDefo") == 0 ||
		strcmp(argv[0], "basicDeformation") == 0 || strcmp(argv[0], "basicDeformations") == 0) {

		output.tag("ResponseType", "U");
		theResponse = new ElementResponse(this, 3, Vector(1));

	}
	else if (strcmp(argv[0], "basicStiffness") == 0) {

		output.tag("ResponseType", "K");
		theResponse = new ElementResponse(this, 4, Matrix(1, 1));

		// a material quantity
	}
	else if (strcmp(argv[0], "material") == 0 || strcmp(argv[0], "-material") == 0) {

		theResponse = theMaterial->setResponse(&argv[1], argc - 1, output);
	}

	output.endTag();
	return theResponse;
}

int
corotTruss2D::getResponse(int responseID, Information &eleInfo)
{
	double strain, force;
	static Vector fVec(1);
	static Matrix kVec(1, 1);

	switch (responseID) {
	case 1:
		return eleInfo.setVector(this->getResistingForce());

	case 2:
		fVec(0) = A * theMaterial->getStress();
		return eleInfo.setVector(fVec);

	case 3:
		if (Lo == 0.0) {
			strain = 0.0;
		}
		else {
			strain = theMaterial->getStrain();
		}
		fVec(0) = Lo * strain;
		return eleInfo.setVector(fVec);

	case 4:
		force = 0.0;
		if (Lo > 0.0)
			force = theMaterial->getTangent();
		kVec(0, 0) = A * force / Lo;
		return eleInfo.setMatrix(kVec);

	default:
		return 0;
	}
}

const Matrix &
corotTruss2D::getInitialStiff(void)
{
	Matrix &K = *theMatrix;
	K.Zero();
	return *theMatrix;
}

const Matrix &
corotTruss2D::getDamp(void)
{
	Matrix &K = *theMatrix;
	K.Zero();
	return *theMatrix;

}


const Matrix &
corotTruss2D::getMass(void)
{
	Matrix &K = *theMatrix;
	K.Zero();
	return *theMatrix;

}