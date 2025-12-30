//
//  EffectPlugin.cpp
//  MyEffect Plugin Source Code
//
//  Used to define the bodies of functions used by the plugin, as declared in EffectPlugin.h.
//

#include "EffectPlugin.h"

////////////////////////////////////////////////////////////////////////////
// EFFECT - represents the whole effect plugin
////////////////////////////////////////////////////////////////////////////

// Called to create the effect (used to add your effect to the host plugin)
extern "C" {
    CREATE_FUNCTION createEffect(float sampleRate) {
        ::stk::Stk::setSampleRate(sampleRate);
        
        //==========================================================================
        // CONTROLS - Use this array to completely specify your UI
        // - tells the system what parameters you want, and how they are controlled
        // - add or remove parameters by adding or removing entries from the list
        // - each control should have an expressive label / caption
        // - controls can be of different types: ROTARY, BUTTON, TOGGLE, SLIDER, or MENU (see definitions)
        // - for rotary and linear sliders, you can set the range of values (make sure the initial value is inside the range)
        // - for menus, replace the three numeric values with a single array of option strings: e.g. { "one", "two", "three" }
        // - by default, the controls are laid out in a grid, but you can also move and size them manually
        //   i.e. replace AUTO_SIZE with { 50,50,100,100 } to place a 100x100 control at (50,50)
        
        const Parameters CONTROLS = {
            //  name,       type,              min, max, initial, size
            {   "Time",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Filter",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Gain",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 3",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 4",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 5",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 6",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 7",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 8",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
            {   "Param 9",  Parameter::ROTARY, 0.0, 1.0, 0.0, AUTO_SIZE  },
        };

        const Presets PRESETS = {
            { "Preset 1", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
            { "Preset 2", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
            { "Preset 3", { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } },
        };

        return (APDI::Effect*)new MyEffect(CONTROLS, PRESETS);
    }
}

// Constructor: called when the effect is first created / loaded
MyEffect::MyEffect(const Parameters& parameters, const Presets& presets)
: Effect(parameters, presets)
{   // Initialise member variables


    
    Pathdelay1.setMaximumDelay(41000);
    Pathdelay1.clear();
    Pathtime1 = 4000;                  // we'll use this with tapOut()
    Pathfilter1.setCutoff(3000);
    Pathmulti1 = 0.4;
   
    Pathdelay2.setMaximumDelay(41000);
    Pathdelay2.clear();
    Pathtime2 = 5000;                  // we'll use this with tapOut()
    Pathfliter2.setCutoff(4000);
    Pathmulti2 = 0.3;
}

// Destructor: called when the effect is terminated / unloaded
MyEffect::~MyEffect()
{
    // Put your own additional clean up code here (e.g. free memory)
}

// EVENT HANDLERS: handle different user input (button presses, preset selection, drop menus)

void MyEffect::presetLoaded(int iPresetNum, const char *sPresetName)
{
    // A preset has been loaded, so you could perform setup, such as retrieving parameter values
    // using getParameter and use them to set state variables in the plugin
}

void MyEffect::optionChanged(int iOptionMenu, int iItem)
{
    // An option menu, with index iOptionMenu, has been changed to the entry, iItem
}

void MyEffect::buttonPressed(int iButton)
{
    // A button, with index iButton, has been pressed
}
float MyEffect::PathProcess(float fIn0, float fIn1)
{
    float fmonoIn = (fIn0 + fIn1)* 0.5;// get a mono mix
        
        float Path1 = Pathdelay1.tapOut(Pathtime1); // delay the music
        Path1 = Pathfilter1.tick(Path1);            // filter the music
        Path1 = Path1 * Pathmulti1;
        
        float Path2 = Pathdelay2.tapOut(Pathtime2); // delay the music
        Path2 = Pathfliter2.tick(Path2);             // filter the music
        Path2 = Path2 * Pathmulti2;
        
        // compute feedback
        float pathFeedbackto1 = 0*(Path1) + 1*(Path2);
        float pathFeedbackto2 = -1*(Path1) + 0*(Path2);
        
        Pathdelay1.tick(pathFeedbackto1 + fmonoIn);
        Pathdelay2.tick(pathFeedbackto2 + fmonoIn);
       
        return pathFeedbackto1 + pathFeedbackto2;
}

void MyEffect::process(const float** inputBuffers, float** outputBuffers, int numSamples)
{
    float fIn0, fIn1, fOut0 = 0, fOut1 = 0;
    const float *pfInBuffer0 = inputBuffers[0], *pfInBuffer1 = inputBuffers[1];
    float *pfOutBuffer0 = outputBuffers[0], *pfOutBuffer1 = outputBuffers[1];
    
    while(numSamples--)
    {
      
        // Get sample from input
        fIn0 = *pfInBuffer0++;
        fIn1 = *pfInBuffer1++;
        
    
        // Add your effect processing here
        fOut0 = PathProcess(fIn0, fIn1);
        fOut1 = fIn1;
        
        // Copy result to output
        *pfOutBuffer0++ = fOut0;
        *pfOutBuffer1++ = fOut1;
    }
}
