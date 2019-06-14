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
 * File:   FisImporter.cpp
 * Author: jcrada
 *
 * Created on 22 December 2012, 5:54 PM
 */

#include "fl/imex/FisImporter.h"

#include "fl/Headers.h"

#include <sstream>
#include <iostream>


namespace fl {

    FisImporter::FisImporter() { }

    FisImporter::~FisImporter() { }

    std::string FisImporter::name() const {
        return "FisImporter";
    }

    Engine* FisImporter::fromString(const std::string& fis) const {
        Engine* engine = new Engine;

        engine->addHedge(new Any);
        engine->addHedge(new Extremely);
        engine->addHedge(new Not);
        engine->addHedge(new Seldom);
        engine->addHedge(new Somewhat);
        engine->addHedge(new Very);

        std::istringstream fisReader(fis);
        std::string line;
        int lineNumber = 0;

        std::vector<std::string> sections;
        while (std::getline(fisReader, line)) {
            ++lineNumber;
            line = Op::trim(line);
            line = fl::Op::findReplace(line, "'", "");
            if (line.empty() or line.at(0) == '#' or line.at(0) == '%')
                continue;

            if ("[System]" == line.substr(0, std::string("[System]").size())
                    or "[Input" == line.substr(0, std::string("[Input").size())
                    or "[Output" == line.substr(0, std::string("[Output").size())
                    or "[Rules]" == line.substr(0, std::string("[Rules]").size())) {
                sections.push_back(line);
            } else {
                if (sections.size() == 0) {
                    FL_LOG("[importer warning] ignoring line because it does not "
                            "belong to any section: " << line);
                } else {
                    sections.at(sections.size() - 1) += "\n" + line;
                }
            }
        }
        std::string andMethod, orMethod, impMethod, aggMethod, defuzzMethod;
        try {
            for (std::size_t i = 0; i < sections.size(); ++i) {
                if ("[System]" == sections.at(i).substr(0, std::string("[System]").size()))
                    importSystem(sections.at(i), engine,
                        andMethod, orMethod, impMethod, aggMethod, defuzzMethod);
                else if ("[Input" == sections.at(i).substr(0, std::string("[Input").size()))
                    importInput(sections.at(i), engine);
                else if ("[Output" == sections.at(i).substr(0, std::string("[Output").size()))
                    importOutput(sections.at(i), engine);
                else if ("[Rules]" == sections.at(i).substr(0, std::string("[Rules]").size()))
                    importRules(sections.at(i), engine);
                else
                    throw fl::Exception("[importer error] unable to parse section: "
                        + sections.at(i), FL_AT);
            }
            if (engine->numberOfInputVariables() == 0
                    and engine->numberOfOutputVariables() == 0
                    and (engine->numberOfRuleBlocks() == 0
                    or engine->getRuleBlock(0)->numberOfRules() == 0)) {
                std::ostringstream ex;
                ex << "[importer error] the FIS code introduced produces an empty engine";
                throw fl::Exception(ex.str(), FL_AT);
            }
            engine->configure(flTnorm(andMethod), flSnorm(orMethod),
                    flTnorm(impMethod), flSnorm(aggMethod),
                    flDefuzzifier(defuzzMethod));
        } catch (fl::Exception& ex) {
            delete engine;
            throw ex;
        }

        return engine;
    }

    void FisImporter::importSystem(const std::string& section, Engine * engine,
            std::string& andMethod, std::string& orMethod,
            std::string& impMethod, std::string& aggMethod,
            std::string& defuzzMethod) const {
        std::istringstream reader(section);
        std::string line;
        std::getline(reader, line); //ignore first line [System]
        while (std::getline(reader, line)) {
            std::vector<std::string> keyValue = fl::Op::split(line, "=");
            if (keyValue.size() != 2)
                throw fl::Exception("[syntax error] expected a property of type "
                    "'key=value', but found < " + line + ">", FL_AT);
            std::string key = fl::Op::trim(keyValue.at(0));
            std::string value = fl::Op::trim(keyValue.at(1));

            if (key == "Name") engine->setName(value);
            else if (key == "Type") {
                if (value != "mamdani")
                    throw fl::Exception("[importer error] fuzzylite supports only mamdani-type "
                        "engines at the moment", FL_AT);

            } else if (key == "AndMethod") andMethod = value;
            else if (key == "OrMethod") orMethod = value;
            else if (key == "ImpMethod") impMethod = value;
            else if (key == "AggMethod") aggMethod = value;
            else if (key == "DefuzzMethod") defuzzMethod = value;
            else FL_LOG("[info] ignoring redundant or non-relevant information "
                    "from line: " << line);
        }
    }

    void FisImporter::importInput(const std::string& section, Engine* engine) const {
        std::istringstream reader(section);
        std::string line;
        std::getline(reader, line); //ignore first line [Input#]

        InputVariable* input = new InputVariable;
        engine->addInputVariable(input);

        while (std::getline(reader, line)) {
            std::vector<std::string> keyValue = fl::Op::split(line, "=");
            if (keyValue.size() != 2)
                throw fl::Exception("[syntax error] expected a property of type "
                    "'key=value', but found < " + line + ">", FL_AT);
            std::string key = fl::Op::trim(keyValue.at(0));
            std::string value = fl::Op::trim(keyValue.at(1));

            if (key == "Name") input->setName(value);
            else if (key == "Range") {
                scalar minimum, maximum;
                extractRange(value, minimum, maximum);
                input->setMinimum(minimum);
                input->setMaximum(maximum);
            } else if (key.substr(0, 2) == "MF") {
                input->addTerm(extractTerm(value));
            } else {
                FL_LOG("[info] ignoring redundant or non-relevant information from line: " << line);
            }
        }
    }

    void FisImporter::importOutput(const std::string& section, Engine* engine) const {
        std::istringstream reader(section);
        std::string line;
        std::getline(reader, line); //ignore first line [Output#]

        OutputVariable* output = new OutputVariable;
        engine->addOutputVariable(output);


        while (std::getline(reader, line)) {
            std::vector<std::string> keyValue = fl::Op::split(line, "=");
            if (keyValue.size() != 2)
                throw fl::Exception("[syntax error] expected a property of type "
                    "'key=value', but found < " + line + ">", FL_AT);
            std::string key = fl::Op::trim(keyValue.at(0));
            std::string value = fl::Op::trim(keyValue.at(1));

            if (key == "Name") output->setName(value);
            else if (key == "Range") {
                scalar minimum, maximum;
                extractRange(value, minimum, maximum);
                output->setMinimum(minimum);
                output->setMaximum(maximum);
            } else if (key.substr(0, 2) == "MF") {
                output->addTerm(extractTerm(value));
            } else if (key == "Default") {
                output->setDefaultValue(fl::Op::toScalar(value));
            } else if (key == "Lock") {
                output->setLockDefuzzifiedValue((int) fl::Op::toScalar(value) == 1);
            } else {
                FL_LOG("[info] ignoring redundant or non-relevant information from line: " << line);
            }
        }
    }

    void FisImporter::importRules(const std::string& section, Engine* engine) const {
        std::istringstream reader(section);
        std::string line;
        std::getline(reader, line); //ignore first line [Rules]

        RuleBlock* ruleblock = new RuleBlock;
        engine->addRuleBlock(ruleblock);

        while (std::getline(reader, line)) {
            std::vector<std::string> inputsAndRest = fl::Op::split(line, ",");
            if (inputsAndRest.size() != 2)
                throw fl::Exception("[syntax error] expected rule to match pattern "
                    "<'i '+, 'o '+ (w) : '1|2'>, but found instead <" + line + ">", FL_AT);

            std::vector <std::string> outputsAndRest = fl::Op::split(inputsAndRest.at(1), ":");
            if (outputsAndRest.size() != 2)
                throw fl::Exception("[syntax error] expected rule to match pattern "
                    "<'i '+, 'o '+ (w) : '1|2'>, but found instead <" + line + ">", FL_AT);

            std::vector<std::string> inputs = fl::Op::split(inputsAndRest.at(0), " ");
            std::vector<std::string> outputs = fl::Op::split(outputsAndRest.at(0), " ");
            std::string weightInParenthesis = outputs.at(outputs.size() - 1);
            outputs.erase(outputs.begin() + outputs.size() - 1);
            std::string connector = fl::Op::trim(outputsAndRest.at(1));

            if ((int) inputs.size() != engine->numberOfInputVariables())
                throw fl::Exception("[syntax error] missing input variables in rule <"
                    + line + ">", FL_AT);

            if ((int) outputs.size() != engine->numberOfOutputVariables())
                throw fl::Exception("[syntax error] missing output variables in rule <"
                    + line + ">", FL_AT);

            std::vector<std::string> antecedent, consequent;

            for (std::size_t i = 0; i < inputs.size(); ++i) {
                std::ostringstream ss;
                scalar inputCode = fl::Op::toScalar(inputs.at(i));
                if (fl::Op::isEq(inputCode, 0.0)) continue;
                ss << engine->getInputVariable(i)->getName() << " "
                        << fl::Rule::FL_IS << " "
                        << translateProposition(inputCode, engine->getInputVariable(i));
                antecedent.push_back(ss.str());
            }

            for (std::size_t i = 0; i < outputs.size(); ++i) {
                std::ostringstream ss;
                scalar outputCode = fl::Op::toScalar(outputs.at(i));
                if (fl::Op::isEq(outputCode, 0.0)) continue;
                ss << engine->getOutputVariable(i)->getName() << " "
                        << fl::Rule::FL_IS << " "
                        << translateProposition(outputCode, engine->getOutputVariable(i));
                consequent.push_back(ss.str());
            }

            std::ostringstream rule;

            rule << fl::Rule::FL_IF << " ";
            for (std::size_t i = 0; i < antecedent.size(); ++i) {
                rule << antecedent.at(i);
                if (i < antecedent.size() - 1) {
                    rule << " ";
                    if (connector == "1") rule << fl::Rule::FL_AND << " ";
                    else if (connector == "2") rule << fl::Rule::FL_OR << " ";
                    else throw fl::Exception("[syntax error] connector <"
                            + connector + "> not recognized", FL_AT);
                }
            }

            rule << " " << fl::Rule::FL_THEN << " ";
            for (std::size_t i = 0; i < consequent.size(); ++i) {
                rule << consequent.at(i);
                if (i < consequent.size() - 1) {
                    rule << " " << fl::Rule::FL_AND << " ";
                }
            }

            std::ostringstream ss;
            for (std::size_t i = 0; i < weightInParenthesis.size(); ++i) {
                if (weightInParenthesis.at(i) == '('
                        or weightInParenthesis.at(i) == ')'
                        or weightInParenthesis.at(i) == ' ') continue;
                ss << weightInParenthesis.at(i);
            }

            scalar weight = fl::Op::toScalar(ss.str());
            if (not fl::Op::isEq(weight, 1.0))
                rule << " " << fl::Rule::FL_WITH << " " << weight;

            ruleblock->addRule(fl::MamdaniRule::parse(rule.str(), engine));
        }
    }

    std::string FisImporter::translateProposition(scalar code, Variable* variable) const {
        scalar intPartScalar;
        scalar fracPart = std::abs(std::modf(code, &intPartScalar));
        int intPart = (int) std::abs(intPartScalar) - 1;

        if (intPart < 0 or intPart >= variable->numberOfTerms()) {
            std::ostringstream ex;
            ex << "[syntax error] the code <" << code << "> refers to a term "
                    "out of range from variable <" << variable->toString() << ">";
            throw fl::Exception(ex.str(), FL_AT);
        }

        std::ostringstream ss;
        if (code < 0) ss << Not().name() << " ";
        if (fl::Op::isEq(fracPart, 0.05)) ss << Somewhat().name() << " ";
        else if (fl::Op::isEq(fracPart, 0.2)) ss << Very().name() << " ";
        else if (fl::Op::isEq(fracPart, 0.3)) ss << Extremely().name() << " ";
        else if (fl::Op::isEq(fracPart, 0.4)) ss << Very().name() << " " << Very().name() << " ";
        else if (not fl::Op::isEq(fracPart, 0))
            throw fl::Exception("[syntax error] no hedge defined in FIS format for <"
                + fl::Op::str(fracPart) + ">", FL_AT);

        ss << variable->getTerm(intPart)->getName();
        return ss.str();
    }

    std::string FisImporter::flTnorm(const std::string & name) const {
        std::string className = name;
        if (name == "min") className = Minimum().className();
        else if (name == "prod") className = AlgebraicProduct().className();
        else if (name == "bounded_difference") className = BoundedDifference().className();
        else if (name == "drastic_product") className = DrasticProduct().className();
        else if (name == "einstein_product") className = EinsteinProduct().className();
        else if (name == "hamacher_product") className = HamacherProduct().className();
        return className;
    }

    std::string FisImporter::flSnorm(const std::string & name) const {
        std::string className = name;
        if (name == "max") className = Maximum().className();
        else if (name == "sum" or name == "probor") className = AlgebraicSum().className();
        else if (name == "bounded_sum") className = BoundedSum().className();
        else if (name == "normalized_sum") className = NormalizedSum().className();
        else if (name == "drastic_sum") className = DrasticSum().className();
        else if (name == "einstein_sum") className = EinsteinSum().className();
        else if (name == "hamacher_sum") className = HamacherSum().className();
        return className;
    }

    std::string FisImporter::flDefuzzifier(const std::string & name) const {
        std::string className = name;
        if (name == "centroid") className = Centroid().className();
        else if (name == "som") className = SmallestOfMaximum().className();
        else if (name == "lom") className = LargestOfMaximum().className();
        else if (name == "mom") className = MeanOfMaximum().className();
        return className;
    }

    Term * FisImporter::extractTerm(const std::string & fis) const {
        std::ostringstream ss;
        for (std::size_t i = 0; i < fis.size(); ++i) {
            if (not (fis.at(i) == '[' or fis.at(i) == ']')) {
                ss << fis.at(i);
            }
        }
        std::string line = ss.str();

        std::vector<std::string> nameTerm = fl::Op::split(line, ":");
        if (nameTerm.size() != 2) {
            throw fl::Exception("[syntax error] expected term in format 'name':'class',[params], "
                    "but found " + line, FL_AT);
        }
        std::vector<std::string> termParams = fl::Op::split(nameTerm.at(1), ",");
        if (termParams.size() != 2) {
            throw fl::Exception("[syntax error] expected term in format 'name':'class',[params], "
                    "but found " + line, FL_AT);
        }

        std::vector<std::string> strParams = fl::Op::split(termParams.at(1), " ");
        std::vector<scalar> params;
        for (std::size_t i = 0; i < strParams.size(); ++i) {
            params.push_back(fl::Op::toScalar(strParams.at(i)));
        }

        return createInstance(termParams.at(0), nameTerm.at(0), params);
    }

    Term * FisImporter::createInstance(const std::string& mClass,
            const std::string& name, const std::vector<scalar>& params) const {
        std::map<std::string, std::string> mapping;
        mapping["discretemf"] = Discrete().className();
        mapping["gbellmf"] = Bell().className();
        mapping["gaussmf"] = Gaussian().className();
        mapping["gauss2mf"] = GaussianProduct().className();
        mapping["pimf"] = PiShape().className();
        mapping["rampmf"] = Ramp().className();
        mapping["rectmf"] = Rectangle().className();
        mapping["smf"] = SShape().className();
        mapping["sigmf"] = Sigmoid().className();
        mapping["dsigmf"] = SigmoidDifference().className();
        mapping["psigmf"] = SigmoidProduct().className();
        mapping["trapmf"] = Trapezoid().className();
        mapping["trimf"] = Triangle().className();
        mapping["zmf"] = ZShape().className();

        std::vector<scalar> sortedParams = params;
        if (mClass == "gbellmf" and params.size() >= 3) {
            sortedParams.at(0) = params.at(2);
            sortedParams.at(1) = params.at(0);
            sortedParams.at(2) = params.at(1);
        } else if (mClass == "gaussmf" and params.size() >= 2) {
            sortedParams.at(0) = params.at(1);
            sortedParams.at(1) = params.at(0);
        } else if (mClass == "gauss2mf" and params.size() >= 4) {
            sortedParams.at(0) = params.at(1);
            sortedParams.at(1) = params.at(0);
            sortedParams.at(2) = params.at(3);
            sortedParams.at(3) = params.at(2);
        } else if (mClass == "sigmf" and params.size() >= 2) {
            sortedParams.at(0) = params.at(1);
            sortedParams.at(1) = params.at(0);
        } else if (mClass == "dsigmf" and params.size() >= 4) {
            sortedParams.at(0) = params.at(1);
            sortedParams.at(1) = params.at(0);
            sortedParams.at(2) = params.at(2);
            sortedParams.at(3) = params.at(3);
        } else if (mClass == "psigmf" and params.size() >= 4) {
            sortedParams.at(0) = params.at(1);
            sortedParams.at(1) = params.at(0);
            sortedParams.at(2) = params.at(2);
            sortedParams.at(3) = params.at(3);
        }

        std::string flClass = mClass;
        std::map<std::string, std::string>::const_iterator it = mapping.find(mClass);
        if (it != mapping.end()) flClass = it->second;

        try {
            Term* result = Factory::instance()->term()->create(flClass, sortedParams);
            result->setName(name);
            return result;
        } catch (fl::Exception& ex) {
            ex.append(FL_AT);
            throw ex;
        }
    }

    void FisImporter::extractRange(const std::string& range, scalar& minimum, scalar & maximum) const {
        std::vector<std::string> parts = fl::Op::split(range, " ");
        if (parts.size() != 2)
            throw fl::Exception("[syntax error] expected range in format '[begin end]',"
                " but found <" + range + ">", FL_AT);
        std::string begin = parts.at(0), end = parts.at(1);
        if (begin.at(0) != '[' or end.at(end.size() - 1) != ']')
            throw fl::Exception("[syntax error] expected range in format '[begin end]',"
                " but found <" + range + ">", FL_AT);
        minimum = fl::Op::toScalar(begin.substr(1));
        maximum = fl::Op::toScalar(end.substr(0, end.size() - 1));
    }




}