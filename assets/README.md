# Assets Directory

Web assets (HTML, CSS, JavaScript) get compiled into the firmware as
PROGMEM string constants, but you don't write those `.h` files by hand -
you write real `.html`/`.css`/`.js` files under `assets/src/`, and a build
script (`generate_web_assets.py`, part of `web_platform`) converts them into
the `assets/*.h` headers that actually get `#include`d and compiled.

## Adding an asset

1. Create the real file under `assets/src/`, e.g. `assets/src/example_page.html`.
   Write normal HTML/CSS/JS here - real editor syntax highlighting/linting
   works because it's a real file with the right extension.

2. Make sure `web_platform` is in this project's `lib_deps` (it almost
   certainly already is, since you need it to register routes/serve
   responses anyway), and add the generator as an `extra_scripts` entry in
   `platformio.ini` if it isn't there already:

   ```ini
   extra_scripts =
       ${test_base.extra_scripts}
       .pio/libdeps/${this.__env__}/web_platform/scripts/generate_web_assets.py
   ```

   (Use the literal env name instead of `${this.__env__}` if that
   substitution doesn't resolve for your PlatformIO version - see how
   `usb_pd_controller`/`maker_api` wire this in for a working example.)

3. Build (`pio run`/`pio test`) - the script runs automatically and writes
   `assets/example_page_html.h`, containing `const char EXAMPLE_PAGE_HTML[]
   PROGMEM = ...`. **Commit the generated header alongside your source
   change** - anything that consumes this repo as a pinned `lib_dep` never
   runs the generator itself, so the checked-in header has to already be
   current (same reason `include/version_autogen.h` is committed too).

### Naming convention

No configuration needed for the common case: `assets/src/<name>.<ext>`
generates `assets/<name>_<ext>.h`, with the PROGMEM array named
`<NAME>_<EXT>` and the include guard `<NAME>_<EXT>_H`. For example:

- `assets/src/example_page.html` -> `assets/example_page_html.h`,
  `EXAMPLE_PAGE_HTML`
- `assets/src/example_style.css` -> `assets/example_style_css.h`,
  `EXAMPLE_STYLE_CSS`
- `assets/src/example_script.js` -> `assets/example_script_js.h`,
  `EXAMPLE_SCRIPT_JS`

If you need a different header filename, variable name, or guard than that
produces, add an entry to `assets/asset_manifest.json` (create it if it
doesn't exist) rather than fighting the naming convention:

```json
{
  "example_style.css": { "var": "CUSTOM_STYLE_NAME" }
}
```

**One file, one constant.** If you're tempted to put two unrelated
HTML/JS/CSS documents in one source file (e.g. a normal page and an error
variant), don't - each source file maps to exactly one generated constant.
Use two files instead.

## Using assets in your module

Include the generated header and serve it in a route handler:

```cpp
#include "../assets/example_page_html.h"
#include "../assets/example_style_css.h"
#include "../assets/example_script_js.h"
```

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

Register the routes in `getHttpRoutes()`:

```cpp
WebRoute("/assets/style.css", WebModule::WM_GET,
         [](RequestT &req, ResponseT &res) {
           res.setProgmemContent(EXAMPLE_STYLE_CSS, "text/css");
           res.setHeader("Cache-Control", "public, max-age=3600");
         },
         {AuthType::NONE})
```

## Best practices

1. **Minify assets** before committing to reduce flash memory usage, if size
   becomes a concern
2. **Add cache headers** for static assets to improve performance
3. **Keep assets small** - flash memory is limited on ESP32
4. **Consider gzip compression** for large assets (WebPlatform supports this)

## Memory considerations

- PROGMEM stores data in flash memory, not RAM
- This is essential for ESP32 with limited RAM
- Flash memory is more abundant (~4MB) than RAM (~520KB)
- Large assets can still impact flash usage - monitor with `pio run --target size`
