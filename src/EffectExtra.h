//
//  EffectExtra.h
//  Additional Plugin Code
//
//  This file is a workspace for developing new DSP objects or functions to use in your plugin.
//
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
class EarlyReflections
{
public:

   EarlyReflections();
   
   void prepare(int maxDelaySamples);
   float process(float fin0 , float fin1);
   private:
   static constexpr int numTaps = 12;
   
   float delayTime[numTaps] = {
   101, 211, 347, 400, 202, 422,
   694, 800, 303, 633, 1041, 1200
   };
  
   float delayLevel[numTaps] = {
        0.90f, 0.82f, 0.75f, 0.69f,
        0.63f, 0.58f, 0.53f, 0.49f,
        0.45f, 0.41f, 0.38f, 0.35f
   };
   Delay Reflections;
  
};
