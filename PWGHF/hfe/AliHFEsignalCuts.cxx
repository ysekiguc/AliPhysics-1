/**************************************************************************
* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
*                                                                        *
* Author: The ALICE Off-line Project.                                    *
* Contributors are mentioned in the code where appropriate.              *
*                                                                        *
* Permission to use, copy, modify and distribute this software and its   *
* documentation strictly for non-commercial purposes is hereby granted   *
* without fee, provided that the above copyright notice appears in all   *
* copies and that both the copyright notice and this permission notice   *
* appear in the supporting documentation. The authors make no claims     *
* about the suitability of this software for any purpose. It is          *
* provided "as is" without express or implied warranty.                  *
**************************************************************************/
//
// Signal cuts
// Checks whether a particle (reconstructed or MC) is coming from MC Signal
// For more information see implementation file
//
// Autor:
//   Markus Fasel <M.Fasel@gsi.de>
//
#include <TClass.h>
#include <TMath.h>
#include <TParticle.h>
#include <TString.h>

#include "AliAODTrack.h"
#include "AliAODMCParticle.h"
#include "AliESDtrack.h"
#include "AliLog.h"
#include "AliMCEvent.h"
#include "AliMCParticle.h"
#include "AliVParticle.h"
#include "TClonesArray.h"

#include "AliHFEsignalCuts.h"
#include "AliHFEmcQA.h"

ClassImp(AliHFEsignalCuts)

//____________________________________________________________
AliHFEsignalCuts::AliHFEsignalCuts():
  AliAnalysisCuts(),
  fMC(NULL),
  fAODArrayMCInfo(NULL),
  fMCQA(NULL)
{
  //
  // Dummy constructor
  //
}

//____________________________________________________________
AliHFEsignalCuts::AliHFEsignalCuts(const Char_t *name, const Char_t *title):
  AliAnalysisCuts(name, title),
  fMC(NULL),
  fAODArrayMCInfo(NULL),
  fMCQA(NULL)
{
  //
  // Default constructor
  //
  fMCQA = new AliHFEmcQA;
  if(fMCQA) fMCQA->Init();
}

//____________________________________________________________
AliHFEsignalCuts::AliHFEsignalCuts(const AliHFEsignalCuts &ref):
  AliAnalysisCuts(ref),
  fMC(ref.fMC),
  fAODArrayMCInfo(NULL),
  fMCQA(ref.fMCQA)
{
  //
  // Copy constructor
  //
}

//____________________________________________________________
AliHFEsignalCuts &AliHFEsignalCuts::operator=(const AliHFEsignalCuts &ref){
  //
  // Assignment operator
  //
  if(this != &ref){
    //fMC = ref.fMC; 
    //fAODArrayMCInfo = ref.fAODArrayMCInfo;
    fMCQA = ref.fMCQA; 
  }
  return *this;
}

//____________________________________________________________
AliHFEsignalCuts::~AliHFEsignalCuts(){
  //
  // Destructor
  //
  if(fMCQA) delete fMCQA;
}

//____________________________________________________________
void AliHFEsignalCuts::SetMCEvent(AliMCEvent *mc){ 
  //
  // Set mc event
  //
  fMC = mc; 
  if(fMCQA) fMCQA->SetMCEvent(mc);
}

//____________________________________________________________
void AliHFEsignalCuts::SetMCAODInfo(TClonesArray *mcarray){ 
  //
  // Set mc array info
  //
  fAODArrayMCInfo = mcarray; 
  if(fMCQA) fMCQA->SetMCArray(mcarray);
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsSelected(TObject *o) {
  //
  // Define signal as electron coming from charm or beauty
  // @TODO: Implement setter so that also either of them can be defined
  // as signal alone
  

  return IsCharmElectron(o) || IsBeautyElectron(o);
/*  
  //saving time?
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources>0)printf("esources= %d\n",esources);
  if(esources == AliHFEmcQA::kDirectCharm || esources == AliHFEmcQA::kDirectBeauty || esources == AliHFEmcQA::kBeautyCharm)  // 1: direct D->e, 2: B->e 3: B->D->e
    return kTRUE;
  else
    return kFALSE;
*/

}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsCharmElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Charm
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA::kDirectCharm)  // 1: direct D->e
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsBeautyElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Beauty
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA::kDirectBeauty || esources == AliHFEmcQA::kBeautyCharm)  // 2: B->e 3: B->D->e
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsGammaElectron(const TObject * const o) const {
  //
  // Check for MC if the electron is coming from Gamma  
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources >= AliHFEmcQA::kGammaPi0 && esources <= AliHFEmcQA::kGammaRho0 )  // 4: conversion electrons
  //if(esources == AliHFEmcQA::kGammaPi0 || esources == AliHFEmcQA::kGammaEta || esources == AliHFEmcQA::kGammaOmega || esources == AliHFEmcQA::kGammaPhi || esources == AliHFEmcQA::kGammaEtaPrime || esources == AliHFEmcQA::kGammaRho0 )  // 4: conversion electrons
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsNonHFElectron(const TObject * const o) const {
  //
  // Check for MC if the electron is coming from NonHFE except for conversion
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA:: kPi0 || esources == AliHFEmcQA::kEta || esources == AliHFEmcQA::kOmega || esources == AliHFEmcQA::kPhi || esources == AliHFEmcQA::kEtaPrime || esources == AliHFEmcQA::kRho0)  // 4: conversion electrons
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsJpsiElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Charm
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA::kJpsi)  // 5: J/psi->ee
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsB2JpsiElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Charm
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA::kB2Jpsi)  // 6: B->Jpsi->ee
    return kTRUE;
  else
    return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsKe3Electron(const TObject * const o) const {
  //
  // Check if mother is coming from Charm
  //
  if(!dynamic_cast<const AliVParticle *>(o)) return kFALSE;
  Int_t esources = GetElecSource(dynamic_cast<const AliVParticle *>(o));
  if(esources == AliHFEmcQA::kKe3)  // 7: K->e
    return kTRUE;
  else
    return kFALSE;
}

/*
//____________________________________________________________
Bool_t AliHFEsignalCuts::IsCharmElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Charm
  //
  if(TMath::Abs(GetTrackPDG(dynamic_cast<const AliVParticle *>(o))) != 11) return kFALSE;
  Int_t motherpdg = TMath::Abs(GetMotherPDG(dynamic_cast<const AliVParticle *>(o)));
  AliDebug(1, Form("Mother PDG %d\n", motherpdg));

  if((motherpdg % 1000) / 100 == 4) return kTRUE;    // charmed meson, 3rd position in pdg code == 4
  if(motherpdg / 1000 == 4) return kTRUE;            // charmed baryon, 4th position in pdg code == 4
  AliDebug(1, "No Charm\n");
  return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsBeautyElectron(const TObject * const o) const {
  //
  // Check if mother is coming from Beauty
  //
  if(TMath::Abs(GetTrackPDG(dynamic_cast<const AliVParticle *>(o))) != 11) return kFALSE;
  Int_t motherpdg = TMath::Abs(GetMotherPDG(dynamic_cast<const AliVParticle *>(o)));
  AliDebug(1, Form("Mother PDG %d\n", motherpdg));

  if((motherpdg % 1000) / 100 == 5) return kTRUE;   // beauty meson, 3rd position in pdg code == 5
  if(motherpdg / 1000 == 5) return kTRUE;           // beauty baryon, 4th position in pdg code == 5   
  AliDebug(1, "No Beauty\n");
  return kFALSE;
}

//____________________________________________________________
Bool_t AliHFEsignalCuts::IsGammaElectron(const TObject * const o) const {
  //
  // Check for MC if the electron is coming from Gamma
  //
  if(TMath::Abs(GetTrackPDG(dynamic_cast<const AliVParticle *>(o))) != 11) return kFALSE;
  Int_t motherpdg = TMath::Abs(GetMotherPDG(dynamic_cast<const AliVParticle *>(o)));
  AliDebug(1, Form("Mother PDG %d\n", motherpdg));

  if(motherpdg!=22){
    AliDebug(1, "No Gamma");
    return kFALSE;
  } else { 
    AliDebug(1, "Gamma");
    return kTRUE;
  }
}
*/

//____________________________________________________________
Int_t AliHFEsignalCuts::GetMotherPDG(const AliVParticle * const track) const {
  //
  // Get Mother Pdg code for reconstructed respectively MC tracks
  // 
  TClass *type = track->IsA();  
  if((!fMC && (type == AliESDtrack::Class())) || (!fAODArrayMCInfo && (type == AliAODTrack::Class()))){
    AliDebug(1, "No MC Event Available\n");
    return 0;
  }
  const AliVParticle *motherParticle = NULL, *mctrack = NULL;
  Int_t label = TMath::Abs(track->GetLabel());
  if(type == AliESDtrack::Class()){
    // Reconstructed track
    if(label) mctrack = fMC->GetTrack(TMath::Abs(label));
    
  } 
  else if(type == AliAODTrack::Class()) {
    // MCParticle
    if(label && label < fAODArrayMCInfo->GetEntriesFast())
      mctrack = (AliVParticle *) fAODArrayMCInfo->At(label);
  }
  else {
    mctrack=track;
  }

  if(!mctrack) return 0;
  
  Int_t motherPDG = 0;
  if(TString(mctrack->IsA()->GetName()).CompareTo("AliMCParticle") == 0){
    // case MC Particle
    const AliMCParticle *esdmctrack = dynamic_cast<const AliMCParticle *>(mctrack);
    if(esdmctrack) motherParticle = fMC->GetTrack(esdmctrack->Particle()->GetFirstMother());
    if(motherParticle){
      const AliMCParticle *esdmcmother = dynamic_cast<const AliMCParticle *>(motherParticle);
      if(esdmcmother) motherPDG = TMath::Abs(esdmcmother->Particle()->GetPdgCode());
    }
  } else {
    // case AODMCParticle
    const AliAODMCParticle *aodmctrack = dynamic_cast<const AliAODMCParticle *>(mctrack);
    if(aodmctrack) {
      if(aodmctrack->GetMother() && aodmctrack->GetMother() < fAODArrayMCInfo->GetEntriesFast())
      motherParticle =  (AliVParticle *) fAODArrayMCInfo->At(aodmctrack->GetMother());
    }
    if(motherParticle){
      const AliAODMCParticle *aodmcmother = dynamic_cast<const AliAODMCParticle *>(motherParticle);
      if(aodmcmother) motherPDG = TMath::Abs(aodmcmother->GetPdgCode());
    }
  }
  return motherPDG;
}

//____________________________________________________________
Int_t AliHFEsignalCuts::GetTrackPDG(const AliVParticle * const track) const {
  //
  // Return PDG code of a particle itself
  //
  TClass *type = track->IsA();  
  if((!fMC && (type == AliESDtrack::Class())) || (!fAODArrayMCInfo && (type == AliAODTrack::Class()))){
    AliDebug(1, "No MC Event Available\n");
    return 0;
  }
  const AliVParticle *mctrack = NULL;
  Int_t label = TMath::Abs(track->GetLabel());
  if(type == AliESDtrack::Class()){
    // Reconstructed track
    if(label) mctrack = fMC->GetTrack(TMath::Abs(label));
    
  } 
  else if(type == AliAODTrack::Class()) {
    // MCParticle
    if(label && label < fAODArrayMCInfo->GetEntriesFast())
      mctrack =  (AliVParticle *) fAODArrayMCInfo->At(label);
  }
  else {
    mctrack=track;
  }

  if(!mctrack) return 0;
  
  TString mctype = mctrack->IsA()->GetName();
  Int_t trackPdg = 0;
  if(!mctype.CompareTo("AliMCParticle")){
    const AliMCParticle *esdmc = dynamic_cast<const AliMCParticle *>(mctrack);
    if(esdmc) trackPdg = esdmc->Particle()->GetPdgCode();
  } else {
    const AliAODMCParticle *aodmc = dynamic_cast< const AliAODMCParticle *>(mctrack);
    if(aodmc) trackPdg = aodmc->GetPdgCode();
  }
  return trackPdg;
}

//____________________________________________________________
Int_t AliHFEsignalCuts::GetElecSource(const AliVParticle * const track) const {
  //
  // Return PDG code of a particle itself
  //
  
  TClass *type = track->IsA();  
  if((!fMC && (type == AliESDtrack::Class())) || (!fAODArrayMCInfo && (type == AliAODTrack::Class()))){
    AliDebug(1, "No MC Event Available\n");
    return 0;
  }
  if(!fMCQA){
    AliDebug(1, "No MCQA Available\n");
    return 0;
  }
  if(!track){
    AliDebug(1, "Track not Available\n");
    return 0;
  }

  const AliVParticle *mctrack = NULL;
  TParticle *mcpart = NULL;
  Int_t label = TMath::Abs(track->GetLabel());
  //AliMCParticle *esdmcmother = NULL;
  if(type == AliESDtrack::Class()){
    // Reconstructed track
    if(label) mctrack = fMC->GetTrack(TMath::Abs(label));
    
  } 
  else if(type == AliAODTrack::Class()) {
    // MCParticle
    if(label && label < fAODArrayMCInfo->GetEntriesFast())
      mctrack = (AliVParticle *) fAODArrayMCInfo->At(label);
  }
  else {
    mctrack=track;
  }
  if(!mctrack) return 0;

  Int_t eSource = 0;
  if(mctrack->IsA() == AliMCParticle::Class()){
    const AliMCParticle *esdmc = dynamic_cast<const AliMCParticle *>(mctrack);
    if(esdmc){
      mcpart = esdmc->Particle();
      eSource=fMCQA->GetElecSource(mcpart);
/* // considering secondary pions
      if(eSource>=AliHFEmcQA::kGammaPi0) {  // conversion electron, be careful with the enum odering 
        Int_t glabel=TMath::Abs(esdmc->GetMother()); // gamma label
        if((esdmcmother= dynamic_cast<AliMCParticle *>(fMC->GetTrack(glabel)))){
          glabel=TMath::Abs(esdmcmother->GetMother()); // gamma's mother's label
          if((esdmcmother= dynamic_cast<AliMCParticle *>(fMC->GetTrack(glabel)))){
            if(glabel>fMC->GetNumberOfPrimaries()) eSource=AliHFEmcQA::kElse;
          }
        }
      }
      else if(eSource==AliHFEmcQA::kPi0 || (eSource>=AliHFEmcQA::kEta && eSource<=AliHFEmcQA::kRho0) ){ // nonHFE except for the conversion electron
        Int_t glabel=TMath::Abs(esdmc->GetMother());
        if((esdmcmother= dynamic_cast<AliMCParticle *>(fMC->GetTrack(glabel)))){
          if(glabel>fMC->GetNumberOfPrimaries()) eSource=AliHFEmcQA::kElse;
        }
      }
*/
    }
  } else {
    eSource=fMCQA->GetElecSource(mctrack);
  }
  return eSource;
}	
