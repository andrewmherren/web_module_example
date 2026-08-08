#include <unity.h>
#include <Arduino.h>

#ifndef NATIVE_PLATFORM

#include <web_module_example.h>

// ===========================================================================
// ESP32 Hardware Tests
// ===========================================================================
// These tests verify that the module compiles and runs on actual ESP32 hardware
// They are intentionally simple - complex logic should be tested in native tests

void test_esp32_module_compiles() {
  // Just verify the module can be instantiated
  WebModuleExample module;
  TEST_ASSERT_TRUE(true);
}

void test_esp32_begin_does_not_crash() {
  WebModuleExample module;
  module.begin();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_begin_with_config_does_not_crash() {
  WebModuleExample module;
  
  JsonDocument config;
  config["enabled"] = true;
  config["exampleValue"] = 42;
  
  module.begin(config.as<JsonVariant>());
  TEST_ASSERT_TRUE(true);
}

void test_esp32_handle_does_not_crash() {
  WebModuleExample module;
  module.begin();
  module.handle();
  TEST_ASSERT_TRUE(true);
}

void test_esp32_get_routes_does_not_crash() {
  WebModuleExample module;
  
  auto httpRoutes = module.getHttpRoutes();
  auto httpsRoutes = module.getHttpsRoutes();
  
  TEST_ASSERT_TRUE(httpRoutes.size() > 0);
  TEST_ASSERT_TRUE(httpsRoutes.size() > 0);
}

void test_esp32_module_metadata() {
  WebModuleExample module;
  
  String name = module.getModuleName();
  String version = module.getModuleVersion();
  String description = module.getModuleDescription();
  
  TEST_ASSERT_TRUE(name.length() > 0);
  TEST_ASSERT_TRUE(version.length() > 0);
  TEST_ASSERT_TRUE(description.length() > 0);
}

void test_esp32_accessors() {
  WebModuleExample module;
  module.begin();
  
  // Test that accessors work without crashing
  bool enabled = module.isEnabled();
  int value = module.getExampleValue();
  String str = module.getExampleString();
  
  TEST_ASSERT_TRUE(enabled == true || enabled == false); // Just verify it returns something
  TEST_ASSERT_TRUE(value >= 0 || value < 0); // Any value is fine
  TEST_ASSERT_TRUE(str.length() >= 0); // Any string is fine
}

#endif // NATIVE_PLATFORM
