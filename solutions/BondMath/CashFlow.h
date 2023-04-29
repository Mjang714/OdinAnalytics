#ifndef OPENANALYTICS_BONDMATH_CASHFLOW_H
#define OPENANALYTICS_BONDMATH_CASHFLOW_H

#include <string>

class CashFlow
{

private:
	std::string AccuralDate;
	double TimeToMaturity;
	double CashFlowPV;

public:
	CashFlow();
	//getter should not modify member variables
	std::string getAccuralDate() const;
	double getTimeToMaturity() const;
	double getCashFlowPV() const;

	void setAccuralDate(std::string& inputAcuuralDate);
	void setTimeToMaturity(double& inputTimeToMaturity);
	void setCashFlowPV(double& inputCashFlowPV);


};

#endif // !OPENANALYTICS_BONDMATH_BONDMATH_H
