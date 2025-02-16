# MIDIPipe

Encode stdin input to MIDI events and send it to the specified ALSA sequencer port.

## Building

```
cd midipipe
mkdir build
cd build
cmake ..
cmake --build .
```

## Example Usage

Use `aconnect -o` to see available output ports.

```
cat /dev/serialdevice | ./midipipe 130:0
```

```
echo -e "\x90\x3c\x7f" | ./midipipe 130:0
```
