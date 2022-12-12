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

#ifndef stateBasedPeridynamics_h
#define stateBasedPeridynamics_h
#ifndef _bool_h
#include "bool.h"
#endif
#include <Element.h>
#include <Matrix.h>
#include <Vector.h>
#include <ID.h>
#include <Truss.h>
#include <fstream>
#include <iostream>
#include <ostream>
#include <ID.h>
#include <PDNode.h>
class Node;
class UniaxialMaterial;
class Response;
class Element;
class stateBasedPeridynamics : public Element
{
  public:
	stateBasedPeridynamics(int tag, int ndm, int nd1, int nd2, double h, double dx,  double thickness);
    ~stateBasedPeridynamics();
    const char *getClassType(void) const {return "stateBasedPeridynamics";};
	ID connectedExternalNodes; // Tags of  nodes
    int getNumExternalNodes(void) const;
    const ID &getExternalNodes(void);
	//notice!!if pdnode??
	Node **getNodePtrs(void);
    int getNumDOF(void);
    void setDomain(Domain *theDomain);
    int commitState(void);
    int revertToLastCommit(void);
    int revertToStart(void);
    int update(void);
    const Matrix &getTangentStiff(void);
    const Matrix &getInitialStiff(void);    
	const Matrix &getMass(void);
    const Vector &getResistingForce(void);
	const Vector& getResistingForceIncInertia(void);
    int sendSelf(int commitTag, Channel &theChannel);
    int recvSelf(int commitTag, Channel &theChannel, FEM_ObjectBroker &theBroker);
    int displaySelf(Renderer &, int mode, float fact, const char **displayModes=0, int numModes=0);
    void Print(OPS_Stream &s, int flag =0);
    Response *setResponse(const char **argv, int argc, OPS_Stream &s);
    int getResponse(int responseID, Information &eleInformation);
	void calculateShapeFunK(); 
	int setParameter(const char **argv, int argc, Parameter &param);
	int updateParameter(int parameterID, Information &info);
	void calculateForceStateT();
	void updatedHorizon();
	void calculateWeight();
  protected:
    
  private:
	PDNode **thePDNodes;
	int dimension;
	double horizonSize; 
	int numExternalNodes;
	double s;   
	Matrix * theTangent;    
	Vector * P;  
	Matrix * Ki;
	Vector **perturb; 
	Vector *xi;
	Vector *eta;
	Matrix * ones;
	Matrix *temp;
	Vector tempTrialDispI;
	Vector tempTrialDispJ;
	Vector dispIJ;
	Matrix massK;
	Vector tempCrdJ;
	double dx;
	double t;
};
#endif

