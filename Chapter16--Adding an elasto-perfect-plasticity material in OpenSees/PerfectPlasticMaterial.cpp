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

// $Revision: 1.6 $
// $Date: 2006-08-15 00:41:05 $
// $Source: /usr/local/cvs/OpenSees/SRC/material/uniaxial/PerfectPlasticMaterial.cpp,v $

// Written: MHS
// Created: Aug 2001
//
// Description: This file contains the class implementation for 
// PerfectPlasticMaterial.

#include <PerfectPlasticMaterial.h>
#include <Vector.h>
#include <Channel.h>
#include <math.h>
#include <float.h>

PerfectPlasticMaterial::PerfectPlasticMaterial(int tag, double pE, double pStress0)
  :UniaxialMaterial(tag,MAT_TAG_PerfectPlasticMaterial)
{
	E = pE;
	Stress0 = pStress0;
	this->revertToStart();

}

PerfectPlasticMaterial::PerfectPlasticMaterial()
  :UniaxialMaterial(0,MAT_TAG_PerfectPlasticMaterial)

{

}

PerfectPlasticMaterial::~PerfectPlasticMaterial()
{

}

int 
PerfectPlasticMaterial::setTrialStrain(double strain, double strainRate)
{
	// elastic predictor
	trialStrain = strain;
	double dStrain = trialStrain - CStrain;
	trialStress = CStress + E * dStrain;
	trialTangent = E;

	// plastic corrector
	double eps = 1e-14;
	if (trialStress > Stress0)
	{
		trialStress = Stress0; trialTangent = eps;
	}
	if (trialStress < -Stress0)
	{
		trialStress = -Stress0; trialTangent = eps;
	}
	return 0;

}

double 
PerfectPlasticMaterial::getStress(void)
{
  return trialStress;
}

double 
PerfectPlasticMaterial::getTangent(void)
{
  return trialTangent;
}

double 
PerfectPlasticMaterial::getInitialTangent(void)
{
  // return the initial tangent
  return 0.0;
}

double 
PerfectPlasticMaterial::getStrain(void)
{
  return trialStrain;
}

int 
PerfectPlasticMaterial::commitState(void)
{
	//record commit value
	CStrain = trialStrain;
	CStress = trialStress;
	CTangent = trialTangent;
	return 0;


  return 0;
}

int 
PerfectPlasticMaterial::revertToLastCommit(void)
{
  trialStrain = CStrain;
  trialStress = CStress;
  trialTangent = CTangent;

  return 0;
}

int 
PerfectPlasticMaterial::revertToStart(void)
{
	trialStrain = 0.;
	trialStress = 0.0;
	trialTangent = 0.0;
	CStrain = 0.;
	CStress = 0.0;
	CTangent = 0.0;
	return 0;

}

UniaxialMaterial *
PerfectPlasticMaterial::getCopy(void)
{
	PerfectPlasticMaterial *theCopy = new PerfectPlasticMaterial(this->getTag(), E, Stress0);
	
	return theCopy;

}

int 
PerfectPlasticMaterial::sendSelf(int cTag, Channel &theChannel)
{
  return -1;
}

int 
PerfectPlasticMaterial::recvSelf(int cTag, Channel &theChannel, 
			      FEM_ObjectBroker &theBroker)
{
  return -1;
}

void 
PerfectPlasticMaterial::Print(OPS_Stream &s, int flag)
{
  s << "PerfectPlasticMaterial : " << this->getTag();

  return;
}

Response*
PerfectPlasticMaterial::setResponse(const char **argv, int argc,
	OPS_Stream &theOutput)
{
	Response *res = UniaxialMaterial::setResponse(argv, argc, theOutput);
	if (res != 0)      return res;
	else return 0;

}

int
PerfectPlasticMaterial::getResponse(int responseID, Information &matInfo)
{
	return UniaxialMaterial::getResponse(responseID, matInfo);
}
