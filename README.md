## OverlayWindowApp

## Dependencies

- [libcpr](https://docs.libcpr.org/advanced-usage.html#asynchronous-requests)

### Content Groups

- Controller Dashboard, This window is a collection of the renderings of each controller instance.

> *Keybind Settings* omitted for now.

> Removed the *delays* section as modifying global delays were confusing to use correctly *(often required browsing the source code)* and would break other dependents in unexpected ways.

> Removed *points of interest* or *positions* as these will be provided by a external service and are expected work. If they do not, it is the responsibility of the vendor to update them.

## Controllers

### Back-End
- <a href="#mouse_controller">`MouseController`</a>
- <a href="#keyboard_controller">`KeyboardController`</a>

### Front-End

- <a href="#auto_clicker_controller">`AutoClickerController`</a>
- <a href="#auto_walker_controller">`AutoWalkerController`</a>

Every controller will implement the *Controller* interface which ensures it is displayed in the current imgui window.

A controller should be runnable by either ui input or a hotkey, never both.

**The following table illustrates the schema of core functionalities and their used by the controller.**

| Functionality | Used | Description |
| --- | --- | --- |
| Display | Yes/No | Whether the controller is presented within the controller dashboard. |
| Overlay Display | Yes/No | Whether the controller shows state to the user while in use in the background. |
| Hotkey | Yes/No | Whether the controller is toggleable by a hotkey and therefore requires access to the hotkey system. |

**The following table illustrates the schema of configuration variables and their usage by their controller.**

| Config Variable | Data Type | Default Value | Description |
| --- | --- | --- | --- |
| query | `string` | fiber | An optional query string to be used when transfering all items. |
| key | `string` | `null` | A key to be used by the auto clicker. If `null`, then the default left click is used unless right click overrides.

### `MouseController`

This controller provides a means to send various mouse click events to the input stream.

This function is implemented using [SendInput](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-sendinput).

Currently supported mouse click types:

- Left
- Right
- Middle

> Clicking with positions specified is onyl supported on the primary monitor is supported and changing display resolution at runtime will result in click miss-alignment.

<a name="auto_clicker_controller"></a>
###  `AutoClickerController`

Clicks various mouse and keyboard buttons at a set constant rate.

| Functionality | Used | Description |
| --- | --- | --- |
| Display | Yes | This controller should be adjustable by the user. |
| Overlay Display | Yes | It can be difficult to know if the auto clicker is running, especially on slower speeds. |
| Hotkey | Yes | This is the only good way to control an auto clicker. |

| Config Variable | Data Type | Default Value | Description |
| --- | --- | --- | --- |
| hotkey | `string` | null | The hotkey used to toggle the auto clicker.
| key | `string` | null | A key to be used by the auto clicker. If `null`, then the default left click is used unless right click overrides.
| interval | `uint` | 50 | The interval in milliseconds that the auto clicker clicks at.

<a name="auto_walker_controller"></a>
### `AutoWalkerController`

Simulates walking or optional sprinting by managing a choosen key and its *down* state.

| Functionality | Used | Description |
| --- | --- | --- |
| Display | Yes | This controller should be adjustable by the user. |
| Overlay Display | Yes | In tense, laggy moments when fighting other players, knowing the state of this hotkey is important. |
| Hotkey | Yes | This should be quickly togglable in-game. |

| Config Variable | Data Type | Default Value | Description |
| --- | --- | --- | --- |
| hotkey | `string` | null | The hotkey used to toggle auto walk.
| key | `string` | w | The key used to simulate auto walking.
| sprint_mod | `string` | shift | The key used in conjunction with *key* variable to simulate sprinting.

#### Flows of Execution for State

The auto walker's state is more complicated than most, therefore in the following list *key* is the key being simulated to walk.

- Off -> Hotkey Pressed -> On
- On -> Hotkey Pressed -> Off
- Off -> Key Pressed -> Off
- **On -> Key Pressed -> Off**

If the user presses the simulate key to walk while auto walk is on, it should turn off essentially seamlessly giving control back to the user.

To achieve this functionality, the `AutoWalkerController` needs to know when the *key* is clicked when the auto walker is running. We can either register *key* as a temporary hotkey, or register a temporary key logger to listen for incoming *key* presses while running. The latter is a simpler option and can be done with [SetWindowHooks](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw) + *WH_KEYBOARD_LL* and then checkout [LowLevelKeyboardProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc). Ensure that once entering an inactive state, the hook is unregistered.

## Configurations

A unique configuration exists for each `Controller` containing nessesary information for execution. Some configurations come with hard-coded default values as a fallback, while others do not.

The two ways to create a configuration are as follow:

- ***Default()***, A static method that returns a configuration using default hard-coded values.
- `async` ***Load()***, A static method that returns a configuration using updated values from an external source.

## HotKeyDispatcher

The `HotKeyDispatcher` is responsible for receiving *WM_HOTKEY* messages in the thread message pump, deciphering the hotkey in question, and calling a handler. 

### Todo

- [x] `Controller`, Abstract class inherited by all controllers.
- [ ] `IHotkeyable`, Implemented by controllers needing hotkey functionalities.
- [ ] `IUsageDisplayable`, Implemented by controllers needing to display state to the screen.

### Unregister

When unregistering a hotkey, all resources should all be released if not already and the controller should be put into a not-running state.
