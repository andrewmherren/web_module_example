#ifndef WEB_MODULE_EXAMPLE_H
#define WEB_MODULE_EXAMPLE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <interface/auth_types.h>
#include <interface/openapi_factory.h>
#include <interface/openapi_types.h>
#include <interface/request_response_types.h>
#include <interface/utils/route_variant.h>
#include <interface/web_module_interface.h>
#include <utility>
#include <web_platform_interface.h>
#include "version_autogen.h"

/**
 * @brief Example/boilerplate web module for WebPlatform
 * 
 * This is a skeleton template showing the correct architecture for
 * WebPlatform modules. Customize this for your specific module needs.
 * 
 * Key features demonstrated:
 * - IWebModule interface implementation
 * - Platform-agnostic request/response handling
 * - JSON configuration parsing
 * - Route registration with authentication
 * - OpenAPI documentation integration
 * - Memory-efficient PROGMEM assets
 * - Testability with dependency injection
 */
class WebModuleExample : public IWebModule {
public:
  // Constructor - default for production, can accept mock provider for testing
  WebModuleExample() = default;

  // Module lifecycle methods (IWebModule interface)
  void begin() override;
  void begin(const JsonVariant &config) override;
  void handle() override;

  // IWebModule interface implementation
  std::vector<RouteVariant> getHttpRoutes() override;
  std::vector<RouteVariant> getHttpsRoutes() override;
  
  // Module metadata
  String getModuleName() const override { return "Web Module Example"; }
  // Version must be injected at build time from library.json as WEB_MODULE_VERSION_STR
  // via PlatformIO extra_script. Intentionally fail build if missing.
#ifndef WEB_MODULE_VERSION_STR
#error "WEB_MODULE_VERSION_STR not defined (version_autogen.h missing)."
#endif
  String getModuleVersion() const override { return WEB_MODULE_VERSION_STR; }
  String getModuleDescription() const override {
    return "Example/boilerplate web module - customize for your needs";
  }

  // RequestT/ResponseT are provided by <interface/request_response_types.h>
  // and select the correct request/response type per platform.

  // Route handler methods (unified signatures for ESP32 and native platforms)
  // Customize these for your module's functionality
  void mainPageHandler(RequestT &req, ResponseT &res);
  void statusHandler(RequestT &req, ResponseT &res);
  void configHandler(RequestT &req, ResponseT &res);
  void updateConfigHandler(RequestT &req, ResponseT &res);

  // Public accessors for testing and diagnostics
  // Add getters for any configuration or state you want to expose
  bool isEnabled() const { return enabled; }
  int getExampleValue() const { return exampleValue; }
  const String &getExampleString() const { return exampleString; }

#if defined(NATIVE_PLATFORM)
  // Test-only helper to apply configuration without initializing hardware
  // This allows testing configuration parsing without side effects
  void __test_applyConfig(const JsonVariant &config) { parseConfig(config); }
#endif

private:
  // Module state and configuration
  // Customize these for your module's needs
  bool enabled = true;
  int exampleValue = 0;
  String exampleString = "default";
  unsigned long lastHandleTime = 0;
  
  // Configuration parsing
  void parseConfig(const JsonVariant &config);
  
  // Initialize hardware/resources (called from begin())
  void initializeModule();

  // Helper to reduce platform lookup duplication when creating JSON responses
  // Usage: respondJson(res, [&](JsonObject &json) { json["key"] = value; });
  template <typename Fn>
  inline void respondJson(ResponseT &res, Fn &&fn) {
    IWebPlatformProvider::getPlatformInstance().createJsonResponse(
        res, std::forward<Fn>(fn));
  }
};

// Global instance for production use
// In production, this will be the single instance used by the application
extern WebModuleExample webModuleExample;

#endif // WEB_MODULE_EXAMPLE_H
