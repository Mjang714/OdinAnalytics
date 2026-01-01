cmake_minimum_required(VERSION 3.20)

include_guard(GLOBAL)

##
# oa_unity_enable_unique_id.cmake
#
# Provides a function to set a target's UNITY_BUILD_UNIQUE_ID to OA_UNITY_ID, a
# macro that is defined to a unique identifier for each agglomerated TU in a
# unity build. The oa/internal/unity.h header provides more details on how to
# use the OA_UNITY_ID macro to ensure internal linkage identifiers are unique.
#

##
# Set thhe UNITY_BUILD_UNIQUE_ID property of a target to "OA_UNITY_ID".
#
# See https://cmake.org/cmake/help/latest/prop_tgt/UNITY_BUILD_UNIQUE_ID.html
# and the oa/internal/unity.h documentation for details.
#
# Arguments:
#   target      Target to set UNITY_BUILD_UNIQUE_ID for
#
function(oa_unity_enable_unique_id target)
    set_target_properties(
        ${target} PROPERTIES
        UNITY_BUILD_UNIQUE_ID "OA_UNITY_ID"
    )
endfunction()
