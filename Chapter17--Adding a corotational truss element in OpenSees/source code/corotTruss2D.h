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


#ifndef corotTruss2D_h
#define corotTruss2D_h


#include <Element.h>
#include <Matrix.h>

class Node;
class Channel;
class UniaxialMaterial;

class corotTruss2D : public Element
{
public:
	corotTruss2D(int tag, int dimension,
		int Nd1, int Nd2,
		UniaxialMaterial &theMaterial,
		double A);
	corotTruss2D();
	~corotTruss2D();
	const char *getClassType(void) const { return "corotTruss2D"; };
	int getNumExternalNodes(void) const;
	const ID &getExternalNodes(void);
	Node **getNodePtrs(void);
	int getNumDOF(void);
	void setDomain(Domain *theDomain);
	int commitState(void);
	int revertToLastCommit(void);
	int revertToStart(void);
	int update(void);
	const Matrix &getDamp(void);
	const Matrix &getMass(void);
	const Matrix &getInitialStiff(void);
	const Matrix &getTangentStiff(void);
	const Vector &getResistingForce(void);
	const Matrix &getRBM(void);
	const Matrix &getRBMTpartialU(void);
	const Matrix &getR(void);
	int sendSelf(int commitTag, Channel &theChannel);
	int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
	int displaySelf(Renderer &, int mode, float fact, const char **displayModes = 0, int numModes = 0);
	void Print(OPS_Stream &s, int flag = 0);
	Response *setResponse(const char **argv, int argc, OPS_Stream &s);
	int getResponse(int responseID, Information &eleInformation);

protected:

private:

	// private attributes - a copy for each object of the class
	UniaxialMaterial *theMaterial;  // pointer to a material
	ID  connectedExternalNodes;     // contains the tags of the end nodes
	int dimension;                  // corotTruss2D in 2 or 3d domain
	int numDOF;	                    // number of dof for corotTruss2D
	Matrix *theMatrix;  // pointer to objects matrix (a class wide Matrix)
	Vector *theVector;  // pointer to objects vector (a class wide Vector)
	double Lo;               // length of corotTruss2D based on undeformed configuration
	double Ln;
	double A;               // area of corotTruss2D
	double cosX[3];  // direction cosines
	Node *theNodes[2];
	double cosBeta;
	double sinBeta;
	// static data - single copy for all objects of the class	
	static Matrix corotTruss2DM4;   // class wide matrix for 4*4
	static Vector corotTruss2DV4;   // class wide Vector for size 4

};

#endif
