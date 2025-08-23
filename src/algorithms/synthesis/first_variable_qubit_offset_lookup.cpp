/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "algorithms/synthesis/first_variable_qubit_offset_lookup.hpp"

#include "ir/Definitions.hpp"

#include <optional>
#include <string_view>

using namespace syrec;

void FirstVariableQubitOffsetLookup::openNewVariableQubitOffsetScope() {
    recordedOffsetsToFirstQubitPerVariableScopes.emplace_back();
}

bool FirstVariableQubitOffsetLookup::closeVariableQubitOffsetScope() {
    if (recordedOffsetsToFirstQubitPerVariableScopes.empty()) {
        return false;
    }
    recordedOffsetsToFirstQubitPerVariableScopes.pop_back();
    return true;
}

bool FirstVariableQubitOffsetLookup::registerOrUpdateOffsetToFirstQubitOfVariableInCurrentScope(const std::string_view& variableIdentifier, const qc::Qubit offsetToFirstQubitOfVariable) {
    if (variableIdentifier.empty() || recordedOffsetsToFirstQubitPerVariableScopes.empty()) {
        return false;
    }
    recordedOffsetsToFirstQubitPerVariableScopes.back()[variableIdentifier] = offsetToFirstQubitOfVariable;
    return true;
}

std::optional<qc::Qubit> FirstVariableQubitOffsetLookup::getOffsetToFirstQubitOfVariableInCurrentScope(const std::string_view& variableIdentifier, const bool alsoSearchInParentScope) const {
    if (variableIdentifier.empty() || recordedOffsetsToFirstQubitPerVariableScopes.empty()) {
        return std::nullopt;
    }

    std::optional<qc::Qubit> foundOffsetForVariableIdentifier = getOffsetToFirstQubitOfVariableInScope(variableIdentifier, recordedOffsetsToFirstQubitPerVariableScopes.back());
    if (!foundOffsetForVariableIdentifier.has_value() && alsoSearchInParentScope && recordedOffsetsToFirstQubitPerVariableScopes.size() > 1) {
        foundOffsetForVariableIdentifier = getOffsetToFirstQubitOfVariableInScope(variableIdentifier, recordedOffsetsToFirstQubitPerVariableScopes.at(recordedOffsetsToFirstQubitPerVariableScopes.size() - 2));
    }
    return foundOffsetForVariableIdentifier;
}

std::optional<qc::Qubit> FirstVariableQubitOffsetLookup::getOffsetToFirstQubitOfVariableInScope(const std::string_view& variableIdentifier, const QubitOffsetScope& scope) {
    const auto& registrationForVariableIdentifier = scope.find(variableIdentifier);
    return registrationForVariableIdentifier != scope.cend() ? std::make_optional(registrationForVariableIdentifier->second) : std::nullopt;
}