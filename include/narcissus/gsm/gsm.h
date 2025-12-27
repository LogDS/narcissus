//
// Created by gyankos on 26/12/25.
//

#ifndef REFLECTION_GSM_H
#define REFLECTION_GSM_H

// #include <rfl/json.hpp>
#include <rfl.hpp>

using score = rfl::Validator<double, rfl::Minimum<0.0>, rfl::Maximum<1.0>>;

struct Phi {
    rfl::Description<"the containment relation associating the parent object to the included child", std::string> containment;
    rfl::Description<"object id providing a content to the object", uint64_t>                                     content;
    rfl::Description<"confidence value of the parent object containing the content object", score> confidence;
};

struct GSMObject {
    rfl::Description<"A unique identifier for an GSM object", uint64_t> id;
    rfl::Description<"list of confidence scores associated to the object", std::vector<score>> scores;
    rfl::Description<"list of labels/types associated to the object", std::vector<std::string>> ell;
    rfl::Description<"list of string-based values associated to the object", std::vector<std::string>> xi;
    rfl::Description<"key-value representation associated to each object", std::unordered_map<std::string, std::vector<std::string>>> properties;
    rfl::Description<"associating a content label to some content", std::vector<Phi>> phi;
};

#endif //REFLECTION_GSM_H