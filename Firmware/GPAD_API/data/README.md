# GPAD API web assets

The firmware now keeps the browser surface static and GPAP-focused. Runtime state and commands are carried by GPAP over serial/MQTT instead of browser runtime endpoints.

Active firmware routes:

- `/` / `/index.html`
- `/monitor`
- `/serial-monitor` (plain text serial mirror)
- `/manual`
- `/PMD_GPAD_API` / `/PMD_GPAD_API.html`
- `/settings/wifi/reset` (plain text POST)
- `/update` (ElegantOTA)
