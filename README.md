## OverlayWindowApp



### Todo Notes

- Implement a better hotkey system so we can use Keys + Modifiers
- Develop the Autonomous Worker and the *async* aspects of controller functions
- Need a better way to return specific errors, maybe use std::expect<T, E> like Result<T, E> from Rust lang
- Should make a custom exception for when users are manually shutting down autonomous mode
- Need some type of logging so that we can get insite into how things are running 

## Dependencies

- [imgui](https://github.com/ocornut/imgui)

### Content Groups

- Controller Dashboard, This window is a collection of the renderings of each controller instance.

> *Keybind Settings* omitted for now.

> Removed the *delays* section as modifying global delays were confusing to use correctly *(often required browsing the source code)* and would break other dependents in unexpected ways.

> Removed *points of interest* or *positions* as these will be provided by a external service and are expected work. If they do not, it is the responsibility of the vendor to update them.

### `GlobalHotKeyManager`

Manages registration, unregistration, and invocation of handler functions for global hotkeys.

This class depends on *win32* [SetWindowsHookExW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw) and [UnregisterHotKey](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterhotkey) to operate.

#### `HotKey`

Represents a *key* and a *modifier* pair used to receive events when a that combo is pressed. 

### `GlobalKeyListener`

Manages registration, unregistration and invocation of handler functions when keys are pressed.

This class depends on *win32* [SetWindowHooks](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw) given *WH_KEYBOARD_LL* to call an application defined [LowLevelKeyboardProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelkeyboardproc). This hook is register with 1 or more listeners and unregisters otherwise.

> Only one instance in memory of this class should exist in a given program.

## Configurations

A configuration class contains high-level and safe representations of persistent state. For example, it would contain a `HotKey` type instead of the indepent `std::string` for both the *key* and *modifier* values. Therefore, allowing the controllers work safer and easier with persistent state.

The type **std::optional** should be used to wrap config variables that may or may not be set by the user.

Data Tranfer Objects *(DTOs)* should be used when originally fetching configurations.

## Controllers

### Back-End
- <a href="#mouse_controller">`MouseController`</a>
- <a href="#keyboard_controller">`KeyboardController`</a>

### Front-End

- <a href="#auto_clicker_controller">`AutoClickerController`</a>
- <a href="#auto_walker_controller">`AutoWalkerController`</a>

A controller should be runnable by either ui input or a hotkey, never both.

#### Interfaces

- `IHotKeyable`, Implement to use hotkeys.
- `IKeyListenable`, Implement to listen to keystrokes.
- `IDisplayCtrlPanel`, Implement to display a usage options control panel.
- `IDisplayHUD`, Implement to display a HUD showing state.

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

| Config Variable | Data Type | Default Value | Description |
| --- | --- | --- | --- |
| hotkey_ | `string` | null | Hotkey used to toggle the auto clicker.
| interval_ | `uint` | 50 | Interval in milliseconds that the auto clicker clicks.
| key_ | `string` | null | An optional key stroke to be used instead of clicking.
| use_right_click_ | `bool` | false | Optional override for *right* clicking.

<a name="auto_walker_controller"></a>
### `AutoWalkerController`

Simulates walking or optional sprinting by managing a choosen key and its *down* state.

| Config Variable | Data Type | Default Value | Description |
| --- | --- | --- | --- |
| hotkey_ | `string` | null | Hotkey used to toggle auto walk.
| walk_forward_key_ | `string` | W | Key used to simulate auto walking and detect is manual override.
| walk_backwards_key_ | `string` | S | Key to detect manual override.
| sprint_mod_ | `string` | shift | Key modifier used to sprint.

#### Flows of Execution for State

The auto walker's state is more complicated than most, therefore in the following list *key* is the key being simulated to walk.

- Off -> Hotkey Pressed -> On
- On -> Hotkey Pressed -> Off
- Off -> Override Key Pressed -> Off
- **On -> Override Key Pressed -> Off**

If the user presses an override key while auto walk is engaged, auto walk will toggle off.

## Configurations

A unique configuration exists for each `Controller` containing nessesary information for execution. Some configurations come with hard-coded default values as a fallback, while others do not.

The two ways to create a configuration are as follow:

- ***Default()***, A static method that returns a configuration using default hard-coded values.
- `async` ***Load()***, A static method that returns a configuration using updated values from an external source.

## HotKeyDispatcher

The `HotKeyDispatcher` is responsible for receiving *WM_HOTKEY* messages in the thread message pump, deciphering the hotkey in question, and calling a handler.

## Asynchronous Notes

Long running controllers will run within a [`std::jthread`](https://en.cppreference.com/w/cpp/thread/jthread) and receive a [std::request_stop](https://en.cppreference.com/w/cpp/thread/jthread/request_stop) to exit early. Therefore, to sleep our thread and be able to exit early, we need to check [std::stop_requested](https://en.cppreference.com/w/cpp/thread/stop_source/stop_requested) periodically.

[Interruptable Sleeping Thread Example](https://stackoverflow.com/questions/59756225/interruptible-sleep-in-stdthread).


To make a function that needs to internally sleep and be cancelable, I need to provide both a `std::mutex` and a `std::condition_variable` used for synchronization.

This application has two modes, user controlled and autonomous *(bot)*. Our focus is autonomous mode as that is where this application's offerings shine the most. Therefore, we want a system that can seamlessly transition from the completion of one task to the start of another or enter an idle state awaiting further instructions.

### Autonomous Mode *(bot - worker)*

Autonomous mode is an approach to execution where all *work* is to be completed via a queue system before finally returning to an idle state allowing controllers to put in request as necessary.

A *task* in this case is defined as an instance of `IAutonomousTask` given to the *enqueue(...)* method.

- The ability to enable/disable autonomous mode is done through an *imgui* interface.
- Autonomous mode runs on a seperate worker thread that is created/destroyed when activating/deactiviting autonomous mode.
- A given task can perform any sys calls or even create new threads, such that completion via default flow or manual intervention must be handled gracefully.
- Performing a thread sleep operation within a task should use the [`conditional_variable`](https://en.cppreference.com/w/cpp/thread/condition_variable)::[wait_for](https://en.cppreference.com/w/cpp/thread/condition_variable/wait_for) *(or similar methods)* so that manual invention can occur *(i.e. a user wants to turn off autonomous mode in the middle of a task's execution)*.
- Once all tasks are complete, the worker enters an idle state until a new task is enqueued.
- Each task will need access to a `conditional_variable` when being executed.

#### `IAutonomousTask`

An interface that when implemented provides a means for the Autonomous mode worker to communicate with the 

#### Methods

- Enqueue(), Pushes a task onto the back of queue.
- Dequeue(), Pops a queue from the start of the queue.