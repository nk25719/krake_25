
(function () {
  KrakeUI.mountLayout('Factory Test / Developer Monitor');
  async function fetchSerialMonitor() {
    try {
      const res = await fetch('/serial-monitor', { cache: 'no-store' });
      if (!res.ok) return;
      const text = await res.text();
      const uartText = KrakeUI.byId('uartText');
      const nearBottom = uartText.scrollTop + uartText.clientHeight >= uartText.scrollHeight - 8;
      uartText.textContent = text || '(No serial output yet)';
      if (nearBottom) uartText.scrollTop = uartText.scrollHeight;
    } catch (_) { KrakeUI.setText('uartText', 'Serial monitor fetch failed'); }
  }
  fetchSerialMonitor(); setInterval(fetchSerialMonitor, 300);
})();
