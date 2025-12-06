# Web Module Example - Boilerplate Template for WebPlatform Modules

This is a boilerplate/skeleton template for creating new [WebPlatform](https://github.com/andrewmherren/web_platform) modules for ESP32 devices. Use this as a starting point for developing your own custom web modules with proper architecture, testing, and documentation.

## 📋 Purpose

This template demonstrates the **correct architecture** for WebPlatform modules:
- ✅ **Module isolation** - communicates only through the `IWebModule` interface
- ✅ **Dependency injection** - uses `IWebPlatformProvider` for testability
- ✅ **Zero-cost abstractions** - efficient production builds with full test coverage
- ✅ **OpenAPI documentation** - optional compile-time API documentation
- ✅ **Comprehensive testing** - both native (fast) and ESP32 (hardware) tests
- ✅ **Memory efficiency** - PROGMEM assets, minimal heap usage

## 🎯 When to Use This Template

Use this template when creating a new WebPlatform module that:
- Provides web UI and/or REST API endpoints
- Integrates hardware peripherals or sensors
- Needs authentication and session management
- Requires both development testing and production deployment
- Should be publishable as an independent PlatformIO library

## 🚀 Quick Start

### 1. Copy This Template

```bash
# Copy the entire web_module_example directory
cp -r lib/web_module_example lib/my_new_module

# Update git remote (after creating your repo)
cd lib/my_new_module
git init
git remote add origin https://github.com/yourusername/my_new_module.git
```

### 2. Rename Files and Classes

Update the following files with your module name:

**Files to rename:**
- `include/web_module_example.h` → `include/my_new_module.h`
- `src/web_module_example.cpp` → `src/my_new_module.cpp`

**Update class names in code:**
- `WebModuleExample` → `MyNewModule`
- `webModuleExample` (global instance) → `myNewModule`

**Update library metadata:**
- `library.json` - name, description, keywords, repository URL
- `README.md` - customize this file for your module's purpose

### 3. Customize Module Functionality

Implement your module's core functionality:

1. **Add module-specific member variables** (sensors, hardware interfaces, state)
2. **Implement route handlers** (web pages, API endpoints)
3. **Create assets** (HTML, CSS, JavaScript in `assets/` directory)
4. **Add configuration parsing** (extend `parseConfig()` method)
5. **Update module metadata** (`getModuleName()`, `getModuleDescription()`, etc.)

### 4. Write Tests

Follow the testing patterns:

**Native tests** (`test/native/src/test_*.cpp`):
- Fast, local C++ tests with mocked Arduino APIs
- Test business logic, request handlers, configuration
- Run with: `pio test -e test_native`

**ESP32 tests** (`test/esp32/src/test_*.cpp`):
- Hardware verification tests on actual ESP32
- Test hardware initialization, compilation
- Run with: `pio test -e test_esp32`

### 5. Build and Deploy

```bash
# Test your module
pio test -e test_native

# Build for ESP32
pio run -e test_esp32

# Publish as PlatformIO library
# Update library.json with proper dependencies
# Tag release and publish to GitHub
```

## 📚 Architecture Overview

### Module Structure

```
my_new_module/
├── library.json              # PlatformIO library metadata
├── platformio.ini            # Build/test configuration (usually universal)
├── README.md                 # Module documentation (this file)
├── LICENSE                   # MIT license
├── include/
│   └── my_new_module.h      # Module header with IWebModule interface
├── src/
│   └── my_new_module.cpp    # Module implementation
├── assets/                   # Static web assets (HTML/CSS/JS)
│   ├── example_page_html.h  # PROGMEM HTML content
│   └── example_script_js.h  # PROGMEM JavaScript
├── test/
│   ├── unity_config.h       # Unity test framework configuration
│   ├── test_entrypoint.cpp  # Test runner entry point
│   ├── native/
│   │   └── src/
│   │       └── test_my_module.cpp  # Native unit tests
│   └── esp32/
│       └── src/
│           └── test_my_module.cpp  # ESP32 hardware tests
└── tools/
    └── generate_coverage.ps1 # Code coverage generation script
```

### Key Interfaces

#### IWebModule Interface

Your module must implement:

```cpp
class MyNewModule : public IWebModule {
public:
  // Lifecycle methods
  void begin() override;
  void begin(const JsonVariant &config) override;
  void handle() override;

  // Route registration
  std::vector<RouteVariant> getHttpRoutes() override;
  std::vector<RouteVariant> getHttpsRoutes() override;

  // Module metadata
  String getModuleName() const override;
  String getModuleVersion() const override;
  String getModuleDescription() const override;

  // Route handlers (unified signatures for ESP32/native)
  void mainPageHandler(RequestT &req, ResponseT &res);
  void apiStatusHandler(RequestT &req, ResponseT &res);
  // ... more handlers ...
};
```

#### Platform-Agnostic Request/Response

Use platform-agnostic typedefs for cross-platform compatibility:

```cpp
#if defined(ARDUINO) || defined(ESP_PLATFORM)
  using RequestT = WebRequest;
  using ResponseT = WebResponse;
#else
  using RequestT = WebRequestCore;
  using ResponseT = WebResponseCore;
#endif
```

## 🔧 Configuration Pattern

Modules receive configuration through `begin(const JsonVariant &config)`:

```cpp
void MyNewModule::begin(const JsonVariant &config) {
  parseConfig(config);
  begin(); // Call parameterless version
}

void MyNewModule::parseConfig(const JsonVariant &config) {
  if (config.isNull()) {
    DEBUG_PRINTLN("MyNewModule: Using default configuration");
    return;
  }

  if (config.containsKey("myParam")) {
    myParam = config["myParam"].as<int>();
  }
  // ... parse other config parameters ...
}
```

**Registration example:**

```cpp
StaticJsonDocument<256> moduleConfig;
moduleConfig["myParam"] = 42;
moduleConfig["enabled"] = true;
webPlatform.registerModule("/my-module", &myNewModule, moduleConfig.as<JsonVariant>());
```

## 🛣️ Route Registration Pattern

Register routes with authentication requirements and optional OpenAPI docs:

```cpp
std::vector<RouteVariant> MyNewModule::getHttpRoutes() {
  return {
    // Web page - session authentication
    WebRoute("/", WebModule::WM_GET,
             [this](RequestT &req, ResponseT &res) {
               mainPageHandler(req, res);
             },
             {AuthType::SESSION}),

    // API endpoint - token or session authentication with OpenAPI docs
    ApiRoute("/api/status", WebModule::WM_GET,
             [this](RequestT &req, ResponseT &res) {
               apiStatusHandler(req, res);
             },
             {AuthType::SESSION, AuthType::TOKEN},
             API_DOC("Get module status",
                     "Returns current operational status and metrics",
                     "getStatus", {"monitoring"})),

    // Static asset - no authentication required
    WebRoute("/assets/script.js", WebModule::WM_GET,
             [](RequestT &req, ResponseT &res) {
               res.setProgmemContent(EXAMPLE_SCRIPT_JS, "application/javascript");
             },
             {AuthType::NONE})
  };
}
```

## 🧪 Testing Patterns

### Native Test Example

```cpp
#include <unity.h>
#include <ArduinoFake.h>
#include <testing/testing_platform_provider.h>
#include "my_new_module.h"

static void test_module_metadata() {
  MyNewModule module;
  TEST_ASSERT_EQUAL_STRING("My New Module", module.getModuleName().c_str());
  TEST_ASSERT_EQUAL_STRING("0.1.0", module.getModuleVersion().c_str());
}

static void test_configuration_parsing() {
  MyNewModule module;
  DynamicJsonDocument doc(256);
  doc["myParam"] = 42;
  module.begin(doc.as<JsonVariant>());
  TEST_ASSERT_EQUAL(42, module.getMyParam());
}

void register_my_module_tests() {
  RUN_TEST(test_module_metadata);
  RUN_TEST(test_configuration_parsing);
  // ... more tests ...
}
```

### ESP32 Hardware Test Example

```cpp
#include <unity.h>
#include <Arduino.h>
#include "my_new_module.h"

void test_esp32_hardware_init() {
  MyNewModule module;
  module.begin();
  TEST_ASSERT_TRUE(true); // Verify no crash
}

void test_esp32_handle_no_crash() {
  MyNewModule module;
  module.begin();
  module.handle();
  TEST_ASSERT_TRUE(true);
}
```

## 📝 OpenAPI Documentation

Enable OpenAPI documentation with build flags:

```ini
# platformio.ini
build_flags = 
    -DWEB_PLATFORM_OPENAPI=1  # Enable OpenAPI docs
```

**Document your API routes:**

```cpp
ApiRoute("/api/data", WebModule::WM_GET,
         [this](RequestT &req, ResponseT &res) {
           dataHandler(req, res);
         },
         {AuthType::TOKEN},
         API_DOC("Get data", "Returns current data readings", "getData", {"data"})
           .withResponseExample(R"({
             "timestamp": "2025-11-08T12:00:00Z",
             "value": 42.5
           })"))
```

## 🔒 Authentication Patterns

Choose appropriate authentication for each route:

- **`AuthType::NONE`** - Public access (use sparingly)
- **`AuthType::LOCAL_ONLY`** - Only from local network
- **`AuthType::SESSION`** - Web interface login required
- **`AuthType::TOKEN`** - API token required
- **`AuthType::PAGE_TOKEN`** - CSRF page token required

**Example combinations:**

```cpp
// Public monitoring endpoint (read-only)
{AuthType::LOCAL_ONLY}

// Web UI page with CSRF protection
{AuthType::SESSION, AuthType::PAGE_TOKEN}

// API endpoint for external access
{AuthType::TOKEN}

// Multi-auth API (session OR token)
{AuthType::SESSION, AuthType::TOKEN}
```

## 💾 Memory Efficiency

### PROGMEM Assets

Store static assets in flash memory:

```cpp
// In assets/example_page_html.h
const char EXAMPLE_PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>My Module</title></head>
<body>
  <h1>My Module Interface</h1>
</body>
</html>
)rawliteral";

// In route handler
res.setProgmemContent(EXAMPLE_PAGE_HTML, "text/html");
```

### Efficient JSON Responses

Use the `respondJson` helper to avoid platform lookup duplication:

```cpp
template <typename Fn>
inline void respondJson(ResponseT &res, Fn &&fn) {
  IWebPlatformProvider::getPlatformInstance().createJsonResponse(
      res, std::forward<Fn>(fn));
}

// Usage in handler
void MyNewModule::statusHandler(RequestT &req, ResponseT &res) {
  respondJson(res, [&](JsonObject &json) {
    json["status"] = "ok";
    json["value"] = currentValue;
  });
}
```

## 🔍 Debug Output

Use `DEBUG_*` macros for platform-independent debug output:

```cpp
DEBUG_PRINTLN("Module initialized");
DEBUG_PRINT("Value: ");
DEBUG_PRINTLN(value);
DEBUG_PRINTF("Formatted: %d %s\n", number, string);
```

These compile to `Serial` calls on Arduino/ESP32 and stdout on native platforms.

## 🚦 Module Lifecycle

**Typical lifecycle in main application:**

```cpp
void setup() {
  Serial.begin(115200);
  
  // 1. Configure module
  StaticJsonDocument<256> config;
  config["param"] = value;
  
  // 2. Register module with platform
  webPlatform.registerModule("/my-module", &myNewModule, config.as<JsonVariant>());
  
  // 3. Start platform (calls module.begin())
  webPlatform.begin("MyDevice");
}

void loop() {
  // Platform calls module.handle() periodically
  webPlatform.handle();
  delay(10);
}
```

## Related Projects

- **[web_platform](https://github.com/andrewmherren/web_platform)**: Core WebPlatform implementation
- **[web_platform_interface](https://github.com/andrewmherren/web_platform_interface)**: Core interface and testing library for WebPlatform ecosystem
- **[maker_api](https://github.com/andrewmherren/maker_api)**: Interactive API explorer module
- **[usb_pd_controller](https://github.com/andrewmherren/usb_pd_controller)**: USB-C Power Delivery control module
- **[ota_update](https://github.com/andrewmherren/ota_update)**: Over-the-air firmware update module

## 📖 Additional Resources

- **[Web Platform Architecture Guide](.github/copilot-instructions.md)** - Full architecture details
- **[WebPlatform Core](https://github.com/andrewmherren/web_platform)** - Core platform library
- **[WebPlatform Interface](https://github.com/andrewmherren/web_platform_interface)** - Interface definitions
- **[USB PD Controller](https://github.com/andrewmherren/usb_pd_controller)** - Real module example
- **[Maker API](https://github.com/andrewmherren/maker_api)** - API explorer module

## ✅ Pre-Release Checklist

Before publishing your module:

- [ ] Update `library.json` with correct name, description, keywords
- [ ] Add `web_platform_interface` to `dependencies_for_release`
- [ ] Create comprehensive README with examples
- [ ] Write native tests covering main functionality
- [ ] Write ESP32 hardware tests
- [ ] All tests pass: `pio test -e test_native && pio test -e test_esp32`
- [ ] Generate assets as `.h` files in `assets/` directory
- [ ] Document all API endpoints with OpenAPI
- [ ] Add authentication to all routes appropriately
- [ ] Test memory usage with heap monitoring
- [ ] Create GitHub repository and update library.json URLs
- [ ] Tag release (v0.1.0) and publish

## 📄 License

MIT License - See LICENSE file for details

**Made with ⚡ for the ESP32 WebPlatform ecosystem**
