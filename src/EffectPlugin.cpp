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
            { "Wet_Dry_mix",Parameter::ROTARY, 0.0, 1.0, 0.3, AUTO_SIZE  },
            {  "Early_Gain",Parameter::ROTARY, 0.0, 1.0, 1.0, AUTO_SIZE  },
            {   "Room_Size",Parameter::ROTARY, 0.5, 4.0, 1.0, AUTO_SIZE  },
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
  earlyReflections.prepare(41000);
}
 EarlyReflections::EarlyReflections()
 {
   for (int i = 0; i < numTaps; i++)
        baseDelayTime[i] = delayTime[i];
 }
 void EarlyReflections::prepare(int maxDelaySamples)
{
    Reflections.setMaximumDelay(maxDelaySamples);
    Reflections.setDelay(maxDelaySamples - 1);
    Reflections.clear();
}
void EarlyReflections::setRoomSize(float scale)
{
for (int i = 0; i < numTaps; i++)
    {
      delayTime[i]= baseDelayTime[i] * scale;
    }
}
 float EarlyReflections::process(float fin1,float fin0)
 {
    //Reflections.setDelay(0); not needed with room size parameter
  float fmono = (fin1 + fin0)*0.5;
   Reflections.tick(fmono);

    float dsMix = 0;
        for (int i=0; i<numTaps; i++)
        {
             dsMix += Reflections.tapOut( delayTime[i] ) * delayLevel[i];
        }
    dsMix *= 0.25;
    return dsMix;
 }
LaterReflection::LaterReflection()
{
        Pathdelay1.setMaximumDelay(41000);
        Pathdelay1.setDelay(basePathtime1);
        Pathdelay1.clear();
        Pathfilter1.setCutoff(3000);
        // we'll use this with tapOut()
       
        Pathdelay2.setMaximumDelay(41000);
        Pathdelay2.setDelay(basePathtime2);
        Pathdelay2.clear();
        Pathfilter2.setCutoff(4000);
        
        
        Pathdelay3.setMaximumDelay(41000);
        Pathdelay3.setDelay(basePathtime3);
        Pathdelay3.clear();
        Pathfilter3.setCutoff(4500);
      
    
        Pathdelay4.setMaximumDelay(41000);
        Pathdelay4.setDelay(basePathtime4);
        Pathdelay4.clear();
        Pathfilter4.setCutoff(5000);
        
        setRoomSize(1.0);
       
}
void LaterReflection::setRoomSize(float scale)
{
// Delay scaling
        Pathtime1 = basePathtime1 * scale;
        Pathtime2 = basePathtime2 * scale;
        Pathtime3 = basePathtime3 * scale;
        Pathtime4 = basePathtime4 * scale;
        
//FeedBack comp

        float fbScale = 1.0/ sqrt(scale);

//Path multi scaling Keep Multipilers always below 7 because there is no dampening
         Pathmulti1 = baseMulti1 * fbScale;
         Pathmulti2 = baseMulti2 * fbScale;
         Pathmulti3 = baseMulti3 * fbScale;
         Pathmulti4 = baseMulti4 * fbScale;
}
float LaterReflection::process(float fin1, float fin0)
{
        float fmonoIn = (fin1 + fin0)* 0.5;
        
        float Path1 = Pathdelay1.tapOut(Pathtime1); // delay the music
        Path1 = Pathfilter1.tick(Path1)*0.98; // frequency dependant decay to stop ringing
        Path1 = Path1 * Pathmulti1;
        
        float Path2 = Pathdelay2.tapOut(Pathtime2); // delay the music
        Path2 = Pathfilter2.tick(Path2);             // filter the music
        Path2 = Path2 * Pathmulti2;
        
        float Path3 = Pathdelay3.tapOut(Pathtime3);
        Path3 = Pathfilter3.tick(Path3);
        Path3= Path3 * Pathmulti3;
        
        float Path4 = Pathdelay4.tapOut(Pathtime4);
        Path4 = Pathfilter4.tick(Path4);
        Path4 = Path4 * Pathmulti4;
        
        constexpr float norm = 0.5; // 1 / sqrt(4)  // feedback matrix norm will always 0.5 hence why I used constexpr
        
        float pathFeedbackto1 = norm * ( Path2 + Path3 - Path4 );
        float pathFeedbackto2 = norm * ( -Path1 - Path3 + Path4 );
        float pathFeedbackto3 = norm * ( -Path1 + Path2 - Path4 );
        float pathFeedbackto4 = norm * ( Path1 - Path2 + Path3 );
        
        Pathdelay1.tick(pathFeedbackto1 * Pathmulti1 + fmonoIn);
        Pathdelay2.tick(pathFeedbackto2 * Pathmulti2 + fmonoIn);
        Pathdelay3.tick(pathFeedbackto3 * Pathmulti3 + fmonoIn);
        Pathdelay4.tick(pathFeedbackto4 * Pathmulti4 + fmonoIn);

        return pathFeedbackto1 + pathFeedbackto2 + pathFeedbackto3 + pathFeedbackto4;
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

void MyEffect::process(const float** inputBuffers, float** outputBuffers, int numSamples)
{
    float fIn0, fIn1, fOut0 = 0, fOut1 = 0;
    const float *pfInBuffer0 = inputBuffers[0], *pfInBuffer1 = inputBuffers[1];
    float *pfOutBuffer0 = outputBuffers[0], *pfOutBuffer1 = outputBuffers[1];
    float mix = parameters[0];
    float earlyGain = parameters[1];
    float roomSize = parameters[2];
        earlyReflections.setRoomSize(roomSize);
        lateBlock0.setRoomSize(roomSize);
        LateBlock1.setRoomSize(roomSize);
    while(numSamples--)
    {
        // Get sample from input
        fIn0 = *pfInBuffer0++;
        fIn1 = *pfInBuffer1++;
        
        float dry = 0.5 * (fIn0 + fIn1);
        
        float early = earlyReflections.process(fIn0, fIn1) * earlyGain;
        float late  = lateBlock0.process(early, early);
        float latest = LateBlock1.process(late, late);

        float wet = (early + late + latest) * 1.5;
        float out = (1.0 - mix) * dry + mix * wet;

        // Add your effect processing here
        fOut0 = out;
        fOut1 = out;
        
        // Copy result to output
        *pfOutBuffer0++ = fOut0;
        *pfOutBuffer1++ = fOut1;
    }
}
