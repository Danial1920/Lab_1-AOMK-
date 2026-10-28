/* CIDs for the Lab 2 calculator graph. */
#ifndef ID_ECO_LAB2_H
#define ID_ECO_LAB2_H
#include "IEcoBase1.h"
#include "IEcoCalculatorX.h"
#include "IEcoCalculatorY.h"
#include "IEcoLab1.h"
#include "IEcoLab2.h"
#define ECO_LAB2_GUID_LAST(n) {0x01,0x10,{0xD3,0xB3,0xE2,0xF1,0x8D,0x21,0x4A,0x6B,0x9A,0x40,0x44,0,0,0,0,n}}
#include "IdEcoCalculatorA.h"
#include "IdEcoCalculatorB.h"
#include "IdEcoCalculatorC.h"
#include "IdEcoCalculatorD.h"
#include "IdEcoCalculatorE.h"
static const UGUID CID_EcoLab2 = ECO_LAB2_GUID_LAST(0x65);
IEcoComponentFactory* GetCalculatorAFactory(void);
IEcoComponentFactory* GetCalculatorBFactory(void);
IEcoComponentFactory* GetCalculatorCFactory(void);
IEcoComponentFactory* GetCalculatorDFactory(void);
IEcoComponentFactory* GetCalculatorEFactory(void);
IEcoComponentFactory* GetEcoLab2Factory(void);
#endif
