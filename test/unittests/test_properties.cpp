/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#include "core/properties.hpp"

#include <any>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <string>

using namespace syrec;

namespace {
    struct BaseType {
        unsigned int value = 1U;
    };

    struct DerivedType: BaseType {
        unsigned int otherValue = 2U;
    };

    constexpr bool operator==(const BaseType& lOperand, const BaseType& rOperand) noexcept {
        return lOperand.value == rOperand.value;
    }

    constexpr bool operator==(const DerivedType& lOperand, const DerivedType& rOperand) noexcept {
        return lOperand.value == rOperand.value && lOperand.otherValue == rOperand.otherValue;
    }

    template<typename T>
    void assertValueForKeyMatches(const Properties& propertiesMap, const std::string& key, const std::optional<T>& expectedValue) {
        std::optional<T> actualValue;
        ASSERT_NO_FATAL_FAILURE(actualValue = propertiesMap.get<T>(key)) << "Value of entry with key '" << key << "' should be fetchable without errors";
        if (expectedValue.has_value()) {
            ASSERT_TRUE(actualValue.has_value());
            ASSERT_EQ(*expectedValue, *actualValue);
        } else {
            ASSERT_FALSE(actualValue.has_value());
        }
    }
} // namespace

TEST(PropertiesTest, GetValueOfNotExistingEntryWithoutDefaultValue) {
    const std::string& key   = "key_one";
    const std::string& value = "value";

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, "key_two", std::nullopt));
}

TEST(PropertiesTest, GetValueOfEntryUsingKeyWithoutDefaultValue) {
    const std::string& key   = "key_one";
    const std::string& value = "value";

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, key, value));
}

TEST(PropertiesTest, GetValueOfNotExistingEntryWithDefaultValue) {
    const std::string& key          = "key_one";
    const std::string& value        = "value";
    const std::string& defaultValue = "otherValue";

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));

    std::string fetchedValue;
    ASSERT_NO_FATAL_FAILURE(fetchedValue = propertiesLookup.get<std::string>("key_two", defaultValue));
    ASSERT_EQ(defaultValue, fetchedValue);
}

TEST(PropertiesTest, GetValueOfNotExistingEntryWithDefaultValueOfDerivedTypeCausesError) {
    const std::string&    key = "key_one";
    constexpr BaseType    value;
    constexpr DerivedType defaultValue = {{4U}};

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));
    ASSERT_THROW(propertiesLookup.get<DerivedType>(key, defaultValue), std::bad_any_cast);
}

TEST(PropertiesTest, GetValueOfNotExistingEntryWithDefaultValueOfAssignableTypeCausesError) {
    const std::string&     key          = "key_one";
    constexpr unsigned int value        = 2U;
    constexpr unsigned int defaultValue = 4U;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));
    ASSERT_THROW(propertiesLookup.get<int>(key, defaultValue), std::bad_any_cast);
}

TEST(PropertiesTest, GetValueOfEntryUsingKeyWithDefaultValueReturnsValueOfEntry) {
    const std::string& key          = "key_one";
    const std::string& value        = "value";
    const std::string& defaultValue = "otherValue";

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(key, value));

    std::string fetchedValue;
    ASSERT_NO_FATAL_FAILURE(fetchedValue = propertiesLookup.get<std::string>(key, defaultValue));
    ASSERT_EQ(value, fetchedValue);
}

TEST(PropertiesTest, CheckWhetherContainerContainsEntryUsingKeyOfNotExistingEntry) {
    const std::string& keyOne   = "key_one";
    const std::string& keyTwo   = "key_two";
    const std::string& keyThree = "keyOne";

    const std::string      keyOneValue   = "value";
    constexpr unsigned int keyTwoValue   = 2U;
    constexpr float        keyThreeValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, keyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, keyTwoValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyThree, keyThreeValue));

    ASSERT_FALSE(propertiesLookup.containsKey("key_four"));
}

TEST(PropertiesTest, CheckWhetherContainerContainsEntryUsingKeyOfExistingEntry) {
    const std::string& keyOne   = "key_one";
    const std::string& keyTwo   = "key_two";
    const std::string& keyThree = "keyOne";

    const std::string      keyOneValue   = "value";
    constexpr unsigned int keyTwoValue   = 2U;
    constexpr float        keyThreeValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, keyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, keyTwoValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyThree, keyThreeValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));
}

TEST(PropertiesTest, RemoveNotExistingEntry) {
    const std::string& keyOne   = "key_one";
    const std::string& keyTwo   = "key_two";
    const std::string& keyThree = "keyOne";

    const std::string&     expectedKeyOneValue   = "value";
    constexpr unsigned int expectedKeyTwoValue   = 2U;
    constexpr float        expectedKeyThreeValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_FALSE(propertiesLookup.remove("key_four"));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));
    ASSERT_TRUE(propertiesLookup.containsKey("key_four"));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));
}

TEST(PropertiesTest, RemoveExistingEntry) {
    const std::string& keyOne   = "key_one";
    const std::string& keyTwo   = "key_two";
    const std::string& keyThree = "keyOne";

    const std::string&     expectedKeyOneValue   = "value";
    constexpr unsigned int expectedKeyTwoValue   = 2U;
    constexpr float        expectedKeyThreeValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.remove(keyOne));

    ASSERT_FALSE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, std::nullopt));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.remove(keyTwo));

    ASSERT_FALSE(propertiesLookup.containsKey(keyOne));
    ASSERT_FALSE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, std::nullopt));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, std::nullopt));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.remove(keyThree));

    ASSERT_FALSE(propertiesLookup.containsKey(keyOne));
    ASSERT_FALSE(propertiesLookup.containsKey(keyTwo));
    ASSERT_FALSE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, std::nullopt));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, std::nullopt));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, std::nullopt));
}

TEST(PropertiesTest, SetNewEntry) {
    const std::string& keyOne   = "key_one";
    const std::string& keyTwo   = "key_two";
    const std::string& keyThree = "keyOne";

    const std::string&     expectedKeyOneValue   = "value";
    constexpr unsigned int expectedKeyTwoValue   = 2U;
    constexpr float        expectedKeyThreeValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyThree, expectedKeyThreeValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_FALSE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));

    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));
    ASSERT_TRUE(propertiesLookup.containsKey(keyThree));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyThree, expectedKeyThreeValue));
}

TEST(PropertiesTest, UpdateValueOfExistingEntry) {
    const std::string& keyOne = "key_one";
    const std::string& keyTwo = "keyOne";

    const std::string& expectedKeyOneValue        = "value";
    constexpr float    expectedKeyTwoInitialValue = 3;
    constexpr float    expectedKeyTwoFinalValue   = 4;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedKeyTwoInitialValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyTwo, expectedKeyTwoInitialValue));

    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedKeyTwoFinalValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyTwo, expectedKeyTwoFinalValue));
}

TEST(PropertiesTest, UpdateValueOfExistingEntryWithValueOfIncompatibleTypePossible) {
    const std::string& keyOne = "key_one";
    const std::string& keyTwo = "keyOne";

    const std::string& expectedKeyOneValue        = "value";
    constexpr float    expectedInitialKeyTwoValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedInitialKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyTwo, expectedInitialKeyTwoValue));

    const std::string& expectedFinalKeyTwoValue = "anotherValue";
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedFinalKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyTwo, expectedFinalKeyTwoValue));
}

TEST(PropertiesTest, UpdateValueOfExistingEntryWithValueOfDerivedTypePossible) {
    const std::string& keyOne = "key_one";
    const std::string& keyTwo = "keyOne";

    const std::string& expectedKeyOneValue        = "value";
    constexpr auto     expectedInitialKeyTwoValue = BaseType();

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedInitialKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<BaseType>(propertiesLookup, keyTwo, expectedInitialKeyTwoValue));

    constexpr auto updateKeyTwoValue = DerivedType();
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, updateKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<DerivedType>(propertiesLookup, keyTwo, updateKeyTwoValue));

    ASSERT_THROW(propertiesLookup.get<BaseType>(keyTwo), std::bad_any_cast);
}

TEST(PropertiesTest, UpdateValueOfExistingEntryWithValueOfAssignableTypePossible) {
    const std::string& keyOne = "key_one";
    const std::string& keyTwo = "keyOne";

    const std::string& expectedKeyOneValue        = "value";
    constexpr float    expectedInitialKeyTwoValue = 3;

    Properties propertiesLookup;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedInitialKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<float>(propertiesLookup, keyTwo, expectedInitialKeyTwoValue));

    constexpr unsigned int expectedFinalKeyTwoValue = 2U;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup.set(keyTwo, expectedFinalKeyTwoValue));

    ASSERT_TRUE(propertiesLookup.containsKey(keyOne));
    ASSERT_TRUE(propertiesLookup.containsKey(keyTwo));

    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<unsigned int>(propertiesLookup, keyTwo, expectedFinalKeyTwoValue));
}

TEST(PropertiesTest, GetValueOfEntryInInvalidLookupMapUsingLookupFunctionAcceptingSmartPointer) {
    Properties::ptr        invalidLookupMap     = nullptr;
    constexpr unsigned int expectedDefaultValue = 2U;

    unsigned int actualFetchedValue;
    ASSERT_NO_FATAL_FAILURE(actualFetchedValue = get<unsigned int>(invalidLookupMap, "key_one", expectedDefaultValue));
    ASSERT_EQ(expectedDefaultValue, actualFetchedValue);
}

TEST(PropertiesTest, GetValueOfEntryUsingKeyWithoutMatchesUsingLookupFunctionAcceptingSmartPointer) {
    auto               propertiesLookup    = std::make_shared<Properties>();
    const std::string& keyOne              = "key_one";
    const std::string& expectedKeyOneValue = "value";
    ASSERT_NO_FATAL_FAILURE(propertiesLookup->set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(*propertiesLookup, keyOne, expectedKeyOneValue));

    const std::string& expectedDefaultValue = "defaultValue";
    std::string        actualFetchedValue;
    ASSERT_NO_FATAL_FAILURE(actualFetchedValue = get<std::string>(propertiesLookup, "key_two", expectedDefaultValue));
    ASSERT_EQ(expectedDefaultValue, actualFetchedValue);
}

TEST(PropertiesTest, GetValueOfEntryUsingKeyUsingLookupFunctionAcceptingSmartPointer) {
    auto               propertiesLookup    = std::make_shared<Properties>();
    const std::string& keyOne              = "key_one";
    const std::string& expectedKeyOneValue = "value";
    ASSERT_NO_FATAL_FAILURE(propertiesLookup->set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(*propertiesLookup, keyOne, expectedKeyOneValue));

    const std::string& keyTwo              = "key_two";
    const std::string& expectedKeyTwoValue = "otherValue";
    ASSERT_NO_FATAL_FAILURE(propertiesLookup->set(keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(*propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(*propertiesLookup, keyTwo, expectedKeyTwoValue));

    std::string actualKeyOneValue;
    ASSERT_NO_FATAL_FAILURE(actualKeyOneValue = get<std::string>(propertiesLookup, keyOne, "OTHER"));
    ASSERT_EQ(expectedKeyOneValue, actualKeyOneValue);

    std::string actualKeyTwoValue;
    ASSERT_NO_FATAL_FAILURE(actualKeyTwoValue = get<std::string>(propertiesLookup, keyTwo, "OTHER"));
    ASSERT_EQ(expectedKeyTwoValue, actualKeyTwoValue);
}

TEST(PropertiesTest, GetValueOfEntryInLookupMapWithInvalidValueTypeUsingLookupFunctionAcceptingSmartPointer) {
    auto               propertiesLookup = std::make_shared<Properties>();
    const std::string& keyOne           = "key_one";
    constexpr BaseType expectedKeyOneValue;
    ASSERT_NO_FATAL_FAILURE(propertiesLookup->set(keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<BaseType>(*propertiesLookup, keyOne, expectedKeyOneValue));

    const std::string& keyTwo              = "key_two";
    const std::string& expectedKeyTwoValue = "otherValue";
    ASSERT_NO_FATAL_FAILURE(propertiesLookup->set(keyTwo, expectedKeyTwoValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<BaseType>(*propertiesLookup, keyOne, expectedKeyOneValue));
    ASSERT_NO_FATAL_FAILURE(assertValueForKeyMatches<std::string>(*propertiesLookup, keyTwo, expectedKeyTwoValue));

    ASSERT_THROW(get<DerivedType>(propertiesLookup, keyOne, DerivedType()), std::bad_any_cast);
    ASSERT_THROW(get<unsigned int>(propertiesLookup, keyOne, 2U), std::bad_any_cast);
}
