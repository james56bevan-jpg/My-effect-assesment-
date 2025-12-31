//
//  EffectPlugin.h
//  MyEffect Plugin Header File
//
//  Used to declare objects and data structures used by the plugin.
//

#pragma once

#include "apdi/Plugin.h"
#include "apdi/Helpers.h"
using namespace APDI;

#include "EffectExtra.h"
class LaterReflection
{
public:
    LaterReflection();
    
    float process(float fin0, float fin1);
    
    float Pathtime1, Pathtime2, Pathtime3, Pathtime4;
    float Pathmulti1, Pathmulti2, Pathmulti3, Pathmulti4;
    
private:
 Delay Pathdelay1;
 Delay Pathdelay2;
 Delay Pathdelay3;
 Delay Pathdelay4;
 
 LPF Pathfilter1;
 LPF Pathfilter2;
 LPF Pathfilter3;
 LPF Pathfilter4;

};

class MyEffect : public APDI::Effect
{
public:
    MyEffect(const Parameters& parameters, const Presets& presets); // constructor (initialise variables, etc.)
    ~MyEffect();                                                    // destructor (clean up, free memory, etc.)

    void setSampleRate(float sampleRate){ stk::Stk::setSampleRate(sampleRate); }
    float getSampleRate() const { return stk::Stk::sampleRate(); };
    
    void process(const float** inputBuffers, float** outputBuffers, int numSamples);
    
    void presetLoaded(int iPresetNum, const char *sPresetName);
    void optionChanged(int iOptionMenu, int iItem);
    void buttonPressed(int iButton);

private:
    Delay reflections;
};

