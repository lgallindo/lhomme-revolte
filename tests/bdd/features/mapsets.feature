Feature: Mapset Loading and Verification
  As a game engine developer
  I want to define strict hardware and feature requirements for mapsets
  So that the engine refuses to load a mod on a platform that cannot run it

  Scenario: Loading a mapset with unsupported requirements
    Given a target compilation platform with 2 keys
    And a mapset metadata defining a requirement of 4 keys
    When the engine attempts to load the mapset
    Then the engine should refuse to load the mapset
    And log an error indicating insufficient keys

  Scenario: Loading a dynamically loaded mapset from file
    Given the engine is compiled with LHR_DYNAMIC_MAPSETS=1
    And a valid mapset file "eviternity.mod" is present
    When the user requests to load "eviternity.mod"
    Then the engine should parse the file header
    And successfully load the mapset if platform requirements are met

  Scenario: Attempting dynamic load on a static build
    Given the engine is compiled with LHR_DYNAMIC_MAPSETS=0
    And a valid mapset file "eviternity.mod" is present
    When the user requests to load "eviternity.mod"
    Then the engine should refuse to load the mapset
    And log an error indicating dynamic loading is disabled
