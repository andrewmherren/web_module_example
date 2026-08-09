#include <unity.h>

#ifdef NATIVE_PLATFORM
#include <ArduinoFake.h>
#include <ArduinoJson.h>
#include <interface/core/web_request_core.h>
#include <interface/core/web_response_core.h>
#include <testing/testing_platform_provider.h>
#include <web_module_example.h>
using namespace fakeit;

// ===========================================================================
// Module Metadata Tests
// ===========================================================================

static void test_module_metadata() {
  WebModuleExample module;
  TEST_ASSERT_EQUAL_STRING("Web Module Example", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING(WEB_MODULE_VERSION_STR, module.getModuleVersion().c_str());
  TEST_ASSERT_TRUE_MESSAGE(module.getModuleDescription().length() > 0,
                           "Description should be non-empty");
}

// ===========================================================================
// Configuration Tests
// ===========================================================================

static void test_parseConfig_with_null_config() {
  WebModuleExample module;
  JsonVariant nullConfig;
  module.__test_applyConfig(nullConfig);
  
  // Should use defaults
  TEST_ASSERT_TRUE(module.isEnabled());
  TEST_ASSERT_EQUAL(0, module.getExampleValue());
  TEST_ASSERT_EQUAL_STRING("default", module.getExampleString().c_str());
}

static void test_parseConfig_with_all_fields() {
  WebModuleExample module;
  JsonDocument doc;
  doc["enabled"] = false;
  doc["exampleValue"] = 42;
  doc["exampleString"] = "custom";

  module.__test_applyConfig(doc.as<JsonVariant>());

  TEST_ASSERT_FALSE(module.isEnabled());
  TEST_ASSERT_EQUAL(42, module.getExampleValue());
  TEST_ASSERT_EQUAL_STRING("custom", module.getExampleString().c_str());
}

static void test_parseConfig_partial_fields() {
  WebModuleExample module;
  JsonDocument doc;
  doc["exampleValue"] = 99;

  module.__test_applyConfig(doc.as<JsonVariant>());

  // Should update only specified field
  TEST_ASSERT_TRUE(module.isEnabled()); // Default
  TEST_ASSERT_EQUAL(99, module.getExampleValue());
  TEST_ASSERT_EQUAL_STRING("default", module.getExampleString().c_str()); // Default
}

static void test_begin_with_config_variant() {
  WebModuleExample module;
  
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  JsonDocument doc;
  doc["enabled"] = false;
  doc["exampleValue"] = 123;

  module.begin(doc.as<JsonVariant>());

  TEST_ASSERT_FALSE(module.isEnabled());
  TEST_ASSERT_EQUAL(123, module.getExampleValue());
}

// ===========================================================================
// Lifecycle Tests
// ===========================================================================

static void test_begin_calls_initialization() {
  WebModuleExample module;
  
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);

  module.begin();

  // Should complete without error
  TEST_ASSERT_TRUE(true);
}

static void test_handle_early_return_due_to_interval() {
  WebModuleExample module;
  When(Method(ArduinoFake(), millis)).AlwaysReturn(0);
  
  module.handle();
  
  // Should complete without error (early return before interval elapsed)
  TEST_ASSERT_TRUE(true);
}

static void test_handle_periodic_task_after_interval() {
  WebModuleExample module;
  
  // Stub millis to return increasing values
  // First handle() call: check (0), then set lastHandleTime (0)
  // Second handle() call: check (11000 - 0 > interval), execute, set lastHandleTime (11000)
  When(Method(ArduinoFake(), millis)).Return(0, 0, 11000, 11000);
  
  module.handle(); // Sets lastHandleTime (calls millis twice: check and set)
  module.handle(); // Should execute periodic task (calls millis twice: check and set)
  
  TEST_ASSERT_TRUE(true);
}

// ===========================================================================
// Route Registration Tests
// ===========================================================================

static void test_routes_built_and_sizes() {
  WebModuleExample module;
  auto http = module.getHttpRoutes();
  auto https = module.getHttpsRoutes();
  
  // Should have main page, status, config get, config post
  TEST_ASSERT_EQUAL(4, http.size());
  TEST_ASSERT_EQUAL(http.size(), https.size());
}

static void test_http_routes_structure() {
  WebModuleExample module;
  auto routes = module.getHttpRoutes();
  
  // All routes should be properly initialized
  for (const auto &route : routes) {
    TEST_ASSERT_TRUE(route.isWebRoute() || route.isApiRoute());
  }
}

// ===========================================================================
// Route Handler Tests
// ===========================================================================

static void test_mainPageHandler_returns_html() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  module.mainPageHandler(req, res);
  
  TEST_ASSERT_EQUAL_STRING("text/html", res.getMimeType().c_str());
  TEST_ASSERT_TRUE(res.getContent().length() > 0);
  TEST_ASSERT_TRUE(res.getContent().find("Web Module Example") != std::string::npos);
}

static void test_statusHandler_builds_json() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  When(Method(ArduinoFake(), millis)).AlwaysReturn(5000);
  
  module.statusHandler(req, res);
  
  TEST_ASSERT_EQUAL_STRING("application/json", res.getMimeType().c_str());
  
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, res.getContent());
  TEST_ASSERT_FALSE_MESSAGE(err, "JSON parse error");
  
  TEST_ASSERT_FALSE(doc["success"].isNull());
  TEST_ASSERT_FALSE(doc["enabled"].isNull());
  TEST_ASSERT_FALSE(doc["exampleValue"].isNull());
  TEST_ASSERT_FALSE(doc["exampleString"].isNull());
  TEST_ASSERT_FALSE(doc["uptime"].isNull());
}

static void test_statusHandler_reflects_configuration() {
  WebModuleExample module;
  
  JsonDocument config;
  config["enabled"] = false;
  config["exampleValue"] = 777;
  module.__test_applyConfig(config.as<JsonVariant>());
  
  WebRequestCore req;
  WebResponseCore res;
  module.statusHandler(req, res);
  
  JsonDocument doc;
  deserializeJson(doc, res.getContent());
  
  TEST_ASSERT_FALSE(doc["enabled"].as<bool>());
  TEST_ASSERT_EQUAL(777, doc["exampleValue"].as<int>());
}

static void test_configHandler_returns_current_config() {
  WebModuleExample module;
  
  JsonDocument config;
  config["enabled"] = false;
  config["exampleValue"] = 555;
  config["exampleString"] = "test";
  module.__test_applyConfig(config.as<JsonVariant>());
  
  WebRequestCore req;
  WebResponseCore res;
  module.configHandler(req, res);
  
  JsonDocument doc;
  deserializeJson(doc, res.getContent());
  
  TEST_ASSERT_FALSE(doc["enabled"].as<bool>());
  TEST_ASSERT_EQUAL(555, doc["exampleValue"].as<int>());
  TEST_ASSERT_EQUAL_STRING("test", doc["exampleString"].as<const char*>());
}

static void test_updateConfigHandler_invalid_json_400() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  req.setBody("not-json");
  module.updateConfigHandler(req, res);
  
  TEST_ASSERT_EQUAL(400, res.getStatus());
  
  JsonDocument doc;
  deserializeJson(doc, res.getContent());
  TEST_ASSERT_FALSE(doc["success"].as<bool>());
  TEST_ASSERT_FALSE(doc["error"].isNull());
}

static void test_updateConfigHandler_success() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  
  req.setBody(R"({
    "enabled": false,
    "exampleValue": 999,
    "exampleString": "updated"
  })");
  
  module.updateConfigHandler(req, res);
  
  TEST_ASSERT_EQUAL(200, res.getStatus());
  
  JsonDocument doc;
  deserializeJson(doc, res.getContent());
  TEST_ASSERT_TRUE(doc["success"].as<bool>());
  TEST_ASSERT_TRUE(doc["changed"].as<bool>());
  
  // Verify config was actually updated
  TEST_ASSERT_FALSE(module.isEnabled());
  TEST_ASSERT_EQUAL(999, module.getExampleValue());
  TEST_ASSERT_EQUAL_STRING("updated", module.getExampleString().c_str());
}

static void test_updateConfigHandler_partial_update() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  
  // Set initial config
  JsonDocument initialConfig;
  initialConfig["enabled"] = true;
  initialConfig["exampleValue"] = 100;
  initialConfig["exampleString"] = "initial";
  module.__test_applyConfig(initialConfig.as<JsonVariant>());
  
  // Update only one field
  req.setBody(R"({"exampleValue": 200})");
  module.updateConfigHandler(req, res);
  
  TEST_ASSERT_EQUAL(200, res.getStatus());
  
  // Verify only specified field changed
  TEST_ASSERT_TRUE(module.isEnabled()); // Unchanged
  TEST_ASSERT_EQUAL(200, module.getExampleValue()); // Changed
  TEST_ASSERT_EQUAL_STRING("initial", module.getExampleString().c_str()); // Unchanged
}

static void test_updateConfigHandler_no_changes() {
  WebModuleExample module;
  WebRequestCore req;
  WebResponseCore res;
  
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(const char *)))
      .AlwaysReturn(1);
  
  // Set initial config
  JsonDocument initialConfig;
  initialConfig["enabled"] = true;
  initialConfig["exampleValue"] = 100;
  module.__test_applyConfig(initialConfig.as<JsonVariant>());
  
  // Send same values
  req.setBody(R"({"enabled": true, "exampleValue": 100})");
  module.updateConfigHandler(req, res);
  
  TEST_ASSERT_EQUAL(200, res.getStatus());
  
  JsonDocument doc;
  deserializeJson(doc, res.getContent());
  TEST_ASSERT_TRUE(doc["success"].as<bool>());
  TEST_ASSERT_FALSE(doc["changed"].as<bool>());
  
  std::string message = doc["message"].as<const char*>();
  TEST_ASSERT_TRUE(message.find("No changes") != std::string::npos);
}

// ===========================================================================
// Test Registration
// ===========================================================================

void register_web_module_example_tests() {
  // Metadata
  RUN_TEST(test_module_metadata);
  
  // Configuration
  RUN_TEST(test_parseConfig_with_null_config);
  RUN_TEST(test_parseConfig_with_all_fields);
  RUN_TEST(test_parseConfig_partial_fields);
  RUN_TEST(test_begin_with_config_variant);
  
  // Lifecycle
  RUN_TEST(test_begin_calls_initialization);
  RUN_TEST(test_handle_early_return_due_to_interval);
  RUN_TEST(test_handle_periodic_task_after_interval);
  
  // Route registration
  RUN_TEST(test_routes_built_and_sizes);
  RUN_TEST(test_http_routes_structure);
  
  // Route handlers
  RUN_TEST(test_mainPageHandler_returns_html);
  RUN_TEST(test_statusHandler_builds_json);
  RUN_TEST(test_statusHandler_reflects_configuration);
  RUN_TEST(test_configHandler_returns_current_config);
  RUN_TEST(test_updateConfigHandler_invalid_json_400);
  RUN_TEST(test_updateConfigHandler_success);
  RUN_TEST(test_updateConfigHandler_partial_update);
  RUN_TEST(test_updateConfigHandler_no_changes);
}

#endif // NATIVE_PLATFORM
