# SSEM Interface Specification

## Splash Screen

The splash screen informs the user that this is the SSEM Emulator.

The splash screen also displays SD card information:

- Is the SD card present?
- SD card capacity

The splash screen is dismissed when either of the following conditions occur:

- The user touches the screen
- 5 seconds have elapsed since the screen was displayed

## Main Interface

The main interface has a banner at the top of the screen and a footer at the bottom of the screen.  Both header and footer have a white background with black text.

The header font is 14 pixels.

The footer font is 12 pixels.

### Header

The header always shows the title **SSEM - Manchester Baby**.

When a program is loaded, the program name (filename stem, without path or `.ssem` extension) is appended in brackets:

```
SSEM - Manchester Baby (Add)
```

The header is redrawn whenever a new program is loaded via `Display::SetProgramName(const string &name)`.  Passing an empty string clears the program name and reverts to the plain title.

### Footer

The footer displays execution statistics:

```
Instructions: 1,234,567  Time: 3.14 s
```

| Field | Format | Initial value |
|---|---|---|
| Instruction count | Decimal with comma thousands separators | 0 |
| Elapsed time | Seconds to two decimal places | 0.00 |

The footer is updated via `Display::UpdateFooter(uint32_t instructionCount, double elapsedSeconds)`.

Loading a program resets both values to zero and redraws the footer immediately (as part of `SetProgramName`).

### Centre Panel

The centre panel is divided into four sections from left to right:

- Storeline number column
- Storeline LEDs
- Storeline text
- Control panel

#### Storeline Number Column

Each storeline row has a zero-based row index (0–31) drawn to the left of the LED box.  This number sits outside the LED box and is 28 pixels wide.

#### Storeline LEDs

The storeline contents are made up of 32 unsigned 32-bit integers.  Each storeline is displayed as a row of 32 LEDs, one per bit.  Each storeline occupies its own row.

The two states of an LED are represented as follows:

- On: white outer circle filled green
- Off: white outer circle filled black

Each LED cell is 20 × 20 pixels.  The outer circle radius is 9 pixels; the inner fill circle radius is 7 pixels, leaving a 2-pixel white border.

The 32 LEDs in each row are grouped into 8 groups of 4, with an 8-pixel gap between adjacent groups.

The total width of the LED section is `(20 × 32) + (7 × 8)` = **696 pixels**.

The LEDs are sized to fill the space between the header and the footer.

The LED area (all storeline rows combined) is enclosed in a white-bordered box with 4 pixels of padding on all sides.

#### Storeline Text

Each storeline has a text column to the right of the LED section.

The text column has a left margin of 16 pixels to provide visual separation from the LED column.

Each storeline text row contains two fields, left to right:

1. **Hex value** — the storeline word rendered as an 8-digit zero-padded uppercase hexadecimal number (no `0x` prefix), e.g. `0000001A`.  Reserved column width: 72 pixels.
2. **Mnemonic label** — a short instruction label string, initially `JMP 0`.

The text area (all storeline text rows combined) is enclosed in a white-bordered box with 4 pixels of padding on all sides.

## Control Panel

The control panel occupies the remaining area to the right of the storeline text column.  It is inset by 8 pixels of padding on all sides.  The following controls appear from top to bottom in the order listed.

### Halted / Running Indicator

A white-bordered rounded rectangle (48 pixels tall, corner radius 8 pixels) spans the full panel width at the top of the panel.  It displays one of two states:

| State | Text | Text colour |
|---|---|---|
| Halted | `Halted` | Red |
| Running | `Running` | Green |

The indicator is redrawn whenever the running state changes.  It can also be updated programmatically via `Display::SetRunning(bool running)`.

When program execution completes (CPU halted or user pressed Stop), `Display::SetRunning(false)` is called from `app_main`, which redraws the indicator, file list, and action buttons in a single atomic operation.

### Speed Radio Buttons

A "Speed:" label appears above two side-by-side radio buttons:

| Option | Meaning |
|---|---|
| Maximum | Run as fast as the hardware allows |
| Original | Simulate the original 1948 Manchester Baby clock rate |

The selected option has its inner circle filled white; the unselected option has a hollow circle (black inner fill) with a white outer ring.

The currently selected speed is available via `Display::GetSpeed()`, which returns a `Display::SpeedSetting` enum value.  The default on startup is `SpeedSetting::Maximum`.

**Disabled state**: while the SSEM CPU is executing, the speed section is rendered entirely in dark grey (label, radio-button outlines, inner fills, and option text).  Touch events on the speed radio buttons are ignored.  The section is re-enabled when execution stops via `Display::SetSpeedEnabled(bool enabled)`.

### Files

A "Files:" label appears above a scrollable list box.  The list is populated by calling `Display::SetFiles(const vector<string> &files)`, which resets the scroll offset and clears any existing selection.  Only the filename portion of each path is displayed (the directory prefix is stripped).

The list box has:

- A white-bordered outer box.
- A scroll-up arrow at the top of the box.  The arrow is white when upward scrolling is available, dark grey otherwise.
- A scroll-down arrow at the bottom of the box.  The arrow is white when downward scrolling is available, dark grey otherwise.
- A row of visible file entries between the arrows.  Each row is 26 pixels tall; the number of visible rows is determined by the available height.
- The selected row (if any) is highlighted with a white background and black text.  All other rows have a black background and white text.

**Disabled state**: while the SSEM CPU is executing, all list items are rendered with dark grey text on a black background, both scroll arrows are dark grey, and touch events on the list (item selection and scroll arrows) are ignored.

### Load Button

A rounded-rectangle button labelled **Load** spans the full panel width above the Stop/Run button.  It is positioned at the bottom of the panel, immediately above the Stop/Run button.

| State | Border | Text |
|---|---|---|
| Enabled | White | White |
| Disabled | Dark grey | Dark grey |

The Load button is enabled automatically when a file is selected in the list.  It becomes disabled whenever the SSEM CPU starts running and is restored to its previous enabled state (enabled if a file was already selected) when execution stops.

When the enabled Load button is pressed, `Display::LoadCallback` is called with the full path of the currently selected file.  The callback is registered via `Display::SetLoadCallback()`.

### Stop / Run Button

A rounded-rectangle button spans the full panel width at the very bottom of the panel.

| Running state | Button label | Border | Text |
|---|---|---|---|
| Halted | `Run` | White | White |
| Running | `Stop` | White | White |
| Disabled (either) | — | Dark grey | Dark grey |

The button is enabled by posting a `DisplayMessage` with a non-null `controlState` field (see [Display Task Behaviour](#display-task-behaviour)).  It can also be enabled or disabled at any time via `Display::SetStopRunEnabled(bool enabled)`.

#### Pressing Run (CPU halted → running)

When the **Run** button is pressed:

1. `_running` is set to `true`.
2. The Halted/Running indicator changes to **Running** (green text).
3. The speed radio buttons are disabled (rendered in dark grey; touch events ignored).
4. The Load button is disabled.
5. The file list items and scroll arrows are rendered in dark grey; touch events on the list are ignored.
6. The button label changes to **Stop**.
7. The framebuffer is flushed to the display.
8. `Display::StopRunCallback` is called with `true` (running).

#### Pressing Stop (CPU running → halted)

When the **Stop** button is pressed:

1. `_running` is set to `false`.
2. The Halted/Running indicator changes to **Halted** (red text).
3. The speed radio buttons are re-enabled (white, touch events active).
4. The Load button is re-enabled if a file is currently selected; otherwise it remains disabled.
5. The file list items and scroll arrows are re-enabled.
6. The button label changes to **Run**.
7. The framebuffer is flushed to the display.
8. `Display::StopRunCallback` is called with `false` (halted).

The callback is registered via `Display::SetStopRunCallback()`.

## Display Task

The Display code runs in a dedicated FreeRTOS task (`DisplayTask`, stack 8192 bytes, priority 5).  Communication uses a depth-4 message queue; `PostMessage()` blocks until the message is accepted.

All M5GFX draw calls — whether from `DisplayTask` or from public methods called directly from `app_main` (e.g. `UpdateFooter`, `SetRunning`) — are serialised by a FreeRTOS mutex (`_displayMutex`).  Every public method that touches the display takes the mutex before the first draw call and releases it after `display()` returns.

### Message Definition (`DisplayMessage`)

The message holds a complete snapshot of the display state:

| Field | Type | Description |
|---|---|---|
| `storelineValues` | `uint32_t[32]` | Current value of each storeline word |
| `storelineText` | `char[32][32]` | Mnemonic label for each storeline |
| `controlState` | `void *` | `nullptr` = no change to Stop/Run state; non-null = enable the Stop/Run button |
| `halted` | `bool` | When `true`, restores the full stopped UI state (indicator, speed section, file list, buttons) |

### Display Task Behaviour

The Display task blocks on `xQueueReceive`.  On each message received it:

1. Compares each incoming storeline value against the stored value.  Only rows whose value has changed are flagged for redraw.
2. Calls `DrawAllStorelines()` to redraw all flagged rows.
3. If `controlState` is non-null, enables the Stop/Run button and redraws the action buttons.
4. Calls `display()` to flush the framebuffer to the MIPI-DSI panel.

### `app_main` Behaviour

The `app_main` function:

1. Calls `Setup()` to initialise the display and all peripherals.
2. Calls `ClearStoreLinesAndUpdateDisplay()`, which posts a `DisplayMessage` with all storeline values set to `0`, all labels set to `JMP 0`, and `controlState` set to `nullptr`.
3. Registers `LoadFile` as the `LoadCallback` via `Display::SetLoadCallback()`.
4. Registers `OnStopRunPressed` as the `StopRunCallback` via `Display::SetStopRunCallback()`.
5. If the SD card is mounted, scans it for `.ssem` files and passes the result to `Display::SetFiles()`.
6. Calls `vTaskDelete(nullptr)` — the `app_main` task is deleted; all further activity is driven by FreeRTOS tasks owned by the component layer.

### Load File Behaviour (`LoadFile` callback)

When the Load button is pressed and a valid file is selected, `LoadFile` is invoked with the full file path.  It:

1. Reads the file from the SD card line by line.
2. Compiles the SSEM source into store lines via `Compiler::Compile()`.
3. Destroys any existing `Cpu` instance and creates a new one from the compiled store lines.
4. Calls `Cpu::Reset()`.
5. Posts a `DisplayMessage` with the compiled storeline values and disassembled mnemonics, and `controlState` set to a non-null sentinel value to enable the Stop/Run button.
6. Strips the directory prefix and `.ssem` extension from the file path to derive the program name, then calls `Display::SetProgramName()`.  This updates the header, resets the footer counters to zero, and redraws the footer.

### Stop/Run Callback Behaviour (`OnStopRunPressed` callback)

`OnStopRunPressed(bool running)` is invoked by the Display layer after the button state and all visual updates have been applied.  It receives the new intended running state (`true` = start execution, `false` = stop) and is responsible for starting or stopping the SSEM CPU accordingly.

#### Execution loop and footer updates

While the CPU is running, `app_main` calls `Display::UpdateFooter(instructionCount, elapsedSeconds)` whenever either of the following conditions is met:

- 1,000 or more instructions have been executed since the last footer update.
- 1 second or more has elapsed since the last footer update.

A final `UpdateFooter` call is made with the definitive values immediately after the execution loop exits.

#### Post-execution UI restore

After the execution loop exits (CPU halted or user stopped), `app_main` calls the following sequence to restore the full stopped UI state:

1. `Display::SetRunning(false)` — indicator → **Halted**, button label → **Run**, file list re-enabled.
2. `Display::SetSpeedEnabled(true)` — speed radio buttons re-enabled.
3. `Display::SetLoadEnabled(true)` — Load button re-enabled.

#### Load Button

The load button can only be enabled when:

- No program is running
- A file has been selected.

#### Stop / Run Button

This button is only enabled when a file has been selected.

The button will raise a callback.

An API allows the state to be changed.

---

## Message Dialog

A modal message dialog can be displayed over the current screen contents at any time by calling:

```cpp
Display::ShowMessageDialog(const char *title, const char *message);
```

### Appearance

The dialog is a centred 600 × 300 pixel rounded-rectangle popup (corner radius 16 pixels) drawn over the existing screen contents.

| Layer | Description |
|---|---|
| Outer border | White, 2 pixels |
| Inner fill | Black |

From top to bottom the dialog contains three elements:

#### Title

- Drawn in bold white using `fonts::Font4`.
- Centred horizontally within the dialog.
- Vertically centred within a 56-pixel title band at the top of the dialog.
- The bold effect is achieved by rendering the string twice: once at the nominal position and once shifted one pixel to the right.
- A white horizontal rule separates the title band from the message body.
- The `title` parameter is mandatory and must not be `nullptr` or empty.

#### Message body

- Drawn in white using `fonts::Font4`.
- Centred horizontally within the dialog.
- The text block is vertically centred in the area between the horizontal rule and the OK button.
- Multi-line messages are supported using `\n` as the line separator.  Each line is drawn individually at 20-pixel line spacing.

#### OK button

- A rounded-rectangle button (160 × 44 pixels, corner radius 8 pixels) centred horizontally at the bottom of the dialog, 20 pixels above the dialog's lower edge.
- White border, black fill, white label text ("OK").

### Behaviour

1. `ShowMessageDialog` draws the dialog immediately, then returns to the caller without blocking.
2. A short-lived FreeRTOS task (`MsgDialogTask`, stack 4096 bytes, priority 5) is spawned to wait for dismissal.
3. The normal panel touch callback (`OnPanelTouch`) is unregistered for the duration; touch events are routed exclusively to the dialog's own handler (`OnMessageDialogTouch`).
4. `DisplayTask` continues to receive and apply queued `DisplayMessage` state updates while the dialog is visible, but suppresses all draw calls until the dialog is dismissed.
5. When the user taps the OK button, `MsgDialogTask` redraws the full main interface (clearing the dialog) and re-registers `OnPanelTouch`.  All 32 storeline dirty flags are forced to `true` before `DrawAllStorelines()` to guarantee a full repaint.
6. `_prevTouched` is set to `true` before re-registering `OnPanelTouch` to prevent the finger-lift from the OK button generating a spurious panel press.

### Thread safety

`ShowMessageDialog` may be called from any task context, including from within a touch callback (e.g. the `LoadCallback` invoked by `TouchTask`).  The non-blocking spawn pattern avoids the deadlock that would result from a blocking wait inside `TouchTask`.

All draw calls inside `ShowMessageDialog` and `MsgDialogTask` are serialised by `_displayMutex`.

### Example usage

```cpp
Display::ShowMessageDialog("Load Error", "File not found:\n/sdcard/missing.ssem");
Display::ShowMessageDialog("Information", "Program loaded successfully.");
```