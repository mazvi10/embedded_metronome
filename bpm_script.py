import numpy as np
import sounddevice as sd
import asyncio
import librosa
from bleak import BleakClient, BleakScanner

DEVICE_NAME = "Chagbb"
UART_WRITE = "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
UART_NOTIFY = "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
LISTEN_SECONDS = 8
RESEND_SECONDS = 4
BPM_MAX = 200
BPM_TOLERANCE = 3
BPM_STALE_SECONDS = 20

# record ambient audio and estimate bpm
def listen_bpm():
    sample_rate = int(sd.query_devices(kind="input")["default_samplerate"])
    audio = sd.rec(
        LISTEN_SECONDS * sample_rate,
        samplerate=sample_rate,
        channels=1,
        dtype="float32",
        blocking=True,
    )[:, 0]

    tempo, beats = librosa.beat.beat_track(y=audio, sr=sample_rate)
    bpm = float(np.asarray(tempo).item())

    # out of range
    if len(beats) < 4 or not 40 <= bpm <= BPM_MAX:
        return None

    return round(bpm)

# printing the Coolterm input (from Nucleo)
def on_uart_data(_characteristic, data):
    print(data.decode("ascii", errors="replace"), end="", flush=True)

#main
async def main():
    print(f"Searching for {DEVICE_NAME}...")
    devices = await BleakScanner.discover(timeout=8)
    bluefruit = next((d for d in devices if d.name == DEVICE_NAME), None)

    if bluefruit is None:
        print(f"Couldn't find {DEVICE_NAME}")
        return

    async with BleakClient(bluefruit) as client:
        print(f"Connected to {bluefruit.name}. Press Ctrl+C to stop.")
        await client.start_notify(UART_NOTIFY, on_uart_data)

        accepted_bpm = None
        last_confirmed = None
        bpm_updated = asyncio.Event()
        loop = asyncio.get_running_loop()

        async def detect_bpm():
            nonlocal accepted_bpm, last_confirmed
            previous = None

            while client.is_connected:
                print("Discombobulating...")
                bpm = await asyncio.to_thread(listen_bpm) # keep bluetooth whilst connecting

                if not client.is_connected:
                    break

                if bpm is None:
                    print("No beat detected.")
                    previous = None
                    continue

                print(f"Estimated {bpm} BPM")

                # Comparing 2 readingd
                stable = previous is not None and abs(bpm - previous) <= BPM_TOLERANCE
                if (
                    accepted_bpm is not None
                    and abs(bpm - accepted_bpm) <= BPM_TOLERANCE
                ):
                    last_confirmed = loop.time()
                elif stable:
                    accepted_bpm = round((previous + bpm) / 2)
                    last_confirmed = loop.time()
                    bpm_updated.set()
                    print(f"Locked to {accepted_bpm} BPM")

                previous = bpm

        async def send_bpm():
            nonlocal accepted_bpm, last_confirmed

            while client.is_connected:
                # Send when the BPM changes or resend after 4 seconds
                try:
                    await asyncio.wait_for(bpm_updated.wait(), RESEND_SECONDS)
                except TimeoutError:
                    pass
                bpm_updated.clear()

                if not client.is_connected:
                    break

                if accepted_bpm is None or last_confirmed is None:
                    continue

                # stop resending an old BPM so the Nucleo can time out
                if loop.time() - last_confirmed >= BPM_STALE_SECONDS:
                    print("BPM lock expired. Waiting for a beat.")
                    accepted_bpm = None
                    last_confirmed = None
                    continue

                await client.write_gatt_char(
                    UART_WRITE,
                    f"{accepted_bpm}\n".encode("ascii"),
                    response=True,
                )
                print(f"Sent {accepted_bpm} BPM")

        async with asyncio.TaskGroup() as tasks:
            tasks.create_task(detect_bpm())
            tasks.create_task(send_bpm())

        print(f"{DEVICE_NAME} disconnected!")

try:
    asyncio.run(main())
except KeyboardInterrupt:
    print("\nStopped.")