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

#ifndef VEINS_INET_INTERQUARTILERANGECALCULATOR_H_
#define VEINS_INET_INTERQUARTILERANGECALCULATOR_H_
#include <omnetpp.h>  // Include this instead of individual headers.

using namespace omnetpp;

namespace osm {

class InterquartileRangeCalculator: public cSimpleModule {
protected:
    virtual void initialize() override;
    virtual void finish() override;
    double calculateInterquartileRange(const std::vector<double> &data);
public:
    std::vector<double> findOutliers(const std::vector<double> &data);

};

}
;
#endif /* VEINS_INET_INTERQUARTILERANGECALCULATOR_H_ */
