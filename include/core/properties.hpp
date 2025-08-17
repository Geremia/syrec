/*
 * Copyright (c) 2023 - 2025 Chair for Design Automation, TUM
 * Copyright (c) 2025 Munich Quantum Software Company GmbH
 * All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License
 */

#pragma once

#include <any>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace syrec {

    /**
    * Property map for storing settings and statistical information
    */
    struct Properties {
        using ptr = std::shared_ptr<Properties>;

        /**
         * Fetch the value of the entry matching the given key.
         * @tparam T The expected type of the value of the entry in the map that must match exactly (i.e. not allowed be a derived type or assignable type of \p T).
         * @param key The key that is used in the search for a matching element.
         * @return The value of the entry matching the given key casted to the template parameter \p T, otherwise std::nullopt.
         */
        template<typename T>
        [[maybe_unused]] std::optional<T> get(const std::string& key) const {
            if (auto matchingEntryForKey = map.find(key); matchingEntryForKey != map.cend()) {
                return std::any_cast<T>(matchingEntryForKey->second);
            }
            return std::nullopt;
        }

        /**
         * Fetch the value of the entry matching the given key or return a default value.
         * @tparam T The expected type of the value of the entry in the map that must match exactly (i.e. not allowed be a derived type or assignable type of \p T).
         * @param key The key that is used in the search for a matching element.
         * @param defaultValue The default value to return if no entry for the given key exists.
         * @return The value of the entry matching the given key, otherwise \p defaultValue.
         * @remarks No new entry in the internal lookup is created in case that no entry for the given key existed.
         */
        template<typename T>
        [[maybe_unused]] T get(const std::string& key, const T& defaultValue) const {
            return get<T>(key).value_or(defaultValue);
        }

        /**
         * Check whether an entry for a given key exists.
         * @param key The key that is used in the search for a matching element.
         * @return Whether an entry for the key exists.
         */
        [[nodiscard]] bool containsKey(const std::string& key) const {
            return map.find(key) != map.cend();
        }

        /**
         * Remove an entry that matches a given key.
         * @param key The key that is used in the search for a matching element.
         * @return Whether an entry was removed.
         */
        [[maybe_unused]] bool remove(const std::string& key) {
            if (auto matchingEntryForKey = map.find(key); matchingEntryForKey != map.cend()) {
                map.erase(matchingEntryForKey);
                return true;
            }
            return false;
        }

        /**
         * Add or update the value of an entry in the internal lookup
         * @tparam T The expected type of the value of the entry in the internal lookup. The same type must be used in all overloads of the get(...) function when attempting to query the value of said entry.
         * @param key The key with which the entry is identified in the internal lookup.
         * @param value The value of the entry.
         */
        template<typename T>
        void set(const std::string& key, const T& value) {
            map[key] = value;
        }

    private:
        std::map<std::string, std::any> map;
    };

    /**
     * Fetch the value of an entry in a properties object or return a default value if no such value exists.
     * @tparam T The expected type of the value of the entry in the map that must match exactly (i.e. not allowed be a derived type or assignable type of \p T).
     * @param settings The properties object that shall be queried.
     * @param key The key that is used in the search for a matching element.
     * @param defaultValue The default value to return in case no matching entry was found.
     * @return The value of the entry matching \p key if \p settings is a non-null properties object and a matching entry for \p key existed, otherwise \p defaultValue.
     */
    template<typename T>
    [[maybe_unused]] T get(const Properties::ptr& settings, const std::string& key, const T& defaultValue) {
        return settings != nullptr ? settings->get<T>(key, defaultValue) : defaultValue;
    }
} // namespace syrec
