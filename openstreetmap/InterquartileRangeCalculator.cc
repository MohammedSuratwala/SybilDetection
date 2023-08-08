//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "InterquartileRangeCalculator.h"

namespace osm {
Define_Module(InterquartileRangeCalculator);

void InterquartileRangeCalculator::initialize() {
    // Sample data points (you can replace this with your data)
//    std::vector<double> data = {-10.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7, 7.8, 8.9, 9.0, 10.1};

//    double interquartileRange = calculateInterquartileRange(data);
//    std::cout  << "Interquartile Range: " << interquartileRange<<"\n";
//
//        std::vector<double> outliers = findOutliers(data);
//        EV << "Outliers: ";
//        if (!outliers.empty()) {
//            for (double outlier : outliers) {
//                std::cout  << outlier << " ";
//            }
//        } else {
    std::cout << "Inside iqr";
//        }
//        std::cout<<"\n";

}

double InterquartileRangeCalculator::calculateInterquartileRange(
        const std::vector<double> &data) {
    // First, sort the data in ascending order
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    // Calculate the first and third quartiles
    int n = sortedData.size();
    int q1Index = n / 4;
    int q3Index = (3 * n) / 4;

    double q1 =
            (n % 2 == 0) ?
                    (sortedData[q1Index - 1] + sortedData[q1Index]) / 2 :
                    sortedData[q1Index];
    double q3 =
            (n % 2 == 0) ?
                    (sortedData[q3Index - 1] + sortedData[q3Index]) / 2 :
                    sortedData[q3Index];

    // Calculate the interquartile range
    double interquartileRange = q3 - q1;

    return interquartileRange;
}

std::vector<double> InterquartileRangeCalculator::findOutliers(
        const std::vector<double> &data) {
    std::vector<double> outliers;
    double interquartileRange = calculateInterquartileRange(data);
    std::vector<double> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end());

    double q1 = sortedData[sortedData.size() / 4];
    double q3 = sortedData[(3 * sortedData.size()) / 4];

    double lowerBound = q1 - 1.5 * interquartileRange;
    double upperBound = q3 + 1.5 * interquartileRange;

    for (double value : data) {
        if (value < lowerBound) {
            outliers.push_back(value);
        }
    }
    return outliers;
}

void InterquartileRangeCalculator::finish() {

}
}
;
