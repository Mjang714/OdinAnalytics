#include <vector>
#include <string>

#ifndef OPENANALYTICS_BONDMATH_BONDMATH_H
#define OPENANALYTICS_BONDMATH_BONDMATH_H
    
    //TODO: make this enum class
    //enumerated type for payment schedules of bonds 
    // PA = Per Anum (annually), SA = Semi Annual, M = Monthly, Q = Quarterly
    enum CouponSchedule : int
    {
        PerAnum = 1,
        SemiAnum = 2, 
        Quarterly = 4,
        Monthly = 12
    };

    int convertPaymentStringToEnum(const std::string& payment_freq_input);

    std::vector<double> computeDiscountedCashflow(double yield, std::vector<double> maturitySchedule, std::vector<double> cashflowSchedule);
    std::vector<double> createCashFlowGen(double years, double principle, std::string paymentFreqStr, double couponRate);
    std::vector<double> createCashFlowGen(double years, double principle, int paymentFreq, double couponRate); 

    double compoundedToContRate(double n, int m);
    double contRateToCompounded(double n, int m);
    double firstOrderPrice(double yield, std::vector<double>maturitySchedule, std::vector<double> cashflowSchedule);
    double price(double yield, std::vector<double>maturitySchedule, std::vector<double> cashflowSchedule);
    double computeYieldHelper(double yield, double price, std::vector<double> maturityScheudle, std::vector<double> cashflowSchedule, double threshold);
    double computeYield(std::vector<double> maturityschedule, std::vector<double> cashflowSchedule, double bondPrice);
    double computeDuration(double yield, double bondPrice, std::vector<double> maturityScheudle, std::vector<double> cashflowSchedule);
    double computeFiniteDifference(double bondPrice, double bondDuration, double difference);
    double computeDV01(double bondPrice, double bondDuration);
	double getYield(double years, double couponRate, double bondPrice, int compounding);
    double getYield(double years, double couponRate, double bondPrice, std::string compoundingStr);
	double getDuration(double years, double couponRate, double bondPrice, int compounding);
    double getDuration(double years, double couponRate, double bondPrice, std::string compoundingStr);

#endif //OPENANALYTICS_BONDMATH_BONDMATH_H
