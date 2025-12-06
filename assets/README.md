# Assets Directory

This directory is for storing static web assets (HTML, CSS, JavaScript) as PROGMEM header files.

## Creating Asset Files

Convert your web assets into C++ header files with PROGMEM storage:

### Example HTML Asset

Create `example_page_html.h`:

```cpp
#ifndef EXAMPLE_PAGE_HTML_H
#define EXAMPLE_PAGE_HTML_H

#include <Arduino.h>

const char EXAMPLE_PAGE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>My Module</title>
  <link rel="stylesheet" href="/my-module/assets/style.css">
</head>
<body>
  <div class="container">
    <h1>My Web Module</h1>
    <p>Custom content here...</p>
  </div>
  <script src="/my-module/assets/script.js"></script>
</body>
</html>
)rawliteral";

#endif // EXAMPLE_PAGE_HTML_H
```

### Example CSS Asset

Create `example_style_css.h`:

```cpp
#ifndef EXAMPLE_STYLE_CSS_H
#define EXAMPLE_STYLE_CSS_H

#include <Arduino.h>

const char EXAMPLE_STYLE_CSS[] PROGMEM = R"rawliteral(
body {
  font-family: Arial, sans-serif;
  margin: 0;
  padding: 20px;
  background-color: #f5f5f5;
}

.container {
  max-width: 800px;
  margin: 0 auto;
  background: white;
  padding: 20px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}
)rawliteral";

#endif // EXAMPLE_STYLE_CSS_H
```

### Example JavaScript Asset

Create `example_script_js.h`:

```cpp
#ifndef EXAMPLE_SCRIPT_JS_H
#define EXAMPLE_SCRIPT_JS_H

#include <Arduino.h>

const char EXAMPLE_SCRIPT_JS[] PROGMEM = R"rawliteral(
(function() {
  'use strict';
  
  // Your JavaScript code here
  console.log('Module loaded');
  
  // Example: Fetch status periodically
  function updateStatus() {
    fetch('/my-module/api/status')
      .then(response => response.json())
      .then(data => {
        console.log('Status:', data);
        // Update UI with status data
      })
      .catch(error => console.error('Error:', error));
  }
  
  // Update every 5 seconds
  setInterval(updateStatus, 5000);
  updateStatus(); // Initial call
})();
)rawliteral";

#endif // EXAMPLE_SCRIPT_JS_H
```

## Using Assets in Your Module

1. **Include the asset header** in your `.cpp` file:
   ```cpp
   #include "../assets/example_page_html.h"
   #include "../assets/example_style_css.h"
   #include "../assets/example_script_js.h"
   ```

2. **Serve the asset** in your route handler:
   ```cpp
   // HTML page
   res.setProgmemContent(EXAMPLE_PAGE_HTML, "text/html");
   
   // CSS stylesheet
   res.setProgmemContent(EXAMPLE_STYLE_CSS, "text/css");
   res.setHeader("Cache-Control", "public, max-age=3600");
   
   // JavaScript
   res.setProgmemContent(EXAMPLE_SCRIPT_JS, "application/javascript");
   res.setHeader("Cache-Control", "public, max-age=3600");
   ```

3. **Register the routes** in `getHttpRoutes()`:
   ```cpp
   WebRoute("/assets/style.css", WebModule::WM_GET,
            [](RequestT &req, ResponseT &res) {
              res.setProgmemContent(EXAMPLE_STYLE_CSS, "text/css");
              res.setHeader("Cache-Control", "public, max-age=3600");
            },
            {AuthType::NONE})
   ```

## Asset Generation Tools

You can use tools to convert files to header format:

```bash
# Example Python script to convert file to header
python -c "
import sys
with open(sys.argv[1], 'r') as f:
    content = f.read()
name = sys.argv[1].upper().replace('.', '_').replace('/', '_')
print(f'#ifndef {name}_H')
print(f'#define {name}_H')
print('#include <Arduino.h>')
print(f'const char {name}[] PROGMEM = R\"rawliteral(')
print(content)
print(')rawliteral\";')
print(f'#endif // {name}_H')
" input.html > output_html.h
```

## Best Practices

1. **Minify assets** before converting to reduce flash memory usage
2. **Use raw string literals** (`R"rawliteral()rawliteral"`) to preserve formatting
3. **Add cache headers** for static assets to improve performance
4. **Keep assets small** - flash memory is limited on ESP32
5. **Consider gzip compression** for large assets (WebPlatform supports this)

## Memory Considerations

- PROGMEM stores data in flash memory, not RAM
- This is essential for ESP32 with limited RAM
- Flash memory is more abundant (~4MB) than RAM (~520KB)
- Large assets can still impact flash usage - monitor with `pio run --target size`
