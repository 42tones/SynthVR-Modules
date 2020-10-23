## SynthVR Modules
This is the open source repository for modules in SynthVR. The main outcome is a native library that can be compiled for Windows, MacOS and Android. 

The plugin exposes an API to the Unity VR app which can manage native processors, parameters and internal processor state. This API is specified in `ProcessorAPI.h`. Most processors wrap the JUCE `AudioProcessor` and are created by inheriting from `BaseProcessor` which provides the basic SynhVR-specific utilities needed.

### Development strategy
This repository uses the git flow branching strategy. That means that any new development should happen through feature branches (`feature/your-new-feature`). 

When a feature is ready to be merged, submit it as a pull request. Once it has been reviewed, it will be merged to `develop`.

### Building the plugin
- Download the [ROLI Projucer](https://juce.com/discover/projucer)
- Open `SynthVR-Native.jucer`. This file contains the configuration for building the native plugin.
- For editing on Windows, export the project using the Visual Studio exporter
- To build the plugin, select the appropriate exporter and build the plugin using that IDE. You are typically able to select whether it should use a Debug or Release configuration from the IDE.

### Implementing a new native processor
- Create a new header and implementation file combo using the Projucer.
- Inherit the processor from `BaseProcessor` within the namespace `synthvr`. See `FreeverbProcessor.h` for an example.
- Implement your processor. You can use any available JUCE functionality or include additional open source code that will help you in your DSP task. Please be mindful of CPU usage as many SynthVR users are on Oculus Quest, which is a relatively "low-end" Android device.
- The processor will likely need to implement `prepareToPlay` and `processBlock`. `prepareToPlay` will be called by the SynthVR host when a processor is added to the processing graph, or when the audio configuration has changed. `processBlock` is called at every audio callback and will supply an audio buffer with the number of channels specified by your processor.
- When the processor is ready, add it to the `ProcessorID` enum and the `CreateProcessorFromID` in `ProcessorFactory.h`. This exposes your processor so that Unity can instantiate it using an integer ID. (Improvement suggestions for this welcome!)
- Build the plugin. The processor is now ready to be used in a SynthVR module!

### To do
- [x] Repurpose SynthVR-Native for this repo
- [ ] Implement test tool for native module code
- [ ] Implement dynamic module specification
- [ ] Open up to 3rd party modules through submoduling + build scripts
- [ ] Research wrapping VCV rack modules