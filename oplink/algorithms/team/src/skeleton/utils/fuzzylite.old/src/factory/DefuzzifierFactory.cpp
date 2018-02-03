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
 * File:   DefuzzifierFactory.cpp
 * Author: jcrada
 * 
 * Created on 8 January 2013, 11:17 PM
 */

#include "fl/factory/DefuzzifierFactory.h"

#include "fl/defuzzifier/Centroid.h"
#include "fl/defuzzifier/MaximumDefuzzifier.h"
#include "fl/Exception.h"

namespace fl {

    DefuzzifierFactory::DefuzzifierFactory() { }

    DefuzzifierFactory::~DefuzzifierFactory() { }

    Defuzzifier* DefuzzifierFactory::create(const std::string& className, int divisions) {
        if (className == Centroid().className()) return new Centroid(divisions);
        if (className == SmallestOfMaximum().className()) return new SmallestOfMaximum(divisions);
        if (className == LargestOfMaximum().className()) return new LargestOfMaximum(divisions);
        if (className == MeanOfMaximum().className()) return new MeanOfMaximum(divisions);
        throw fl::Exception("[factory error] Defuzzifier of class <" + className + "> not recognized", FL_AT);
    }

    std::vector<std::string> DefuzzifierFactory::available() const {
        std::vector<std::string> result;
        result.push_back(Centroid().className());
        result.push_back(SmallestOfMaximum().className());
        result.push_back(LargestOfMaximum().className());
        result.push_back(MeanOfMaximum().className());
        return result;
    }

}
