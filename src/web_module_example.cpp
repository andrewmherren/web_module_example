#include "web_module_example.h"

// Uncomment and customize these includes as you add assets
// #include "../assets/example_page_html.h"
// #include "../assets/example_script_js.h"
// #include "../assets/example_style_css.h"

// Allow tests to override the periodic handle interval (default 10s)
// Customize this based on your module's needs
#ifndef WEB_MODULE_EXAMPLE_HANDLE_INTERVAL_MS
#define WEB_MODULE_EXAMPLE_HANDLE_INTERVAL_MS 10000UL
#endif

#if defined(ARDUINO) || defined(ESP_PLATFORM)
// Create global instance for production use
// Only available on Arduino/ESP32 platforms; native tests create their own instances
WebModuleExample webModuleExample;
#endif

// ===========================================================================
// Module Lifecycle Methods
// ===========================================================================

void WebModuleExample::begin() {
  DEBUG_PRINTLN("Web Module Example: Initializing...");
  initializeModule();
  DEBUG_PRINTLN("Web Module Example: Initialized successfully");
}

void WebModuleExample::begin(const JsonVariant &config) {
  parseConfig(config);
  begin(); // Call the parameterless version
}

void WebModuleExample::initializeModule() {
  DEBUG_PRINTLN("Web Module Example: Initializing module resources...");
  
  // TODO: Add your module's initialization code here
  // Examples:
  // - Initialize sensors or hardware peripherals
  // - Set up communication protocols (I2C, SPI, etc.)
  // - Load saved state from preferences/EEPROM
  // - Validate configuration
  
  // Example initialization (customize for your module):
  if (enabled) {
    DEBUG_PRINTLN("Web Module Example: Module is enabled");
    // Initialize enabled functionality
  } else {
    DEBUG_PRINTLN("Web Module Example: Module is disabled");
  }
  
  DEBUG_PRINTF("Web Module Example: Example value: %d\n", exampleValue);
  DEBUG_PRINTF("Web Module Example: Example string: %s\n", exampleString.c_str());
}

void WebModuleExample::handle() {
  // Periodic tasks - customize the interval based on your module's needs
  // Check if it's time to run periodic tasks
  if (millis() - lastHandleTime <= WEB_MODULE_EXAMPLE_HANDLE_INTERVAL_MS) {
    return;
  }

  lastHandleTime = millis();
  
  // TODO: Add your periodic tasks here
  // Examples:
  // - Read sensor values
  // - Update cached state
  // - Check for status changes
  // - Perform periodic maintenance
  // - Monitor connection status
  
  // Example periodic task (customize for your module):
  if (enabled) {
    // DEBUG_PRINTLN("Web Module Example: Periodic task running");
    // Perform your periodic work here
  }
}

void WebModuleExample::parseConfig(const JsonVariant &config) {
  if (config.isNull()) {
    DEBUG_PRINTLN("Web Module Example: Using default configuration");
    return;
  }

  // TODO: Parse your module's configuration parameters
  // Customize these based on your module's needs
  
  if (!config["enabled"].isNull()) {
    enabled = config["enabled"].as<bool>();
    DEBUG_PRINTF("Web Module Example: Configured enabled: %s\n",
                 enabled ? "true" : "false");
  }

  if (!config["exampleValue"].isNull()) {
    exampleValue = config["exampleValue"].as<int>();
    DEBUG_PRINTF("Web Module Example: Configured exampleValue: %d\n",
                 exampleValue);
  }

  if (!config["exampleString"].isNull()) {
    exampleString = config["exampleString"].as<const char*>();
    DEBUG_PRINTF("Web Module Example: Configured exampleString: %s\n", 
                 exampleString.c_str());
  }

  // Add more configuration parsing as needed:
  // - Hardware pin assignments
  // - Communication parameters
  // - Feature flags
  // - Operational thresholds
}

// ===========================================================================
// Route Registration
// ===========================================================================

std::vector<RouteVariant> WebModuleExample::getHttpRoutes() {
  return {
    // Main page route - customize authentication based on your needs
    // AuthType::NONE = public, AuthType::SESSION = login required
    WebRoute("/", WebModule::WM_GET,
             [this](RequestT &req, ResponseT &res) {
               mainPageHandler(req, res);
             },
             {AuthType::SESSION}), // Requires login - change to {AuthType::NONE} for public

    // TODO: Uncomment and customize when you add static assets
    // Static assets - typically no authentication required
    // WebRoute("/assets/script.js", WebModule::WM_GET,
    //          [](RequestT &req, ResponseT &res) {
    //            res.setProgmemContent(EXAMPLE_SCRIPT_JS, "application/javascript");
    //            res.setHeader("Cache-Control", "public, max-age=3600");
    //          },
    //          {AuthType::NONE}),

    // Status endpoint - read-only, can be less restricted
    // LOCAL_ONLY allows access from local network without login
    ApiRoute("/api/status", WebModule::WM_GET,
             [this](RequestT &req, ResponseT &res) {
               statusHandler(req, res);
             },
             {AuthType::LOCAL_ONLY, AuthType::SESSION, AuthType::TOKEN},
             API_DOC("Get module status",
                     "Returns current operational status and configuration",
                     "getModuleStatus", {"monitoring"})),

    // Configuration read endpoint
    ApiRoute("/api/config", WebModule::WM_GET,
             [this](RequestT &req, ResponseT &res) {
               configHandler(req, res);
             },
             {AuthType::SESSION, AuthType::TOKEN},
             API_DOC("Get module configuration",
                     "Returns current module configuration settings",
                     "getModuleConfig", {"configuration"})),

    // Configuration update endpoint - requires authentication
    ApiRoute("/api/config", WebModule::WM_POST,
             [this](RequestT &req, ResponseT &res) {
               updateConfigHandler(req, res);
             },
             {AuthType::SESSION, AuthType::PAGE_TOKEN, AuthType::TOKEN},
             API_DOC("Update module configuration",
                     "Updates module configuration settings",
                     "updateModuleConfig", {"configuration"})
               .withRequestBody(R"({
                 "required": true,
                 "content": {
                   "application/json": {
                     "schema": {
                       "type": "object",
                       "description": "Configuration update request",
                       "properties": {
                         "enabled": {
                           "type": "boolean",
                           "description": "Enable or disable the module"
                         },
                         "exampleValue": {
                           "type": "integer",
                           "description": "Example numeric configuration value"
                         },
                         "exampleString": {
                           "type": "string",
                           "description": "Example string configuration value"
                         }
                       }
                     }
                   }
                 }
               })")
               .withRequestExample(R"({
                 "enabled": true,
                 "exampleValue": 42,
                 "exampleString": "custom value"
               })")
               .withResponseExample(R"({
                 "success": true,
                 "message": "Configuration updated successfully"
               })"))
  };
}

std::vector<RouteVariant> WebModuleExample::getHttpsRoutes() {
  // For most modules, HTTPS routes are identical to HTTP routes
  // Customize if you need HTTPS-specific behavior
  return getHttpRoutes();
}

// ===========================================================================
// Route Handler Implementations
// ===========================================================================

void WebModuleExample::mainPageHandler(RequestT &req, ResponseT &res) {
  // TODO: Uncomment when you create your HTML asset file
  // res.setProgmemContent(EXAMPLE_PAGE_HTML, "text/html");
  
  // Placeholder response - replace with actual HTML asset
  const char* html = R"(
<!DOCTYPE html>
<html>
<head>
  <title>Web Module Example</title>
  <style>
    body { font-family: Arial, sans-serif; margin: 20px; }
    .container { max-width: 800px; margin: 0 auto; }
    .status { padding: 10px; background: #f0f0f0; border-radius: 5px; }
  </style>
</head>
<body>
  <div class="container">
    <h1>Web Module Example</h1>
    <div class="status">
      <h2>Module Status</h2>
      <p>This is a placeholder page. Customize this for your module.</p>
      <p>TODO: Create assets/example_page_html.h with your custom interface</p>
    </div>
  </div>
</body>
</html>
  )";
  
  res.setContent(html, "text/html");
}

void WebModuleExample::statusHandler(RequestT &req, ResponseT &res) {
  // Return current module status as JSON
  respondJson(res, [&](JsonObject &json) {
    json["success"] = true;
    json["enabled"] = enabled;
    json["exampleValue"] = exampleValue;
    json["exampleString"] = exampleString.c_str();
    json["uptime"] = millis();
    json["lastHandleTime"] = lastHandleTime;
    
    // TODO: Add your module-specific status information
    // Examples:
    // json["connected"] = isConnected;
    // json["temperature"] = currentTemperature;
    // json["sensor_value"] = sensorReading;
  });
}

void WebModuleExample::configHandler(RequestT &req, ResponseT &res) {
  // Return current configuration as JSON
  respondJson(res, [&](JsonObject &json) {
    json["enabled"] = enabled;
    json["exampleValue"] = exampleValue;
    json["exampleString"] = exampleString.c_str();
    
    // TODO: Add your module-specific configuration fields
  });
}

void WebModuleExample::updateConfigHandler(RequestT &req, ResponseT &res) {
  // Parse JSON from request body
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, req.getBody());

  if (error) {
    res.setStatus(400); // Bad Request
    respondJson(res, [&](JsonObject &json) {
      json["success"] = false;
      json["error"] = "Invalid JSON in request body";
    });
    return;
  }

  // TODO: Validate configuration values before applying
  // Add your validation logic here
  
  // Apply configuration updates
  bool changed = false;
  
  if (!doc["enabled"].isNull()) {
    bool newEnabled = doc["enabled"].as<bool>();
    if (newEnabled != enabled) {
      enabled = newEnabled;
      changed = true;
      DEBUG_PRINTF("Web Module Example: Enabled set to %s\n", 
                   enabled ? "true" : "false");
    }
  }

  if (!doc["exampleValue"].isNull()) {
    int newValue = doc["exampleValue"].as<int>();
    if (newValue != exampleValue) {
      exampleValue = newValue;
      changed = true;
      DEBUG_PRINTF("Web Module Example: Example value set to %d\n", exampleValue);
    }
  }

  if (!doc["exampleString"].isNull()) {
    const char* newString = doc["exampleString"].as<const char*>();
    if (exampleString != newString) {
      exampleString = newString;
      changed = true;
      DEBUG_PRINTF("Web Module Example: Example string set to %s\n", 
                   exampleString.c_str());
    }
  }

  // TODO: Apply additional configuration changes
  // If configuration changes require reinitialization:
  // if (changed) {
  //   initializeModule();
  // }

  // Return success response
  respondJson(res, [&](JsonObject &json) {
    json["success"] = true;
    json["changed"] = changed;
    json["message"] = changed ? "Configuration updated successfully" 
                              : "No changes made";
  });
}
