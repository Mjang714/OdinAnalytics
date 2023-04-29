#include "CashFlow.h"

CashFlow::CashFlow() : AccuralDate("NONE"), TimeToMaturity(0.0), CashFlowPV(0.0)
{

}

std::string CashFlow::getAccuralDate() const
{
	return this->AccuralDate;
}

double CashFlow::getTimeToMaturity() const
{
	return this->TimeToMaturity;
}

double CashFlow::getCashFlowPV() const
{
	return this->CashFlowPV;
}

void CashFlow::setAccuralDate(std::string& inputAcuuralDate)
{
	this->AccuralDate = inputAcuuralDate;
}

void CashFlow::setTimeToMaturity(double& inputTimeToMaturity)
{
	this->TimeToMaturity = inputTimeToMaturity;
}

void CashFlow::setCashFlowPV(double& inputCashFlowPV)
{
	this->CashFlowPV = inputCashFlowPV;
}
