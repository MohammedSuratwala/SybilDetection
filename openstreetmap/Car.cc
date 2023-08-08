//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <algorithm>
#include "Car.h"

namespace osm {

Define_Module(Driver);
Define_Module(RoutePlanner);


void Driver::initialize(int stage) {

    if (stage == 0) {
        osmModule = check_and_cast<OpenStreetMap*>(
                getModuleByPath(par("mapModule").stringValue()));
        routePlanner = check_and_cast<RoutePlanner*>(
                getModuleByPath(par("routePlannerModule").stringValue()));

        carFigure = createCarFigure();
        cCanvas *canvas = getSystemModule()->getCanvas();
        canvas->addFigure(carFigure);

        // Pass the speed values to the InterquartileRangeCalculator module
        double minSpeed = 8.0;
        double maxSpeed = 20.0;
        speedValues.clear();
        for (int i = 0; i < numSpeedValues * (1 - sybilRate); i++) {
            double randomSpeed = uniform(minSpeed, maxSpeed);
            speedValues.push_back(randomSpeed);
        }
        for (int i = 0; i < numSpeedValues * sybilRate; i++) {
            double randomSpeed = uniform(minSpeed - 50, maxSpeed);
            speedValues.push_back(randomSpeed);
        }
//        std::cout << "Actual Sybil Nodes:" << round(numSpeedValues * sybilRate)<< "\n";

    } else if (stage == 1) {
        p1 = getNextTarget();
        p2 = getNextTarget();
        t1 = simTime();
        t2 = t1 + computeTravelTime(p1, p2);
        scheduleAt(t2, new cMessage("timer"));
    }
}

cFigure* Driver::createCarFigure() {
    cRectangleFigure *f = new cRectangleFigure();
    f->setName("car");
    f->setFilled(true);
    f->setFillColor(
            cFigure::GOOD_LIGHT_COLORS[intrand(cFigure::NUM_GOOD_LIGHT_COLORS)]);
    f->setLineColor(
            cFigure::GOOD_DARK_COLORS[intrand(cFigure::NUM_GOOD_DARK_COLORS)]);
    f->setBounds( { -5, -2, 10, 4 });
    f->setAssociatedObject(getParentModule()); // the Car compound module
    f->setZIndex(10); // top
    return f;
}

cFigure::Point Driver::getNextTarget() {
    const Node *node = routePlanner->getNextTarget();
    return osmModule->toCanvas(node->lat, node->lon);
}

simtime_t Driver::computeTravelTime(const Point &from, const Point &to) {
    double dist = from.distanceTo(to);
    double v = dist / 5.0;
    v = std::max(std::min(v, 20.0), 8.0);
    return dist / v;
}

void Driver::handleMessage(cMessage *timer) {
    t1 = t2;
    p1 = p2;

    p2 = getNextTarget();
    t2 = t1 + computeTravelTime(p1, p2);

    scheduleAt(t2, timer);

}

void Driver::refreshDisplay() const {
    simtime_t t = simTime();
    Point p = p1 + (p2 - p1) * ((t - t1) / (t2 - t1));
    double heading = (p2 - p1).getAngle();
    carFigure->setTransform(cFigure::Transform().rotate(heading).translate(p));
}

void Driver::finish() {

    std::vector<double> outliers = iqrCalculator->findOutliers(speedValues);
//    std::cout << "Outliers: ";
//    if (!outliers.empty()) {
//        for (double outlier : outliers) {
//            std::cout << outlier << " ";
//        }
//    } else {
//        std::cout << "None";
//    }
//    std::cout << "\n";
//    std::cout << "Detected Sybil Nodes:" << outliers.size() << "\n";
    recordScalar("OutliersDetected", outliers.size());
}
//-----------------

void RoutePlanner::initialize() {
    osmModule = check_and_cast<OpenStreetMap*>(
            getModuleByPath(par("mapModule").stringValue()));

    // initial placement
    const auto &streets = osmModule->getStreetNetwork().streets;
    ASSERT(!streets.empty());
    way = streets[intrand(streets.size())];
    nodeIndex = intrand(way->nodes.size());
    downstream = intrand(2);
}

const Node* RoutePlanner::getNextTarget() {
    Enter_Method("getNextTarget()");
    const Node *currentNode = way->nodes[nodeIndex];
    const auto &intersections = osmModule->getStreetNetwork().intersections;
    auto it = intersections.find(currentNode);
    if (it == intersections.end()) {
        // no connecting roads: drive on, or turn around if we reached the end
        if (downstream) {
            if (nodeIndex < way->nodes.size() - 1)
                nodeIndex++; // drive straight
            else {
                downstream = false; // turn around
                nodeIndex--;
            }
        } else {
            if (nodeIndex > 0)
                nodeIndex--; // drive straight
            else {
                downstream = true; // turn around
                nodeIndex++;
            }
        }
    } else {
        // count alternatives (incl. turning around)
        const auto &connectingWayNodes = it->second;
        std::vector<WayNode> possibilities;
        for (const WayNode &wayNode : connectingWayNodes) {
            if (wayNode.nodeIndex != 0)
                possibilities.push_back(
                        { wayNode.way, wayNode.nodeIndex - 1 });
            if (wayNode.nodeIndex != wayNode.way->nodes.size() - 1)
                possibilities.push_back(
                        { wayNode.way, wayNode.nodeIndex + 1 });
        }

        // and choose a random one
        WayNode nextNode = possibilities[intrand(possibilities.size())];

        way = nextNode.way;
        nodeIndex = nextNode.nodeIndex;
        downstream =
                (nodeIndex > 0 && way->nodes[nodeIndex - 1] == currentNode) ?
                        true : false;
    }

    EV << "next: " << way->id << "#" << nodeIndex << endl;
    return way->nodes[nodeIndex];
}

}
;
//namespace

