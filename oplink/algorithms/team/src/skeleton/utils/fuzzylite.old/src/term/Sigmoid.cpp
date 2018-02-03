/*   Copyright 2013 Juan Rada-Vilela

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
 * Sigmoid.cpp
 *
 *  Created on: 30/11/2012
 *      Author: jcrada
 */

#include "fl/term/Sigmoid.h"

#include <cmath>
#include <sstream>

namespace fl {

    Sigmoid::Sigmoid(const std::string& name, scalar inflection, scalar slope)
    : Term(name), _inflection(inflection), _slope(slope){ }

    Sigmoid::~Sigmoid() { }

    std::string Sigmoid::className() const {
        return "Sigmoid";
    }

    Sigmoid* Sigmoid::copy() const {
        return new Sigmoid(*this);
    }

    scalar Sigmoid::membership(scalar x) const {
        if (fl::Op::isNan(x)) return std::numeric_limits<scalar>::quiet_NaN();
        return 1.0 / (1.0 + std::exp(-_slope * (x - _inflection)));
    }

    std::string Sigmoid::toString() const {
        std::ostringstream ss;
        ss << std::setprecision(FL_DECIMALS) << std::fixed;
        ss << className() << " (" << _inflection << ", " << _slope << ")";

        return ss.str();
    }

    void Sigmoid::setSlope(scalar a) {
        this->_slope = a;
    }

    scalar Sigmoid::getSlope() const {
        return this->_slope;
    }

    void Sigmoid::setInflection(scalar c) {
        this->_inflection = c;
    }

    scalar Sigmoid::getInflection() const {
        return this->_inflection;
    }

} 
