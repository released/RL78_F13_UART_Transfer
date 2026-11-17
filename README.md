# RL78_F13_UART_Transfer

Standalone, non-destructive UART ISP communication sample for R5F10BMG. Sources, configuration and build helpers are included in this project.

## Board and interface

| Item | Configuration |
| --- | --- |
| Clock | X1 4 MHz / CPU 32 MHz |
| Update link | UART1 TX / RX P74 / P75, 115200 8-N-1 |
| Diagnostic Log | UART0 TX / RX P15 / P16, 115200 8-N-1 |
| Update interrupt | UART1 RX vector 0x40 |
| Timer vector | 0x1E |
| Flash implementation | FSL Type01; Flash-busy call chain executes from RAM |

Vector values are physical byte addresses, not channel numbers. UART0 carries text only. Cross UART TX/RX, or connect matching SCL/SDA with voltage-compatible pull-ups; share ground and verify logic levels.

## Image and interrupt ownership

Boot is 0x0000..0x1FFF (8 KiB), with at least 512 bytes spare. App vectors are 0x2000..0x20FF, metadata is 0x2100..0x21FF (256 bytes), and App code starts at 0x2200.
Maximum complete App BIN is 119 KiB; 0x1FC00..0x1FFFF is protected. Size includes vectors, metadata, padding and the four-byte CRC32.
Boot owns physical vectors: AA selects Boot handlers; 55 forwards through the App table. Normal handlers use RETI; BRK retains RETB.

## Configuration and application editing

- `image_metadata_config.h`: neutral MCU/Flash/format/Boot identity.
- `isp_transport_config.h`: selected ISP bus/protocol settings; metadata readers use the same values.
- `boot_app_handoff.h`: reserved RAM and AA/55 owner.
- `hal/isp/`: protocol and transport runtime; `hal/device/` and `hal/flash/` own hardware adapters.
- `image_metadata.c/.h`, `image_metadata_profile.c`: independent parser and expected profile.

No protocol parser, Flash operation or formatted log belongs in an ISR.

## Build and files

Run in this project root using installed CS+/CC-RL and Windows PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
```

Use `-Compiler <CubeSuite+.exe>` for a different installed CS+ path. Firmware authoring/build requires no Python or JSON editor. Generated INC/identity files are outputs, not inputs. App/Transfer wrappers build only themselves.

| File under DefaultBuild | Use |
| --- | --- |
| `RL78_F13_UART_Transfer.hex` | Programmer installation of this standalone reset-based diagnostic image |

This is neither an App update BIN nor a Boot-only HEX. It reports device/protocol identity, not installed Boot/App metadata.

## Tool operation and protocol

App/Transfer can advertise optional command0x25 UART diagnostics. Device RX
errors/drops,TX timeouts and queue snapshots are separate from bridge counters.
See [diagnostic fields](https://released.github.io/slide_boot_loader_for_RL78/serial-device.html#serial-uart-diagnostics).

Install the standalone HEX with a programmer. Select **UART / Xmodem > Custom Transfer**, then **Discover Test Target > Transfer Test**. A BIN is not required. Flash update commands and ENTER are rejected; INFO capability 0x10 identifies this role.

See the local [protocol reference](PROTOCOL.md) for byte layouts and transport behavior.
[Tool operation and wiring](https://released.github.io/slide_simple_renesas_isp_tool/#uart-device-operation) provides the common operation flow and report links.
Offline build/model results do not establish electrical, physical IRQ/Flash, dynamic-stack, restart or power-interruption acceptance.

## User code entry

`custom_func.c` provides `hardware_init()` for initialization and `loop()` for
foreground service. Add application initialization and foreground work through
these callbacks, retaining the existing communication service calls.
The selected main/startup adapter owns entry and handoff. Generated peripheral
files remain under `src/`; retain the project's MTPJ source selection when using
Code Generator or Smart Configurator, then rebuild to validate the image and vectors.

## Project layout and standalone build

This project includes its own source files, configuration headers and build tools.
A sibling Boot, App or Transfer checkout is not required to compile it.
Keep the project directory name aligned with its `.mtpj` filename; remove a ZIP download suffix when necessary.
Build with CS+ Rebuild, or run the local PowerShell entry:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\build.ps1
```

| Location | Responsibility |
| --- | --- |
| `custom_func.c` | Project initialization in `hardware_init()` and recurring work in `loop()`; runtime orchestration stays here |
| `hal/can` | CAN service, controller driver and CAN adapter, when used by this project |
| `hal/device` | Device, clock, peripheral and CPU handoff APIs |
| `hal/image` | Metadata parser/profile, image validation and software CRC |
| `hal/flash` | Flash access/geometry APIs, when required by the selected image |
| `hal/isp` | Update/Transfer protocol and transport processing APIs |
| Root configuration headers | Local MCU/Flash, handoff and communication contract |
| `tools` and root PowerShell helpers | Local metadata/IRQ generation and linked-image checks |
| `src` | Code Generator or Smart Configurator peripheral sources |

Only the sources registered in this project's CS+ configuration are linked.
Generated main/vector implementations must not become a second active entry.
Retain the `custom_func.c` callbacks and the selected startup adapter after code generation.

Boot and App each carry a local contract. Keep their MCU, Flash layout, image format,
IRQ ABI, handoff and communication settings compatible when installing a pair.
App releases are edited in `app_image_config.h` and `app_irq_handlers.h` in the App project.
Rebuild the selected App to regenerate its metadata, vector table, update BIN and timing BINs.
The App build does not read or rewrite a Boot checkout.

## UART0 local console

- Diagnostic UART0 uses115200,8-N-1; UART1 or I2C retains binary ISP traffic.
- `z` or `Z` requests a normal MCU reset after the current reply completes; I2C includes final NACK/STOP.
- Foreground RX/TX traces show16-bit sequence, command, length and complete validated payload. `LOG_DROP` counts discarded text records.
- G16 services text from existing packet buffers for at most200ms before accepting another framed request; other MCUs use bounded snapshot queues.

## GitHub updates

`github_push.bat -DryRun` previews the source/image snapshot. `github_push.bat` updates the PUBLIC `released/RL78_F13_UART_Transfer` main branch after matching board acceptance. Only the reviewed literal file list and the role delivery are staged; local build outputs stay in place. Repository creation is a one-time setup step.
