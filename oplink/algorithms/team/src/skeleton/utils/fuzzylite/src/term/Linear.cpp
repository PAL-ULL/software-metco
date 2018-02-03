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


#include "fl/term/Linear.h"

#include <cstdarg>

namespace fl {

    Linear::Linear(const std::string& name,
            const std::vector<scalar>& coefficients,
            const std::vector<InputVariable*>& inputVariables)
    : Term(name), coefficients(coefficients) {
        this->inputVariables = std::vector<const InputVariable*>
                (inputVariables.begin(), inputVariables.end());
    }

    Linear::~Linear() { }

    template <typename T>
    Linear* Linear::create(const std::string& name,
            const std::vector<InputVariable*>& inputVariables,
            T firstCoefficient, ...)  {
        std::vector<scalar> coefficients;
        coefficients.push_back(firstCoefficient);

        va_list args;
        va_start(args, firstCoefficient);
        for (std::size_t i = 0; i < inputVariables.size(); ++i) {
            coefficients.push_back((scalar) va_arg(args, T));
        }
        va_end(args);
        return new Linear(name, coefficients, inputVariables);
    }

    template FL_EXPORT Linear* Linear::create(const std::string& name,
            const std::vector<InputVariable*>& inputVariables,
            scalar firstCoefficient, ...) throw (fl::Exception);

    template FL_EXPORT Linear* Linear::create(const std::string& name,
            const std::vector<InputVariable*>& inputVariables,
            int firstCoefficient, ...) throw (fl::Exception);

    std::string Linear::className() const {
        return "Linear";
    }

    Linear* Linear::copy() const {
        return new Linear(*this);
    }

    scalar Linear::membership(scalar x) const {
        (void) x;
        if (coefficients.size() != inputVariables.size() + 1) {
            std::ostringstream ss;
            ss << "[linear term] the number of coefficients <" << coefficients.size() << "> "
                    "need to be equal to the number of input variables "
                    "<" << inputVariables.size() << "> plus a constant c "
                    "(i.e. ax + by + c)";
            throw fl::Exception(ss.str(), FL_AT);
        }
        scalar result = 0;
        for (std::size_t i = 0; i < inputVariables.size(); ++i) {
            result += coefficients.at(i) * inputVariables.at(i)->getInput();
        }
        if (coefficients.size() > inputVariables.size()) {
            result += coefficients.back();
        }

        return result;
    }

    std::string Linear::toString() const {
        std::ostringstream ss;
        ss << className() << " (";
        for (std::size_t i = 0; i < coefficients.size(); ++i) {
            ss << fl::Op::str(coefficients.at(i));
            if (i < coefficients.size() - 1) {
                ss << ", ";
            }
        }
        ss << ")";

        return ss.str();
    }

    void Linear::set(const std::vector<scalar>& coefficients,
            const std::vector<InputVariable*>& inputVariables) throw (fl::Exception) {
        set(coefficients, std::vector<const InputVariable*>
                (inputVariables.begin(), inputVariables.end()));
    }

    void Linear::set(const std::vector<scalar>& coefficients,
            const std::vector<const InputVariable*>& inputVariables) throw (fl::Exception) {
        if (coefficients.size() != inputVariables.size() + 1) {
            std::ostringstream ss;
            ss << "[linear term] the number of coefficients <" << coefficients.size() << "> "
                    "need to be equal to the number of input variables "
                    "<" << inputVariables.size() << "> plus a constant c "
                    "(i.e. ax + by + c)";
            throw fl::Exception(ss.str(), FL_AT);
        }
        this->coefficients = coefficients;
        this->inputVariables = inputVariables;
    }
}
